#include <cblas.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define P_DIE(msg) (perror(msg), exit(1))
#define DIE(fmt, ...) (fprintf(stderr, fmt, __VA_ARGS__), exit(1))

#define DEFAULT_IN  "angle.in"
#define DEFAULT_ANS "angle.ans"

#define INITIAL_VEC_LEN 16

enum State {ST_A, ST_B, ST_SEP} state = ST_A;

int main(int argc, char ** argv) {
  // either accept defaults, or specify custom
  if (argc == 2 || argc > 3) {
    DIE("Usage: %s [input answer]\n", argv[0]);
    exit(1);
  }

  char * infn = DEFAULT_IN;
  char * ansfn = DEFAULT_ANS;
  if (argc == 3) {
    infn = argv[1];
    ansfn = argv[2];
  }

  FILE * in = fopen(infn, "r");
  if (!in) P_DIE(infn);
  FILE * ans = fopen(ansfn, "r");
  if (!ans) P_DIE(ansfn);

  int line_num = 0;
  char * in_line = NULL, * line_tmp;
  size_t in_pos;
  char * end;
  int n_A = 0;
  int n = 0;
  double * x; // temp array

  double * A = malloc(INITIAL_VEC_LEN * sizeof(double));
  double * B = malloc(INITIAL_VEC_LEN * sizeof(double));
  int max_len = INITIAL_VEC_LEN;

  while (getline(&in_line, &in_pos, in) > 0) {
    line_num++;
    line_tmp = in_line;

    if (state == ST_A) {
      x = A;
      state = ST_B;
    } else if (state == ST_B) {
      n_A = n; // save first length
      x = B;
      state = ST_SEP;
    } else { // ST_SEP
      if (line_tmp[0] == ';') {
        // Compute dot product
        if (n != n_A) {
          DIE("%s: length mismatch on line %d: %d vs %d\n", infn, line_num, n_A, n);
        }
        printf("%f\n", acos(cblas_ddot(n, A, 1, B, 1) / (cblas_dnrm2(n, A, 1) * cblas_dnrm2(n, B, 1))));
        state = ST_A;
        continue;
      } else {
        DIE("%s: expected ';' on line %d, got %c (%u)\n", infn, line_num, line_tmp[0], line_tmp[0]);
      }
    }

    n = 0;
    while (*line_tmp != '\n') {
      // printf("writing to %p\n", x);
      x[n] = strtod(line_tmp, & end);
      // printf("%f ", x[n]);
      if (end == line_tmp)
        break;

      if (++n >= max_len) {
        // printf("read %d numbers, bumping\n", n);
        max_len *= 2;
        A = realloc(A, max_len * sizeof(double));
        B = realloc(B, max_len * sizeof(double));

        // state has already been switched, but have to update x
        x = (state == ST_B) ? A : B;
      }
      
      line_tmp = end; // continue search after this one
      while(*(++line_tmp) == ' '); // but eat spaces first
      // printf("il now %c (%d)\n", *line_tmp, *line_tmp);
    }
    // printf("(%c) n = %d\n", state == ST_B ? 'A' : 'B', n);
  }
}
