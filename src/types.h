
#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <cstdint>

typedef uint32_t u32;

typedef int32_t i32;
typedef int64_t i64;

// TODO: check this definitions
typedef float f32;
typedef double f64;

static u32 empty_u32 = u32(-1);

namespace type
{

enum Value
{
    I32,
    I64,
    F32,
    F64
};

enum Elem
{
    FuncRef
};

typedef struct
{
    std::vector<Value> args;
    std::vector<Value> result;
} Func;

typedef struct
{
    u32 min;
    u32 max = empty_u32;
} Limits;

typedef struct
{
    Limits limits;
} Memory;

typedef struct
{
    Limits limits;
    Elem elem;
} Table;

typedef struct
{
    bool mut;
    Value value;
} Global;

typedef struct
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
        Memory memory;
        Global global;
    };
} Extern;

}; // namespace type

#endif