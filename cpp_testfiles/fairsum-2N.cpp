#include "pzhelp"

#define MAX 1000000
int x[MAX];

PROGRAM {
  int N = READ_INT();
  int sum = 0;
  for (int i = 0; i < N; ++i) {
    x[i] = READ_INT();
    sum += x[i];
  }
  int best = sum;
  int i = 0, j = 0, curr = 0;
  while (i < 2*N) {
    // invariant: curr == \sum_{k=i}^{j-1} x[k]
    if (2*curr <= sum && j < 2*N) curr += x[j++ % N]; else curr -= x[i++ % N];
    best = min(best, abs(curr - (sum-curr)));
  }
  WRITELN(best);
}
