#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

template <typename T> using vec = std::vector<T>;

using byte = unsigned char;
using string = std::string;

// TODO: check these definitions
using f32 = float;
using f64 = double;

constexpr u32 empty_u32 = u32(-1);

namespace type {

using Name = std::string;

enum class Value { i32, i64, f32, f64 };

inline std::ostream &operator<<(std::ostream &os, const Value &v) {
  switch (v) {
    case Value::i32: return os << "i32";
    case Value::i64: return os << "i64";
    case Value::f32: return os << "f32";
    case Value::f64: return os << "f64";
  }
}

enum class Elem { funcref };

struct Func {
  vec<Value> args;
  vec<Value> result;
};

struct Limits {
  u32 min;
  u32 max = empty_u32;
};

struct Memory {
  Limits limits;
};

struct Table {
  Limits limits;
  Elem elem;
};

struct Global {
  bool mut;
  Value value;
};

struct Extern {
  enum { FUNC, TABLE, MEMORY, GLOBAL } tag;
  union {
    Func func;
    Table table;
    Memory mem;
    Global global;
  };
};

}; // namespace type

#endif
