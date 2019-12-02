#ifndef AST_H
#define AST_H

#include "types.h"
// #include "instructions.h"
typedef i32 Expr;

// Following the spec (Structure -> Modules)

/*
    Indices
*/
typedef u32 typeidx;
typedef u32 funcidx;
typedef u32 tableidx;
typedef u32 memidx;
typedef u32 globalidx;
typedef u32 localidx;
typedef u32 labelidx;

/*
    Functions
*/
struct Func {
    typeidx type;
    vec<type::Value> locals;
    Expr body;
};

/*
    Tables
*/
struct Table {
    type::Table type;
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
    Expr offset; // The offset is given by a constant expression
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
};

/*
    Exports
*/
struct exportdesc {
    enum {FUNC, TABLE, MEM, GLOBAL} tag;
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
    enum {FUNC, TABLE, MEM, GLOBAL} tag;
    union {
        typeidx func;
        type::Table table;
        type::Memory mem;
        type::Global global;
    };
};

struct Import {
    type::Name module; // Each import is labeled by a two-level name space, consisting
    type::Name name;   // of amodule name and a name for an entity within that module.
    importdesc desc;
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
};

#endif
