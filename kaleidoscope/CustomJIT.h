// Custom Kaleidoscope JIT
// LLVM JIT Tutorial

#ifndef LLVM_EXECUTIONENGINE_ORC_KALEIDOSCOPEJIT_H
#define LLVM_EXECUTIONENGINE_ORC_KALEIDOSCOPEJIT_H

#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace orc {

class KaleidoscopeJIT {
private:
  std::unique_ptr<TargetMachine> TM;    // build compiler instance
  const DataLayout DL;                  // symbol mangling
  RTDyldObjectLinkingLayer ObjectLayer; // on-the-fly linking
  // IR -> obj compiling
  IRCompileLayer<decltype(ObjectLayer), SimpleCompiler> CompileLayer;

public:
  using ModuleHandle = decltype(CompileLayer)::ModuleHandleT;

  KaleidoscopeJIT()
      : TM(EngineBuilder().selectTarget()), DL(TM->createDataLayout()),
        ObjectLayer([]() { return std::make_shared<SectionMemoryManager>(); }),
        CompileLayer(ObjectLayer, SimpleCompiler(*TM)) {
    llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
  }

  TargetMachine &getTargetMachine() { return * TM; }

  ModuleHandle addModule(std::unique_ptr<Module> M) {
    // build symbol resolver: look inside JIT first, then external
    auto Resolver = createLambdaResolver(
      // internal resolution rule
      [&](const std::string &Name) {
        if (auto Sym = CompileLayer.findSymbol(Name, false))
          return Sym;
        return JITSymbol(nullptr);
      },
      // external resolution rule
      [](const std::string &Name) {
        if (auto SymAddr =
              RTDyldMemoryManager::getSymbolAddressInProcess(Name))
          return JITSymbol(SymAddr, JITSymbolFlags::Exported);
        return JITSymbol(nullptr);
      });

    // Add to JIT
    return cantFail(CompileLayer.addModule(std::move(M), std::move(Resolver)));
  }

  JITSymbol findSymbol(const std::string Name) {
    std::string MangledName;
    raw_string_ostream MangledNameStream(MangledName);
    Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
    return CompileLayer.findSymbol(MangledNameStream.str(), true);
  }

  JITTargetAddress getSymbolAddress(const std::string Name) {
    return cantFail(findSymbol(Name).getAddress());
  }

  void removeModule(ModuleHandle H) {
    cantFail(CompileLayer.removeModule(H));
  }
  
};

} // end namespace orc
} // end namespace llvm

#endif // header guard

