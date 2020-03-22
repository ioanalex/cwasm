#include "validate.hpp"

#include "util.hpp"

Context context;

void InitContext(Module &mod) {
  context.types = mod.types;
  // each mod.func has a typeidx to types
  itloop(mod.funcs) {
    type::Func functype = mod.types[(*it).type];
    context.funcs.emplace_back(functype);
  }
  itloop(mod.tables) {
    type::Table tabletype = (*it).type;
    context.tables.emplace_back(tabletype);
  }
  itloop(mod.mems) {
    type::Memory memtype = (*it).type;
    context.mems.emplace_back(memtype);
  }
  itloop(mod.globals) {
    type::Global globaltype = (*it).type;
    context.globals.emplace_back(globaltype);
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

std::stack<valtype> opds;  // value or unknown
std::stack<frame> ctrls;

void push_opd(valtype v) { opds.push(v); }
valtype pop_opd() {
  if (opds.size() == ctrls.top().height && ctrls.top().unreachable)
    return valtype::Unknown;
  if (opds.size() == ctrls.top().height)
    FATAL("cannot consume stack values from parent.\n");
  valtype ret = opds.top();
  opds.pop();
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
  ctrls.push(fr);
}
vec<valtype> pop_ctrl() {
  if (ctrls.empty()) FATAL("control stack is empty.\n");
  frame fr = ctrls.top();
  pop_opds(fr.end_types);
  if (opds.size() != fr.height)
    FATAL("the stack has not been cleaned properly\n");
  ctrls.pop();
  return fr.end_types;
}

void unreachable() {
  // resize the operand stack to it's original height
  while (ctrls.size() > ctrls.top().height) opds.pop();
  ctrls.top().unreachable = true;
}

bool Validate::funcs(Module &m) {
  itloop(m.funcs) {
    bool is_ok = Validate::func(*it);
    if (!is_ok) return false;
  }
  return true;
}

bool Validate::func(Func &f) {
  type::Func functype(context.types.at(f.type));  // TODO: add exception
                                                  // handling
  context.locals = functype.args + f.locals;
  context.labels.resize(functype.result.size());
  std::copy(functype.result.begin(), functype.result.end(),
            context.labels.begin());
  if (functype.result.empty())
    context.return_ = type::Result();
  else
    context.return_ = type::Result(functype.result.front());

  std::cout << "changed context to:" << std::endl;
  PrintContext();

  return Validate::expr(f.body);
}

bool Validate::expr(Expr &ex) {
  itloop(ex) if (!it->validate()) return false;
  return true;
}