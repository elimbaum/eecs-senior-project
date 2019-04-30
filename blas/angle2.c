#include <cblas.h>
#include <m5_mmap.h>
#include <gem5/m5ops.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const int ITERS = 100;

const int START_SIZE = 1;
const int END_SIZE = 254;

void print_array(double * array, int N) {
  printf("[ ");
  for (int i = 0; i < N; i++) {
    printf("%f ", array[i]);
  }
  printf("]\n");
}

// don't seed random so we get same arrays every time
void fill_array(double * array, int N) {
  for (int i = 0; i < N; i++) {
    array[i] = (double) rand() / RAND_MAX;
  }
  // print_array(array, N);
}

int main(int argc, char ** argv) {
#ifdef SIMULATED
  map_m5_mem();
  m5_reset_stats(0, 0);
#endif

  for (int N = START_SIZE; N < END_SIZE; N = N * 1.5+ 1) {
    printf("size %d\n", N);

    double A[N];
    double B[N];
    double angle;

    for (int i = 0; i < ITERS; i++) {
      fill_array(A, N);
      fill_array(B, N);

      angle = acos(cblas_ddot(N, A, 1, B, 1) /
          (cblas_dnrm2(N, A, 1) * cblas_dnrm2(N, B, 1)));
    }

    printf("\tdone: %f\n", angle);

#ifdef SIMULATED
    m5_dump_reset_stats(0, 0);
#endif
  }
  m5_exit(0);
}

