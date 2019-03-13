#include <cstdio>
#include <cmath>

extern "C" double printd2(double X) {
  fprintf(stderr, "(val: %f)\n", X);
  return X;
}

extern "C" double _hyp(double x, double y) {
  // fprintf(stderr, "running replaced function...\n");
  return hypot(x, y);
}
