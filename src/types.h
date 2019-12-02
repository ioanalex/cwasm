#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <cstdint>
#include <string>

typedef uint32_t u32;

typedef int32_t i32;
typedef int64_t i64;

template<typename T>
using vec = std::vector<T>;

typedef unsigned char byte;

typedef std::string string;

// TODO: check these definitions
typedef float f32;
typedef double f64;

constexpr u32 empty_u32 = u32(-1);

namespace type
{

typedef std::string Name;

enum Value
{
    I32,
    I64,
    F32,
    F64,
    any
};

enum Elem
{
    FuncRef
};

struct Func
{
    vec<Value> args;
    vec<Value> result;
};

struct Limits
{
    u32 min;
    u32 max = empty_u32;
};

struct Memory
{
    Limits limits;
};

struct Table
{
    Limits limits;
    Elem elem;
};

struct Global
{
    bool mut;
    Value value;
};

struct Extern
{
    enum
    {
        FUNC,
        TABLE,
        MEMORY,
        GLOBAL
    } tag;
    union {
        Func func;
        Table table;
        Memory mem;
        Global global;
    };
};

}; // namespace type

#endif
