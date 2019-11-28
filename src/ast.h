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
typedef struct {
    typeidx type;
    vec<type::Value> locals;
    Expr body;
} Func;

/*
    Tables
*/
typedef struct {
    type::Table type;
} Table;

/* 
    Memories
*/
typedef struct {
    type::Memory type;
} Memory;

/*
    Globals
*/
typedef struct {
    type::Global type;
    Expr init;
} Global;

typedef struct {
    tableidx table; // Currently only one table is allowed so tableidx must me 0
    Expr offset; // The offset is given by a constant expression
    vec<funcidx> init;
} Elem;

/*
    Data Segments
*/
typedef struct {
    memidx data;
    Expr offset;
    
} Data;

/*
    Start Function
*/
typedef struct {
    bool defined = false;
    funcidx func;
} Start;

/*
    Exports
*/
typedef struct {
    enum {FUNC, TABLE, MEM, GLOBAL} tag;
    union {
        funcidx func;
        tableidx table;
        memidx mem;
        globalidx global;
    };
} exportdesc;

typedef struct {
    type::Name name;
    exportdesc desc;
} Export;

/*
    Imports
*/
typedef struct {
    enum {FUNC, TABLE, MEM, GLOBAL} tag;
    union {
        typeidx func;
        type::Table table;
        type::Memory mem;
        type::Global global;
    };
} importdesc;

typedef struct {
    type::Name module; // Each import is labeled by a two-level name space, consisting 
    type::Name name;   // of amodule name and a name for an entity within that module.
    importdesc desc;
} Import;

/* 
    Module
*/
typedef struct {
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
} Module;

#endif