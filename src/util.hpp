#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include "types.hpp"

inline u32 read_u32(byte *bytes, u32 &pos){
    pos+=4;
    return ((u32 *) (bytes + pos - 4) )[0];
}
// u64 read_LEB_(byte , u32 , u32 , bool );
u64 read_LEB(byte* , u32* , u32 );
u64 read_LEB_signed(byte* , u32* , u32 );

#endif