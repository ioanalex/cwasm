#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "util.hpp"

using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

template <typename T>
using vec = std::vector<T>;

using byte = unsigned char;
using string = std::string;

// TODO: check these definitions
using f32 = float;
using f64 = double;

constexpr u32 empty_u32 = u32(-1);

namespace type {

enum class Value { i32, i64, f32, f64 };

inline std::ostream &operator<<(std::ostream &os, const Value &v) {
  switch (v) {
    case Value::i32:
      return os << "i32";
    case Value::i64:
      return os << "i64";
    case Value::f32:
      return os << "f32";
    case Value::f64:
      return os << "f64";
    default:
      return os << "illegal type value";
  }
}

struct Result {
  bool has_type = false;
  Value type;
  Result() {}
  Result(type::Value &v) : has_type(true), type(v) {}
  Result(std::optional<type::Value> &v) {
    if (v.has_value()) {
      has_type = true;
      type = v.value();
    } else {
      has_type = false;
    }
  }
};
inline std::ostream &operator<<(std::ostream &os, const Result &r) {
  os << "result::";
  if (r.has_type) return os << r.type;
  return os << "none";
}

using Block = Result;

enum class Elem { funcref };

struct Func {
  vec<Value> args;
  vec<Value> result;
};

inline std::ostream &operator<<(std::ostream &os, const Func &v) {
  os << "[";
  for (const auto &arg : v.args) {
    os << " " << arg;
  }
  os << " ] -> [";
  for (const auto &res : v.result) {
    os << " " << res;
  }
  os << " ]";
  return os;
}

struct Limits {
  u32 min;
  std::optional<u32> max;
};
inline std::ostream &operator<<(std::ostream &os, const Limits &l) {
  os << "from " << l.min;
  if (l.max.has_value()) os << "to" << l.max.value();
  return os;
}

struct Memory {
  Limits limits;
};
inline std::ostream &operator<<(std::ostream &os, const Memory &m) {
  return os << "Memory " << m.limits;
}

struct Table {
  Limits limits;
  Elem elem;
};
inline std::ostream &operator<<(std::ostream &os, const Table &t) {
  return os << "Table :: funcref " << t.limits;
}

struct Global {
  bool mut;
  Value value;
};
inline std::ostream &operator<<(std::ostream &os, const Global &g) {
  return os << "Global " << ((g.mut) ? "(mut)" : "") << g.value;
}

struct Extern {
  enum { FUNC, TABLE, MEMORY, GLOBAL } tag;
  union {
    Func func;
    Table table;
    Memory mem;
    Global global;
  };
};

using Name = std::string;
};  // namespace type
inline string name2str(const type::Name &v) {
  return string(v.begin(), v.end());
}

// get type::Value from the 32 bit binary code
inline type::Value decode_type(u32 v) {
  switch (v) {
    case 0x7F:
      return type::Value::i32;
    case 0x7E:
      return type::Value::i64;
    case 0x7D:
      return type::Value::f32;
    case 0x7C:
      return type::Value::f64;
    default:
      FATAL("invalid value 0x%x not a valtype\n", v);
  }
}

#endif
