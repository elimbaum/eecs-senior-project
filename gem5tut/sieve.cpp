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
  printf("Primes less than %ld (search up to %d)\n", UpperLimit, MaxSearch);

  long nPrimes = 0;
  for (long a = 2; a <= MaxSearch; a++) {
    if (sieve[a]) continue;

    // start at square of prime!
    nPrimes++;
    for (long b = a * a; b <= UpperLimit; b += a) {
      sieve[b] = true;
    }
  }

  printf("Counting...\n");

  // count pass
  for (long i = MaxSearch; i < UpperLimit; i++) {
    if (sieve[i]) continue;
    // found a prime!
    // printf("%d\n", i);
    nPrimes++;
  }
  printf("%ld\n", nPrimes);
}
