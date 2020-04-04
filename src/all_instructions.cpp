#include "all_instructions.hpp"

#include "validate.hpp"

extern Context context;

// NOTE: this is a clean (?) way to validate instructions based on their type
// right now it only works for instructions with no parametric types, but it
// can be generalized. Insted of doing this at runtime it would be better if
// code was generated here and labeled for goto commands.
// TODO: This should be implemented as a basic optimization.
bool ByType(byte opcode) {
  instr_type type = profiles[opcode].get_type();
  iloop(type.args) {
    if (type.args[i].index() == 1) {
      warn("i am not that smart yet (0x%x)\n", opcode);
      return true;
    }
    pop_opd(gettype(std::get<type::Value>(type.args[i])));
  }
  iloop(type.ret) {
    if (type.ret[i].index() == 1) FATAL("i am not that smart yet\n");
    push_opd(gettype(std::get<type::Value>(type.ret[i])));
  }
  return true;
}

#define validateByType(i) \
  bool i::validate() { return ByType(code()); }
// Unimplemented things
#define UNIMPLEMENTED(i) \
  bool i::validate() { return true; }

using namespace Instruction;

bool Unreachable::validate() {
  unreachable();
  return true;
}

bool Nop::validate() { return true; }

// NOTE: this is not finished
// TODO: finish this (:
bool Block::validate() {
  valtype type = gettype(blocktype);
  AddLabel(blocktype);
  push_ctrl(vec<valtype>({type}), vec<valtype>({type}));

  // validate the instructions enclosed by the block
  iloop(instrs) {
    instrs[i].validate();
    PrintStacks();
  }
  RemoveLabel(blocktype);
  // simulate END instr (remember that blocks eat up their END opcode)
  auto results = pop_ctrl();
  push_opds(results);

  return true;
}
// NOTE: this is not finished
// TODO: finish this (:
bool Loop::validate() {
  valtype type = gettype(blocktype);
  AddLabel(type::Result());
  push_ctrl(vec<valtype>(), vec<valtype>({type}));
  std::cout << "ENTER LOOP" << std::endl;
  // validate the instructions enclosed by the loop
  iloop(instrs) {
    instrs[i].validate();
    PrintStacks();
  }
  std::cout << "EXIT LOOP" << std::endl;

  RemoveLabel(blocktype);
  // simulate END instr (remember that loops eat up their END opcode)
  auto results = pop_ctrl();
  push_opds(results);

  return true;
}

bool If::validate() {
  valtype type = gettype(blocktype);
  AddLabel(blocktype);

  pop_opd(valtype::I32);
  push_ctrl(vec<valtype>({type}), vec<valtype>({type}));

  std::cout << "ENTER IF" << std::endl;
  // validate the instructions in if body
  iloop(ifinstrs) {
    ifinstrs[i].validate();
    PrintStacks();
  }

  std::cout << "EXIT IF" << std::endl;

  if (has_else) {
    auto results = pop_ctrl();
    push_ctrl(results, results);
    std::cout << "ENTER ELSE" << std::endl;

    // validate the instructions in else body
    iloop(elseinstrs) {
      elseinstrs[i].validate();
      PrintStacks();
    }

    std::cout << "EXIT ELSE" << std::endl;
  }

  RemoveLabel(blocktype);
  // simulate END instr (remember that ifs eat up their END opcode)
  auto results = pop_ctrl();
  push_opds(results);

  return true;
}

bool Br::validate() {
  // 1. check context
  if (context.labels.size() <= this->value) {
    warn("Label %d is not defined\n", this->value.value());
  }
  // The following statement performs the same check as above, but based on the
  // control stack. This is redundant but is a good sanity check for our
  // datastructures (this is why the first if just warns)
  if (ctrls_size() <= this->value) FATAL("Label index is too big\n");
  // 2. get labeltype
  valtype labeltype = res2valtype(context.labels[this->value]);
  valtype temp = n_frame(this->value).label_types.front();
  ASSERT(labeltype == temp,
         "Labeltypes from context and ctrls must much. Got %s from context and "
         "%s from ctrls\n",
         val2str(labeltype), val2str(temp));

  // 3. then

  PrintContext();
  pop_opd(labeltype);
  unreachable();

  return true;
}

bool Br_If::validate() {
  // 1. check context
  if (context.labels.size() <= this->value) warn("Label is not defined\n");
  // The following statement performs the same check as above, but based on the
  // control stack. This is redundant but is a good sanity check for our
  // datastructures (this is why the first if just warns)
  if (ctrls_size() <= this->value) FATAL("Label index is too big\n");
  // 2. get labeltype
  valtype labeltype = res2valtype(context.labels[this->value]);
  valtype temp = n_frame(this->value).label_types.front();
  ASSERT(labeltype == temp,
         "Labeltypes from context and ctrls must much. Got %s from context and "
         "%s from ctrls\n",
         val2str(labeltype), val2str(temp));

  // 3. then
  pop_opd(valtype::I32);
  pop_opd(labeltype);
  push_opd(labeltype);

  return true;
}

