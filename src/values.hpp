#ifndef __VALUES_HPP__
#define __VALUES_HPP__

#include <iostream>
#include <variant>

#include "types.hpp"

// Variants are in the same order as in enum type::Value.
using Value = std::variant<i32, i64, f32, f64>;

inline Value from_i32(i32 x) { return x; }
inline Value from_i64(i64 x) { return x; }
inline Value from_f32(f32 x) { return x; }
inline Value from_f64(f64 x) { return x; }

inline Value false_() { return i32(0); }
inline Value true_() { return i32(1); }

inline type::Value type_(const Value &v) { return type::Value(v.index()); }

inline std::ostream &operator<<(std::ostream &os, const Value &v) {
  os << type_(v) << '(';
  std::visit([&os](auto t) { os << t; }, v);
  return os << ')';
}

#endif
