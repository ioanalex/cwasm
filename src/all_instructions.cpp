#include "all_instructions.hpp"

#include <math.h>

#include "validate.hpp"

// NOTE: this is a clean (?) way to validate instructions based on their type
// right now it only works for instructions with no parametric types, but it
// can be generalized. Insted of doing this at runtime it would be better if
// code was generated here and labeled for goto commands.
// TODO: This should be implemented as a basic optimization.
bool ByType(const instr_type &type, Validator *validator) {
  vec<valtype> args;  //, ret;
  iloop(type.args) {
    if (type.args[i].index() == 1) {
      warn("i am not that smart yet\n");
      return false;
    }
    args.push_back(gettype(std::get<type::Value>(type.args[i])));
    // validator->pop_opd(gettype(std::get<type::Value>(type.args[i])));
  }
  validator->pop_opds(args);
  iloop(type.ret) {
    if (type.ret[i].index() == 1) {
      warn("i am not that smart yet\n");
      return false;
    }
    validator->push_opd(gettype(std::get<type::Value>(type.ret[i])));
  }
  return true;
}

#define VALIDATE_BY_TYPE(i)                                 \
  bool i::validate(Validator *validator) {                  \
    return ByType(profiles[code()]->get_type(), validator); \
  }

// Unimplemented things
#define UNIMPLEMENTED(i) \
  bool i::validate(Validator *validator) { return true; }

using namespace Instruction;

bool Unreachable::validate(Validator *validator) {
  validator->unreachable();
  return true;
}

bool Nop::validate(Validator *validator) { return true; }

// NOTE: this is not finished
// TODO: finish this (:
bool Block::validate(Validator *validator) {
  valtype type = gettype(blocktype);
  validator->AddLabel(blocktype);
  if (blocktype.has_value())
    validator->push_ctrl({type}, {type});
  else
    validator->push_ctrl({}, {});
  std::cout << "ENTER BLOCK" << std::endl;
  // validate the instructions enclosed by the block
  iloop(instrs) {
    if (!instrs[i].validate(validator)) return false;
    validator->PrintStacks();
  }
  std::cout << "EXIT BLOCK" << std::endl;

  validator->RemoveLabel(blocktype);
  // simulate END instr (remember that blocks eat up their END opcode)
  validator->push_opds(validator->pop_ctrl());

  return true;
}

// NOTE: this is not finished
// TODO: finish this (:
bool Loop::validate(Validator *validator) {
  valtype type = gettype(blocktype);
  validator->AddLabel(type::Result());
  if (blocktype.has_value())
    validator->push_ctrl({}, {type});
  else
    validator->push_ctrl({}, {});
  std::cout << "ENTER LOOP" << std::endl;
  // validate the instructions enclosed by the loop
  iloop(instrs) {
    if (!instrs[i].validate(validator)) return false;
    validator->PrintStacks();
  }
  std::cout << "EXIT LOOP" << std::endl;

  validator->RemoveLabel(type::Result());
  // simulate END instr (remember that loops eat up their END opcode)
  validator->push_opds(validator->pop_ctrl());

  return true;
}

bool If::validate(Validator *validator) {
  valtype type = gettype(blocktype);
  validator->AddLabel(blocktype);

  validator->pop_opd(valtype::I32);
  if (blocktype.has_value())
    validator->push_ctrl({type}, {type});
  else
    validator->push_ctrl({}, {});

  std::cout << "ENTER IF" << std::endl;
  // validate the instructions in if body
  iloop(ifinstrs) {
    if (!ifinstrs[i].validate(validator)) return false;
    validator->PrintStacks();
  }

  std::cout << "EXIT IF" << ((has_else) ? " expect else " : "") << std::endl;

  if (has_else) {
    auto results = validator->pop_ctrl();
    validator->push_ctrl(results, results);
    std::cout << "ENTER ELSE" << std::endl;

    // validate the instructions in else body
    iloop(elseinstrs) {
      if (!elseinstrs[i].validate(validator)) return false;
      validator->PrintStacks();
    }

    std::cout << "EXIT ELSE" << std::endl;
  }

  validator->RemoveLabel(blocktype);
  // simulate END instr (remember that ifs eat up their END opcode)
  validator->push_opds(validator->pop_ctrl());

  return true;
}

