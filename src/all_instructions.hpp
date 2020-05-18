/*
  This file defines the hierarchy of Instructions and their constructors.
  Each constructor gets a pointer to the reader and parses itself.
  Macros are used for code that is repeating due to similarities in the spec.
*/

#ifndef __ALL_INSTRUCTIONS_HPP__
#define __ALL_INSTRUCTIONS_HPP__

#include <cstring>
#include <optional>
#include <utility>

#include "Reader.hpp"
#include "instructions.hpp"

namespace Instruction {
#define DUMMY_VIRTUAL(SpecialInstrImpl) \
public:                                 \
  virtual void run() {}                 \
  virtual bool validate() { return true; }

#define DUMMY_INSTR_IMPL(SpecialInstrImpl) \
public:                                    \
  SpecialInstrImpl(Reader *reader) : InstrImpl(reader) {}

/* This class is used to abstract all instructions
 * that have an immediate argument
 */
template <typename T>
class ImmediateImpl : public InstrImpl {
public:
  ImmediateImpl(Reader *reader) : InstrImpl(reader) {}
  void setImmediate(T &imm) { value = std::move(imm); }

protected:
  T value;
};

/* */

class Unreachable : public InstrImpl {
public:
  Unreachable(Reader *reader) : InstrImpl(reader) {}
  void run() {}
  bool validate();
};

class Nop : public InstrImpl {
  DUMMY_INSTR_IMPL(Nop);
  void run() {}
  bool validate();
};

class Block : public InstrImpl {
public:
  Block(Reader *reader) : InstrImpl(reader) {
    debug("BLOCK\n");
    if (reader->unless(0x40)) {
      blocktype = reader->parse_valtype();
    }
    while (reader->unless(0x0B)) {
      instrs.emplace_back(Instr::create(reader));
    }
    debug("ENDBLOCK\n");
  }

  void run() {}
  bool validate();

  vec<Instr> instrs;
  std::optional<type::Value> blocktype;
};

// NOTE: Block and Loop have the same constructor
// (we could use the SAME_CONSTR macro)
class Loop : public InstrImpl {
public:
  Loop(Reader *reader) : InstrImpl(reader) {
    debug("LOOP\n");
    if (reader->unless(0x40)) {
      blocktype = reader->parse_valtype();
    }
    while (reader->unless(0x0B)) {
      instrs.emplace_back(Instr::create(reader));
    }
    debug("ENDLOOP\n");
  }

  void run() {}
  bool validate();

  vec<Instr> instrs;
  std::optional<type::Value> blocktype;
};

class If : public InstrImpl {
public:
  If(Reader *reader) : InstrImpl(reader), has_else(false) {
    debug("IF\n");
    if (reader->unless(0x40)) {
      blocktype = reader->parse_valtype();
    }
    while (reader->upto(0x0B, 0x05)) {
      ifinstrs.emplace_back(Instr::create(reader));
    }
    if (reader->maybe(0x05)) {
      debug("ELSE\n");
      has_else = true;

      while (reader->upto(0x0B)) {
        elseinstrs.emplace_back(Instr::create(reader));
      }
      debug("ENDELSE\n");
    }
    reader->expect(0x0B);
    debug("ENDIF\n");
  }

  void run() {}
  bool validate();

