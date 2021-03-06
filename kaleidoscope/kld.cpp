#include "KaleidoscopeJIT.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Statistic.h"
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
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/Analysis/Passes.h"
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

#include "../passes/test/test.h"

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

  // flow control
  tok_if = -6, tok_then = -7, tok_else = -8,
  tok_for = -9, tok_in = -10,

  // operators
  tok_binary = -11, tok_unary = -12,

  // var definition
  tok_var = -13,

  tok_import = -14
};

// Reads charcters from source.
class Reader {
  std::vector<FILE *> sources;

public:
  char getchar() {
    if (sources.size() == 0)
      return EOF;

    return getc(sources.back());
  }

  void pushsource(FILE * _source) {
    sources.push_back(_source);
  }

  void popsource() {
    sources.pop_back();
  }

  bool isStdin() {
    return sources.back() == stdin;
  }
};

Reader reader;

bool DoReplacement = false;
bool DoIRPrint = false;

static std::string IdentifierStr;
static double NumVal;

static bool isident(char c) {
  return isalnum(c) || c == '_';
}

static int gettok() {
  static int LastChar = ' ';
  static int incomplete = false;
  int retval = 0;

  // skip whitespace
  while (isspace(LastChar)) {
    // No prompts if reading from file.
    if (LastChar == '\n' && reader.isStdin()) {
      if (incomplete)
        fprintf(stderr, ".... ");
      else
        fprintf(stderr, "kld> ");
    }

    LastChar = reader.getchar();
  }

  if (isalpha(LastChar) || LastChar == '_') { // identifier
    IdentifierStr = LastChar;
    // eat maximal string
    while (isident((LastChar = reader.getchar())))
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
    else if (IdentifierStr == "for")
      retval = tok_for;
    else if (IdentifierStr == "in")
      retval = tok_in;
    else if (IdentifierStr == "binary")
      retval = tok_binary;
    else if (IdentifierStr == "unary")
      retval = tok_unary;
    else if (IdentifierStr == "var")
      retval = tok_var;
    else if (IdentifierStr == "import")
      retval = tok_import;
    else
      retval = tok_identifier;
  }

  else if (isdigit(LastChar) || LastChar == '.') 
  { // number
    std::string NumStr;
    do { 
      NumStr += LastChar;
      LastChar = reader.getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    // optional exponent
    // buggy but it works
    if (LastChar == 'e') {
      std::string exp = "e";
      LastChar = reader.getchar();
      if (LastChar == '-' || LastChar == '+') {
        exp += LastChar;
        LastChar = reader.getchar();
      }

      while (isdigit(LastChar)) {
        exp += LastChar;
        LastChar = reader.getchar();
      }

      NumStr += exp;
    }

    NumVal = strtod(NumStr.c_str(), 0);
    retval = tok_number;
  }

  // comments
  else if (LastChar == '#') {
    do
      LastChar = reader.getchar();
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != EOF)
      retval = gettok();
  }

  else if (LastChar == EOF) {
    // might have another file coming, reset
    LastChar = '\n';
    return tok_eof;
  }

  else {
    // Return raw character
    int ThisChar = LastChar;
    LastChar = reader.getchar();
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
  const std::string &getName() const { return Name; }
};

// var/in allows a list of names to be declared at once
class VarExprAST : public ExprAST {
  std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames;
  std::unique_ptr<ExprAST> Body;

public:
  VarExprAST(std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames,
             std::unique_ptr<ExprAST> Body)
    : VarNames(std::move(VarNames)), Body(std::move(Body)) {}

  Value * codegen() override;
};

class VectorExprAST : public ExprAST {
  std::vector<std::unique_ptr<ExprAST>> Elements;

public:
  VectorExprAST(std::vector<std::unique_ptr<ExprAST>> Elements)
    : Elements(std::move(Elements)) {}

  Value * codegen() override;
};

// Load library
class ImportExprAST : public ExprAST {
  std::vector<std::string> Files;

public:
  ImportExprAST(std::vector<std::string> Files) : Files(std::move(Files)) {}

  Value * codegen() override;
};

class UnaryExprAST : public ExprAST {
  char Opcode;
  std::unique_ptr<ExprAST> Operand;

public:
  UnaryExprAST(char Opcode, std::unique_ptr<ExprAST> Operand)
    : Opcode(Opcode), Operand(std::move(Operand)) {}

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

class ForExprAST : public ExprAST {
  std::string VarName;
  std::unique_ptr<ExprAST> Start, End, Step, Body;

public:
  ForExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Start,
              std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
              std::unique_ptr<ExprAST> Body)
    : VarName(VarName), Start(std::move(Start)), End(std::move(End)),
      Step(std::move(Step)), Body(std::move(Body)) {}

  Value * codegen() override;
};

class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;
  bool IsOperator;
  unsigned Precedence; // only if binary

public:
  PrototypeAST(const std::string &name, std::vector<std::string> Args,
               bool IsOperator = false, unsigned Prec = 0)
    : Name(name), Args(std::move(Args)), IsOperator(IsOperator),
      Precedence(Prec) {}

  Function * codegen();
  const std::string &getName() const { return Name; }

  bool isUnaryOp() const { return IsOperator && Args.size() == 1; }
  bool isBinaryOp() const { return IsOperator && Args.size() == 2; }

  char getOperatorName() const {
    assert(isUnaryOp() || isBinaryOp());
    return Name[Name.size() - 1];
  }

  unsigned getBinaryPrecedence() const { return Precedence; }
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

static std::unique_ptr<ExprAST> ParseVectorExpr() {
  getNextToken(); // eat [

  std::vector<std::unique_ptr<ExprAST>> Elements;

  while (CurTok != ']') {
    auto V = ParseExpression();
    if (!V)
      return LogError("expected expression in list");

    Elements.push_back(std::move(V));
  }

  getNextToken(); // eat ]
  return llvm::make_unique<VectorExprAST>(std::move(Elements));
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

static std::unique_ptr<ExprAST> ParseForExpr() {
  getNextToken(); // eat for

  if (CurTok != tok_identifier)
    return LogError("expected identifier after for");

  std::string IdName = IdentifierStr;
  getNextToken(); // eat id

  if (CurTok != '=')
    return LogError("expected '=' after for");
  getNextToken(); // eat =

  auto Start = ParseExpression();
  if (! Start)
    return nullptr;

  if (CurTok != ',')
    return LogError("expected ',' after start");
  getNextToken();

  auto End = ParseExpression();
  if (! End)
    return nullptr;

  std::unique_ptr<ExprAST> Step;
  if (CurTok == ',') { // step is optional
    getNextToken();
    Step = ParseExpression();
    if (!Step)
      return nullptr;
  }

  if (CurTok != tok_in)
    return LogError("expected 'in' after for");
  getNextToken(); // eat in

  auto Body = ParseExpression();
  if (!Body)
    return nullptr;

  return llvm::make_unique<ForExprAST>(IdName, std::move(Start), std::move(End),
                                       std::move(Step), std::move(Body));
}

static std::unique_ptr<ExprAST> ParseVarExpr() {
  getNextToken(); // eat 'var'

  std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames;

  // at least 1 var required
  if (CurTok != tok_identifier)
    return LogError("expected identifier after var");

  while (true) {
    std::string Name = IdentifierStr;
    getNextToken();

    // Read optional initializer
    std::unique_ptr<ExprAST> Init;
    if (CurTok == '=') {
      getNextToken();

      Init = ParseExpression();
      if (! Init) return nullptr;
    }

    VarNames.push_back(std::make_pair(Name, std::move(Init)));

    // end of list
    if (CurTok != ',') break;
    getNextToken();

    if (CurTok != tok_identifier)
      return LogError("expected identifier list after var");
  }

  // All variables are parsed!
  
  if (CurTok != tok_in)
    return LogError("expected 'in' keyword after 'var'");
  getNextToken();

  auto Body = ParseExpression();
  if (!Body) return nullptr;

  return llvm::make_unique<VarExprAST>(std::move(VarNames), std::move(Body));
}

// import file1 [, file2 [, ...]]
// Syntax similar to var expr
static std::unique_ptr<ExprAST> ParseImportExpr() {
  getNextToken(); // eat 'import'

  std::vector<std::string> Files;

  // at least one file required
  if (CurTok != tok_identifier)
    return LogError("expected filename after import");

  while (true) {
    // unfortunately no whitespace/special character filenames are supported
    std::string File = IdentifierStr;
    getNextToken();

    Files.push_back(File);

    // list?
    if (CurTok != ',') break;
    getNextToken();

    if (CurTok != tok_identifier)
      return LogError("expected filename list after import");
  }

  // All filenames parsed!
  return llvm::make_unique<ImportExprAST>(std::move(Files));
}

// primary
static std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
  default:
    fprintf(stderr, "Bad token: ");
    if (CurTok > ' ')
      fprintf(stderr, "%c\n", CurTok);
    else
      fprintf(stderr, "%d\n", CurTok);

    return LogError("unknown token when expecting an expression");
  case tok_identifier:
    return ParseIdentifierExpr();
  case tok_number:
    return ParseNumberExpr();
  case '(':
    return ParseParenExpr();
  case '[':
    return ParseVectorExpr();
  case tok_if:
    return ParseIfExpr();
  case tok_for:
    return ParseForExpr();
  case tok_var:
    return ParseVarExpr();
  case tok_import:
    return ParseImportExpr();
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

static std::unique_ptr<ExprAST> ParseUnary() {
  if (!isascii(CurTok) || CurTok == '[' || CurTok == '(' || CurTok == ',')
    return ParsePrimary();

  int Opc = CurTok;
  getNextToken();
  if (auto Operand = ParseUnary())
    return llvm::make_unique<UnaryExprAST>(Opc, std::move(Operand));
  return nullptr;
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

    auto RHS = ParseUnary();
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
  auto LHS = ParseUnary();
  if (!LHS)
    return nullptr;

  return ParseBinOpRHS(0, std::move(LHS));
}


// function prototypes
static std::unique_ptr<PrototypeAST> ParsePrototype() {
  std::string FnName;

  unsigned Kind = 0; // 0 id, 1 un, 2 bi
  unsigned BinaryPrecendence = 30;

  switch (CurTok) {
  default:
    return LogErrorP("Expected function name in prototype");
  case tok_identifier:
    FnName = IdentifierStr;
    Kind = 0;
    getNextToken();
    break;
  case tok_unary:
    getNextToken();
    if (!isascii(CurTok))
      return LogErrorP("Expected unary operator");
    FnName = "unary";
    FnName += (char)CurTok;
    Kind = 1;
    getNextToken();
    break;

  case tok_binary:
    getNextToken();
    if (!isascii(CurTok))
      return LogErrorP("Expected binary operator");

    FnName = "binary";
    FnName += (char)CurTok;
    Kind = 2;
    getNextToken();

    // Read prec
    if (CurTok == tok_number) {
      if (NumVal < 1 || NumVal > 100)
        return LogErrorP("Invalid precedence: must be 1..100");
      BinaryPrecendence = (unsigned)NumVal;
      getNextToken();
    }
    break;
  }

  if (CurTok != '(')
    return LogErrorP("Expected '(' in prototype");

  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_identifier)
    ArgNames.push_back(IdentifierStr);
  if (CurTok != ')')
    return LogErrorP("Expected ')' in prototype");

  getNextToken(); // eat ')'

  if (Kind && ArgNames.size() != Kind)
    return LogErrorP("Invalid number of operands for operator");

  return llvm::make_unique<PrototypeAST>(FnName, std::move(ArgNames), Kind != 0,
                                         BinaryPrecendence);
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
static std::map<std::string, AllocaInst *> NamedValues;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;
static std::unique_ptr<KaleidoscopeJIT> TheJIT;
static std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;

Value *LogErrorV(const char * Str) {
  LogError(Str);
  return nullptr;
}

// Create alloca instruction for the function (create mutable var)
static AllocaInst * CreateEntryBlockAlloca(Function * TheFunction,
                                           const std::string &VarName) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::getDoubleTy(TheContext), 0, VarName.c_str());
}

