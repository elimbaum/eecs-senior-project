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

  io_map[IDX_N] = 3.0;

  for(int i = 0; i < N; i++) {
    int xi = IDX_START_X + i;
    int yi = xi + N;
    // cout << xi << ", " << yi << "\n";
    io_map[xi] = X[i];
    io_map[yi] = Y[i];
  }

  // run!
  io_map[IDX_FUNCTION] = 2; // dot

  cout << io_map[IDX_ALPHA] << "\n";

  cout << "done\n";
}
