#include "pzhelp"

PROGRAM {
  int n = READ_INT();
  int a = READ_INT();
  int b = READ_INT();
  if (n % a == n % b && (n / a) % a == (n / b) % b) WRITELN("yes");
  else WRITELN("no");
}
