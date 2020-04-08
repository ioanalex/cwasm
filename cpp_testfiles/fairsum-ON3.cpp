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
  for (int i = 0; i < N; ++i)
    for (int j = i; j < N; ++j) {
      int a = 0;
      for (int k = i; k < j; ++k) a += x[k];
      best = min(best, abs(a - (sum-a)));
    }
  WRITELN(best);
}
