#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "fu_mmap.h"

using namespace llvm;

extern char * _io_map;

namespace {
  struct CallReplPass : public FunctionPass {
    static char ID;
    Constant * IO_Map_Ptr;
    CallReplPass() : FunctionPass(ID) {}

    bool doInitialization(Module &M) {
      // TODO may only want to create this if any relevant functions are called
      
      LLVMContext& Ctx = M.getContext();
      IO_Map_Ptr = M.getOrInsertGlobal("_io_map", Type::getInt8PtrTy(Ctx));

      // TODO if mapping goes wrong at any point, fall back to original function
      //   this would require conditionals on every instruction. might not be
      //   worth -- map should die instead
      return true;
    }

    bool runOnFunction(Function &F) {
      LLVMContext& Ctx = F.getContext();

      if (F.getName() == "main") {
        errs() << "Trying to create initial map...\n";
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
        errs() << "Created map call\n";
      }

      for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (auto * call = dyn_cast<CallInst>(&*I)) {
          // only look at cblas functions
          // if (! call->getCalledFunction()->getName().startswith("cblas_"))
          //  continue;

          // test with hypot
          Function * oldF = call->getCalledFunction();
          if (oldF->getName() != "hypot") continue;

          IRBuilder<> builder(call);
          builder.SetInsertPoint(call->getParent(), ++builder.GetInsertPoint());

          // store first arg
          // store second arg
          // load result (can be instantaneous on in-order CPU)

          // Casting the array as an array of doubles. Makes sense for testing, might need to change
          // Global is a POINTER so we have to load it before it can be used.
          Value * io_map = builder.CreateBitCast(
                             builder.CreateLoad(Type::getInt8PtrTy(Ctx), IO_Map_Ptr),
                             Type::getDoublePtrTy(Ctx));
         
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

          // replace old uses with new
          for (auto &U : call->uses()) {
            User * user = U.getUser();
            user->setOperand(U.getOperandNo(), C);
          }

          // delete old call
          call->eraseFromParent();

          // errs() << F;

          verifyFunction(F);
          return true;
        }
      }

      return false;
    }
  };
}

char CallReplPass::ID = 0;
static RegisterPass<CallReplPass> X("callrepl", "Call Replace Pass", true, false);

