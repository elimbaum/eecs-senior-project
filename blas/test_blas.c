#include <cblas.h>
#include <stdio.h>

int main()
{
  double A[6] = {1.5, 2.0, 1.0, -3.0, 4.0, -1.0};
  double B[6] = {0.2, 2.1, 9.0, -1.0, 0.9, -9.0};
  
  // printf("dot: %lf\n", cblas_ddot(6, A, 1, B, 1));
  printf("nrm: %f\n", cblas_dnrm2(6, A, 1));
  printf("sum: %f\n", cblas_dasum(6, A, 1));
  printf("max: %d\n", cblas_idamax(6, A, 1));
}
