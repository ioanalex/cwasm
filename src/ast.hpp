#ifndef __AST_HPP__
#define __AST_HPP__

#include <optional>
#include <vector>

#include "instructions.hpp"
#include "named.hpp"
#include "types.hpp"
#include "values.hpp"

// Following the spec (Structure -> Modules)

// Indices
using typeidx = Named<u32, struct typeidx_>;
using funcidx = Named<u32, struct funcidx_>;
using tableidx = Named<u32, struct tableidx_>;
using memidx = Named<u32, struct memidx_>;
using globalidx = Named<u32, struct globalidx_>;
using localidx = Named<u32, struct localidx_>;
using labelidx = Named<u32, struct labelidx_>;

// Expressions
using Expr = std::vector<Instr>;

// Functions
struct Func {
  typeidx type;
  vec<type::Value> locals;
  Expr body;
  Func(typeidx type) : type(type), locals(), body() {}
};

// Tables
struct Table {
  type::Table type;
  Table(type::Table type) : type(type) {}
};

// Memories
struct Memory {
  type::Memory type;
  Memory(type::Memory type) : type(type) {}
};

struct Memarg {
  u32 align;
  u32 offset;
};

// Globals
struct Global {
  type::Global type;
  Value init;
  Global(type::Global type) : type(type) {}
  Global(type::Global type, Value init) : type(type), init(init) {}
};

struct Elem {
  tableidx table;  // Currently only one table is allowed so tableidx must be 0
  Value offset;    // The offset is given by a constant expression that is
                   // evaluated to a value
  vec<funcidx> init;
  Elem(tableidx table, Value offset, vec<funcidx> &init)
      : table(table), offset(offset), init(init) {}
};

// Data Segments
struct Data {
  memidx data;
  Value offset;
  vec<byte> init;
  Data(memidx &data, Value &offset, vec<byte> &init)
      : data(data), offset(offset), init(init) {}
};

// Start Function
using Start = std::optional<funcidx>;

// Exports
struct exportdesc {
  enum { FUNC, TABLE, MEM, GLOBAL } tag;
  union {
    funcidx func;
    tableidx table;
    memidx mem;
    globalidx global;
  };
  exportdesc(funcidx f) : tag(FUNC), func(f) {}
  exportdesc(tableidx t) : tag(TABLE), table(t) {}
  exportdesc(memidx m) : tag(MEM), mem(m) {}
  exportdesc(globalidx g) : tag(GLOBAL), global(g) {}
};

struct Export {
  type::Name name;
  exportdesc desc;
  Export(type::Name &name, exportdesc &desc) : name(name), desc(desc) {}
};

// Imports
struct importdesc {
  enum { FUNC, TABLE, MEM, GLOBAL } tag;
  union {
    typeidx func;
    type::Table table;
    type::Memory mem;
    type::Global global;
  };
  importdesc(typeidx f) : tag(FUNC), func(f) {}
  importdesc(type::Table t) : tag(TABLE), table(t) {}
  importdesc(type::Memory m) : tag(MEM), mem(m) {}
  importdesc(type::Global g) : tag(GLOBAL), global(g) {}
};

struct Import {
  // Each import is labeled by a two-level name space, consisting of
  // a module name and a name for an entity within that module.
  type::Name module;
  type::Name name;
  importdesc desc;
  Import(type::Name &name, type::Name &mod, importdesc &desc)
      : module(mod), name(name), desc(desc) {}
};

// Module
struct Module {
  vec<type::Func> types;
  vec<Func> funcs;
  vec<Table> tables;
  vec<Memory> mems;
  vec<Global> globals;
  vec<Elem> elem;
  vec<Data> data;
  Start start;
  vec<Import> imports;
  vec<Export> exports;

  Module()
      : types(),
        funcs(),
        tables(),
        mems(),
        globals(),
        elem(),
        data(),
        start(0),
        imports(),
        exports() {}
};

void load_module(Module &, byte *, u32);

#endif
