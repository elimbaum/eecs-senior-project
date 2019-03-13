#include <stdio.h>
#include <math.h>

double hyp(double a, double b) {
  printf("(int) ");
  return sqrt(a * a + b * b);
}

int main() {
  printf("hello world!\n");
  printf("hypot: %f\n", hypot(3, 4));
  printf("hyp:   %f\n", hyp(3, 4));
  return 0;
}
