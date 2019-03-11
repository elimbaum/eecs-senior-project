#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "test.h"

using namespace llvm;

namespace {
  struct TestPass : public FunctionPass {
    static char ID;
    TestPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) {
      if (F.getName() != "hyp")
        return false;

      errs() << "Mutating!\n";
      
      LLVMContext& Ctx = F.getContext();
      std::vector<Type *> paramTypes = {Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx)};
      Type * retType = Type::getDoubleTy(Ctx);

      FunctionType * _funcType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee _func = F.getParent()->getOrInsertFunction("_hyp", _funcType);
      
      IRBuilder<> builder(& F.front());
      auto A = F.args().begin();
      // A->print(llvm::errs());
      // builder.CreateCall(_func, {A, ++A});
      for (auto &B : F) { // loop through BBs
        for (auto &I : B) {
          if (auto *ret = dyn_cast<ReturnInst>(&I)) {
            // IRBuilder<> builder(op);

            // Value * a = op->getOperand(0);
            // Value * b = op->getOperand(1);
            // Value * args[] = {a, b};

            // builder.CreateCall(_func, args);
            // return true;
            errs() << "Found return inst: ";
            ret->print(llvm::errs());
            errs() << "\n";

            IRBuilder<> builder(ret);
            Value * f = builder.CreateCall(_func, {A, ++A});
            auto * r = builder.CreateRet(f);
            errs() << "New: "; F.print(llvm::errs()); errs() << "\n";
            ret->eraseFromParent();
            
            return true;
          }
        }
        // This BB did not contain the return.
        B.removeFromParent();
      }

      return false;
    }
  };

}

char TestPass::ID = 0;

// Example code specifies auto-registration here that I think only applies
// if I wanted to apply this pass to my compiled code, instead of the JIT.

FunctionPass * llvm::createTestPass() {
  return new TestPass();
}

extern "C" double pi(void) {
  return 3;
}

