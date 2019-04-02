#include <iostream>
#include <string>
#include <cblas.h>
#include <cmath>

using namespace std;
#include "macc.h"

int main(int argc, char ** argv) {
  string str;

  double A, B;
  cout << "A? ";
  cin >> str;
  A = stod(str);

  cout << "B? ";
  cin >> str;
  B = stod(str);
  // end input stage; begin calculation

  // compute with MACC
  Reg rX, rS, rR, rK;
  Macc MA, MB;

  // this might be MACC-able (x = 0 + A*A, S = x + B*B), but might be double access
  double S = A * A + B * B;
  rX.set(S / 2); // initial estimate
  rS.set(S);
  rK.set(-0.5); // constant

  // Now compute sqrt
  MA.connect(& rX, & rS, & rR); // x + S/x
  MB.connect(& rX, & rX, & rK); // / 2
  // this limit is kind of dependent on floating point precision... just do fixed?
  int limit = ceil(log(rS.get())) + 1;
  cout << "Runing for " << limit << " iterations\n";
  for(int i = 0; i < limit; i++) {
    rR.set(1 / rX.get()); // this is annoying!
    MA.execute();
    MB.execute();
  }

  double ma_hyp = rX.get();

  cout << "hyp: " << ma_hyp << "\n";

  double correct_hyp = hypot(A, B);
  if (ma_hyp != correct_hyp) {
    cout << "correct: " << correct_hyp << "\n";
  }
  cout << "MA\n"; MA.print_stats();
  cout << "MB\n"; MB.print_stats();
}
