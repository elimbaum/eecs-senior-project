#include "blas_operation.h"

double _dscal(int N, double alpha, double * X, double * Y) {
  cout << "_dscal\n";
  cblas_dscal(N, alpha, X, INC_X);
  return 0;
}

double _daxpy(int N, double alpha, double * X, double * Y) {
  cout << "_daxpy\n";
  cblas_daxpy(N, alpha, X, INC_X, Y, INC_Y);
  return 0;
}

double _ddot(int N, double alpha, double * X, double * Y) {
  cout << "_ddot\n";
  return cblas_ddot(N, X, INC_X, Y, INC_Y);
}

double _dnrm2(int N, double alpha, double * X, double * Y) {
  cout << "_dnrm2 " << N << "\n";
  for (int i = 0; i < N; i++) {
    cout << X[i] << " ";
  }
  cout << "\n";
  for (int i = 0; i < N; i++) {
    cout << Y[i] << " ";
  }
  cout << "\n";
  return cblas_dnrm2(N, X, INC_X);
}

double _dasum(int N, double alpha, double * X, double * Y) {
  cout << "_dasum\n";
  return cblas_ddot(N, X, INC_X, Y, INC_Y);
}

double _idamax(int N, double alpha, double * X, double * Y) {
  cout << "_idamax\n";
  return cblas_idamax(N, X, INC_X);
}

// TODO make dict of these for lookup (name -> index) ???
// TODO might need to use std::function for this
blasop operations[] = {
  // name          returns/func pointer
  {"cblas_dscal",  false, & _dscal  },
  {"cblas_daxpy",  false, & _daxpy  },
  {"cblas_ddot",   true,  & _ddot   },
  {"cblas_dnrm2",  true,  & _dnrm2  },
  {"cblas_dasum",  true,  & _dasum  },
  {"cblas_idamax", true,  & _idamax },
};