#ifndef __ALL_INSTRUCTIONS_HPP__
#define __ALL_INSTRUCTIONS_HPP__

#include <utility>

#include "indices.hpp"
#include "instructions.hpp"
#include "named.hpp"
#include "parse.hpp"
namespace Instruction {
#define DUMMY_VIRTUAL(SpecialInstrImpl) \
public:                                 \
  virtual void run() {}                 \
  virtual void verify() {}

#define DUMMY_INSTR_IMPL(SpecialInstrImpl)                         \
public:                                                            \
  SpecialInstrImpl(byte *bytes, u32 *pos) : InstrImpl((*pos)++) {} \
  DUMMY_VIRTUAL(SpecialInstrImpl)

class Unreachable : public InstrImpl {
  DUMMY_INSTR_IMPL(Unreachable)
};

class Nop : public InstrImpl {
  DUMMY_INSTR_IMPL(Nop)
};

class End : public InstrImpl {
  DUMMY_INSTR_IMPL(End)
};

class Return : public InstrImpl {
  DUMMY_INSTR_IMPL(Return)
};

class Drop : public InstrImpl {
  DUMMY_INSTR_IMPL(Drop)
};

class Select : public InstrImpl {
  DUMMY_INSTR_IMPL(Select)
};

// TODO: add some abstraction here
class Numeric : public InstrImpl {
  DUMMY_INSTR_IMPL(Numeric)
};

template <typename T>
class ImmediateImpl : public InstrImpl {
public:
  ImmediateImpl(u32 pos, const T &imm) : InstrImpl(pos), value(imm) {}
  ImmediateImpl(u32 pos, T &&imm) : InstrImpl(pos), value(std::move(imm)) {}

protected:
  T value;
};

class Call : public ImmediateImpl<funcidx> {
public:
  Call(byte *bytes, u32 *pos)
      : ImmediateImpl<funcidx>{(*pos)++, funcidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(Call)
};

// define usefull macro for classes that have the same constructor
#define same_constr(ab, b) \
public:                    \
  ab(byte *bytes, u32 *pos) : b(bytes, pos) {}
// end of macro

// ---------------------- LOCAL ----------------------
// class that abstracts all local.<something> instructions
class Local : public ImmediateImpl<localidx> {
public:
  Local(byte *bytes, u32 *pos)
      : ImmediateImpl<localidx>{(*pos)++, localidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(Local)
};

class LocalGet : public Local {
  same_constr(LocalGet, Local) DUMMY_VIRTUAL(Local)
};

class LocalSet : public Local {
  same_constr(LocalSet, Local) DUMMY_VIRTUAL(Local)
};
class LocalTee : public Local {
  same_constr(LocalTee, Local) DUMMY_VIRTUAL(Local)
};
// --------------------------------------------------

// ---------------------- GLOBAL ----------------------
// class that abstracts all local.<something> instructions
class Global : public ImmediateImpl<globalidx> {
public:
  Global(byte *bytes, u32 *pos)
      : ImmediateImpl<globalidx>{(*pos)++, globalidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(Global)
};

class GlobalGet : public Global {
  same_constr(GlobalGet, Global) DUMMY_VIRTUAL(GloGloablGetbal)
};

class GlobalSet : public Global {
  same_constr(GlobalSet, Global) DUMMY_VIRTUAL(GlobalSet)
};
// --------------------------------------------------
}  // namespace Instruction
#endif
