#ifndef __DEV_HPP__
#define __DEV_HPP__

#include "types.hpp"

extern byte *bytes;   // buffer used for file
extern u32 pos;       // position on the buffer
extern long filelen;  // length of the file

inline void WaitEnter() {
  for (auto i = pos; i < filelen - pos + 1; i++) {
    printf("%x -> %x\n", pos, bytes[pos]);
  }
  std::cout << "Press Enter to continue...";
  auto a = getchar();
  while (a != '\n')
    ;
}

#endif