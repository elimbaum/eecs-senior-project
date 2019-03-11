#include <cstdio>

extern "C" double printd2(double X) {
  fprintf(stderr, "(3: %f)\n", X);
  return X;
}
