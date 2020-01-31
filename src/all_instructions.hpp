#ifndef __ALL_INSTRUCTIONS_HPP__
#define __ALL_INSTRUCTIONS_HPP__

#include <utility>

#include "indices.hpp"
#include "instructions.hpp"
#include "named.hpp"
#include "parse.hpp"

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
      : ImmediateImpl<funcidx>{*pos++, funcidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(Call)
};

class LocalGet : public ImmediateImpl<localidx> {
public:
  LocalGet(byte *bytes, u32 *pos)
      : ImmediateImpl<localidx>{*pos++, localidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(LocalGet)
};

#endif
