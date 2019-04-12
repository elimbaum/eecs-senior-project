#include <iostream>
#include "blas_operation.h"

using namespace std;

extern "C" {
#include "fu_mmap.h"
}

double * io_map;

int main() {
  _create_io_map();

  io_map = (double *) _io_map;

  const int N = 3;

  double X[N] = {1, 0.4, 9.8};
  double Y[N] = {-2, 3.8, 10};

  io_map[IDX_N] = N;

  for(int i = 0; i < N; i++) {
    io_map[IDX_START_X + i] = X[i];
    io_map[IDX_START_X + N+ i] = Y[i];
  }

  // run!
  io_map[IDX_FUNCTION] = 3; // nrm2

  cout << "done\n";
}
