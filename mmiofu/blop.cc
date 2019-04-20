#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>

enum class RetTy {
  VOID,
  DOUBLE,
  INT
};

enum class Arg {
  N,
  ALPHA,
  X,
  Y
};

// Need common function signature so call can work. But some arguments are
// ignored by different functions.
typedef double (*cblasfunc_t)(int N, double alpha, double * X, double * Y, int * latency);

class BlasOperation {
  private:
    // static fields
    static std::map<std::string, BlasOperation*> NameLU;
    static std::map<int,         BlasOperation*> IdLU;
    
    // member fields
    std::string cblas_name;
    cblasfunc_t func;

    RetTy ret;
    std::vector<Arg> argv;
  
  public:
    static BlasOperation* get(int index) {
      auto it = IdLU.find(index);
      if (it == IdLU.end()) {
        return nullptr;
      }
      return it->second;
    }

    static BlasOperation* find(std::string name) {
      auto it = NameLU.find(name);
      if (it == NameLU.end()) {
        return nullptr;
      }
      return it->second;
    }

    BlasOperation(std::string name, cblasfunc_t func, int index, RetTy ret, std::vector<Arg> argv) :
      cblas_name(name), func(func), ret(ret), argv(std::move(argv))
    {
      NameLU[name] = this;
      IdLU[index] = this;
    }

};