Function * getFunction(std::string Name) {
  // has it already been added to the current module?
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
    return LogErrorV("Unknown variable name");

  // load value off stack
  return Builder.CreateLoad(V, Name.c_str());
}

Value * VarExprAST::codegen() {
  std::vector<AllocaInst *> OldBindings;

  Function * TheFunction = Builder.GetInsertBlock()->getParent();
  
  for (unsigned i = 0, e = VarNames.size(); i != e; ++i) {
    const std::string &VarName = VarNames[i].first;
    ExprAST * Init = VarNames[i].second.get();

    // Emit initializer
    Value * InitVal;
    if (Init) {
      InitVal = Init->codegen();
      if (!InitVal)
        return nullptr;
    } else { // not specified, default 0
      InitVal = ConstantFP::get(TheContext, APFloat(0.0));
    }

    AllocaInst * Alloca = CreateEntryBlockAlloca(TheFunction, VarName);
    Builder.CreateStore(InitVal, Alloca);

    // Remember the old variable, if it already existed
    OldBindings.push_back(NamedValues[VarName]);

    // Update new
    NamedValues[VarName] = Alloca;
  }

  Value * BodyVal = Body->codegen();
  if (!BodyVal) return nullptr;

  // Restore
  for (unsigned i = 0, e = VarNames.size(); i != e; ++i)
    NamedValues[VarNames[i].first] = OldBindings[i];

  return BodyVal;
}

