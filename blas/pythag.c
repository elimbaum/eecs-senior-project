#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "gem5/m5ops.h"

#define LIMIT 50

extern void map_m5_mem();

// this is not the most efficient way of generating pythagorean triples:
// besides floating point accuracy issues, there is also a direct way of
// computing triples (formula is on wikipedia)

int main(int argc, char ** argv) {
  map_m5_mem();
  // sweep on progressive diagonals
  for (int x = 1; x <= LIMIT; x++) {
    // only check the lower half!
    for(int y = 1; y <= x / 2; y++) {
      double c = hypot(x - y, y);
      double r = fmod(c, 1); // fractional part
      if (r == 0) {
        printf("(%f) %d %d -> %d\n", (double) y / (x - y), y, x - y, (int)c);
      }
    }
  }
  m5_exit(0);
}
