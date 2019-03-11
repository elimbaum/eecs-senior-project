#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
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

      for (auto &B : F) { // loop through BBs
        for (auto &I : B) {
          if (auto *op = dyn_cast<BinaryOperator>(&I)) {
            IRBuilder<> builder(op);

            Value *lhs = op->getOperand(0);
            Value *rhs = op->getOperand(1);
            Value *mul = builder.CreateFAdd(lhs, rhs);

            // replace operand
            for (auto& U : op->uses()) {
              User * user = U.getUser();
              user->setOperand(U.getOperandNo(), mul);
            }
            return true;
          }
        }
      }

      return false;
    }
  };

}

char TestPass::ID = 0;

// Example code specifies auto-registration below that I think only applies
// if I wanted to apply this pass to my compiled code, instead of the JIT.

FunctionPass * llvm::createTestPass() {
  return new TestPass();
}
