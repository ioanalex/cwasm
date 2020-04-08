#include "pzhelp"

#define MAX 1000000

int a[MAX];

PROGRAM {
  int N = READ_INT();
  int S = READ_INT();
  for (int i = 0; i < N; ++i) a[i] = READ_INT();

  int best = -1;
  for (int i = 0; i < N; ++i)
    for (int j = i; j <= N; ++j) {
      int sum = 0;
      for (int k = i; k < j; ++k) sum += a[k];
      if (sum == S) best = max(best, j - i);
    }
  WRITELN(best);
}