bool Br::validate(Validator *validator) {
  // 1. check context
  if (validator->context().labels.size() <= this->value) {
    warn("Label %d is not defined\n", this->value.value());
    return false;
  }
  // The following statement performs the same check as above, but based on the
  // control stack. This is redundant but is a good sanity check for our
  // datastructures (this is why the first if just warns)
  if (validator->ctrls_size() <= this->value) {
    warn("Label index is too big\n");
    return false;
  }
  // 2. get labeltype
  // valtype labeltype = res2valtype(validator->context()labels[this->value]);
  // std::cout << labeltype << std::endl;
  // PrintContext();
  vec<valtype> temp = validator->n_frame(this->value).label_types;

  // 3. then
  validator->PrintContext();
  validator->pop_opds(temp);
  validator->unreachable();
  return true;
}

bool Br_If::validate(Validator *validator) {
  // 1. check context
  if (validator->context().labels.size() <= this->value) {
    warn("Label is not defined\n");
    return false;
  }
  // The following statement performs the same check as above, but based on the
  // control stack. This is redundant but is a good sanity check for our
  // datastructures (this is why the first if just warns)
  if (validator->ctrls_size() <= this->value) {
    warn("Label index is too big\n");
    return false;
  }
  // 2. get labeltype
  // valtype labeltype = res2valtype(validator->context()labels[this->value]);
  vec<valtype> temp = validator->n_frame(this->value).label_types;

  // 3. then
  validator->pop_opd(valtype::I32);
  validator->pop_opds(temp);
  validator->push_opds(temp);

  return true;
}

bool Br_Table::validate(Validator *validator) {
  // 1. check context
  if (validator->context().labels.size() <= this->labelN) {
    warn("Label is not defined size:%ld\n", validator->context().labels.size());
    return false;
  }
  // The following statement performs the same check as above, but based on the
  // control stack. This is redundant but is a good sanity check for our
  // datastructures (this is why the first if just warns)
  if (validator->ctrls_size() <= this->labelN) {
    warn("Label index is too big\n");
    return false;
  }
  // 2. get labelNtype
  // valtype labelNtype = res2valtype(validator->context()labels[this->labelN]);
  vec<valtype> temp = validator->n_frame(this->labelN).label_types;
  // ASSERT(labelNtype == temp,
  //        "Labeltypes from context and ctrls must much. Got %s from context
  //        and "
  //        "%s from ctrls\n",
  //        val2str(labelNtype), val2str(temp));

  // 3. for all indexes in labels
  iloop(this->labels) {
    // This is the redundant check
    // if(ctrls_size() < this->labels[i] ||
    // n_frame(this->labels[i]).label_types.front() !=  ....)
    if (validator->context().labels.size() <= this->labels[i]) {
      warn("The %d labels (idx: %d) is not defined\n", i,
           this->labels[i].value());
      return false;
    }
    // valtype labeltype =
    // res2valtype(validator->context()labels[this->labels[i]]);
    if (validator->n_frame(this->labels[i].value()).label_types != temp) {
      warn("Label mismatch\n");
      return false;
    }
  }
  // 4. then
  validator->pop_opd(valtype::I32);
  validator->pop_opds(temp);
  validator->unreachable();

  return true;
}

bool End::validate(Validator *validator) {
  validator->push_opds(validator->pop_ctrl());
  return true;
}

bool Return::validate(Validator *validator) {
  // 1. check that return_ is defined, that is true
  // 2. get return type
  type::Result ret = validator->context().return_;
  vec<valtype> rets;
  if (ret.has_type) {
    valtype ret_ = res2valtype(ret);
    rets.push_back(ret_);
  }
  // 3. then
  validator->pop_opds(rets);
  validator->unreachable();

  return true;
}

bool Call::validate(Validator *validator) {
  // 1. check context
  if (this->value >= validator->context().funcs.size()) {
    warn("called a function that is not defined\n");
    return false;
  }
  // 2. get the type
  type::Func ftype = validator->context().funcs.at(this->value);

  vec<valtype> args = gettypes(ftype.args);
  vec<valtype> ret = gettypes(ftype.result);
  validator->pop_opds(args);
  validator->push_opds(ret);
  return true;
}

bool CallIndirect::validate(Validator *validator) {
  // 1.check context (C.tables[0])
  if (validator->context().tables.size() < 1) {
    warn("C.tables[0] is not defined\n");
    return false;
  }
  // 2. get C.tables[0]
  // type::Table t = validator->context()tables[0];
  // 3. check that elemtype is funcref (not needed since there is no alternative
  // yet)

  // 4. Check context for type
  if (this->value >= validator->context().types.size()) {
    warn("The type is not defined\n");
    return false;
  }
  // 5. get function type
  type::Func ftype = validator->context().types.at(this->value);
  vec<valtype> args = gettypes(ftype.args);
  vec<valtype> ret = gettypes(ftype.result);

  // then
  args.push_back(valtype::I32);
  validator->pop_opds(args);
  validator->push_opds(ret);

  return true;
}

