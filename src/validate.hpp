#ifndef __VALIDATE_HPP__
#define __VALIDATE_HPP__

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

namespace Validate {
bool expr(Expr &);
bool func(Func &);
bool funcs(Module &);
}  // namespace Validate
#endif