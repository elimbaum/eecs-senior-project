#include <iostream>
#include <string>
#include <cblas.h>

#define BASE_ADDR 0xFFFEF000

// TODO put this in its own header file/enum?
#define IDX_FUNCTION 0
#define IDX_N        1
#define IDX_INCX     2
#define IDX_INCY     3
#define IDX_START_X  4

#define NUM_FUNCS 6

// TODO be careful using sizeof(double), since host and sim machine may differ
#define GET_IDX(x) (BASE_ADDR + x * sizeof(double))

using namespace std;

void _dscal(int N, double alpha, double * X, int incX) {
  cout << "_dscal\n";
  cblas_dscal(N, alpha, X, incX);
}

void _daxpy(int N, double alpha, double * X, int incX, double * Y, int incY) {
  cout << "_daxpy\n";
  cblas_daxpy(N, alpha, X, incX, Y, incY);
}


double _ddot(int N, double * X, int incX, double * Y, int incY) {
  cout << "_ddot\n";
  return cblas_ddot(N, X, incX, Y, incY);
}

double _dnrm2(int N, double * X, int incX) {
  cout << "_dnrm2\n";
  return cblas_dnrm2(N, X, incX);
}

double _dasum(int N, double * X, int incX, double * Y, int incY) {
  cout << "_dasum\n";
  return cblas_ddot(N, X, incX, Y, incY);
}

int _idamax(int N, double * X, int incX) {
  cout << "_idamax\n";
  return cblas_idamax(N, X, incX);
}

typedef struct BlasOperation {
  string cblas_name;
  
  bool hasScalar;
  int vargc; // how many vector arguments
  bool returns; // ignore type; assume all returns are double

  void (*func)();
} blasop;

// TODO make dict of these for lookup (name -> index) ???
// TODO might need to use std::function for this
blasop operations[] = {
  // name           scalar/argc/return/func pointer
  {"cblas_dscal",   true,   1, false, (void(*)()) & _dscal  },
  {"cblas_daxpy",   true,   2, false, (void(*)()) & _daxpy  },
  {"cblas_ddot",    false,  2, true,  (void(*)()) & _ddot   },
  {"cblas_dnrm2",   false,  1, true,  (void(*)()) & _dnrm2  },
  {"cblas_dasum",   false,  1, true,  (void(*)()) & _dasum  },
  {"cblas_idamax",  false,  1, true,  (void(*)()) & _idamax },
};
