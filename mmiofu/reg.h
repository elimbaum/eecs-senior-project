#include <iostream>

class Reg {
  private:
    long num_gets = 0;
    long num_sets = 0;

  public:
    double value = 0;

    bool set(double _v) {
      value = _v;
      num_sets++;
      return true;
    }

    double get() {
      num_gets++;
      return value;
    }

    void print_stats() {
      cout << "Gets: " << num_gets << "; Sets: " << num_sets << "\n";
    }
};
