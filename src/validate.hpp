#ifndef __VALIDATE_HPP__
#define __VALIDATE_HPP__

#include <stack>

#include "ast.hpp"

struct Context {
  vec<type::Func> types;
  vec<type::Func> funcs;
  vec<type::Table> tables;
  vec<type::Memory> mems;
  vec<type::Global> globals;
  vec<type::Value> locals;
  vec<type::Result> labels;
  type::Result return_;
};

void InitContext(Module&);
void PrintContext();
void UpdateContext(vec<type::Value>&, vec<type::Value>&);
void UpdateContext(vec<type::Value>&, vec<type::Value>&, type::Result);
void AddLabel(type::Result);
void RemoveLabel(type::Result);

// define the data structures needed for the body validation
enum valtype { I32, I64, F32, F64, Unknown };

const char* val2str(valtype v);

inline std::ostream& operator<<(std::ostream& os, const valtype& v) {
  static const char* valtype2str[]{"i32", "i64", "f32", "f64", "Unknown"};
  return os << valtype2str[v];
}

valtype res2valtype(const type::Result&);
valtype gettype(std::optional<type::Value>);
vec<valtype> gettypes(const vec<type::Value>&);

struct frame {
  vec<valtype> label_types;  // this should have a size of 1
  vec<valtype> end_types;    // this should have a size of 1
  u32 height;
  bool unreachable;
};

inline std::ostream& operator<<(std::ostream& os, const frame& f) {
  os << "h:" << f.height << " unr:" << f.unreachable << " labels"
     << " end_types" << std::endl;
  iloop(f.label_types) {
    tabs(3);
    os << "  " << f.label_types[i] << std::endl;
  }
  iloop(f.end_types) {
    tabs(4);
    os << "  " << f.end_types[i] << std::endl;
  }
  return os;
}

void PrintStacks();
// functions to access the stacks
void push_opd(valtype);
valtype pop_opd();
valtype pop_opd(valtype);

void push_opds(vec<valtype>);
void pop_opds(vec<valtype>);

void push_ctrl(vec<valtype>, vec<valtype>);
vec<valtype> pop_ctrl();
void unreachable();

long unsigned int ctrls_size();
frame n_frame(int n);

// end of body validation

namespace Validate {
bool expr(Expr&);
bool func(Func&);
bool funcs(Module&);
}  // namespace Validate

#endif
