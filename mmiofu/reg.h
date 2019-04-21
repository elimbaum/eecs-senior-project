#include <iostream>

// times in cycles
#define SET_TIME 1
#define GET_TIME 1

class Reg {
  private:
    long num_gets = 0;
    long num_sets = 0;

    double value = 0;

  public:
    Reg(double v = 0) {
      value = v;
    }

    bool set(double _v) {
      value = _v;
      num_sets++;
      return true;
    }

    double get() {
      num_gets++;
      return value;
    }

    void reset_stats() {
      num_gets = 0;
      num_sets = 0;
    }

    void print_stats() {
      std::cout << "Gets: " << num_gets << "; Sets: " << num_sets << "\n";
    }

    int get_cycles() {
      int c = SET_TIME * num_sets + GET_TIME * num_gets;
      reset_stats();
      return c;
    }
};
