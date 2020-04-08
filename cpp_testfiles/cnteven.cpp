#include "pzhelp"

PROGRAM {
  int a = READ_INT();
  int b = READ_INT();
  int result = (b-a+1) / 2;
  if (a%2 == 0 && b%2 == 0) ++result;
  WRITELN(result);
}
