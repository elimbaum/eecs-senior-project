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
    CallReplPass() : FunctionPass(ID) {}

    bool doInitialization(Module &M) {
      // TODO may only want to create this if any relevant functions are called
      // LLVMContext& Ctx = M.getContext();
      // create call to mmap
      // Constant * mapFunc = M.getOrInsertFunction("_create_io_map", Type::getInt32Ty(Ctx));
      // TODO save virtual pointer in global?
      // TODO if mapping goes wrong at any point, fall back to original function
      return true;
    }

    bool runOnFunction(Function &F) {
      LLVMContext& Ctx = F.getContext();

      if (F.getName() == "main") {
        errs() << "Trying to create initial map...\n";
        FunctionCallee mapFunc = F.getParent()->getOrInsertFunction("_create_io_map", Type::getInt32Ty(Ctx));
        // Make this the first line of main (first front gets BB, second gets Inst)
        IRBuilder<> builder(& F.front().front());
        builder.CreateCall(mapFunc);
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
          // need adds for array acces! NO, use GEP!

          // create a new call (can't reuse old one, because replace will mess up)
          // in real system, this will probably be a load/store pair
          Constant * io_map = F.getParent()->getOrInsertGlobal("_io_map", Type::getInt8Ty(Ctx));
          Value * io_map_d = builder.CreateBitCast(io_map, Type::getDoublePtrTy(Ctx), "_io_map_d");
          
          std::vector<Value *> idx_list(1);
          idx_list[0] = ConstantInt::get(Ctx, APInt(32, 1 /*sizeof(double)*/, true));

          Value * A = io_map_d; // builder.CreateBitCast(io_map, Type::getDoublePtrTy(Ctx));
          // Value * B = builder.CreateBitCast(
          //               builder.CreateInBoundsGEP(io_map, idx_list),
          //               Type::getDoublePtrTy(Ctx));
          Value * B = builder.CreateInBoundsGEP(Type::getDoubleTy(Ctx), io_map_d, idx_list);
          auto arg = call->arg_begin();
          builder.CreateStore(arg->get(), A);
          arg++; // next arg
          builder.CreateStore(arg->get(), B);
            
          // auto * newCall = builder.Insert(call->clone());
          // add 1
          // Value * add = builder.CreateFAdd(newCall, ConstantFP::get(Ctx, APFloat(1.0)));

          // replace old uses with new
          // for (auto &U : call->uses()) {
          //  User * user = U.getUser();
          //  user->setOperand(U.getOperandNo(), A);
          // }

          // delete old call
          // call->eraseFromParent();

          errs() << F;

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

