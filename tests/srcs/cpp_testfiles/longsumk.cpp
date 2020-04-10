#include "pzhelp"

#define MAX 1000000

int a[MAX];

PROGRAM {
  int N = READ_INT();
  int S = READ_INT();
  for (int i = 0; i < N; ++i) a[i] = READ_INT();

  int sum = 0, best = -1, i = 0, j = 0;
  while (true) {
      while (sum > S) sum -= a[i++];
      while (j < N && sum + a[j] <= S) sum += a[j++];
      if (sum == S) best = max(best, j - i);
      if (j < N) sum += a[j++]; else if (sum <= S) break;
  }
  WRITELN(best);
}
