#include <sys/io.h>
#include <stdio.h>

int main() {
  for (int i = 0; i < 0x1000; i++) {
    if (ioperm(i, 1, 1) >= 0) {
      printf("%X AVAIL\n", i);
    } 
  }
}
