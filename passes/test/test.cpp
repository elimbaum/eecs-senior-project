#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
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

      // errs() << "Mutating!\n";
      
      LLVMContext& Ctx = F.getContext();
      std::vector<Type *> paramTypes = {Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx)};
      Type * retType = Type::getDoubleTy(Ctx);

      FunctionType * _funcType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee _func = F.getParent()->getOrInsertFunction("_hyp", _funcType);
      
      BasicBlock * mutBB = BasicBlock::Create(Ctx, "mutate", & F);
      IRBuilder<> builder(mutBB);

      auto A = F.args().begin();
      // Eventually, this might look like a load/store
      auto * f = builder.CreateCall(_func, {A, ++A});
      auto * r = builder.CreateRet(f);

      // move the new call before the existing entry point, so that it doesn't
      // run. DCE pass will remove this code.
      mutBB->moveBefore(& (F.getEntryBlock()));

      // errs() << "New: "; F.print(llvm::errs()); errs() << "\n";
      verifyFunction(F);
      return true;
    }
  };

}

char TestPass::ID = 0;

// Example code specifies auto-registration here that I think only applies
// if I wanted to apply this pass to my compiled code, instead of the JIT.

FunctionPass * llvm::createTestPass() {
  return new TestPass();
}

// JIT can also call functions here, to possible set options etc.
extern "C" double pi(void) {
  return 3;
}