Value * VectorExprAST::codegen() {
  if (Elements.size() > 0) {
    return Elements.back()->codegen();
  } else {
    return nullptr;
  }
}

static void MainLoop(); 

// JIT handles import.
Value * ImportExprAST::codegen() {
  for (unsigned i = 0, e = Files.size(); i != e; ++i) {
    FILE * f = fopen((Files[i] + ".kld").c_str(), "r");
    if (!f) {
      perror("import");
      return LogErrorV("Could not open file");
    }

    reader.pushsource(f);
  }

  // Don't actually generate any IR
  return nullptr;
}

Value * UnaryExprAST::codegen() {
  Value * OperandV = Operand->codegen();
  if (!OperandV)
    return nullptr;

  Function * F = getFunction(std::string("unary") + Opcode);
  if (!F) {
    std::string err = "Unknown unary operator: ";
    err += Opcode;
    return LogErrorV(err.c_str());
  }
  return Builder.CreateCall(F, OperandV, "unop");
}

Value * BinaryExprAST::codegen() {
  // Special case for assignment
  if (Op == '=') {
    VariableExprAST * LHSE = static_cast<VariableExprAST*>(LHS.get());
    if (!LHSE)
      return LogErrorV("destination of '=' must be a variable");

    Value * Val = RHS->codegen();
    if (! Val)
      return nullptr;

    Value * Variable = NamedValues[LHSE->getName()];
    if (! Variable)
      return LogErrorV("Unknown variable name");

    Builder.CreateStore(Val, Variable);
    return Val; // return value to allow chained assignments!
  }

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
  case '/':
    return Builder.CreateFDiv(L, R, "divtmp");
  case '<':
    L =  Builder.CreateFCmpULT(L, R, "cmptmp");
    // convert bool to double (lang only supports double)
    return Builder.CreateUIToFP(L, Type::getDoubleTy(TheContext),
                                "booltmp");
  default:
    break;
  }

  // user defined
  Function * F = getFunction(std::string("binary") + Op);
  assert(F && "binary operator not found!");

  Value * Ops[2] = { L, R };
  return Builder.CreateCall(F, Ops, "binop");
}

