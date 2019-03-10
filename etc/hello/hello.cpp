#include <iostream>

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

int main() {
  printf("Hello world, " ARCH "!\n");
}
