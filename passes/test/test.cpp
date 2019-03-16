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
      errs() << "Running on " << F.getName() << "\n";
      if (F.getName() != "cblas_ddot")
        return false;

      errs() << "Mutating!\n";
      
      LLVMContext& Ctx = F.getContext();
      std::vector<Type *> paramTypes = {Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx)};
      Type * retType = Type::getDoubleTy(Ctx);

      FunctionType * _funcType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee _func = F.getParent()->getOrInsertFunction("_dot", _funcType);
      
      BasicBlock * mutBB = BasicBlock::Create(Ctx, "mutate", & F);
      IRBuilder<> builder(mutBB);

      auto A = F.args().begin();
      // Eventually, this might look like a load/store
      auto * f = builder.CreateCall(_func, {A, ++A});
      auto * r = builder.CreateRet(f);

      // move the new call before the existing entry point, so that it doesn't
      // run. dead code elim pass will remove this code.
      mutBB->moveBefore(& (F.getEntryBlock()));

      errs() << "New: "; F.print(llvm::errs()); errs() << "\n";
      verifyFunction(F);
      return true;
    }
  };

}

char TestPass::ID = 0;
static RegisterPass<TestPass> X("test", "Test Pass", true, false);

// auto registration
// static void registerTestPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
//   PM.add(new TestPass());
// }
// 
// static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerTestPass);

// for custom JIT Pass Manager
FunctionPass * llvm::createTestPass() {
  return new TestPass();
}

// JIT can also call functions here, to possibly set options etc.
extern "C" double pi(void) {
  return 3;
}

