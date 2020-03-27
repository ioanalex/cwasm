#include "validate.hpp"

#include "util.hpp"

Context context;

void InitContext(Module &mod) {
  context.types = mod.types;
  // each mod.func has a typeidx to types
  // itloop(mod.funcs) {
  //   type::Func functype = mod.types[(*it).type];
  //   context.funcs.emplace_back(functype);
  // }
  // itloop(mod.tables) {
  //   type::Table tabletype = (*it).type;
  //   context.tables.emplace_back(tabletype);
  // }
  // itloop(mod.mems) {
  //   type::Memory memtype = (*it).type;
  //   context.mems.emplace_back(memtype);
  // }
  // itloop(mod.globals) {
  //   type::Global globaltype = (*it).type;
  //   context.globals.emplace_back(globaltype);
  // }
  context.funcs.resize(mod.funcs.size());
  for (auto i = 0; i < mod.funcs.size(); i++) {
    context.funcs[i] = mod.types[mod.funcs[i].type];
  }

  context.tables.resize(mod.tables.size());
  for (auto i = 0; i < mod.tables.size(); i++) {
    context.tables[i] = mod.tables[i].type;
  }

  context.mems.resize(mod.mems.size());
  for (auto i = 0; i < mod.mems.size(); i++) {
    context.mems[i] = mod.mems[i].type;
  }

  context.globals.resize(mod.globals.size());
  for (auto i = 0; i < mod.globals.size(); i++) {
    context.globals[i] = mod.globals[i].type;
  }
}

void PrintContext() {
  std::cout << "Validation Context:" << std::endl;
  std::cout << "  Types:" << std::endl;
  printvec(context.types, 2) std::cout << "  Funcs:" << std::endl;
  printvec(context.funcs, 2) std::cout << "  Tables:" << std::endl;
  printvec(context.tables, 2) std::cout << "  Memories:" << std::endl;
  printvec(context.mems, 2) std::cout << "  Globals" << std::endl;
  printvec(context.globals, 2) std::cout << "  Locals:" << std::endl;
  printvec(context.locals, 2) std::cout << "  Labels:" << std::endl;
  printvec(context.labels, 2) std::cout << "  Return:" << std::endl << "\t\t";
  std::cout << context.return_ << std::endl;
}

const char *val2str(valtype v) {
  switch (v) {
    case valtype::I32:
      return "I32";
    case valtype::I64:
      return "I64";
    case valtype::F32:
      return "F32";
    case valtype::F64:
      return "F64";
    case valtype::Unknown:
      return "Unknown";
  }
  return "";  // this is so that the compiler doesn't produce warning;
}
valtype res2valtype(const type::Result &res) {
  return (res.has_type) ? gettype(res.type) : valtype::Unknown;
}
valtype gettype(std::optional<type::Value> t) {
  valtype type;
  if (t.has_value()) {
    switch (t.value()) {
      case type::Value::i32:
        type = valtype::I32;
        break;
      case type::Value::i64:
        type = valtype::I64;
        break;
      case type::Value::f32:
        type = valtype::F32;
        break;
      case type::Value::f64:
        type = valtype::F64;
        break;
    }
  } else
    type = valtype::Unknown;
  return type;
}

vec<valtype> gettypes(const vec<type::Value> &v) {
  vec<valtype> res(v.size());
  int i = 0;
  itloop(v) {
    res[i] = gettype(*it);
    i++;
  }
  return res;
}

vec<valtype> opds;  // value or unknown
vec<frame> ctrls;

void push_opd(valtype v) { opds.push_back(v); }
valtype pop_opd() {
  if (opds.size() == ctrls.back().height && ctrls.back().unreachable)
    return valtype::Unknown;
  if (opds.size() == ctrls.back().height)
    FATAL("cannot consume stack values from parent.\n");
  valtype ret = opds.back();
  opds.pop_back();
  return ret;
}
valtype pop_opd(valtype expect) {
  valtype actual = pop_opd();
  if (actual == valtype::Unknown) return expect;
  if (expect == valtype::Unknown) return actual;
  if (actual != expect)
    FATAL("got %s, expected %s .\n", val2str(actual), val2str(expect));
  return actual;
}

void push_opds(vec<valtype> types) {
  for (auto t : types) push_opd(t);
}
void pop_opds(vec<valtype> types) {
  for (unsigned i = types.size(); i-- > 0;) pop_opd(types.at(i));
}

void push_ctrl(vec<valtype> labels, vec<valtype> out) {
  frame fr = {labels, out, opds.size(), false};
  ctrls.push_back(fr);
}
vec<valtype> pop_ctrl() {
  if (ctrls.empty()) FATAL("control stack is empty.\n");
  frame fr = ctrls.back();
  pop_opds(fr.end_types);
  if (opds.size() != fr.height)
    FATAL("the stack has not been cleaned properly\n");
  ctrls.pop_back();
  return fr.end_types;
}

void unreachable() {
  // resize the operand stack to it's original height
  // std::cout << "original height is " << ctrls.back().height << std::endl;
  while (ctrls.size() > ctrls.back().height + 1) {
    // std::cout << "Stack at " << ctrls.size() << ", popping" << std::endl;
    opds.pop_back();
  }
  ctrls.back().unreachable = true;
}

bool Validate::funcs(Module &m) {
  itloop(m.funcs) {
    if (it->body.empty()) continue;  // this is an imported function
    std::cout << "Validating func " << *it << std::endl;
    bool is_ok = Validate::func(*it);
    std::cout << "OK" << std::endl;
    if (!is_ok) return false;
  }
  return true;
}

bool Validate::func(Func &f) {
  // 1. check context
  if (f.type >= context.types.size()) FATAL("the type is not defined\n");
  // 2. get the type from the context
  type::Func functype(context.types.at(f.type));
  // 3. update context (C') by getting locals, labels and return
  context.locals = functype.args + f.locals;
  context.labels.resize(functype.result.size());
  std::copy(functype.result.begin(), functype.result.end(),
            context.labels.begin());
  if (functype.result.empty()) {
    context.return_ = type::Result();
    // 5. add the initial frame to the control stack
    push_ctrl(vec<valtype>(), vec<valtype>());
  } else if (functype.result.size() == 1) {
    context.return_ = type::Result(functype.result.front());
    // 5. add the initial frame to the control stack
    valtype res = res2valtype(context.return_);
    push_ctrl(vec<valtype>({res}), vec<valtype>({res}));
  } else {
    FATAL("Functions can only return one value\n");
  }

  std::cout << "changed context to:" << std::endl;
  PrintContext();

  bool res = Validate::expr(f.body);

  ASSERT(ctrls.empty(), "The control stack is not empty\n");
  ASSERT(opds.size() == 1,
         "The operand stack should be left with just one valtype\n");
  debug("function validated with result type :: ");
  std::cout << opds.back() << std::endl;

  opds.pop_back();
  return res;
}

bool Validate::expr(Expr &ex) {
  std::cout << ex.size() << " instructions to validate" << std::endl;
  for (auto i = 0; i < ex.size(); i++) {
    if (!(ex[i].validate())) return false;
#if PRINT_STACKS
    printvec(ctrls, 2);
    printvec(opds, 1);
#endif
  }
  debug("finished with body\n");
#if PRINT_STACKS
  printvec(ctrls, 2);
  printvec(opds, 1);
#endif

  return true;
}
