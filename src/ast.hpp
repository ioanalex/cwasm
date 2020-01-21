#ifndef __AST_HPP__
#define __AST_HPP__

#include "named.hpp"
#include "types.hpp"

typedef vec<byte> Expr;

// Following the spec (Structure -> Modules)

/*
    Indices
*/
using typeidx = Named<u32, struct typeidx_>;
using funcidx = Named<u32, struct funcidx_>;
using tableidx = Named<u32, struct tableidx_>;
using memidx = Named<u32, struct memidx_>;
using globalidx = Named<u32, struct globalidx_>;
using localidx = Named<u32, struct localidx_>;
using labelidx = Named<u32, struct labelidx_>;

/*
    Functions
*/
struct Func {
  typeidx type;
  vec<type::Value> locals;
  Expr body;
  Func(typeidx type):type(type) {}
};

/*
    Tables
*/
struct Table {
  type::Table type;
  Table(type::Table type): type(type) {}
};

/*
    Memories
*/
struct Memory {
  type::Memory type;
};

/*
    Globals
*/
struct Global {
  type::Global type;
  Expr init;
};

struct Elem {
  tableidx table; // Currently only one table is allowed so tableidx must me 0
  Expr offset;    // The offset is given by a constant expression
  vec<funcidx> init;
};

/*
    Data Segments
*/
struct Data {
  memidx data;
  Expr offset;
};

/*
    Start Function
*/
struct Start {
  bool defined = false;
  funcidx func;
  Start(u32 &f): func(f){}
};

/*
    Exports
*/
struct exportdesc {
  enum { FUNC, TABLE, MEM, GLOBAL } tag;
  union {
    funcidx func;
    tableidx table;
    memidx mem;
    globalidx global;
  };
};

struct Export {
  type::Name name;
  exportdesc desc;
};

/*
    Imports
*/
struct importdesc {
  enum { FUNC, TABLE, MEM, GLOBAL } tag;
  union {
    typeidx func;
    type::Table table;
    type::Memory mem;
    type::Global global;
  };
  importdesc(typeidx);
  importdesc(type::Table);
  importdesc(type::Memory);
  importdesc(type::Global);
};

struct Import {
  type::Name module; // Each import is labeled by a two-level name space, consisting
  type::Name name; // of amodule name and a name for an entity within that module.
  importdesc desc;
  Import(type::Name, type::Name, importdesc);
};

/*
    Module
*/
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

  Module(vec<type::Func> &, vec<Func> &, vec<Table> &,
         vec<Memory> &, vec<Global> &, vec<Elem> &,
         vec<Data> &, Start &, vec<Import> &, vec<Export> &
         );
};

Module * load_module(byte*, u32);

#endif
