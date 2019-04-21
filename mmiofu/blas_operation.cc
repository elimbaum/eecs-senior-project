#include <cmath>
#include "macc.h"

double _dscal(int N, double alpha, double * X, double * Y, int * latency) {
  cout << "hardware dscal\n";
  Reg rX, rK(alpha - 1);
  Macc m;
  
  m.connect(&rX, &rX, &rK);
  for (int i = 0; i < N; i++) {
    rX.set(X[i]);
    m.execute();
    X[i] = rX.get();
  }

  *latency = m.get_cycles() + rX.get_cycles() + rK.get_cycles();

  // cblas_dscal(N, alpha, X, INC_X);
  return 0;
}

// DAXPY swaps arguments to keep X as the modified value
double _daxpy(int N, double alpha, double * X, double * Y, int * latency) {
  cout << "hardware daxpy\n";
  Reg rX, rY, rA(alpha);
  Macc m;

  m.connect(&rX, &rY, &rA);
  for (int i = 0; i < N; i++) {
    rX.set(X[i]);
    rY.set(Y[i]);
    m.execute();
    X[i] = rX.get();
    Y[i] = rY.get();
  }


  *latency = m.get_cycles() + rX.get_cycles() + rY.get_cycles() + rA.get_cycles();

  //cblas_daxpy(N, alpha, Y, INC_Y, X, INC_X);
  return 0;
}

double _ddot(int N, double alpha, double * X, double * Y, int * latency) {
  cout << "hardware ddot\n";
  Reg total, rA, rB;
  Macc M;
  M.connect(& total, & rA, & rB);
  for (int i = 0; i < N; i++) {
    rA.set(X[i]);
    rB.set(Y[i]);
    M.execute();
  }
  double r = total.get();

  *latency = M.get_cycles() + total.get_cycles() + rA.get_cycles() + rB.get_cycles();
  return r;
}

double _dnrm2(int N, double alpha, double * X, double * Y, int * latency) {
  cout << "hardware dnrm2\n";
  
  Reg rX, rS, rR;
  Macc mA;

  // S = sum(x * x)
  // this is dot product, but we can save register operations by consolidating
  mA.connect(&rS, &rX, &rX);
  for (int i = 0; i < N; i++) {
    rX.set(X[i]);
    mA.execute();
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
  rX.set(ldexp(m, exp >> 1)); // div 2; 1 cycle

  mA.connect(&rX, &rS, &rR); // x + S/x

  for (int i = 0; i < SQRT_ITERS; i++) {
    rR.set(1 / rX.get()); // 2 cycles
    mA.execute();
    rX.set(rX.get() / 2); // 1 cycle
  }

  double r = rX.get();

  // add extra cycles for 1/X and X/2 (per iter) and one for estimate
  *latency = mA.get_cycles() + rX.get_cycles() + rS.get_cycles() + rR.get_cycles() +
             SQRT_ITERS * 3 + 1;
  return r;
  // return cblas_dnrm2(N, X, INC_X);
}

double _dasum(int N, double alpha, double * X, double * Y, int * latency) {
  cout << "hardware dasum\n";
  Macc S;
  Reg total, V, one(1);
  S.connect(&total, &V, &one);
  for (int i = 0; i < N; i++) {
   // I'm thinking  abs is not an extra cycle, just sign bitset
    V.set(abs(X[i]));
    S.execute();
  }
  double r = total.get();
  *latency = S.get_cycles() + total.get_cycles() + V.get_cycles() + one.get_cycles();
  return r;
}

// For call compatability, this must be a double. However, LLVM pass will cast
// back into an integer for the top-level program
//
// this can't be done with Macc. what do?
// might just forget this for the time being
/*
double _idamax(int N, double alpha, double * X, double * Y, int * latency) {
  cout << "idamax UNIMPL\n";

  // idamax needs to do N compares, as well as anywhere from 1 to N sets.
  // average?
  // TODO this is definitely not enough; not counting register operations
  *latency = N + N / 2;
  return cblas_idamax(N, X, INC_X);
}
*/
