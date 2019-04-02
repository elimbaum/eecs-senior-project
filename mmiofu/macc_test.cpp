#include <iostream>
#include <string>
#include <cblas.h>
#include <cmath>

using namespace std;
#include "macc.h"

int main(int argc, char ** argv) {
  cout << "N? ";
  
  string str;
  int N;
  cin >> str;
  N = stoi(str);

  double A[N], B[N];
  cout << "A? ";
  for (int i = 0; i < N; i++) {
    cin >> str;
    A[i] = stod(str);
  }

  cout << "B? ";
  for (int i = 0; i < N; i++) {
    cin >> str;
    B[i] = stod(str);
  }
  // end input stage; begin calculation

  // compute with MACC
  Reg total, rA, rB;
  Macc MA;

  MA.connect(& total, & rA, & rB);
  for(int i = 0; i < N; i++) {
    // don't update total register
    rA.set(A[i]);
    rB.set(B[i]);
    MA.execute();
  }
  double ma_dot = total.get();

  cout << "dot: " << ma_dot << "\n";

  double correct_dot = cblas_ddot(N, A, 1, B, 1);
  if (ma_dot != correct_dot) {
    cout << "correct: " << correct_dot << "\n";
  }
  MA.print_stats();
}
