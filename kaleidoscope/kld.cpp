#include "KaleidoscopeJIT.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;
using namespace llvm::orc;


// ============================================================
// LEXER 
// ============================================================

// return ASCII values, or token.
enum Token {
  tok_eof = -1,

  // cmd
  tok_def = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number = -5,

  tok_if = -6,
  tok_then = -7,
  tok_else = -8,
};

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
  static int LastChar = ' ';
  static int incomplete = false;
  int retval;

  // skip whitespace
  while (isspace(LastChar)) {
    if (LastChar == '\n') {
      if (incomplete)
        fprintf(stderr, "... ");
      else
        fprintf(stderr, "kld> ");
    }

    LastChar = getchar();
  }

  if (isalpha(LastChar)) { // identifier
    IdentifierStr = LastChar;
    // eat maximal string
    while (isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    if (IdentifierStr == "def")
      retval = tok_def;
    else if (IdentifierStr == "extern")
      retval = tok_extern;
    else if (IdentifierStr == "if")
      retval = tok_if;
    else if (IdentifierStr == "then")
      retval = tok_then;
    else if (IdentifierStr == "else")
      retval = tok_else;
    else
      retval = tok_identifier;
  }

  else if (isdigit(LastChar) || LastChar == '.') { // number
    std::string NumStr;
    do { 
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    retval = tok_number;
  }

  // comments
  else if (LastChar == '#') {
    do
      LastChar = getchar();
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != EOF)
      retval = gettok();
  }

  else if (LastChar == EOF)
    return tok_eof;
 
  else {
    // Return raw character
    int ThisChar = LastChar;
    LastChar = getchar();
    retval = ThisChar;
  }

  if (LastChar == '\n' && retval != ';') {
    // unterminated expression
    incomplete = true;
    // fprintf(stderr, "... ");
  } else {
    incomplete = false;
  }

  return retval;
}


// ============================================================
// AST
// ============================================================

namespace {

class ExprAST {
public:
  virtual ~ExprAST() = default;
  virtual Value * codegen() = 0;
};

class NumberExprAST : public ExprAST {
  double Val;
 
public:
  NumberExprAST(double Val) : Val(Val) {}
  Value * codegen() override;
};

class VariableExprAST : public ExprAST {
  std::string Name;

public:
  VariableExprAST(const std::string &Name) : Name(Name) {}
  Value * codegen() override;
};

class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                         std::unique_ptr<ExprAST> RHS)
    : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  Value * codegen() override;
};

class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
  Value * codegen() override;
};

class IfExprAST : public ExprAST {
  std::unique_ptr<ExprAST> Cond, Then, Else;

public:
  IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
            std::unique_ptr<ExprAST> Else)
    : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

  Value * codegen() override;
};

class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> Args)
    : Name(name), Args(std::move(Args)) {}

  Function * codegen();
  const std::string &getName() const { return Name; }
};

class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}

  Function * codegen();
};

} // end anon namespace

// Helpers

static int CurTok;
static int getNextToken() {
  return CurTok = gettok();
}

std::unique_ptr<ExprAST> LogError(const char * Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}
std::unique_ptr<PrototypeAST> LogErrorP(const char * Str) {
  LogError(Str);
  return nullptr;
}


// Parser Routines

static std::unique_ptr<ExprAST> ParseExpression();

static std::unique_ptr<ExprAST> ParseNumberExpr() {
  auto Result = llvm::make_unique<NumberExprAST>(NumVal);
  getNextToken(); // consume number
  return std::move(Result);
}

static std::unique_ptr<ExprAST> ParseParenExpr() {
  getNextToken(); // eat (
  auto V = ParseExpression();
  if (!V) return nullptr;

  if (CurTok != ')')
    return LogError("expected ')'");

  getNextToken(); // eat )
  return V;
}

static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
  std::string IdName = IdentifierStr;
  getNextToken();

  if (CurTok != '(')
    return llvm::make_unique<VariableExprAST>(IdName);

  // Call
  getNextToken();
  std::vector<std::unique_ptr<ExprAST>> Args;
  if (CurTok != ')') {
    while (1) {
      if (auto Arg = ParseExpression())
        Args.push_back(std::move(Arg));
      else
        return nullptr;

      if (CurTok == ')')
        break;

      if (CurTok != ',')
        return LogError("Expected ')' or ',' in argument list");
      getNextToken();
    }
  }

  getNextToken();

  return llvm::make_unique<CallExprAST>(IdName, std::move(Args));
}

static std::unique_ptr<ExprAST> ParseIfExpr() {
  getNextToken();

  auto Cond = ParseExpression();
  if (! Cond)
    return nullptr;

  if (CurTok != tok_then)
    return LogError("expected then");
  getNextToken(); // eat then

  auto Then = ParseExpression();
  if (! Then)
    return nullptr;

  if (CurTok != tok_else)
    return LogError("expected else");

  getNextToken();

  auto Else = ParseExpression();
  if (!Else)
    return nullptr;

  return llvm::make_unique<IfExprAST>(std::move(Cond), std::move(Then),
                                      std::move(Else));
}

