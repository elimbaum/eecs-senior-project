#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#ifndef _BLAS_OP
#define _BLAS_OP

#define BASE_ADDR 0xFFFEF000
#define PAGE_LEN  0x1000

#define IDX_FUNCTION 0
#define IDX_ALPHA    1
#define IDX_N        2
#define IDX_START_X  3

// How many iterations to run the Babylonian square root algorithm for.
//
// In general, this is NOT a constant! What works for small inputs will be off
// for large inputs. However, by using the floating point estimate of the
// square root (half exponent), we get an incredibly good estimate which
// converges very quickly, and uniformly so. From my tests, 5 iterations
// appears to be optimal; after that, the relative accuracy levels off.
#define SQRT_ITERS 5

// TODO eventually, this shouldn't be hardcoded
#define INC_X 1
#define INC_Y 1

// Be careful using sizeof(double), since host and sim machine may differ
// However, on a non-simulated system, this wouldn't be an issue.
#define GET_ADDR(idx) (BASE_ADDR + idx * sizeof(double))
#define GET_INDEX(addr) ((addr - BASE_ADDR) / sizeof(double))

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

double _dscal (int N, double alpha, double * X, double * Y, int * latency);
double _daxpy (int N, double alpha, double * X, double * Y, int * latency);
double _ddot  (int N, double alpha, double * X, double * Y, int * latency);
double _dnrm2 (int N, double alpha, double * X, double * Y, int * latency);
double _dasum (int N, double alpha, double * X, double * Y, int * latency);
double _idamax(int N, double alpha, double * X, double * Y, int * latency);

class BlasOperation {
  private:
    // static fields
    static std::map<std::string, BlasOperation *> NameLU;
    static std::map<int,         BlasOperation *> IdLU;

  public:
    // member fields
    const std::string cblas_name; // necessary?
    const cblasfunc_t func;
    const int id;
    const RetTy ret;
    const std::vector<Arg> argv;

    static BlasOperation * get(int index);
    static BlasOperation * find(std::string name);
    static void init();

    BlasOperation(std::string name, cblasfunc_t func, int index, RetTy ret, std::vector<Arg> argv);
};

#endif // _BLAS_OP

