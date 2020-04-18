#include "validate.hpp"

#include <algorithm>

#include "util.hpp"

Context context;

void InitContext(Module &mod) {
  context.types.resize(mod.types.size());
  std::copy(mod.types.begin(), mod.types.end(), context.types.begin());
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
  // std::cout << "Validation Context:" << std::endl;
  // std::cout << "  Types:" << std::endl;
  // printvec(*(context.types), 2) std::cout << "  Funcs:" << std::endl;
  // printvec(context.funcs, 2) std::cout << "  Tables:" << std::endl;
  // printvec(context.tables, 2) std::cout << "  Memories:" << std::endl;
  // printvec(context.mems, 2) std::cout << "  Globals" << std::endl;
  // printvec(context.globals, 2) std::cout << "  Locals:" << std::endl;
  // printvec(context.locals, 2)
  std::cout << "  Labels:" << std::endl;
  printvec(context.labels, 2)  // std::cout << "  Return:" << std::endl <<
  // "\t\t"; std::cout << context.return_ << std::endl;
}

void UpdateContext(vec<type::Value> &locals, vec<type::Value> &labels) {
  context.locals.resize(locals.size());
  std::copy(locals.begin(), locals.end(), context.locals.begin());

  // is labels are empty we should add a label of type Result::none
  if (labels.empty()) {
    context.labels.resize(1);
    context.labels[0] = type::Result();
  } else {
    context.labels.resize(labels.size());
    std::copy(labels.begin(), labels.end(), context.labels.begin());
  }

  context.return_ = type::Result();
}

void UpdateContext(vec<type::Value> &locals, vec<type::Value> &labels,
                   type::Result return_) {
  context.locals.resize(locals.size());
  std::copy(locals.begin(), locals.end(), context.locals.begin());

  // is labels are empty we should add a label of type Result::none
  if (labels.empty()) {
    context.labels.resize(1);
    context.labels[0] = type::Result();
  } else {
    context.labels.resize(labels.size());
    std::copy(labels.begin(), labels.end(), context.labels.begin());
  }

  context.return_ = return_;
}

void AddLabel(type::Result label) {
  context.labels.insert(context.labels.begin(), label);
}

void RemoveLabel(type::Result expect) {
  type::Result l = context.labels.front();
  if ((l.has_type && expect.has_type && l.type != expect.type) ||
      (l.has_type && !expect.has_type) || (!l.has_type && expect.has_type)) {
    std::cout << "Got " << l << " | expected " << expect << std::endl;
    FATAL("Removed a label that was different than the one I added\n");
  }
  context.labels.erase(context.labels.begin());
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

type::Result valtype2res(const valtype &res) {
  type::Value ret;
  switch (res) {
    case valtype::Unknown:
      return type::Result();
    case valtype::I32:
      ret = type::Value::i32;
      break;
    case valtype::I64:
      ret = type::Value::i64;
      break;
    case valtype::F32:
      ret = type::Value::f32;
      break;
    case valtype::F64:
      ret = type::Value::f64;
      break;
  }
  return type::Result(ret);
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
  iloop(v) { res[i] = gettype(v[i]); }
  return res;
}

vec<valtype> opds;  // value or unknown
vec<frame> ctrls;
void PrintStacks() {
#if PRINT_STACKS
  printvec(ctrls, 2);
  printvec(opds, 1);
  // WaitEnter();
#endif
}

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
  for (unsigned i = types.size(); i-- > 0;) {
    // std::cout << "poping" << types.at(i) << std::endl;
    pop_opd(types.at(i));
  }
}

