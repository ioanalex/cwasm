#ifndef VALUES_H
#define VALUES_H

#include <iostream>

#include "types.h"

typedef struct {
    type::Value type;
    union{
        i32 i;
        i64 I;
        f32 f;
        f64 F;
    };
} Value;

bool operator==(const Value&, const Value&);
std::ostream& operator<<(std::ostream& , const Value& );

Value from_i32(i32);
Value from_i64(i64);
Value from_f32(f32);
Value from_f64(f64);

Value false_();
Value true_();
type::Value type_(Value v);

#endif