#include <iostream>
#include <string>
#include <cblas.h>

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

struct BlasOperation {
  string cblas_name;
  
  bool hasScalar;
  int argc;

  void (*func)();
};

// TODO make dict of these for lookup (name -> index)
// TODO need to know about return type (all except for scal/axpy return scalar values)
// TODO might need to use std::function for this
struct BlasOperation operations[] = {
  {"cblas_dscal",   true,   1, (void(*)()) & _dscal   }, // no return
  {"cblas_daxpy",   true,   2, (void(*)()) & _daxpy   }, // no return
  {"cblas_ddot",    false,  2, (void(*)()) & _ddot    }, // double
  {"cblas_dnrm2",   false,  1, (void(*)()) & _dnrm2   }, // double
  {"cblas_dasum",   false,  1, (void(*)()) & _dasum   }, // double
  {"cblas_idamax",  false,  1, (void(*)()) & _idamax  }, // int (size_t)
};
