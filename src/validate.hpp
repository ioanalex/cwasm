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

void InitContext(Module &);
void PrintContext();

// define the data structures needed for the body validation
enum class valtype { I32, I64, F32, F64, Unknown };

struct frame {
  vec<valtype> label_types;
  vec<valtype> end_types;
  u32 height;
  bool unreachable;
};

// functions to access the stacks
void push_opd(valtype);
valtype pop_opd();
valtype pop_opd(valtype);

void push_opds(vec<valtype>);
void pop_opds(vec<valtype>);

void push_ctrl(vec<valtype>, vec<valtype>);
vec<valtype> pop_ctrl();
void unreachable();

// end of body validation
namespace Validate {
bool expr(Expr &);
bool func(Func &);
bool funcs(Module &);
}  // namespace Validate
#endif