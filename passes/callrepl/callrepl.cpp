#include <map>
#include <tuple>
#include <vector>

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "blas_operation.h"
#include "fu_mmap.h"

using namespace llvm;

extern char * _io_map;
extern blasop operations[]; 

enum RetType {
  RET_VOID,
  RET_DOUBLE,
  RET_INT
};

enum ArgType {
  ARG_N,
  ARG_ALPHA,
  ARG_X,
  ARG_Y
};

// TODO maybe this map could be combined with the array in blas_operation.h
// TODO somehow need to get indices from the header file
// TODO this should be a struct, maybe
// daxpy X/Y are swapped so that X remains as the modified array
std::map<std::string, std::pair<RetType, std::vector<ArgType>>> functions = {
  {"cblas_dscal",  {RET_VOID  , { ARG_N, ARG_ALPHA, ARG_X        }}},
  {"cblas_daxpy",  {RET_VOID  , { ARG_N, ARG_ALPHA, ARG_Y, ARG_X }}},
  {"cblas_ddot",   {RET_DOUBLE, { ARG_N,            ARG_X, ARG_Y }}},
  {"cblas_dnrm2",  {RET_DOUBLE, { ARG_N,            ARG_X        }}},
  {"cblas_dasum",  {RET_DOUBLE, { ARG_N,            ARG_X        }}},
  {"cblas_idamax", {RET_INT   , { ARG_N,            ARG_X        }}}
};


namespace {
  struct CallReplPass : public FunctionPass {
    static char ID;

    Constant * IO_Map_Ptr;
    Value * io_map;

    CallReplPass() : FunctionPass(ID) {}

    void sendScalar(IRBuilder<> Bldr, int array_idx, Value * v) {
      Value * array_loc = Bldr.CreateInBoundsGEP(io_map, Bldr.getInt64(array_idx));
      // TODO: v could be double or int?
      Value * dbl_arg = Bldr.CreateUIToFP(v, Bldr.getDoubleTy());
      Bldr.CreateStore(dbl_arg, array_loc);
    }

    void MemCpyWrapper(IRBuilder<> Bldr, Value * dest, Value * src, Value * len) {
      // Compute size (in bytes) of the array. Thisis just len * sizeof(double)
      Value * array_size =
        Bldr.CreatePtrToInt(
            Bldr.CreateInBoundsGEP(
              ConstantPointerNull::get(Type::getDoublePtrTy(Bldr.getContext())), len),
            Bldr.getInt32Ty());
      Bldr.CreateMemCpy(dest, 0, src, 0, array_size);
    }

    void recvVector(IRBuilder<> Bldr, Value * dest, Value * src_i, Value * len) {
      Value * src = Bldr.CreateInBoundsGEP(io_map, src_i);
      MemCpyWrapper(Bldr, dest, src, len);
    }

    void sendVector(IRBuilder<> Bldr, Value * dest_i, Value * src, Value * len) {
      Value * dest = Bldr.CreateInBoundsGEP(io_map, dest_i);
      MemCpyWrapper(Bldr, dest, src, len);
    }


    bool doInitialization(Module &M) {
      // I don't fully understand why this is an 8-bit pointer, but it is
      IO_Map_Ptr = M.getOrInsertGlobal("_io_map", Type::getInt8PtrTy(M.getContext()));
      return true;
    }

