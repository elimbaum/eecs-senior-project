#include <cblas.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int N = 4;

void print_array(double * array) {
  for (int i = 0; i < N; i++) {
    printf("%f ", array[i]);
  }
  printf("\n");
}

int main() {
  double A[] = {-5, 8.9, 115.9, 0.043};

  print_array(A);
  cblas_daxpy(N, 0.2, A, 1, A, 1);
  print_array(A);
}
