#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char ** argv) {
  ifstream m;
  m.open("/dev/mem");
  m.seekg(strtol(argv[1], NULL, 0));

  if (! m.good()) {
    cout << "bad stream\n";
    return 1;
  }

  printf("req\n");
  int q = m.get();
  
  
  if (q < 0) {
    perror("get char");
  } else {
    printf("Got %d!\n", q);
  }
}
