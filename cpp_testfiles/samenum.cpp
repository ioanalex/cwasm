#include "pzhelp"

#define MAXV 2000001

int p[MAXV];  // initialized to zero

PROGRAM {
  int N = READ_INT();
  int best = -1;
  for (int i = 1; i <= N; ++i) {
    int x = READ_INT();
    if (p[x] == 0) p[x] = i; else best = max(best, i-p[x]);
  }
  WRITELN(best);
}
