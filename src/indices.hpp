#ifndef __INDICES_HPP
#define __INDICES_HPP

#include "named.hpp"
#include "types.hpp"
// Indices
using typeidx = Named<u32, struct typeidx_>;
using funcidx = Named<u32, struct funcidx_>;
using tableidx = Named<u32, struct tableidx_>;
using memidx = Named<u32, struct memidx_>;
using globalidx = Named<u32, struct globalidx_>;
using localidx = Named<u32, struct localidx_>;
using labelidx = Named<u32, struct labelidx_>;

#endif