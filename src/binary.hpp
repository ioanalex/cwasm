#ifndef __BINARY_HPP__
#define __BINARY_HPP__

#include <cstring>
#include <iostream>

#include "types.hpp"
#include "util.hpp"

/*
TODO: after finalizing the design of the Reader class, change the
functions to read from the fstream and not from a plain buffer.

Keep in mind that it is important to check if we are reading past the end of the
file. This might be handled in the Reader class.
*/

// TODO: get a cleaner read_LEB function that can correctly handle overflow...

// taken from https://github.com/kanaka/wac/blob/master/util.c
inline u64 read_LEB_(byte *bytes, u32 *pos, u32 maxbits, bool sign) {
  u64 result = 0;
  u32 shift = 0;
  u32 bcnt = 0;
  u32 startpos = *pos;
  u64 byte;

  while (true) {
    byte = bytes[*pos];
    *pos += 1;
    result |= ((byte & 0x7f) << shift);
    shift += 7;
    if ((byte & 0x80) == 0) {
      break;
    }
    bcnt += 1;
    if (bcnt > (maxbits + 7 - 1) / 7) {
      // FATAL("Unsigned LEB at byte %d overflow", startpos);
      std::cout << "Unsigned LEB at byte " << startpos << "overflow"
                << std::endl;
    }
  }
  if (sign && (shift < maxbits) && (byte & 0x40)) {
    // Sign extend
    result |= -(1 << shift);
  }
  return result;
}

inline u64 read_LEB(byte *bytes, u32 *pos, u32 maxbits) {
  return read_LEB_(bytes, pos, maxbits, false);
}

inline u64 read_LEB_signed(byte *bytes, u32 *pos, u32 maxbits) {
  return read_LEB_(bytes, pos, maxbits, true);
}

inline type::Name read_name(byte *bytes, u32 *pos) {
  u32 namelen = read_LEB(bytes, pos, 32);
  byte *name_c = new byte[namelen + 1]();
  std::memcpy(name_c, bytes + *pos, namelen);
  *pos += namelen;
  name_c[namelen] = '\0';
  type::Name name{name_c, name_c + namelen};
  delete[] name_c;
  return name;
}

inline u32 read_u32(byte *bytes, u32 &pos) {
  pos += 4;
  return ((u32 *)(bytes + pos - 4))[0];
}

#endif