#include <cstdio>
#include <cmath>

extern "C" double printd2(double X) {
  fprintf(stderr, "(3: %f)\n", X);
  return X;
}

extern "C" double _hyp(double x, double y) {
  fprintf(stderr, "computing hyp %f, %f == %f?\n", x, y, hypot(x, y));
  return 6;
}
