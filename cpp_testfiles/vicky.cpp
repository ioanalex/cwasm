#include "pzhelp"

PROGRAM {
  int a = READ_INT();
  int b = READ_INT();
  if (a % 2 == 1) ++a;
  if (b % 2 == 0) ++b;
  WRITELN((b-a+1)/2);
}