bool Drop::validate(Validator *validator) {
  validator->pop_opd();
  return true;
}
bool Select::validate(Validator *validator) {
  validator->pop_opd(valtype::I32);
  valtype t1 = validator->pop_opd();
  valtype t2 = validator->pop_opd(t1);
  validator->push_opd(t2);
  return true;
}

bool LocalGet::validate(Validator *validator) {
  // 1. check the context
  if (this->value >= validator->context().locals.size()) {
    warn("The local is not defined\n");
    return false;
  }
  // 2. get the type from context
  valtype type = gettype(validator->context().locals.at(this->value));
  // 3. push it in the operand stack
  validator->push_opd(type);
  // 4. return
  return true;
}

bool LocalSet::validate(Validator *validator) {
  // 1. check the context
  if (this->value >= validator->context().locals.size()) {
    warn("The local is not defined\n");
    return false;
  }
  // 2. get the type from context
  valtype type = gettype(validator->context().locals.at(this->value));
  // 3. pop it from the operand stack
  validator->pop_opd(type);
  // 4. return
  return true;
}

bool LocalTee::validate(Validator *validator) {
  // 1. check the context
  if (this->value >= validator->context().locals.size()) {
    warn("The local is not defined\n");
    return false;
  }
  // 2. get the type from context
  valtype type = gettype(validator->context().locals.at(this->value));
  // 3. pop it from the operand stack and the push it
  validator->pop_opd(type);
  validator->push_opd(type);
  // 4. return
  return true;
}

bool GlobalGet::validate(Validator *validator) {
  // 1. check the context
  if (this->value >= validator->context().globals.size()) {
    warn("The global is not defined\n");
    return false;
  }
  // 2. get the type from context
  valtype type = gettype(validator->context().globals.at(this->value).value);
  // 3. push it to the operand stack
  validator->push_opd(type);
  // 4. return
  return true;
}

bool GlobalSet::validate(Validator *validator) {
  // 1. check the context
  if (this->value >= validator->context().globals.size()) {
    warn("The global is not defined\n");
    return false;
  }
  // 2. get the type from context
  type::Global gtype = validator->context().globals.at(this->value);
  valtype type = gettype(gtype.value);
  // 3. check if global is mutable
  if (!gtype.mut) FATAL("The global is not mutable\n");
  // 4. pop it from the operand stack
  validator->pop_opd(type);
  // 5. return
  return true;
}

#define CHECK_MEM()                           \
  if (validator->context().mems.size() < 1) { \
    warn("unknown memory\n");                 \
    return false;                             \
  }
#define CHECK_ALIGN()                                                      \
  auto &memarg = value;                                                    \
  auto bitwidth = 32;                                                      \
  if (type == type::Value::f64 || type == type::Value::i64) bitwidth = 64; \
  if (opt.has_value()) {                                                   \
    switch (opt.value().size) {                                            \
      case _8:                                                             \
        bitwidth = 8;                                                      \
        break;                                                             \
      case _16:                                                            \
        bitwidth = 16;                                                     \
        break;                                                             \
      case _32:                                                            \
        bitwidth = 32;                                                     \
    }                                                                      \
  }                                                                        \
  if (pow(2, memarg.align) > bitwidth / 8) {                               \
    warn("invalid align\n");                                               \
    return false;                                                          \
  }

bool Load::validate(Validator *validator) {
  CHECK_MEM()
  CHECK_ALIGN()
  return ByType(profiles[code()]->get_type(), validator);
}

bool Store::validate(Validator *validator) {
  CHECK_MEM()
  CHECK_ALIGN()
  return ByType(profiles[code()]->get_type(), validator);
}

bool MemorySize::validate(Validator *validator) {
  CHECK_MEM()
  return ByType(profiles[code()]->get_type(), validator);
}

bool MemoryGrow::validate(Validator *validator) {
  CHECK_MEM()
  return ByType(profiles[code()]->get_type(), validator);
}

VALIDATE_BY_TYPE(Const)
VALIDATE_BY_TYPE(Numeric)

#undef VALIDATE_BY_TYPE
#undef UNIMPLEMENTED
#undef CHECK_MEM
#undef CHECK_ALIGN
