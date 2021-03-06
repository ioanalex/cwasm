/*
  Here we define all index types as described in the specification.
*/

#ifndef __INDICES_HPP
#define __INDICES_HPP

#include "named.hpp"
#include "types.hpp"

using typeidx = Named<u32, struct typeidx_>;
using funcidx = Named<u32, struct funcidx_>;
using tableidx = Named<u32, struct tableidx_>;
using memidx = Named<u32, struct memidx_>;
using globalidx = Named<u32, struct globalidx_>;
using localidx = Named<u32, struct localidx_>;
using labelidx = Named<u32, struct labelidx_>;

#define PRINTABLE_INDEX(idx)                                        \
  inline std::ostream &operator<<(std::ostream &os, const idx &i) { \
    return os << i.value();                                         \
  }

PRINTABLE_INDEX(typeidx)
PRINTABLE_INDEX(funcidx)
PRINTABLE_INDEX(tableidx)
PRINTABLE_INDEX(memidx)
PRINTABLE_INDEX(globalidx)
PRINTABLE_INDEX(localidx)
PRINTABLE_INDEX(labelidx)

#undef PRINTABLE_INDEX

#endif
