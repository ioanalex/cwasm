#! /bin/sh
grep "^#include \"" src/*.hpp | sed -e 's:src/::' -e 's:\:#include: :' -e 's:"::g'
grep "^#include \"" src/*.cpp | sed -e 's:src/::' -e 's:\:#include: :' -e 's:"::g'
