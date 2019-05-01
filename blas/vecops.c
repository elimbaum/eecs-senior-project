#include <cblas.h>
#include <m5_mmap.h>
#include <gem5/m5ops.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const int ITERS = 100;

const int START_SIZE = 1;
const int MAX_SIZE = 254;

void fill_array(double * array, int N) {
  for (int i = 0; i < N; i++) {
    array[i] = (double) rand() / RAND_MAX;
  }
}

int main() {
#ifdef SIMULATED
  map_m5_mem();
  m5_reset_stats(0, 0);
#endif

  for (int N = START_SIZE; N < MAX_SIZE; N = N * 1.5 + 1) {
    printf("size %d\n", N);

    double A[N];
    double B[N];
    double mag, sum;

    for (int i = 0; i < ITERS; i++) {
      fill_array(A, N);
      fill_array(B, N);

      // B = -1 * A + B = B - A
      cblas_daxpy(N, -1, A, 1, B, 1);
      mag = cblas_dnrm2(N, B, 1);

      // normalize vector
      cblas_dscal(N, 1/mag, B, 1);
      sum = cblas_dasum(N, B, 1);
    }
    printf("\t%f\n", sum);

#ifdef SIMULATED
    m5_dump_reset_stats(0, 0);
  }
  m5_exit(0);
#else
  }
#endif
}
