#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstdbool>
#include <climits>
#include <vector>
using namespace std;

int main(int argc, char ** argv) {
  long UpperLimit = 1000000;

  if (argc == 2) {
    UpperLimit = strtol(argv[1], NULL, 0);
  } else if (argc > 2) {
    fprintf(stderr, "Usage: %s [upper limit]\n", argv[0]);
    exit(1);
  }

  vector<bool> sieve (UpperLimit);

  int MaxSearch = floor(sqrt(UpperLimit));
  // printf("Primes less than %d (search up to %d)\n", UpperLimit, MaxSearch);

  for (long a = 2; a <= MaxSearch; a++) {
    if (sieve[a]) continue;

    for (long b = 2 * a; b <= UpperLimit; b += a) {
      sieve[b] = true;
    }
  }

  // print pass
  long nPrimes = 0;
  for (long i = 2; i < UpperLimit; i++) {
    if (sieve[i]) continue;
    // found a prime!
    // printf("%d\n", i);
    nPrimes++;
  }
  printf("%ld\n", nPrimes);
}
