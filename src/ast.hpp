#ifndef __AST_HPP__
#define __AST_HPP__

#include <optional>
#include <vector>

#include "indices.hpp"
#include "instructions.hpp"
#include "named.hpp"
#include "types.hpp"
#include "util.hpp"
#include "values.hpp"

// Following the spec (Structure -> Modules)

// Expressions
using Expr = std::vector<Instr>;

// Functions
struct Func {
  typeidx type;
  vec<type::Value> locals;
  Expr body;
  Func(typeidx type) : type(type), locals(), body() {}
};
inline std::ostream &operator<<(std::ostream &os, const Func &f) {
  os << "func :: @" << f.type << std::endl << "\t\tlocals:" << std::endl;
  printvec(f.locals, 3);
  os << "\t\t" << f.body.size() << " instructions" << std::endl;
  printvec(f.body, 3);
  return os;
}

// Tables
struct Table {
  type::Table type;
  Table(type::Table type) : type(type) {}
};
inline std::ostream &operator<<(std::ostream &os, const Table &t) {
  return os << t.type << std::endl;
}

// Memories
struct Memory {
  type::Memory type;
  Memory(type::Memory type) : type(type) {}
};
inline std::ostream &operator<<(std::ostream &os, const Memory &m) {
  return os << m.type << std::endl;
}
struct Memarg {
  u32 align;
  u32 offset;
};

// Globals
struct Global {
  type::Global type;
  Expr init;
  Global(type::Global type) : type(type) {}
  // Global(type::Global type, Expr init) : type(type), init(init) {}
};
inline std::ostream &operator<<(std::ostream &os, const Global &g) {
  os << g.type << std::endl;
  printvec(g.init, 2);
  return os;
}

struct Elem {
  tableidx table;  // Currently only one table is allowed so tableidx must be 0
  Expr offset;     // The offset is given by a constant expression that is
                   // evaluated to a value
  vec<funcidx> init;
  Elem(tableidx table) : table(table) {}
  // Elem(tableidx table, Expr &offset, vec<funcidx> &init)
  //     : table(table), offset(offset), init(init) {}
};

// Data Segments
struct Data {
  memidx data;
  Expr offset;
  vec<byte> init;
  Data(memidx &data) : data(data) {}
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
void PrintModule(Module &);

#endif