  bool has_else;
  vec<Instr> ifinstrs;
  vec<Instr> elseinstrs;
  std::optional<type::Value> blocktype;
};

// TODO: maybe this should be removed
class End : public InstrImpl {
public:
  End(Reader *reader) : InstrImpl(reader) { debug("END\n"); }
  void run() {}
  bool validate();
};

// ---------------------- BRANCH --------------------
class Br : public ImmediateImpl<labelidx> {
public:
  Br(Reader *reader) : ImmediateImpl<labelidx>(reader) {
    labelidx imm(reader->parse_idx());
    setImmediate(imm);
  }
  void run() {}
  bool validate();
};

class Br_If : public ImmediateImpl<labelidx> {
public:
  Br_If(Reader *reader) : ImmediateImpl<labelidx>(reader) {
    labelidx imm(reader->parse_idx());
    setImmediate(imm);
  }
  void run() {}
  bool validate();
};

class Br_Table : public InstrImpl {
public:
  Br_Table(Reader *reader) : InstrImpl(reader), labels(), labelN(0) {
    u32 count = reader->read_LEB(32);
    while (count-- > 0) {
      labels.emplace_back(labelidx(reader->parse_idx()));
    }
    labelN = labelidx(reader->parse_idx());
  }
  void run() {}
  bool validate();
  vec<labelidx> labels;
  labelidx labelN;
};
// -------------------------------------------------

class Return : public InstrImpl {
  DUMMY_INSTR_IMPL(Return)
  void run() {}
  bool validate();
};

// ---------------------- CALL ----------------------
class Call : public ImmediateImpl<funcidx> {
public:
  Call(Reader *reader) : ImmediateImpl<funcidx>(reader) {
    funcidx f(reader->parse_idx());
    setImmediate(f);
  }
  void run() {}
  bool validate();
};

class CallIndirect : public ImmediateImpl<typeidx> {
public:
  CallIndirect(Reader *reader) : ImmediateImpl<typeidx>(reader) {
    typeidx t(reader->parse_idx());
    setImmediate(t);
    ASSERT(reader->read_byte() == 0x00,
           "Call indirect must have a 0x00 in the end\n");
  }
  void run() {}
  bool validate();
};
// ---------------------------------------------------

class Drop : public InstrImpl {
  DUMMY_INSTR_IMPL(Drop);
  void run() {}
  bool validate();
};

class Select : public InstrImpl {
  DUMMY_INSTR_IMPL(Select);
  void run() {}
  bool validate();
};

// define useful macro for classes that have the same constructor
#define SAME_CONSTR(derived, base) derived(Reader *reader) : base(reader)
// end of macro

// ---------------------- LOCAL ----------------------
// class that abstracts all local.<something> instructions
class Local : public ImmediateImpl<localidx> {
public:
  Local(Reader *reader) : ImmediateImpl<localidx>(reader) {
    localidx l(reader->parse_idx());
    setImmediate(l);
  }
  DUMMY_VIRTUAL(Local)
};

class LocalGet : public Local {
public:
  SAME_CONSTR(LocalGet, Local) {}
  void run() {}
  bool validate();
};

class LocalSet : public Local {
public:
  SAME_CONSTR(LocalSet, Local) {}
  void run() {}
  bool validate();
};

class LocalTee : public Local {
public:
  SAME_CONSTR(LocalTee, Local) {}
  void run() {}
  bool validate();
};
// -------------------------------------------------

// ---------------------- GLOBAL -------------------
// class that abstracts all local.<something> instructions
class Global : public ImmediateImpl<globalidx> {
public:
  Global(Reader *reader) : ImmediateImpl<globalidx>(reader) {
    globalidx g(reader->parse_idx());
    setImmediate(g);
  }
  DUMMY_VIRTUAL(Global)
};

class GlobalGet : public Global {
public:
  SAME_CONSTR(GlobalGet, Global) { set_const(); }
  void run() {}
  bool validate();
};

class GlobalSet : public Global {
public:
  SAME_CONSTR(GlobalSet, Global) {}
  void run() {}
  bool validate();
};
// --------------------------------------------------

// ------------------------- MEMORY ---------------------------
enum _opt_size { _8, _16, _32 };
enum _opt_sign { _u, _s };
struct opt_st_size {
  _opt_size size;
  _opt_sign sign;
};

#define LOAD_STORE(name)                                        \
  class name : public ImmediateImpl<Memarg> {                   \
  public:                                                       \
    name(Reader *reader, type::Value type)                      \
        : ImmediateImpl<Memarg>(reader), type(type) {           \
      Memarg marg(reader->parse_memarg());                      \
      setImmediate(marg);                                       \
    }                                                           \
    name(Reader *reader, type::Value type, opt_st_size opt)     \
        : ImmediateImpl<Memarg>(reader), type(type), opt(opt) { \
      Memarg marg(reader->parse_memarg());                      \
      setImmediate(marg);                                       \
    }                                                           \
    type::Value getType() const { return type; }                \
    void run() {}                                               \
    bool validate();                                            \
                                                                \
  private:                                                      \
    type::Value type;                                           \
    std::optional<opt_st_size> opt;                             \
  };

LOAD_STORE(Load)
LOAD_STORE(Store)

class MemorySize : public InstrImpl {
public:
  MemorySize(Reader *reader) : InstrImpl(reader) {
    ASSERT(reader->read_byte() == 0x00,
           "memory.size reserved byte should be 0\n");
  }
  void run() {}
  bool validate();
};

class MemoryGrow : public InstrImpl {
public:
  MemoryGrow(Reader *reader) : InstrImpl(reader) {
    ASSERT(reader->read_byte() == 0x00,
           "memory.grow reserved byte should be 0\n");
  }
  void run() {}
  bool validate();
};

//---------------------------------------------------------

// ------------------- Numbers ----------------------

class Const : public ImmediateImpl<Value> {
public:
  Const(Reader *reader, type::Value type) : ImmediateImpl<Value>(reader) {
    set_const();
    Value v = reader->parse_value(type);
    setImmediate(v);
  }
  void run() {}
  bool validate();
};

// TODO: add some abstraction here
class Numeric : public InstrImpl {
  DUMMY_INSTR_IMPL(Numeric);
  void run() {}
  bool validate();
};
// --------------------------------------------------

#undef DUMMY_VIRTUAL
#undef DUMMY_INSTR_IMPL
#undef SAME_CONSTR
#undef LOAD_STORE

}  // namespace Instruction

#undef DUMMY_VIRTUAL
#undef DUMMY_INSTR_IMPL
#undef SAME_CONSTR
#undef LOAD_STORE

#endif
