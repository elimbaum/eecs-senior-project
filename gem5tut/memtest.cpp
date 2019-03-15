#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char ** argv) {
  ifstream m;
  m.open("/dev/mem");
  m.seekg(strtol(argv[1], NULL, 0));

  printf("req\n");
  char q = m.get();
  
  if (q < 0) {
    perror("memtest");
  } else {
    printf("Got %d!\n", q);
  }
}
