#include <cblas.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ITERS 1000
#define N     1000

void print_array(double * array) {
  for (int i = 0; i < N; i++) {
    printf("%f ", array[i]);
  }
  printf("\n");
}

void fill_array(double * array) {
  for (int i = 0; i < N; i++) {
    array[i] = (double) rand() / RAND_MAX;
  }
  // print_array(array);
}

int main() {
  double A[N];
  double B[N];

  // don't see random so we get same arrays every time
  for (int i = 0; i < ITERS; i++) {
    fill_array(A);
    fill_array(B);

    double angle = acos(cblas_ddot(N, A, 1, B, 1) /
        (cblas_dnrm2(N, A, 1) * cblas_dnrm2(N, B, 1)));
    printf("%f\n", angle);
  }

}

