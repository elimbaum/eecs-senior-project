#include <cstdio>

extern double printd2(double);
extern "C" double printd3(double);

int main()
{
  printd2(4);
  printd3(5);
}
