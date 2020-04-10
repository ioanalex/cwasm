#include "pzhelp"

#define MAX 1000000

int a[MAX];

PROGRAM {
  int N = READ_INT();
  int S = READ_INT();
  for (int i = 0; i < N; ++i) a[i] = READ_INT();

  int best = S > 0 ? -1 : 0;
  for (int i = 0; i < N; ++i) {
    int sum = 0;
    for (int j = i; j < N; ++j) {
      sum += a[j];
      if (sum == S) best = max(best, j - i + 1);
    }
  }
  WRITELN(best);
}
