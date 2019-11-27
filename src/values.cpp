#include <cassert>
#include <iostream>

#include "types.h"
#include "values.h"

bool operator==(const Value &a, const Value &b)
{
    if (a.type == b.type)
    {
        assert(a.type < 4); // 4 possible types exist, so check for safety
        switch (a.type)
        {
        case type::I32:
            return a.i == b.i;
        case type::I64:
            return a.I == b.I;
        case type::F32:
            return a.f == b.f;
        case type::F64:
            return a.F == b.F;
        }
    }
    return false;
}

std::ostream &operator<<(std::ostream &os, const Value &v)
{
    switch (v.type)
    {
    case type::I32:
        std::cout << "I32 val of " << v.i;
        break;
    case type::I64:
        std::cout << "I64 val of " << v.I;
        break;
    case type::F32:
        std::cout << "F32 val of " << v.f;
        break;
    case type::F64:
        std::cout << "F64 val of " << v.F;
        break;
    }
    return os;
}

Value from_i32(i32 x)
{
    Value v;
    v.type = type::I32;
    v.i = x;
    return v;
}
Value from_i64(i64 x)
{
    Value v;
    v.type = type::I64;
    v.I = x;
    return v;
}
Value from_f32(f32 x)
{
    Value v;
    v.type = type::F32;
    v.f = x;
    return v;
}
Value from_f64(f64 x)
{
    Value v;
    v.type = type::F64;
    v.F = x;
    return v;
}

Value false_()
{
    Value v;
    v.type = type::I32;
    v.i = 0;
    return v;
}
Value true_()
{
    Value v;
    v.type = type::I32;
    v.i = 1;
    return v;
}
type::Value type_(Value v)
{
    return v.type;
}