// primary
static std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
  default:
    return LogError("unknown token when expecting an expression");
  case tok_identifier:
    return ParseIdentifierExpr();
  case tok_number:
    return ParseNumberExpr();
  case '(':
    return ParseParenExpr();
  case tok_if:
    return ParseIfExpr();
  }
}

// Binary operator parsinG
static std::map<char, int> BinopPrecedence;

static int GetTokPrecedence() {
  if (!isascii(CurTok))
    return -1;

  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0) return -1;
  return TokPrec;
}

static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                              std::unique_ptr<ExprAST> LHS) {
  while(true) {
    int TokPrec = GetTokPrecedence();

    // precedence too low -- stop
    if (TokPrec < ExprPrec)
      return LHS;

    int BinOp = CurTok;
    getNextToken(); // eat operator

    auto RHS = ParsePrimary();
    if (!RHS)
      return nullptr;

    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec) {
      RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
      if (!RHS)
        return nullptr;
    }

    // merge
    LHS = llvm::make_unique<BinaryExprAST>(BinOp, std::move(LHS),
                                                  std::move(RHS));
  }
}

static std::unique_ptr<ExprAST> ParseExpression() {
  auto LHS = ParsePrimary();
  if (!LHS)
    return nullptr;

  return ParseBinOpRHS(0, std::move(LHS));
}

// function prototypes
static std::unique_ptr<PrototypeAST> ParsePrototype() {
  if (CurTok != tok_identifier)
    return LogErrorP("Expected function name in prototype");

  std::string FnName = IdentifierStr;
  getNextToken();

  if (CurTok != '(')
    return LogErrorP("Expected '(' in prototype");

  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_identifier)
    ArgNames.push_back(IdentifierStr);
  if (CurTok != ')')
    return LogErrorP("Expected ')' in prototype");

  // success
  getNextToken(); // eat ')' at end of prototype
  return llvm::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

static std::unique_ptr<FunctionAST> ParseDefinition() {
  getNextToken(); // eat def
  auto Proto = ParsePrototype();
  if (!Proto) return nullptr;

  if (auto E = ParseExpression())
    return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  return nullptr;
}

// extern
static std::unique_ptr<PrototypeAST> ParseExtern() {
  getNextToken();
  return ParsePrototype();
}

// top level expr
static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    // anonymous proto
    auto Proto = llvm::make_unique<PrototypeAST>("__anon_expr",
                                                 std::vector<std::string>());
    return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }
  return nullptr;
}

// ===================================
// Code gen
// ===================================

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;
static std::unique_ptr<KaleidoscopeJIT> TheJIT;
static std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

Value *LogErrorV(const char * Str) {
  LogError(Str);
  return nullptr;
}

Function * getFunction(std::string Name) {
  // has it already been added to the current module?
  // maybe, change the order of these. see which of these is being run when.
  if (auto *F = TheModule->getFunction(Name)) {
    return F;
  }

  // if not, look for existing prototype
  auto FI = FunctionProtos.find(Name);
  if (FI != FunctionProtos.end()) {
    return FI->second->codegen();
  }

  return nullptr;
}

Value * NumberExprAST::codegen() {
  return ConstantFP::get(TheContext, APFloat(Val));
}

Value * VariableExprAST::codegen() {
  // Check if this variable exists
  Value * V = NamedValues[Name];
  if (!V)
    LogErrorV("Unknown variable name");
  return V;
}

Value * BinaryExprAST::codegen() {
  Value * L = LHS->codegen();
  Value * R = RHS->codegen();
  if (!L || !R)
    return nullptr;

  switch (Op) {
  case '+':
    return Builder.CreateFAdd(L, R, "addtmp");
  case '-':
    return Builder.CreateFSub(L, R, "subtmp");
  case '*':
    return Builder.CreateFMul(L, R, "multmp");
  case '<':
    L =  Builder.CreateFCmpULT(L, R, "cmptmp");
    // convert bool to double (lang only supports double)
    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext),
                                "booltmp");
  default:
    return LogErrorV("invalid binary operator");
  }
}

Value * CallExprAST::codegen() {
  Function * CalleeF = getFunction(Callee);
  if (!CalleeF)
    return LogErrorV("Unknown function referened");

  // check for arg mismatch
  if (CalleeF -> arg_size() != Args.size())
    return LogErrorV("Incorrect # arguments passed");

  std::vector<Value *> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i) {
    ArgsV.push_back(Args[i]->codegen());
    if (!ArgsV.back())
      return nullptr;
  }

  return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

