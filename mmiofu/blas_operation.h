#include <iostream>
#include <functional>
#include <string>
#include <cblas.h>

using namespace std;

#ifndef _BLAS_OP
#define _BLAS_OP

#define BASE_ADDR 0xFFFEF000
#define PAGE_LEN  0x1000

// TODO put this in its own header file/enum?
#define IDX_FUNCTION 0
#define IDX_ALPHA    1
#define IDX_N        2
// #define IDX_INCX     3
// #define IDX_INCY     4
#define IDX_START_X  3

// TODO don't hardcode this
#define NUM_FUNCS 6

// Find out optimal value for this
#define SQRT_ITERS 10

// TODO don't hardcode this
#define INC_X 1
#define INC_Y 1

// TODO be careful using sizeof(double), since host and sim machine may differ
#define GET_ADDR(idx) (BASE_ADDR + idx * sizeof(double))
#define GET_INDEX(addr) ((addr - BASE_ADDR) / sizeof(double))

double _dscal (int N, double alpha, double * X, double * Y, int* latency);
double _daxpy (int N, double alpha, double * X, double * Y, int* latency);
double _ddot  (int N, double alpha, double * X, double * Y, int* latency);
double _dnrm2 (int N, double alpha, double * X, double * Y, int* latency);
double _dasum (int N, double alpha, double * X, double * Y, int* latency);
double _idamax(int N, double alpha, double * X, double * Y, int* latency);

// TODO can this just be an enum?
typedef struct BlasOperation {
  string cblas_name;
  
  // bool hasScalar;
  // int vargc; // how many vector arguments
  bool returns; // ignore type; assume all returns are double

  // void (*func)();
  double (*func)(int, double, double*, double*, int*);
} blasop;

#endif
