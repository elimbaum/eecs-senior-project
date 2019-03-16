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

using namespace llvm;

namespace {
  struct CallReplPass : public FunctionPass {
    static char ID;
    CallReplPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) {
      LLVMContext& Ctx = F.getContext();

      for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        if (auto * call = dyn_cast<CallInst>(&*I)) {
          // only look at cblas functions
          // if (! call->getCalledFunction()->getName().startswith("cblas_"))
          //  continue;

          // test with hypot
          auto * oldF = call->getCalledFunction();
          if (oldF->getName() != "hypot") continue;

          IRBuilder<> builder(call);
          builder.SetInsertPoint(call->getParent(), ++builder.GetInsertPoint());

          // create a new call (can't reuse old one, because replace will mess up)
          auto * newCall = builder.Insert(call->clone());
          // add 1
          Value * add = builder.CreateFAdd(newCall, ConstantFP::get(Ctx, APFloat(1.0)));

          // replace old uses with new
          for (auto &U : call->uses()) {
           User * user = U.getUser();
           user->setOperand(U.getOperandNo(), add);
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