void push_ctrl(vec<valtype> labels, vec<valtype> out) {
  ASSERT(labels.size() <= 1, "Zero or one result type per label, not %ld\n",
         labels.size());
  ASSERT(out.size() <= 1, "One result type to return\n");
  frame fr = {labels, out, (u32)opds.size(), false};
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

long unsigned int ctrls_size() { return ctrls.size(); }

frame n_frame(int n) {
  // std::cout << "requested frame " << n << std::endl;
  // std::cout << "true frame is" << ctrls_size() - 1 - n << std::endl;

  return ctrls[ctrls_size() - 1 - n];
}

void unreachable() {
  // resize the operand stack to it's original height
  // std::cout << "original height is " << ctrls.back().height << std::endl;
  // while (ctrls.size() > ctrls.back().height + 1) {
  //   // std::cout << "Stack at " << ctrls.size() << ", popping" << std::endl;
  //   opds.pop_back();
  // }
  opds.erase(opds.begin() + ctrls.back().height, opds.end());
  ctrls.back().unreachable = true;
}

/* ----------------------------
 * ---- namespace Validate ----
 * ----------------------------
 */

bool Validate::limits(type::Limits &l, u32 range) {
  bool min_range = l.min <= range;
  bool min_max = true;
  bool max_range = true;
  if (l.max.has_value()) {
    max_range = l.max.value() <= range;
    min_max = l.min <= l.max.value();
  }
  // debug("range: %u\n", range);
  // debug("min: %u   max:%u\n", l.min, (l.max.has_value()) ? l.max.value() :
  // -1);
  if (!min_range) warn("size minimum must not be greater than %u\n", range);
  if (!max_range) warn("size maximum must not be greater than %u\n", range);
  if (!min_max) warn("size minimum must not be greater than maximum\n");

  return min_range && max_range && min_max;
}

bool Validate::tables(Module &m) {
  if (m.tables.size() > 1) {
    warn("multiple tables\n");
    return false;
  }
  // this for loop is not needed, but it is correct
  // keep it like this in case more tables are allowed
  // in the futute
  for (auto &table : m.tables)
    return Validate::limits(table.type.limits, 0xFFFFFFFF);

  return true;
}

bool Validate::mems(Module &m) {
  if (m.mems.size() > 1) {
    warn("multiple memories\n");
    return false;
  }
  // same as tables. The loop is not needed
  for (auto &mem : m.mems) return Validate::limits(mem.type.limits, 0x10000);
  return true;
}

bool Validate::globals(Module &m) {
  for (auto &g : m.globals) {
    // 1. g.type ust be valid with type mut t -- always true
    type::Value t = g.type.value;
    // 2. the init expr must be valid with type t
    type::Result r(t);
    valtype vt = res2valtype(r);
    push_ctrl(vec<valtype>({vt}), vec<valtype>({vt}));
    bool is_init_valid = Validate::expr(g.init);
    if (!is_init_valid) {
      warn("invalid init expr in global\n");
      return false;
    }
    for (auto &i : g.init) {
      if (!i.is_const() && i.code() != 0x0b) {
        warn("non const instruction in global\n");
        return false;
      }
    }
  }
  return true;
}

bool Validate::elems(Module &m) {
  for (auto &e : m.elem) {
    // 1. check that e.table is defined
    if (e.table >= m.tables.size()) {
      warn("uknown table\n");
      return false;
    }

    // 2. check that offset is a valid expr with type i32
    push_ctrl(vec<valtype>({valtype::I32}), vec<valtype>({valtype::I32}));
    bool is_offset_valid = Validate::expr(e.offset);
    if (!is_offset_valid) {
      warn("type mismatch\n");
      return false;
    }

    // 3. make sure offset is a constant expr
    for (auto &i : e.offset) {
      if (!i.is_const() && i.code() != 0x0b) {
        warn("constant expression required\n");
        return false;
      }
    }

    // 4. make sure all funcidxs are defined
    for (auto &y : e.init) {
      if (y >= m.funcs.size()) {
        warn("unknown function\n");
        return false;
      }
    }
  }
  return true;
}

bool Validate::datas(Module &m) {
  for (auto &d : m.data) {
    // 1. check that the memory is defined
    if (d.data >= m.mems.size()) {
      warn("unknown memory\n");
      return false;
    }

    // 2. check that offset is a valid expr with type i32
    push_ctrl(vec<valtype>({valtype::I32}), vec<valtype>({valtype::I32}));
    bool is_offset_valid = Validate::expr(d.offset);
    if (!is_offset_valid) {
      warn("type mismatch\n");
      return false;
    }

    // 3. make sure offset is a constant expr
    for (auto &i : d.offset) {
      if (!i.is_const() && i.code() != 0x0b) {
        warn("constant expression required\n");
        return false;
      }
    }
  }

  return true;
}

bool Validate::start(Module &m) {
  auto &start = m.start;
  // if no start function is defined all is ok!
  if (!start.has_value()) return true;

  // 1. check that the function is defined
  if (start.value() >= m.funcs.size()) {
    warn("unknown function\n");
    return false;
  }
  auto &fidx = start.value();

  // 2. check that the function has type [] -> []
  auto &ftype = m.types[m.funcs.at(fidx).type];
  if (!ftype.args.empty() || !ftype.result.empty()) {
    warn("wrong return type\n");
    return false;
  }

  return true;
}

bool Validate::exports(Module &m) {
  // store all names in this vector
  vec<type::Name> names;
  for (auto &ex : m.exports) {
    auto &desc = ex.desc;
    switch (desc.tag) {
      case exportdesc::FUNC:
        // check that the function is defined
        if (desc.func >= m.funcs.size()) {
          warn("unknown function\n");
          return false;
        }
        break;
      case exportdesc::TABLE:
        // check that the table is defined
        if (desc.table >= m.tables.size()) {
          warn("unknown table\n");
          return false;
        }
        break;
      case exportdesc::MEM:
        // check that the memory is defined
        if (desc.mem >= m.mems.size()) {
          warn("unknown memory\n");
          return false;
        }
        break;
      case exportdesc::GLOBAL:
        // check that the global is defined
        if (desc.global >= m.globals.size()) {
          warn("unknown global\n");
          return false;
        }
        break;
    }

    // add name to names
    names.push_back(ex.name);
  }

  // sort the vector of names
  std::sort(names.begin(), names.end());
  auto it = std::unique(names.begin(), names.end());

  bool was_unique = (it == names.end());
  if (!was_unique) {
    warn("duplicate export name\n");
    for (auto &n : names) {
      std::cout << string(n.begin(), n.end()) << std::endl;
    }
    return false;
  }

  return true;
}

bool Validate::imports(Module &m) {
  for (auto &im : m.imports) {
    auto &desc = im.desc;
    switch (desc.tag) {
      case importdesc::FUNC:
        // check that the function is defined
        if (desc.func >= m.funcs.size()) {
          warn("unknown function\n");
          return false;
        }
        break;
      case importdesc::TABLE:
        // validate the table type (limits)
        if (!Validate::limits(desc.table.limits, 0xFFFFFFFF)) {
          warn("invalid table\n");
          return false;
        }
        break;
      case importdesc::MEM:
        // Validate the memory type
        if (!Validate::limits(desc.mem.limits, 0x10000)) {
          warn("invalid memory\n");
          return false;
        }
        break;
      case importdesc::GLOBAL:
        // Validate the global type
        // always valid, do nothing
        break;
    }
  }

  return true;
}

bool Validate::funcs(Module &m) {
  iloop(m.funcs) {
    if (m.funcs[i].body.empty()) continue;  // this is an imported function
    // std::cout << "Validating func " << m.funcs[i] << std::endl;
    bool is_ok = Validate::func(m.funcs[i]);
    // std::cout << "OK" << std::endl;
    if (!is_ok) return false;
  }
  return true;
}

bool Validate::func(Func &f) {
  // clear stacks
  // opds.resize(0);
  // ctrls.resize(0);
  // 1. check context
  if (f.type.value() >= context.types.size()) {
    warn("the type %d is not defined\n", f.type.value());
    return false;
  }
  // 2. get the type from the context
  type::Func functype(context.types.at(f.type));
  // 3. update context (C') by getting locals, labels and return
  vec<type::Value> newlocals = functype.args + f.locals;
  if (functype.result.empty()) {
    UpdateContext(newlocals, functype.result);
    // 4.1 . add the initial frame to the control stack
    push_ctrl(vec<valtype>(), vec<valtype>());
  } else if (functype.result.size() == 1) {
    type::Result ret(functype.result.front());
    UpdateContext(newlocals, functype.result, ret);

    // 4.1. add the initial frame to the control stack
    valtype res = res2valtype(ret);
    push_ctrl(vec<valtype>({res}), vec<valtype>({res}));
  } else {
    warn("Functions can only return one value\n");
    return false;
  }
  // 4.2 . validate body
  bool res = Validate::expr(f.body);

  ASSERT(ctrls.empty(), "The control stack is not empty\n");
  ASSERT(opds.size() < 2,
         "The operand stack should be left empty or  with just one valtype not "
         "%ld\n",
         opds.size());
  type::Result result;
  if (opds.size() == 0) {
    result = type::Result();
  } else {
    result = valtype2res(opds.back());
    opds.pop_back();
  }
  // 5. result type of function
  // std::cout << "function validated with result type :: " << result <<
  // std::endl;

  return res;
}

bool Validate::expr(Expr &ex) {
  // std::cout << ex.size() << " instructions to validate" << std::endl;
  for (auto i = 0; i < ex.size(); i++) {
    if (!(ex[i].validate())) return false;
    PrintStacks();
  }
  // debug("finished with body\n");
  PrintStacks();
  return true;
}
