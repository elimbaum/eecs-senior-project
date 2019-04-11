#include <iostream>
using namespace std;

extern "C" {
#include "fu_mmap.h"
}

double * io_map;

int main() {
  _create_io_map();

  io_map = (double *) _io_map;

  for(double i = 0; i < 6; i++)
    io_map[0] = i;

  cout << "done\n";
}
