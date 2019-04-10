#include <iostream>
using namespace std;

extern "C" {
#include "fu_mmap.h"
}

int main() {
  _create_io_map();

  for(int i = 0; i < 6; i++)
    _io_map[0] = i;

  cout << "done\n";
}
