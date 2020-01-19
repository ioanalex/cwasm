#include "types.hpp"
#include <iostream>
#include <cstring>

// taken from https://github.com/kanaka/wac/blob/master/util.c

u64 read_LEB_(byte *bytes, u32 *pos, u32 maxbits, bool sign) {
    u64 result = 0;
    u32 shift = 0;
    u32 bcnt = 0;
    u32 startpos = *pos;
    u64 byte;

    while (true) {
        byte = bytes[*pos];
        *pos += 1;
        result |= ((byte & 0x7f)<<shift);
        shift += 7;
        if ((byte & 0x80) == 0) {
            break;
        }
        bcnt += 1;
        if (bcnt > (maxbits + 7 - 1) / 7) {
            // FATAL("Unsigned LEB at byte %d overflow", startpos);
            std::cout << "Unsigned LEB at byte "<< startpos << "overflow" << std::endl;
        }
    }
    if (sign && (shift < maxbits) && (byte & 0x40)) {
        // Sign extend
        result |= - (1 << shift);
    }
    return result;
}

u64 read_LEB(byte *bytes, u32 *pos, u32 maxbits) {
    return read_LEB_(bytes, pos, maxbits, false);
}

u64 read_LEB_signed(byte *bytes, u32 *pos, u32 maxbits) {
    return read_LEB_(bytes, pos, maxbits, true);
}

type::Name read_name(byte *bytes, u32 *pos){
    u32 namelen = read_LEB(bytes, pos, 32);
    char * name_c = (char *)malloc(sizeof(char) * namelen);
    std::memcpy(name_c, bytes + *pos, namelen);
    type::Name name(name_c);
    return name;
}

// --------------------------------------------------------------------------------------