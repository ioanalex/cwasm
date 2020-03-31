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
  push_ctrl(vec<valtype>({type}), vec<valtype>({type}));

  // validate the instructions enclosed by the block
  iloop(instrs) instrs[i].validate();

  // simulate END instr (remember that blocks eat up their END opcode)
  auto results = pop_ctrl();
  push_opds(results);

  return true;
}
// NOTE: this is not finished
// TODO: finish this (:
bool Loop::validate() {
  valtype type = gettype(blocktype);
  push_ctrl(vec<valtype>(), vec<valtype>({type}));
std::cout << "ENTER LOOP" << std::endl;
  // validate the instructions enclosed by the loop
  iloop(instrs) instrs[i].validate();
std::cout << "EXIT LOOP" << std::endl;

  // simulate END instr (remember that loops eat up their END opcode)
  auto results = pop_ctrl();
  push_opds(results);

  return true;
}

bool End::validate() {
  auto results = pop_ctrl();
  push_opds(results);
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
  if (this->value >= context.locals.size())
    FATAL("The local is not defined\n");
  // 2. get the type from context
  valtype type = gettype(context.locals.at(this->value));
  // 3. push it in the operand stack
  push_opd(type);
  // 4. return
  return true;
}

bool LocalSet::validate() {
  // 1. check the context
  if (this->value >= context.locals.size())
    FATAL("The local is not defined\n");
  // 2. get the type from context
  valtype type = gettype(context.locals.at(this->value));
  // 3. pop it from the operand stack
  pop_opd(type);
  // 4. return
  return true;
}

bool LocalTee::validate() {
  // 1. check the context
  if (this->value >= context.locals.size())
    FATAL("The local is not defined\n");
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

UNIMPLEMENTED(If);
UNIMPLEMENTED(Br);
UNIMPLEMENTED(Br_If);
UNIMPLEMENTED(Br_Table);
UNIMPLEMENTED(Return);
UNIMPLEMENTED(CallIndirect);
