/*
  In this file we define all substructures of a Module, as we as the Module
  itself. We also overload the operator<< to be able to print those structures.
  Each structure has an sDebug() function that produces minimum debug
  information about it.
*/

#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>
#include <optional>
#include <variant>

#include "indices.hpp"
#include "instructions.hpp"
#include "types.hpp"
#include "util.hpp"
#include "values.hpp"

// This is defined in parse.hpp, just declared here.
class Reader;

// Expressions
using Expr = vec<Instr>;

// Functions
struct Func {
  //
  const typeidx type;       // index to types array
  vec<type::Value> locals;  // vector of locals
  Expr body;                // the instructions

  // Constructor
  Func(typeidx type) : type(type), locals(), body() {}

  // Prints just the typeidx of the function, useful for development
  inline string sDebug() const { return " D> f :: @" + type.value(); }
};
inline std::ostream &operator<<(std::ostream &os, const Func &f) {
  os << "func :: @" << f.type << std::endl << "\t\tlocals:" << std::endl;
  os << printvec(f.locals, 3);
  os << "\t\t" << f.body.size() << " instructions" << std::endl;
  os << printvec(f.body, 3);
  return os;
}

// Tables
struct Table {
  //
  type::Table type;  // [min, max?] funcref

  // Constructor
  Table(type::Table type) : type(type) {}

  // Minimum debug string
  inline string sDebug() const {
    return " D> Table :: " /* TODO: add call to type::Table sDebug here */;
  }
};
inline std::ostream &operator<<(std::ostream &os, const Table &t) {
  return os << t.type << std::endl;
}

// Memories
struct Memory {
  //
  type::Memory type;  // [min, max?]

  // Constructor
  Memory(type::Memory type) : type(type) {}

  // Minimum debug string
  inline string sDebug() const {
    return " D> Memory :: " /* TODO: add call to type::Memory sDebug here */;
  }
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
  //
  type::Global type;  // (mut)Value
  Expr init;          // the initialization expression for the global

  // Constructor
  Global(type::Global type) : type(type) {}

  // Minimum debug string
  inline string sDebug() const {
    return " D> Global :: " /* TODO: add call to type::Global sDebug here */;
  }
};
inline std::ostream &operator<<(std::ostream &os, const Global &g) {
  os << g.type << std::endl;
  os << printvec(g.init, 2);
  return os;
}

struct Elem {
  //
  const tableidx table;  // Index to the table vector (currently == 0)
  Expr offset;           // The offset in the table - constant expression
  vec<funcidx> init;     // function pointers to store in the table

  // Constructor
  Elem(tableidx table) : table(table) {}

  // Minimum debug string
  inline string sDebug() const {
    string s = " D> Elem :: Table#" + table.value();
    s += "[ ";
    for (const auto &f : init) s += std::to_string(f.value()) + " ";
    s += "]";
    return s;
  }
};

// Data Segments
struct Data {
  //
  const memidx data;  // Index to the memories vector (currently == 0)
  Expr offset;        // The offset in the memory - constant expression
  vec<byte> init;     // Initial data to store in the memory

  // Constructor
  Data(memidx &data) : data(data) {}

  // Minimum debug string
  inline string sDebug() const {
    string s = " D> Data :: Memory#" + data.value();
    /* TODO: and the code from ast.cpp that prints the contents */
    return s;
  }
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

  // Minimum debug string
  inline string sDebug() const {
    string s = "";
    switch (tag) {
      case FUNC:
        s += std::to_string(func.value());
        break;
      case TABLE:
        s += std::to_string(table.value());
        break;
      case MEM:
        s += std::to_string(mem.value());
        break;
      case GLOBAL:
        s += std::to_string(global.value());
        break;
    }
    return s;
  }
};

struct Export {
  //
  type::Name name;  // Name of the exported item
  exportdesc desc;  // Description

  // Constructor
  Export(type::Name &name, exportdesc &desc) : name(name), desc(desc) {}