Value * IfExprAST::codegen() {
  Value * CondV = Cond->codegen();
  if (!CondV)
    return nullptr;

  // bool to double
  CondV = Builder.CreateFCmpONE(
            CondV, ConstantFP::get(TheContext, APFloat(0.0)), "ifcond");

  Function * TheFunction = Builder.GetInsertBlock()->getParent();

  BasicBlock *ThenBB = BasicBlock::Create(TheContext, "then", TheFunction);
  BasicBlock *ElseBB = BasicBlock::Create(TheContext, "else");
  BasicBlock *MergeBB = BasicBlock::Create(TheContext, "ifcont");

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);

  Builder.SetInsertPoint(ThenBB);

  Value *ThenV = Then->codegen();
  if (!ThenV)
    return nullptr;

  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();

  // else block
  TheFunction->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);

  Value * ElseV = Else->codegen();
  if (!ElseV)
    return nullptr;

  Builder.CreateBr(MergeBB);
  ElseBB = Builder.GetInsertBlock();

  // Merge then/else
  TheFunction->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);
  PHINode * PN = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");
  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);

  return PN;
}


Function * PrototypeAST::codegen() {
  std::vector<Type *> Doubles(Args.size(),
                              Type::getDoubleTy(TheContext));
  FunctionType * FT =
    FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);

  Function * F = 
    Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  return F;
}

Function * FunctionAST::codegen() {
  auto &P = *Proto;
  FunctionProtos[Proto->getName()] = std::move(Proto);
  Function * TheFunction = getFunction(P.getName());
  if (!TheFunction)
    return nullptr;

  // create basic block for body
  BasicBlock * BB = BasicBlock::Create(TheContext, "entry", TheFunction);
  Builder.SetInsertPoint(BB);

  NamedValues.clear();
  for (auto &Arg : TheFunction->args())
    NamedValues[Arg.getName()] = &Arg;

  if (Value *RetVal = Body->codegen()) {
    Builder.CreateRet(RetVal);

    verifyFunction(*TheFunction);

    TheFPM->run(*TheFunction); // optimize!

    return TheFunction;
  }

  // error
  TheFunction->eraseFromParent();
  return nullptr;
}

// ===================================
// Top level parsing & JIT driver
// ===================================

void InitializeModuleAndPassManager(void) {
  // Open module
  TheModule = llvm::make_unique<Module>("kld_jit", TheContext);
  TheModule->setDataLayout(TheJIT->getTargetMachine().createDataLayout());
  
  // Create pass manager
  TheFPM = llvm::make_unique<legacy::FunctionPassManager>(TheModule.get());
  TheFPM->add(createInstructionCombiningPass());
  TheFPM->add(createReassociatePass());
  TheFPM->add(createGVNPass()); // common subexpr
  TheFPM->add(createCFGSimplificationPass());

  TheFPM->doInitialization();
}

static void HandleDefinition() {
  if (auto FnAST = ParseDefinition()) {
    if (auto * FnIR = FnAST->codegen()) {
      fprintf(stderr, "Read function definition:");

      FnIR->print(errs());
      fprintf(stderr, "\n");
      // add to JIT
      TheJIT->addModule(std::move(TheModule));
      InitializeModuleAndPassManager();
    }
  } else
    getNextToken(); // error, skip
}

static void HandleExtern() {
  if (auto ProtoAST = ParseExtern()) {
    if (auto * FnIR = ProtoAST->codegen()) {
      fprintf(stderr, "Read extern:");
      FnIR->print(errs());
      fprintf(stderr, "\n");
      FunctionProtos[ProtoAST->getName()] = std::move(ProtoAST);
    }
  } else
    getNextToken();
}

static void HandleTopLevelExpression() {
  if (auto FnAST = ParseTopLevelExpr()) {
    if (auto *FnIR = FnAST->codegen()) {
      // create anon func for eval
      auto H = TheJIT->addModule(std::move(TheModule));
      InitializeModuleAndPassManager();

      auto ExprSymbol = TheJIT->findSymbol("__anon_expr");
      assert(ExprSymbol && "Function not found");

      // cast to double-returning function
      double (*FP)() = (double (*)())(intptr_t)cantFail(ExprSymbol.getAddress());
      fprintf(stderr, "Evaluated to %f\n", FP());

      // remove anon func
      TheJIT->removeModule(H);
    }
  } else
    getNextToken();
}


static void MainLoop() {
  while (true) {
    // TODO fix space-separated TLExpr

    switch (CurTok) {
    case tok_eof:
      fprintf(stderr, "\n");
      return;
    case ';': // ignore top-level semicolons
      getNextToken();
      break;
    case tok_def:
      HandleDefinition();
      break;
    case tok_extern:
      HandleExtern();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT double putchard(double X) {
  fputc((char)X, stderr);
  return 0;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" DLLEXPORT double printd(double X) {
  fprintf(stderr, "%f\n", X);
  return 0;
}

int main() {
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;

  fprintf(stderr, "kaleidoscope interpreter\n");
  fprintf(stderr, "kld> ");
  getNextToken();

  TheJIT = llvm::make_unique<KaleidoscopeJIT>();
  InitializeModuleAndPassManager();

  MainLoop();

  // print generated code
  // TheModule->print(errs(), nullptr);

  return 0;
}