bool Br_Table::validate() {
  // 1. check context
  if (context.labels.size() <= this->labelN) warn("Label is not defined\n");
  // The following statement performs the same check as above, but based on the
  // control stack. This is redundant but is a good sanity check for our
  // datastructures (this is why the first if just warns)
  if (ctrls_size() <= this->labelN) FATAL("Label index is too big\n");
  // 2. get labelNtype
  valtype labelNtype = res2valtype(context.labels[this->labelN]);
  valtype temp = n_frame(this->labelN).label_types.front();
  ASSERT(labelNtype == temp,
         "Labeltypes from context and ctrls must much. Got %s from context and "
         "%s from ctrls\n",
         val2str(labelNtype), val2str(temp));

  // 3. for all indexes in labels
  iloop(this->labels) {
    // This is the redundant check
    // if(ctrls_size() < this->labels[i] ||
    // n_frame(this->labels[i]).label_types.front() !=  ....)
    if (context.labels.size() <= this->labels[i])
      FATAL("The %d labels (idx: %d) is not defined\n", i,
            this->labels[i].value());
    valtype labeltype = res2valtype(context.labels[this->labels[i]]);
    if (labeltype != labelNtype)
      FATAL("Label #%d (idx: %d) is of type %s, not %s\n", i,
            this->labels[i].value(), val2str(labeltype), val2str(labelNtype));
  }
  // 4. then
  pop_opd(valtype::I32);
  pop_opd(labelNtype);
  unreachable();

  return true;
}

bool End::validate() {
  auto results = pop_ctrl();
  push_opds(results);
  return true;
}

bool Return::validate() {
  // 1. check that return_ is defined, that is true
  // 2. get return type
  type::Result ret = context.return_;
  valtype ret_ = res2valtype(ret);

  // 3. then
  pop_opd(ret_);
  unreachable();

  return true;
}

bool Call::validate() {
  // 1. check context
  if (this->value >= context.funcs.size())
    FATAL("called a function that is not defined\n");
  // 2. get the type
  type::Func ftype = context.funcs.at(this->value);

  vec<valtype> args = gettypes(ftype.args);
  vec<valtype> ret = gettypes(
      ftype.result);  // NOTE: when validating types check size of result
  pop_opds(args);
  push_opds(ret);
  return true;
}
// UNIMPLEMENTED(CallIndirect);

bool CallIndirect::validate() {
  // 1.check context (C.tables[0])
  if (context.tables.size() < 1) FATAL("C.tables[0] is not defined\n");
  // 2. get C.tables[0]
  // type::Table t = context.tables[0];
  // 3. check that elemtype is funcref (not needed since there is no alternative
  // yet)

  // 4. Check context for type
  if (this->value >= context.types.size()) FATAL("The type is not defined\n");
  // 5. get function type
  type::Func ftype = context.types.at(this->value);
  vec<valtype> args = gettypes(ftype.args);
  vec<valtype> ret = gettypes(ftype.result);

  // then
  args.push_back(valtype::I32);
  pop_opds(args);
  push_opds(ret);

  return true;
}

bool Drop::validate() {
  pop_opd();
  return true;
}
bool Select::validate() {
  pop_opd(valtype::I32);
  valtype t1 = pop_opd();
  valtype t2 = pop_opd(t1);
  push_opd(t2);
  return true;
}

bool LocalGet::validate() {
  // 1. check the context
  if (this->value >= context.locals.size()) FATAL("The local is not defined\n");
  // 2. get the type from context
  valtype type = gettype(context.locals.at(this->value));
  // 3. push it in the operand stack
  push_opd(type);
  // 4. return
  return true;
}

bool LocalSet::validate() {
  // 1. check the context
  if (this->value >= context.locals.size()) FATAL("The local is not defined\n");
  // 2. get the type from context
  valtype type = gettype(context.locals.at(this->value));
  // 3. pop it from the operand stack
  pop_opd(type);
  // 4. return
  return true;
}

bool LocalTee::validate() {
  // 1. check the context
  if (this->value >= context.locals.size()) FATAL("The local is not defined\n");
  // 2. get the type from context
  valtype type = gettype(context.locals.at(this->value));
  // 3. pop it from the operand stack and the push it
  pop_opd(type);
  push_opd(type);
  // 4. return
  return true;
}

bool GlobalGet::validate() {
  // 1. check the context
  if (this->value >= context.globals.size())
    FATAL("The global is not defined\n");
  // 2. get the type from context
  valtype type = gettype(context.globals.at(this->value).value);
  // 3. push it to the operand stack
  push_opd(type);
  // 4. return
  return true;
}

bool GlobalSet::validate() {
  // 1. check the context
  if (this->value >= context.globals.size())
    FATAL("The global is not defined\n");
  // 2. get the type from context
  type::Global gtype = context.globals.at(this->value);
  valtype type = gettype(gtype.value);
  // 3. check if global is mutable
  if (!gtype.mut) FATAL("The global is not mutable\n");
  // 4. pop it from the operand stack
  pop_opd(type);
  // 5. return
  return true;
}

validateByType(Load);
validateByType(Store);
validateByType(MemorySize);
validateByType(MemoryGrow);
validateByType(Const);
validateByType(Numeric);