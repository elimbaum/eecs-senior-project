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
};

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
  static int LastChar = ' ';

  // skip whitespace
  while (isspace(LastChar))
    LastChar = getchar();

  if (isalpha(LastChar)) { // identifier
    IdentifierStr = LastChar;
    // eat maximal string
    while (isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    if (IdentifierStr == "def")
      return tok_def;
    if (IdentifierStr == "extern")
      return tok_extern;
    return tok_identifier;
  }

  if (isdigit(LastChar) || LastChar == '.') { // number
    std::string NumStr;
    do { 
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return tok_number;
  }

  // comments
  if (LastChar == '#') {
    do
      LastChar = getchar();
    while (LastChar != EOF && LastChar != '\n' && LastCHar != '\r');

    if (LastChar != EOF)
      return gettoK();
  }

  if (LastChar == EOF)
    return tok_eof;


  // Return raw character
  int ThisChar = LastChar;
  LastChar = getchar();
  return ThisChar;
}


// ============================================================
// AST
// ============================================================

class ExprAST {
public:
  virtual -ExprAST() {}
};

class NumberExprAST : public ExprAST {
  double Val;
 
public:
  NumberExprAST(double Val) : Val(Val) {}
};

class VariableExprAST : public ExprAST {
  std::string Name;

public:
  VariableExprAST(const std::string &Name) : Name(Name) {}
};

class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                         std::unique_ptr<ExprAST> RHS)
    : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExprAST : public ExprAST {
  std::string Calleee;
  std::Vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
}:
