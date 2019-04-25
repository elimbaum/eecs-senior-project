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

#include "blop.hh"
#include "fu_mmap.h"

using namespace llvm;

extern char * _io_map;

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

    void recvVector(IRBuilder<> Bldr, Value * dest, Value * src_i, Value * len) {
      Value * src = Bldr.CreateInBoundsGEP(io_map, src_i);
      Bldr.CreateMemCpy(dest, 0, src, 0, len);
    }

    void sendVector(IRBuilder<> Bldr, Value * dest_i, Value * src, Value * len) {
      Value * dest = Bldr.CreateInBoundsGEP(io_map, dest_i);
      Bldr.CreateMemCpy(dest, 0, src, 0, len);
    }


    bool doInitialization(Module &M) {
      // I don't fully understand why this is an 8-bit pointer, but it is
      IO_Map_Ptr = M.getOrInsertGlobal("_io_map", Type::getInt8PtrTy(M.getContext())); 

      // Load the operations
      errs() << "[PASS] init\n";
      BlasOperation::init();

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

          auto op = BlasOperation::find(oldF->getName());
          if (!op) {
            // This function not found.
            // errs() << "Couldn't find function " << oldF->getName() << " in map.\n";
            continue;
          }
          errs() << "[PASS] Replacing function @ " << op->cblas_name << "\n";

          // save next instruction so can restore later
          auto next_inst = ++I;
          IRBuilder<> Bldr(call);
          
          // move insertion point after call -- not sure if necessary
          Bldr.SetInsertPoint(call->getParent(), ++Bldr.GetInsertPoint());

          // load the io_map array if it hasn't been already
          if (! io_map_loaded) {
            io_map = Bldr.CreateBitCast(
                               Bldr.CreateLoad(Bldr.getInt8PtrTy(), IO_Map_Ptr),
                               Type::getDoublePtrTy(Bldr.getContext()));
            io_map_loaded = true;
          }

          Value * N;
          Value * user_X, * user_Y;
          Value * X_start_i = Bldr.getInt32(IDX_START_X);
          Value * array_size_in_bytes;

          // loop through arguments
          // op->argv is the internal map for looking up signatures
          // call->arg... is the arguments in the actual function call
          auto i = op->argv.begin();
          auto arg = call->arg_begin();
          for (; i != op->argv.end() && arg != call->arg_end(); ++i, ++arg) {
            // check which argument this is
            if (*i == Arg::N) {
              N = arg->get();
              sendScalar(Bldr, IDX_N, N);
              
              // Compute size (in bytes) of the array. This is just N * sizeof(double)
              array_size_in_bytes =
                Bldr.CreatePtrToInt(
                    Bldr.CreateInBoundsGEP(
                      ConstantPointerNull::get(Type::getDoublePtrTy(Bldr.getContext())), N),
                  Bldr.getInt32Ty());
            } else if (*i == Arg::ALPHA) {
              sendScalar(Bldr, IDX_ALPHA, arg->get());
            } else if (*i == Arg::X) {
              user_X = arg->get();
              sendVector(Bldr, X_start_i, user_X, array_size_in_bytes);
              ++arg; // eat INC_X
            } else if (*i == Arg::Y) {
              user_Y = arg->get();
              // TODO: if Y is the same array as X, don't resend.
              errs() << "Y is X? " << (user_X->getValueName() == user_Y->getValueName()) << "\n";
              sendVector(Bldr, Bldr.CreateAdd(X_start_i, N), user_Y, array_size_in_bytes);
              ++arg; // eat INC_Y
            }
          }

          // run function! (store function ID into array)
          Value * func_idx = Bldr.CreateInBoundsGEP(io_map, Bldr.getInt64(IDX_FUNCTION));
          Bldr.CreateStore(ConstantFP::get(Bldr.getContext(), APFloat((double)op->id)), func_idx);

          if (op->ret == RetTy::VOID) {
            // non-returning subroutines modify X; must copy back
            recvVector(Bldr, user_X, X_start_i, array_size_in_bytes);
          } else {
            // get result from alpha
            Value * r = Bldr.CreateLoad(
                Bldr.CreateInBoundsGEP(io_map, Bldr.getInt64(IDX_ALPHA)));

            if (op->ret == RetTy::INT) {
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
            
          // restore to last new instruction inserted... for loop will increment
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

