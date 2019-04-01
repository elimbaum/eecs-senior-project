#include <iostream>
#include "reg.h"

class Macc {
  private:
    Reg * A, * B, * C;
    long num_execs = 0;

  public:
    bool connect(Reg * _A, Reg * _B, Reg * _C) {
      A = _A;
      B = _B;
      C = _C;
      return true;
    }

    bool execute() {
      num_execs++;
      return A->set(A->get() + B->get() * C->get());
    }

    void print_stats() {
      cout << "A: "; A->print_stats();
      cout << "B: "; B->print_stats();
      cout << "C: "; C->print_stats();
      cout << "Execs: " << num_execs << "\n";
    }
};
