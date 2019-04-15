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
std::map<std::string, std::pair<RetType, std::vector<ArgType>>> functions = {
  {"cblas_dscal",  {RET_VOID  , { ARG_N, ARG_ALPHA, ARG_X        }}},
  {"cblas_daxpy",  {RET_VOID  , { ARG_N, ARG_ALPHA, ARG_X, ARG_Y }}},
  {"cblas_ddot",   {RET_DOUBLE, { ARG_N,            ARG_X, ARG_Y }}},
  {"cblas_dnrm2",  {RET_DOUBLE, { ARG_N,            ARG_X        }}},
  {"cblas_dasum",  {RET_DOUBLE, { ARG_N,            ARG_X        }}},
  {"cblas_idamax", {RET_INT   , { ARG_N,            ARG_X        }}}
};


namespace {
  struct CallReplPass : public FunctionPass {
    static char ID;
    Constant * IO_Map_Ptr;
    CallReplPass() : FunctionPass(ID) {}

    bool doInitialization(Module &M) {
      // TODO may only want to create this if any relevant functions are called
      
      LLVMContext& Ctx = M.getContext();
      // TODO why is this an 8-bit ptr?
      IO_Map_Ptr = M.getOrInsertGlobal("_io_map", Type::getInt8PtrTy(Ctx));

      // TODO if mapping goes wrong at any point, fall back to original function
      //   this would require conditionals on every instruction. might not be
      //   worth -- map should die instead
      return true;
    }

    bool runOnFunction(Function &F) {
      LLVMContext& Ctx = F.getContext();
      bool modified = false;

      if (F.getName() == "main") {
        errs() << "[PASS] Trying to create initial map...\n";
        // TODO actual function type is void, not bool. but that doesn't compile
        // due to some kind of debuginfo inlining issue. falsely declaring the
        // return type doesn't appear to affect anything, since the "return
        // value" is never used.
        FunctionType * FTy = FunctionType::get(Type::getInt1Ty(Ctx), false);
        FunctionCallee mapFunc = F.getParent()->getOrInsertFunction("_create_io_map", FTy);

        // Make this the first line of main (first front gets BB, second gets Inst)
        IRBuilder<> builder(& F.front().front());

        auto map_call = builder.CreateCall(mapFunc);
        // map_call->setDebugLoc(builder.getCurrentDebugLocation());
        verifyFunction(F);
        errs() << "[PASS] Created map call\n";
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

          errs() << "[PASS] Found function " << oldF->getName() << " with sig ";
          for (auto i = argv.begin(); i != argv.end(); ++i) {
            errs() << *i << " ";
          }
          errs() << "\n";

          IRBuilder<> builder(call);
          
          // move insertion point after call -- not sure if necessary
          builder.SetInsertPoint(call->getParent(), ++builder.GetInsertPoint());

          // load the io_map array
          // TODO only load io_map once per function
          Value * io_map = builder.CreateBitCast(
                             builder.CreateLoad(Type::getInt8PtrTy(Ctx), IO_Map_Ptr),
                             Type::getDoublePtrTy(Ctx));

          Value * N;

          // loop through arguments
          // TODO this is really messy.
          auto i = argv.begin();
          auto arg = call->arg_begin();
          for (; i != argv.end() && arg != call->arg_end(); ++i, ++arg) {

            // default array index is for alpha to allow case fallthrough
            auto scalar_idx = IDX_ALPHA;
            auto vector_idx = IDX_START_X; // TODO don't actually know where Y is at compile time

            // check which argument this is
            switch(*i) {
              case ARG_N:
                scalar_idx = IDX_N;
                N = arg->get();
                // fall through
              case ARG_ALPHA:
                {
                  Value * array_loc = builder.CreateInBoundsGEP(io_map,
                      ConstantInt::get(Ctx, APInt(64, scalar_idx, true)));
                  Value * d_arg = builder.CreateUIToFP(arg->get(), Type::getDoubleTy(Ctx));
                  builder.CreateStore(d_arg, array_loc);
                }
                break;

              case ARG_X:
                vector_idx = IDX_START_X;
              case ARG_Y:
                {
                  // copy over each element... memcpy call
                  // void * memcpy(void *, void *, size_t)
                  
                  Value * array_loc = builder.CreateInBoundsGEP(io_map,
                        ConstantInt::get(Ctx, APInt(64, vector_idx, true)));
                  Value * array_len = 
                    builder.CreatePtrToInt(
                      builder.CreateInBoundsGEP(
                        ConstantPointerNull::get(Type::getDoublePtrTy(Ctx)), N),
                      Type::getInt32Ty(Ctx));

                  Type * arg_ty_list[] = {Type::getDoublePtrTy(Ctx), Type::getDoublePtrTy(Ctx), Type::getInt32Ty(Ctx)};
                  FunctionType * FTy = FunctionType::get(Type::getInt64PtrTy(Ctx), arg_ty_list, false);
                  Value * arg_list[] = {array_loc, arg->get(), array_len};
                  builder.CreateCall(F.getParent()->getOrInsertFunction("memcpy", FTy), arg_list);

                  // eat inc_x / inc_y
                  ++arg;
                }

                break;


            }
            // get array index
            // store
            // for arrays: loop through and copy
          }

          // run function! (store function ID into array)
          Value * func_idx = builder.CreateInBoundsGEP(io_map,
              ConstantInt::get(Ctx, APInt(64, IDX_FUNCTION, true)));
          builder.CreateStore(ConstantFP::get(Ctx, APFloat((double)func_id)), func_idx);

          if (ret != RET_VOID) {
            Value * r = builder.CreateLoad(
                builder.CreateInBoundsGEP(io_map, ConstantInt::get(Ctx, APInt(64, IDX_ALPHA, true))),
                oldF->getName() + "_ret");

            if (ret == RET_INT) {
              r = builder.CreateFPToSI(r, Type::getInt32Ty(Ctx));
            }
            // ret == RET_DOUBLE: nop, already have a double
            
            // replace
            for (auto &U : call->uses()) {
              User * user = U.getUser();
              user->setOperand(U.getOperandNo(), r);
            }
          }
          // if no return, need to copy X back!

          // test with hypot
          /*
          // To specify multiple indices:
          // Value * idx_list[] = {ConstantInt::get(Ctx, APInt(64, 0, true))};
          // Value * A = builder.CreateGEP(io_map, idx_list);
          Value * A = builder.CreateInBoundsGEP(io_map, ConstantInt::get(Ctx, APInt(64, 0, true)));
          Value * B = builder.CreateInBoundsGEP(io_map, ConstantInt::get(Ctx, APInt(64, 1, true)));

          auto arg = call->arg_begin();
          builder.CreateStore(arg->get(), A);
          arg++; // next arg
          builder.CreateStore(arg->get(), B);

          // get return
          Value * C = builder.CreateLoad(
              builder.CreateInBoundsGEP(io_map, ConstantInt::get(Ctx, APInt(64, 2, true))));

          */

          // delete old call
          call->eraseFromParent();

          I = --next_inst;
          verifyFunction(F);
          modified = true;
          // TODO i think i need to advance the iterator to AFTER 
          // errs() << F;
        }
      }

      return modified;
    }
  };
}

char CallReplPass::ID = 0;
static RegisterPass<CallReplPass> X("callrepl", "Call Replace Pass", true, false);