void InitializeModuleAndPassManager();

Value * CallExprAST::codegen() {
  Function * CalleeF = getFunction(Callee);
  if (!CalleeF)
    return LogErrorV("Unknown function referenced");

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

Value * ForExprAST::codegen() {
  // basic block
  Function *TheFunction = Builder.GetInsertBlock()->getParent();

  // Allocate loop variable
  AllocaInst * Alloca = CreateEntryBlockAlloca(TheFunction, VarName);

  Value * StartVal = Start->codegen();
  if (!StartVal)
    return nullptr;

  Builder.CreateStore(StartVal, Alloca);
  BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);

  // fall through into loop
  Builder.CreateBr(LoopBB);

  Builder.SetInsertPoint(LoopBB);

  // save old variable name, to be restored later
  // (variable may exist in other scope)
  AllocaInst * OldVal = NamedValues[VarName];
  NamedValues[VarName] = Alloca; // add loop var to symbol table

  if (!Body->codegen())
    return nullptr;

  Value *StepVal = nullptr;
  if (Step) {
    StepVal = Step->codegen();
    if (!StepVal)
      return nullptr;
  } else {
    // unspecified, default 1
    StepVal = ConstantFP::get(TheContext, APFloat(1.0));
  }

  // end condition
  Value *EndCond = End->codegen();
  if (! EndCond)
    return nullptr;

  // increment
  Value * CurVar = Builder.CreateLoad(Alloca, VarName.c_str());
  Value * NextVar = Builder.CreateFAdd(CurVar, StepVal, "nextvar");
  Builder.CreateStore(NextVar, Alloca);

  EndCond = Builder.CreateFCmpONE(
              EndCond, ConstantFP::get(TheContext, APFloat(0.0)), "loopcond");

  BasicBlock *AfterBB =
    BasicBlock::Create(TheContext, "afterloop", TheFunction);

  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
  Builder.SetInsertPoint(AfterBB);

  if (OldVal)
    NamedValues[VarName] = OldVal;
  else
    NamedValues.erase(VarName);

  return Constant::getNullValue(Type::getDoubleTy(TheContext));
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

  // operator?
  if (P.isBinaryOp())
    BinopPrecedence[P.getOperatorName()] = P.getBinaryPrecedence();

  // create basic block for body
  BasicBlock * BB = BasicBlock::Create(TheContext, "entry", TheFunction);
  Builder.SetInsertPoint(BB);

  NamedValues.clear();
  for (auto &Arg : TheFunction->args()) {
    AllocaInst * Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName());
    Builder.CreateStore(&Arg, Alloca);
    NamedValues[Arg.getName()] = Alloca;
  }

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
  // TheFPM->add(createPromoteMemoryToRegisterPass()); // mem2reg
  if (DoReplacement)
    TheFPM->add(createTestPass());

  TheFPM->add(createInstructionCombiningPass());
  TheFPM->add(createReassociatePass());
  TheFPM->add(createGVNPass()); // common subexpr
  TheFPM->add(createCFGSimplificationPass());
  if (DoIRPrint)
    TheFPM->add(createPrintFunctionPass(llvm::errs()));

  // Not sure how to get this to work (outside of opt)
  TheFPM->add(createInstCountPass());

  TheFPM->doInitialization();
}

