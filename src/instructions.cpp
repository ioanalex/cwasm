#include "all_instructions.hpp"
#include "parse.hpp"
#include "util.hpp"

#define UNIMPLEMENTED FATAL("Instruction is unimplemented, exiting\n")

InstrImpl::InstrImpl(Reader *reader) {
  pos_ = reader->get_pos();
  code_ = reader->read_byte();
}

using namespace Instruction;

Instr Instr::create(Reader *reader) {
  byte opcode = reader->peek_byte();
  debug("Instr::create, reading OPCODE: %hhx <-- %x\n", opcode,
        reader->get_pos());
  profiles.at(opcode).use();
  switch (opcode) {
    case 0x00:
      return Instr(new Unreachable(reader));
    case 0x01:
      return Instr(new Nop(reader));
    case 0x02:
      return Instr(new Block(reader));
    case 0x03:
      return Instr(new Loop(reader));
    case 0x04:
      return Instr(new If(reader));
    case 0x0B:
      return Instr(new End(reader));
    case 0x0c:
      return Instr(new Br(reader));
    case 0x0d:
      return Instr(new Br_If(reader));
    case 0x0e:
      return Instr(new Br_Table(reader));
    case 0x0f:
      return Instr(new Return(reader));
    case 0x10:
      return Instr(new Call(reader));
    case 0x11:
      return Instr(new CallIndirect(reader));

    case 0x1a:
      return Instr(new Drop(reader));
    case 0x1b:
      return Instr(new Select(reader));

    case 0x20:
      return Instr(new LocalGet(reader));
    case 0x21:
      return Instr(new LocalSet(reader));
    case 0x22:
      return Instr(new LocalTee(reader));
    case 0x23:
      return Instr(new GlobalGet(reader));
    case 0x24:
      return Instr(new GlobalSet(reader));

    case 0x28:
      return Instr(new Load(reader, type::Value::i32));
    case 0x29:
      return Instr(new Load(reader, type::Value::i64));
    case 0x2A:
      return Instr(new Load(reader, type::Value::f32));
    case 0x2B:
      return Instr(new Load(reader, type::Value::f64));
    case 0x2C: {
      opt_st_size opt = {_8, _s};
      return Instr(new Load(reader, type::Value::i32, opt));
    }
    case 0x2D: {
      opt_st_size opt = {_8, _u};
      return Instr(new Load(reader, type::Value::i32, opt));
    }
    case 0x2E: {
      opt_st_size opt = {_16, _s};
      return Instr(new Load(reader, type::Value::i32, opt));
    }
    case 0x2F: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(reader, type::Value::i32, opt));
    }
    case 0x30: {
      opt_st_size opt = {_8, _s};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x31: {
      opt_st_size opt = {_8, _u};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x32: {
      opt_st_size opt = {_16, _s};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x33: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x34: {
      opt_st_size opt = {_32, _s};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x35: {
      opt_st_size opt = {_32, _u};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x36:
      return Instr(new Store(reader, type::Value::i32));
    case 0x37:
      return Instr(new Store(reader, type::Value::i64));
    case 0x38:
      return Instr(new Store(reader, type::Value::f32));
    case 0x39:
      return Instr(new Store(reader, type::Value::f64));
    case 0x3a: {
      opt_st_size opt = {_8, _u};  // when storing the sign doesn't matter
      return Instr(new Load(reader, type::Value::i32, opt));
    }
    case 0x3b: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(reader, type::Value::i32, opt));
    }
    case 0x3c: {
      opt_st_size opt = {_8, _u};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x3d: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x3e: {
      opt_st_size opt = {_32, _u};
      return Instr(new Load(reader, type::Value::i64, opt));
    }
    case 0x3f:
      return Instr(new MemorySize(reader));
    case 0x40:
      return Instr(new MemoryGrow(reader));

    case 0x41:
      return Instr(new Const(reader, type::Value::i32));
    case 0x42:
      return Instr(new Const(reader, type::Value::i64));
    case 0x43:
      return Instr(new Const(reader, type::Value::f32));
    case 0x44:
      return Instr(new Const(reader, type::Value::f64));
    case 0x45 ... 0xbf:
      return Instr(new Numeric(reader));
    default:
      FATAL("Instruction: %s(%x) is unimplemented\n",
            profiles.at(opcode).get_name().c_str(),
            profiles.at(opcode).get_code());
  }
}

#undef UNIMPLEMENTED
