#ifndef __ALL_INSTRUCTIONS_HPP__
#define __ALL_INSTRUCTIONS_HPP__

#include <cstring>
#include <optional>
#include <utility>

#include "binary.hpp"
#include "indices.hpp"
#include "instructions.hpp"
#include "named.hpp"
#include "parse.hpp"

namespace Instruction {
#define DUMMY_VIRTUAL(SpecialInstrImpl) \
public:                                 \
  virtual void run() {}                 \
  virtual bool validate() {}

#define DUMMY_INSTR_IMPL(SpecialInstrImpl)                         \
public:                                                            \
  SpecialInstrImpl(byte *bytes, u32 *pos) : InstrImpl((*pos)++) {} \
  DUMMY_VIRTUAL(SpecialInstrImpl)

/* This class is used to abstract all instructions
 * that have an immediate argument
 */
template <typename T>
class ImmediateImpl : public InstrImpl {
public:
  ImmediateImpl(u32 pos) : InstrImpl(pos) {}
  ImmediateImpl(u32 pos, const T &imm) : InstrImpl(pos), value(imm) {}
  ImmediateImpl(u32 pos, T &&imm) : InstrImpl(pos), value(std::move(imm)) {}

protected:
  T value;
};

/* */

class Unreachable : public InstrImpl {
  DUMMY_INSTR_IMPL(Unreachable)
};

class Nop : public InstrImpl {
  DUMMY_INSTR_IMPL(Nop)
};

class Block : public InstrImpl {
public:
  Block(byte *bytes, u32 *pos) : InstrImpl((*pos)++) {
    debug("BLOCK\n");
    if (bytes[*pos] != 0x40) {
      blocktype = parse_valtype(bytes, pos);
    }
    while (bytes[*pos] != 0x0B) {
      instrs.emplace_back(Instr::create(bytes, pos));
    }
    (*pos)++;  // to skip the END(0x0B) instruction
    debug("ENDBLOCK\n");
  }

  DUMMY_VIRTUAL(Block)

  vec<Instr> instrs;
  std::optional<type::Value> blocktype;
};

// TODO: Block and Loop have the same constructor (we could use the same_constr
// macro)
class Loop : public InstrImpl {
public:
  Loop(byte *bytes, u32 *pos) : InstrImpl((*pos)++) {
    debug("LOOP\n");

    if (bytes[*pos] != 0x40) {
      blocktype = parse_valtype(bytes, pos);
    }
    while (bytes[*pos] != 0x0B) {
      instrs.emplace_back(Instr::create(bytes, pos));
    }
    (*pos)++;  // to skip the END(0x0B) instruction
    debug("ENDLOOP\n");
  }

  DUMMY_VIRTUAL(Loop)

  vec<Instr> instrs;
  std::optional<type::Value> blocktype;
};

class If : public InstrImpl {
public:
  If(byte *bytes, u32 *pos) : InstrImpl((*pos)++) {
    debug("IF\n");
    if (bytes[*pos] != 0x40) {
      blocktype = parse_valtype(bytes, pos);
    }

    while (bytes[*pos] != 0x0B && bytes[*pos] != 0x05) {
      ifinstrs.emplace_back(Instr::create(bytes, pos));
    }
    if (bytes[*pos] == 0x05) {
      debug("ELSE\n");

      (*pos)++;

      while (bytes[*pos] != 0x0B) {
        elseinstrs.emplace_back(Instr::create(bytes, pos));
      }
      debug("ENDELSE\n");
    }
    (*pos)++;  // to skip the END(0x0B) instruction
    debug("ENDIF\n");
  }

  DUMMY_VIRTUAL(If)

