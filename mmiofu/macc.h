#include <iostream>
#include "reg.h"

// times in cycles; assume synchronous system running @ main clock
#define CONNECT_TIME 1
#define EXEC_TIME 3

class Macc {
  private:
    Reg * A, * B, * C;
    int num_execs = 0, num_connects = 0;
    // TODO keep internal stats for registers?

  public:
    // TODO add constructor connect
    bool connect(Reg * _A, Reg * _B, Reg * _C) {
      num_connects++;
      A = _A;
      B = _B;
      C = _C;
      return true;
    }

    bool execute() {
      num_execs++;
      // if any registers are equal, access once
      /*
      if (A == B && B == C) {
        double a = A->get();
        return A->set(a + a * a);
      } else if (A == B) {
        double a = A->get();
        return A->set(a + a * C->get());
      } else if (A == C) {
        double a = A->get();
        return A->set(a + B->get() * a);
      } else if (B == C) {
        double b = B->get();
        return A->set(A->get() + b * b);
      } else {
        return A->set(A->get() + B->get() * C->get());
      } */
      return A->set(A->get() + B->get() * C->get());
    }

    void print_stats() {
      // TODO if multiple Maccs access the same registers, stats will overlap
      // should be macc-local
      cout << "A: "; A->print_stats();
      cout << "B: "; B->print_stats();
      cout << "C: "; C->print_stats();
      cout << "Connects: " << num_connects << "\n";
      cout << "Execs: " << num_execs << "\n";
    }

    int get_cycles() {
      return CONNECT_TIME * num_connects + EXEC_TIME * num_execs;
    }
};
