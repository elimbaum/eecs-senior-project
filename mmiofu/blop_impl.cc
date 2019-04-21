#include <cmath>
#include "macc.h"

// Common register definitions
//
// These uses are not enforced but are intended just to stay organized
//
// rRet   functions that return a value accumulate it here
// rS     sum of squares (for dnrm2)
// rR     reciprocal (for dnrm2)
// rX     X vector
// rY     Y vector
// rK     constants, alpha, etc
Reg rRet, rS, rR, rX, rY, rK;

// dscal
//
// x = a * x
// equivalent to: x = x + (a - 1) * x
// so MACC can be used.
//
// cblas_dscal(N, alpha, X, INC_X);

Macc M_dscal(&rX, &rX, &rK);
double _dscal(int N, double alpha, double * X, double * Y, int * latency) {
  std::cout << "hardware dscal\n";
  
  rK.set(alpha - 1);

  for (int i = 0; i < N; i++) {
    rX.set(X[i]);
    M_dscal.execute();
    X[i] = rX.get();
  }

  *latency = M_dscal.get_cycles() + rX.get_cycles() + rK.get_cycles();
  return 0;
}

// daxpy
//
// arguments are swapped to maintain MACC modification invariant
// cblas function updates y, not x
//
// cblas_daxpy(N, alpha, Y, INC_Y, X, INC_X);

Macc M_daxpy(&rX, &rY, &rK);
double _daxpy(int N, double alpha, double * X, double * Y, int * latency) {
  std::cout << "hardware daxpy\n";
  
  rK.set(alpha);

  for (int i = 0; i < N; i++) {
    rX.set(X[i]);
    rY.set(Y[i]);
    M_daxpy.execute();
    X[i] = rX.get();
    Y[i] = rY.get();
  }

  *latency = M_daxpy.get_cycles() + rX.get_cycles() + rY.get_cycles() + rK.get_cycles();
  return 0;
}

Macc M_ddot(&rRet, &rX, &rY);
double _ddot(int N, double alpha, double * X, double * Y, int * latency) {
  std::cout << "hardware ddot\n";

  for (int i = 0; i < N; i++) {
    rX.set(X[i]);
    rY.set(Y[i]);
    M_ddot.execute();
  }
  double r = rRet.get();

  *latency = M_ddot.get_cycles() + rRet.get_cycles() + rX.get_cycles() + rY.get_cycles();
  return r;
}

// dnrm2
//
// square root of sum of squares
// use floating point representation to create a (pretty good) approximiation
//
// return cblas_dnrm2(N, X, INC_X);

Macc M_sumsq(&rS,   &rX, &rX);
Macc M_sqrt (&rRet, &rS, &rR);
double _dnrm2(int N, double alpha, double * X, double * Y, int * latency) {
  std::cout << "hardware dnrm2\n";
  
  // S = sum(x * x)
  // this is dot product, but we can save register operations by consolidating
  for (int i = 0; i < N; i++) {
    rX.set(X[i]);
    M_sumsq.execute();
  }

  // compute sqrt(S) (iterative babylonian method)

  // initial estimate: half the floating point exponent
  //
  // S = M * B^E
  // sqrt(S) approx M * B^(E/2)
  //
  // off by a factor of sqrt(M); M is [0.5, 1), so sqrt(M) bounded [0.71, 1)
  //
  // In hardware, this is a simple bitshift
  int exp;
  double m = frexp(rS.get(), & exp);
  rRet.set(ldexp(m, exp >> 1)); // div 2; 1 cycle

  // x_n+1 = (S + S / x_n) / 2

  for (int i = 0; i < SQRT_ITERS; i++) {
    rR.set(1 / rRet.get()); // 2 cycles
    M_sqrt.execute();
    rRet.set(rRet.get() / 2); // 1 cycle
  }

  double r = rX.get();

  // add extra cycles for 1/X and X/2 (per iter) and one for estimate
  *latency = M_sumsq.get_cycles() + M_sqrt.get_cycles() + rX.get_cycles() + 
             rS.get_cycles() + rR.get_cycles() + rRet.get_cycles() +
             SQRT_ITERS * 3 + 1;
  return r;
}

// dasum
//
// sum up absolute values * 1

Macc M_dasum(&rRet, &rX, &rK);
double _dasum(int N, double alpha, double * X, double * Y, int * latency) {
  std::cout << "hardware dasum\n";
  rK.set(1);

  for (int i = 0; i < N; i++) {
   // I'm thinking abs is not an extra cycle, since just sign bitset
   // (that is, could be done in hardware by hardwiring sign bit))
    rX.set(std::abs(X[i]));
    M_dasum.execute();
  }
  double r = rRet.get();
  *latency = M_dasum.get_cycles() + rRet.get_cycles() + rX.get_cycles() + rK.get_cycles();
  return r;
}

// For call compatability, this must be a double. However, LLVM pass will cast
// back into an integer for the top-level program
//
// this can't be done with Macc. what do?
// might just forget this for the time being
/*
double _idamax(int N, double alpha, double * X, double * Y, int * latency) {
  std::cout << "idamax UNIMPL\n";

  // idamax needs to do N compares, as well as anywhere from 1 to N sets.
  // average?
  // TODO this is definitely not enough; not counting register operations
  *latency = N + N / 2;
  return cblas_idamax(N, X, INC_X);
}
*/
