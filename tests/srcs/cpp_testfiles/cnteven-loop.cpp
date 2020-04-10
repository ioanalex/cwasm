#include "pzhelp"

PROGRAM {
  int a = READ_INT();
  int b = READ_INT();
  int count = 0;
  for (int i = a; i <= b; ++i) if (i % 2 == 0) ++count;
  WRITELN(count);
}