static void HandleDefinition() {
  if (auto FnAST = ParseDefinition()) {
    if (auto * FnIR = FnAST->codegen()) {
      // fprintf(stderr, "Read function definition:");
      // FnIR->print(errs());
      // fprintf(stderr, "\n");

      // add to JIT
      // I think to allow function update, we need to save the VModuleKey
      // returned here, and then (if a function of that name already exists)
      // first remove the old module, then add the new one.
      TheJIT->addModule(std::move(TheModule));
      InitializeModuleAndPassManager();
    }
  } else
    getNextToken(); // error, skip
}

static void HandleExtern() {
  if (auto ProtoAST = ParseExtern()) {
    if (auto * FnIR = ProtoAST->codegen()) {
      //fprintf(stderr, "Read extern:");
      // FnIR->print(errs());
      // fprintf(stderr, "\n");
      
      FunctionProtos[ProtoAST->getName()] = std::move(ProtoAST);
    }
  } else
    getNextToken();
}

static void HandleTopLevelExpression() {
  if (auto FnAST = ParseTopLevelExpr()) {
    if (auto * FnIR = FnAST->codegen()) {
      // print IR
      // FnIR->print(errs());
      // fprintf(stderr, "\n");

      // create anon func for eval
      auto H = TheJIT->addModule(std::move(TheModule));
      InitializeModuleAndPassManager();

      auto ExprSymbol = TheJIT->findSymbol("__anon_expr");
      assert(ExprSymbol && "Function not found");

      // cast to double-returning function
      // double (*FP)() = (double (*)())(intptr_t)cantFail(ExprSymbol.getAddress());
      
      double (*FP)() = nullptr;
      if (auto ExprAddr = ExprSymbol.getAddress()) {
        FP = (double(*)())*ExprAddr;
        fprintf(stderr, "Evaluated to %g\n", FP());
      } else {
        logAllUnhandledErrors(ExprAddr.takeError(), llvm::errs(), "kaleidoscope error: ");
        exit(1);
      }

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
      if (reader.isStdin()) {
        fprintf(stderr, "\n");
        return;
      }
      else {
        reader.popsource();
        getNextToken();
        break;
      }
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
    // Doesn't work
    // PrintStatistics();
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
  return X;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" DLLEXPORT double printd(double X) {
  fprintf(stderr, "%f\n", X);
  return X;
}

int main(int argc, char ** argv) {
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  TheJIT = llvm::make_unique<KaleidoscopeJIT>();
  InitializeModuleAndPassManager();

  BinopPrecedence['='] = 2;
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;
  BinopPrecedence['/'] = 40;

  reader.pushsource(stdin);

  fprintf(stderr, "kaleidoscope interpreter\n");

  if (argc > 1) {
    if (strchr(argv[1], 'r')) {
      fprintf(stderr, "Running with FU replacements!\n");
      DoReplacement = true;
    }
    if (strchr(argv[1], 'p')) {
      fprintf(stderr, "Printing IR!\n");
      DoIRPrint = true;
    }
  }

  fprintf(stderr, "kld> ");
  getNextToken();

  MainLoop();

  // print generated code
  // TheModule->print(errs(), nullptr);

  return 0;
}
