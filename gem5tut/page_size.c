#include <stdio.h>
#include <unistd.h>

int main() {
  printf("%ld\n", sysconf(_SC_PAGE_SIZE));
  return 0;
}
