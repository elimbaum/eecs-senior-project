#include <iostream>
#include <cmath>
using namespace std;

#ifdef __arm__
#define ARCH "ARM"
#endif
#ifdef __aarch64__
#define ARCH "AARCH64"
#endif
#ifdef __x86_64__
#define ARCH "X86"
#endif

#ifndef ARCH
#define ARCH "UNK"
#endif

extern "C" double hyp(double a, double b) {
  printf("local ");
  return 1 + sqrt(a * a + b * b);
}

int main() {
  cout << "Hello world, " ARCH "!\n";
  cout << "Local:  " << hyp(3, 4) << "\n";
  cout << "Extern: " << hypot(3, 4) << "\n";
}