  vec<Instr> ifinstrs;
  vec<Instr> elseinstrs;
  std::optional<type::Value> blocktype;
};

// TODO: maybe this should be removed
class End : public InstrImpl {
  DUMMY_INSTR_IMPL(End)
};

// ---------------------- BRANCH --------------------
class Br : public ImmediateImpl<labelidx> {
public:
  Br(byte *bytes, u32 *pos)
      : ImmediateImpl<labelidx>{(*pos)++, labelidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(Br)
};

class Br_If : public ImmediateImpl<labelidx> {
public:
  Br_If(byte *bytes, u32 *pos)
      : ImmediateImpl<labelidx>{(*pos)++, labelidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(Br_If)
};

class Br_Table : public InstrImpl {
public:
  Br_Table(byte *bytes, u32 *pos) : InstrImpl((*pos)++), labels(), labelN(0) {
    u32 count = read_LEB(bytes, pos, 32);
    for (auto i = 0; i < count; i++) {
      labels.emplace_back(labelidx(parse_idx(bytes, pos)));
    }
    labelN = labelidx(parse_idx(bytes, pos));
  }
  DUMMY_VIRTUAL(Br_Table)
  vec<labelidx> labels;
  labelidx labelN;
};
// -------------------------------------------------

class Return : public InstrImpl {
  DUMMY_INSTR_IMPL(Return)
};

// ---------------------- CALL ----------------------
class Call : public ImmediateImpl<funcidx> {
public:
  Call(byte *bytes, u32 *pos)
      : ImmediateImpl<funcidx>{(*pos)++, funcidx(parse_idx(bytes, pos))} {}
  DUMMY_VIRTUAL(Call)
};

class CallIndirect : public ImmediateImpl<typeidx> {
public:
  CallIndirect(byte *bytes, u32 *pos)
      : ImmediateImpl<typeidx>{(*pos)++, typeidx(parse_idx(bytes, pos))} {
    ASSERT(bytes[*pos] == 0x00, "Call indirect must have a 0x00 in the end\n");
    (*pos)++;  // to skip the 0x00
  }
  DUMMY_VIRTUAL(CallIndirect)
};
// ---------------------------------------------------

class Drop : public InstrImpl {
  DUMMY_INSTR_IMPL(Drop)
};

class Select : public InstrImpl {
  DUMMY_INSTR_IMPL(Select)
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
// -------------------------------------------------

// ---------------------- GLOBAL -------------------
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

// ------------------------- MEMORY ---------------------------
enum _opt_size { _8, _16, _32 };
enum _opt_sign { _u, _s };
struct opt_st_size {
  _opt_size size;
  _opt_sign sign;
};

#define LOAD_STORE(name)                                             \
  class name : public ImmediateImpl<Memarg> {                        \
  public:                                                            \
    name(byte *bytes, u32 *pos, type::Value type)                    \
        : ImmediateImpl<Memarg>{(*pos)++, parse_memarg(bytes, pos)}, \
          type(type) {}                                              \
    name(byte *bytes, u32 *pos, type::Value type, opt_st_size opt)   \
        : ImmediateImpl<Memarg>{(*pos)++, parse_memarg(bytes, pos)}, \
          type(type),                                                \
          opt(opt) {}                                                \
    DUMMY_VIRTUAL(name)                                              \
  private:                                                           \
    type::Value type;                                                \
    std::optional<opt_st_size> opt;                                  \
  };

LOAD_STORE(Load)
LOAD_STORE(Store)

class MemorySize : public InstrImpl {
public:
  MemorySize(u32 *pos) : InstrImpl((*pos)++) { warn("MEMORYSIZEINSTR\n"); }
  DUMMY_VIRTUAL(MemorySize)
};

class MemoryGrow : public InstrImpl {
public:
  MemoryGrow(u32 *pos) : InstrImpl((*pos)++) { warn("MEMORYGROWINSTR\n"); }
  DUMMY_VIRTUAL(MemoryGrow)
};

//---------------------------------------------------------

// ------------------- Numbers ----------------------

class Const : public ImmediateImpl<Value> {
public:
  Const(byte *bytes, u32 *pos, type::Value type)
      : ImmediateImpl<Value>((*pos)++) {
    // TODO: write read_value function based on type..
    // This code repeats (parse.cpp:const_eval)
    switch (type) {
      case type::Value::i32:
        v = from_i32(read_LEB(bytes, pos, 32));
        break;
      case type::Value::i64:
        v = from_i64(read_LEB(bytes, pos, 64));
        break;
      case type::Value::f32: {
        f32 f;
        std::memcpy(&f, bytes + *pos, 4);
        v = from_f32(f);
        *pos = *pos + 4;
        break;
      }
      case type::Value::f64: {
        f64 f;
        std::memcpy(&f, bytes + *pos, 8);
        v = from_f64(f);
        *pos = *pos + 8;
        break;
      }
    }
  }
  DUMMY_VIRTUAL(Const)
private:
  Value v;
};

// TODO: add some abstraction here
class Numeric : public InstrImpl {
  DUMMY_INSTR_IMPL(Numeric)
};
// --------------------------------------------------
}  // namespace Instruction

#endif