    bool runOnFunction(Function &F) {
      bool modified = false;
      bool io_map_loaded = false;

      if (F.getName() == "main") {
        // errs() << "[PASS] Trying to create initial map...\n";

        // Make this the first line of main (first front gets BB, second gets Inst)
        IRBuilder<> Bldr(& F.front().front());

        // TODO actual function type is void, not bool. but that doesn't compile
        // due to some kind of debuginfo inlining issue. falsely declaring the
        // return type doesn't appear to affect anything, since the "return
        // value" is never used.
        FunctionType * FTy = FunctionType::get(Bldr.getInt1Ty(), false);
        FunctionCallee mapFunc = F.getParent()->getOrInsertFunction("_create_io_map", FTy);

        auto map_call = Bldr.CreateCall(mapFunc);
        // map_call->setDebugLoc(Bldr.getCurrentDebugLocation());
        verifyFunction(F);
        errs() << "[PASS] Created map call\n";
        modified = true;
      }

      for (inst_iterator I = inst_begin(F); I != inst_end(F); ++I) {
        if (auto * call = dyn_cast<CallInst>(&*I)) {
          Function * oldF = call->getCalledFunction();

          auto it = functions.find(oldF->getName());
          if (it == functions.end()) {
            // This function not found.
            // errs() << "Couldn't find function " << oldF->getName() << " in map.\n";
            continue;
          }

          // look for the index
          int func_id;
          for (func_id = 0; func_id < NUM_FUNCS; func_id++) {
            if (operations[func_id].cblas_name == oldF->getName())
              break;
          }
          if (func_id == NUM_FUNCS) {
            errs() << "[PASS] Couldn't find function " << oldF->getName() << " in op array.\n";
            continue;
          }

          auto next_inst = ++I;

          auto signature = it->second;
          auto ret = signature.first;
          auto argv = signature.second;

          errs() << "[PASS] Replacing function " << oldF->getName() << "\n"
            
          /* << " with sig ";
          for (auto i = argv.begin(); i != argv.end(); ++i) {
            errs() << *i << " ";
          }
          errs() << "\n"; */

          IRBuilder<> Bldr(call);
          
          // move insertion point after call -- not sure if necessary
          Bldr.SetInsertPoint(call->getParent(), ++Bldr.GetInsertPoint());

          // load the io_map array if it hasn't already
          if (! io_map_loaded) {
            io_map = Bldr.CreateBitCast(
                               Bldr.CreateLoad(Bldr.getInt8PtrTy(), IO_Map_Ptr),
                               Type::getDoublePtrTy(Bldr.getContext()));
            io_map_loaded = true;
          }

          Value * N;
          Value * user_X;
          Value * X_start_i = Bldr.getInt32(IDX_START_X);

          // loop through arguments
          // TODO this is really messy.
          auto i = argv.begin();
          auto arg = call->arg_begin();
          for (; i != argv.end() && arg != call->arg_end(); ++i, ++arg) {
            // check which argument this is
            if (*i == ARG_N) {
              N = arg->get();
              sendScalar(Bldr, IDX_N, N);
            } else if (*i == ARG_ALPHA) {
              sendScalar(Bldr, IDX_ALPHA, arg->get());
            } else if (*i == ARG_X) {
              user_X = arg->get();
              sendVector(Bldr, X_start_i, user_X, N);
              ++arg; // eat INC_X
            } else if (*i == ARG_Y) {
              sendVector(Bldr, Bldr.CreateAdd(X_start_i, N), arg->get(), N);
              ++arg; // eat INC_Y
            }
          }

          // run function! (store function ID into array)
          Value * func_idx = Bldr.CreateInBoundsGEP(io_map, Bldr.getInt64(IDX_FUNCTION));
          Bldr.CreateStore(ConstantFP::get(Bldr.getContext(), APFloat((double)func_id)), func_idx);

          if (ret == RET_VOID) {
            // non-returning subroutines modify X; must copy back
            recvVector(Bldr, user_X, X_start_i, N);
          } else {
            // get result from alpha
            Value * r = Bldr.CreateLoad(
                Bldr.CreateInBoundsGEP(io_map, Bldr.getInt64(IDX_ALPHA)));

            if (ret == RET_INT) {
              r = Bldr.CreateFPToSI(r, Bldr.getInt32Ty());
            }
            // ret == RET_DOUBLE: nop, already have a double
            
            // replace
            for (auto &U : call->uses()) {
              User * user = U.getUser();
              user->setOperand(U.getOperandNo(), r);
            }
          } 

          // delete old call
          call->eraseFromParent();

          I = --next_inst;
          verifyFunction(F);
          modified = true;
          // errs() << F;
        }
      }

      return modified;
    }
  };
}

char CallReplPass::ID = 0;
static RegisterPass<CallReplPass> X("callrepl", "Call Replace Pass", true, false);

