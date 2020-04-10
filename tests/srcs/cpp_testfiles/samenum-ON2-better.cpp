#include "pzhelp"

#define MAX 1000000

int a[MAX];

PROGRAM {
  int N = READ_INT();
  for (int i = 0; i < N; ++i) a[i] = READ_INT();

  int best = 0;
  for (int i = 0; i < N; ++i)
    for (int j = N-1; j > i + best; --j)
      if (a[i] == a[j]) { best = j - i; break; }
  WRITELN(best > 0 ? best : -1);
}
