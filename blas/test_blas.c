#include <cblas.h>
#include <stdio.h>
#include <gem5/m5ops.h>
#include <m5_mmap.h>

void print_vec(int len, double * V)
{
  for (int i = 0; i < len; i++) {
    printf("%f ", V[i]);
  }
  printf("\n");
}

int main()
{
#ifdef SIMULATED
  map_m5_mem();
  m5_reset_stats(0, 0);
#endif

  double A[6] = {1.5, 20, 103, -3.0, 4.0, -1.0};
  double B[6] = {0.3, 7.7, -9.0, 1.3, 0.9, -9.0};
  
  printf("nrm: %f\n", cblas_dnrm2(6, A, 1));
  printf("sum: %f\n", cblas_dasum(6, A, 1));
  // no MMIOFU support for idamax, really
  // printf("max: %d\n", cblas_idamax(6, A, 1));
  printf("\ndot: %f\n", cblas_ddot(6, A, 1, B, 1));
  printf("\nA:    "); print_vec(6, A);
  cblas_dscal(6, 2.0, A, 1);
  printf("scal: "); print_vec(6, A);
  cblas_daxpy(6, 1.79, B, 1, A, 1);
  printf("axpy: "); print_vec(6, A);

#ifdef SIMULATED
  m5_dump_stats(0, 0);
#endif
}
