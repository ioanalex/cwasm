#include "pzhelp"

#define MAX 1000000

int a[MAX];

PROGRAM {
  int N = READ_INT();
  for (int i = 0; i < N; ++i) a[i] = READ_INT();

  int best = -1;
  for (int i = 0; i < N; ++i)
    for (int j = i+1; j < N; ++j)
      if (a[i] == a[j]) best = max(best, j - i);
  WRITELN(best);
}
