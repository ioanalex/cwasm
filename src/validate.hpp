#ifndef __VALIDATE_HPP__
#define __VALIDATE_HPP__

#include <optional>

#include "ast.hpp"
#include "types.hpp"

// The validation Context
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

// The operand values inserted in the operand stack
enum valtype { I32, I64, F32, F64, Unknown };
inline const char* val2str(valtype v) {
  static const char* valtype2str[]{"i32", "i64", "f32", "f64", "Unknown"};
  return valtype2str[v];
}
inline std::ostream& operator<<(std::ostream& os, const valtype& v) {
  return os << val2str(v);
}

// TODO:
// These methods can produce bugs and should be refined
valtype res2valtype(const type::Result&);
valtype gettype(std::optional<type::Value>);
vec<valtype> gettypes(const vec<type::Value>&);

// The control frames inserted in the control stack
struct frame {
  vec<valtype> label_types;  // this should have a size of 1
  vec<valtype> end_types;    // this should have a size of 1
  u32 height;
  bool unreachable;
};
inline std::ostream& operator<<(std::ostream& os, const frame& f) {
  os << "h:" << f.height << " unr:" << f.unreachable << " labels"
     << " end_types" << std::endl;
  iloop(f.label_types) os << indent(3) << "  " << f.label_types[i] << std::endl;
  iloop(f.end_types) os << indent(4) << "  " << f.end_types[i] << std::endl;
  return os;
}

class Validator {
public:
  // constructor
  Validator(){};

private:
  Context c;  // the validation context
public:
  const Context context() const { return c; }
  void InitContext(Module&);
  void PrintContext();
  void UpdateContext(vec<type::Value>&, vec<type::Value>&);
  void UpdateContext(vec<type::Value>&, vec<type::Value>&, type::Result);
  void AddLabel(type::Result);
  void RemoveLabel(type::Result);

private:
  vec<valtype> opds;  // the operand stack
  vec<frame> ctrls;   // the control stack
  // add all methods used to control the above structures
public:
  // a helper function to print the operand and control stacks
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

public:
  bool validate_module(Module&);  // validates a module
private:
  // add all methods used to validate the module
  // e.g. the Validate::funcs method could be included as
  bool expr(Expr&);
  bool limits(type::Limits&, u32);
  bool func(Func&);
  bool funcs(Module&);
  bool tables(Module&);
  bool mems(Module&);
  bool globals(Module&);
  bool elems(Module&);
  bool datas(Module&);
  bool start(Module&);
  bool exports(Module&);
  bool imports(Module&);
};

#endif