  // Minimum debug string
  inline string sDebug() const {
    return "D> Export :: " + name2str(name) + " " + desc.sDebug();
  }
};

// Imports
class importdesc {
public:
  importdesc(typeidx f) : var(f) {}
  importdesc(type::Table t) : var(t) {}
  importdesc(type::Memory m) : var(m) {}
  importdesc(type::Global g) : var(g) {}

  enum tag_type { FUNC, TABLE, MEM, GLOBAL };
  tag_type tag() const { return tag_type(var.index()); }

  typeidx &func() { return std::get<FUNC>(var); }
  type::Table &table() { return std::get<TABLE>(var); }
  type::Memory &mem() { return std::get<MEM>(var); }
  type::Global &global() { return std::get<GLOBAL>(var); }

  const typeidx &func() const { return std::get<FUNC>(var); }
  const type::Table &table() const { return std::get<TABLE>(var); }
  const type::Memory &mem() const { return std::get<MEM>(var); }
  const type::Global &global() const { return std::get<GLOBAL>(var); }

  // Minimum debug string
  inline string sDebug() const {
    string s = "";
    switch (tag()) {
      case FUNC:
        s += "func @" + std::to_string(func().value());
        break;
      case TABLE:
        s += "table" /* TODO:call table.sDebug */;
        break;
      case MEM:
        s += "memory" /* TODO:call mem.sDebug */;
        break;
      case GLOBAL:
        s += "global" /* TODO:call global.sDebug */;
        break;
    }
    return s;
  }

private:
  std::variant<typeidx, type::Table, type::Memory, type::Global> var;
};

struct Import {
  //
  // Each import is labeled by a two-level name space, consisting of
  // a module name and a name for an entity within that module.
  type::Name module;
  type::Name name;
  importdesc desc;

  // Constructor
  Import(type::Name &name, type::Name &mod, importdesc &desc)
      : module(mod), name(name), desc(desc) {}

  // Minimum debug string
  inline string sDebug() const {
    return "D> Import :: " + name2str(module) + "." + name2str(name) + " " +
           desc.sDebug();
  }
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
        start(),
        imports(),
        exports() {}

  friend std::ostream &operator<<(std::ostream &out, const Module &m) {
    out << "Module:" << std::endl;
    out << "  Types:" << std::endl << printvec(m.types, 2);
    out << "  Funcs:" << std::endl << printvec(m.funcs, 2);
    out << "  Tables:" << std::endl << printvec(m.tables, 2);
    out << "  Memories:" << std::endl << printvec(m.mems, 2);
    out << "  Globals" << std::endl << printvec(m.globals, 2);
    return out;
  }

  // Minimum debug string
  inline string sDebug() const {
    string s = " MODULE>\n";
    s += "  Types:\n";
    // for (const auto &it : types) /* TODO: call the type::Func sDebug() here
    // */
    ;
    s += "  Functions:\n";
    for (const auto &it : funcs) s += "\t" + it.sDebug() + "\n";
    s += "  Tables:\n";
    for (const auto &it : tables) s += "\t" + it.sDebug() + "\n";
    s += "  Memories:\n";
    for (const auto &it : mems) s += "\t" + it.sDebug() + "\n";
    s += "  Globals:\n";
    for (const auto &it : globals) s += "\t" + it.sDebug() + "\n";
    s += "  Elements:\n";
    for (const auto &it : elem) s += "\t" + it.sDebug() + "\n";
    s += "  Datas:\n";
    for (const auto &it : data) s += "\t" + it.sDebug() + "\n";

    s += " Start:\n";
    if (start.has_value()) {
      s += "starts with func :: @" + std::to_string(start.value());
    }

    s += "  Export:\n";
    for (const auto &it : exports) s += "\t" + it.sDebug() + "\n";
    s += "  Import:\n";
    for (const auto &it : imports) s += "\t" + it.sDebug() + "\n";

    return s;
  }
};

#endif
