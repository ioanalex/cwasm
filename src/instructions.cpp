#include "all_instructions.hpp"
#include "global.hpp"
#include "util.hpp"

#define UNIMPLEMENTED FATAL("Instruction is unimplemented, exiting\n")

using namespace Instruction;

Instr Instr::create(byte *bytes, u32 *pos) {
  byte opcode = bytes[*pos];
  debug("Instr::create, reading OPCODE: %hhx <-- %x\n", opcode, *pos);
  profiles.at(opcode).use();
  switch (opcode) {
    case 0x00:
      return Instr(new Unreachable(bytes, pos));
    case 0x01:
      return Instr(new Nop(bytes, pos));
    case 0x02:
      return Instr(new Block(bytes, pos));
    case 0x03:
      return Instr(new Loop(bytes, pos));
    case 0x04:
      return Instr(new If(bytes, pos));
    case 0x0B:
      return Instr(new End(bytes, pos));
    case 0x0c:
      return Instr(new Br(bytes, pos));
    case 0x0d:
      return Instr(new Br_If(bytes, pos));
    case 0x0e:
      return Instr(new Br_Table(bytes, pos));
    case 0x0f:
      return Instr(new Return(bytes, pos));
    case 0x10:
      return Instr(new Call(bytes, pos));
    case 0x11:
      return Instr(new CallIndirect(bytes, pos));

    case 0x1a:
      return Instr(new Drop(bytes, pos));
    case 0x1b:
      return Instr(new Select(bytes, pos));

    case 0x20:
      return Instr(new LocalGet(bytes, pos));
    case 0x21:
      return Instr(new LocalSet(bytes, pos));
    case 0x22:
      return Instr(new LocalTee(bytes, pos));
    case 0x23:
      return Instr(new GlobalGet(bytes, pos));
    case 0x24:
      return Instr(new GlobalSet(bytes, pos));

    case 0x28:
      return Instr(new Load(bytes, pos, type::Value::i32));
    case 0x29:
      return Instr(new Load(bytes, pos, type::Value::i64));
    case 0x2A:
      return Instr(new Load(bytes, pos, type::Value::f32));
    case 0x2B:
      return Instr(new Load(bytes, pos, type::Value::f64));
    case 0x2C: {
      opt_st_size opt = {_8, _s};
      return Instr(new Load(bytes, pos, type::Value::i32, opt));
    }
    case 0x2D: {
      opt_st_size opt = {_8, _u};
      return Instr(new Load(bytes, pos, type::Value::i32, opt));
    }
    case 0x2E: {
      opt_st_size opt = {_16, _s};
      return Instr(new Load(bytes, pos, type::Value::i32, opt));
    }
    case 0x2F: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(bytes, pos, type::Value::i32, opt));
    }
    case 0x30: {
      opt_st_size opt = {_8, _s};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x31: {
      opt_st_size opt = {_8, _u};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x32: {
      opt_st_size opt = {_16, _s};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x33: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x34: {
      opt_st_size opt = {_32, _s};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x35: {
      opt_st_size opt = {_32, _u};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x36:
      return Instr(new Store(bytes, pos, type::Value::i32));
    case 0x37:
      return Instr(new Store(bytes, pos, type::Value::i64));
    case 0x38:
      return Instr(new Store(bytes, pos, type::Value::f32));
    case 0x39:
      return Instr(new Store(bytes, pos, type::Value::f64));
    case 0x3a: {
      opt_st_size opt = {_8, _u};  // when storing the sign doesn't matter
      return Instr(new Load(bytes, pos, type::Value::i32, opt));
    }
    case 0x3b: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(bytes, pos, type::Value::i32, opt));
    }
    case 0x3c: {
      opt_st_size opt = {_8, _u};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x3d: {
      opt_st_size opt = {_16, _u};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x3e: {
      opt_st_size opt = {_32, _u};
      return Instr(new Load(bytes, pos, type::Value::i64, opt));
    }
    case 0x3f:
      return Instr(new MemorySize(pos));
    case 0x40:
      return Instr(new MemoryGrow(pos));

    case 0x41:
      return Instr(new Const(bytes, pos, type::Value::i32));
    case 0x42:
      return Instr(new Const(bytes, pos, type::Value::i64));
    case 0x43:
      return Instr(new Const(bytes, pos, type::Value::f32));
    case 0x44:
      return Instr(new Const(bytes, pos, type::Value::f64));
    case 0x45 ... 0xbf:
      return Instr(new Numeric(bytes, pos));
    default:
      FATAL("Instruction: %s(%x) is unimplemented\n",
            profiles.at(bytes[*pos]).get_name().c_str(),
            profiles.at(bytes[*pos]).get_code());
  }
}

#undef UNIMPLEMENTED
