#include "pzhelp"

PROGRAM {
  int a = READ_INT();
  int b = READ_INT();
  int c = READ_INT();
  if (a == b) WRITELN(c);
  else if (b == c) WRITELN(a);
  else WRITELN(b);
}
