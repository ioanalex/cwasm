#include "all_instructions.hpp"

#define UNIMPLEMENTED return Instr(nullptr)

Instr Instr::create(byte *bytes, u32 *pos) {
  byte opcode = bytes[*pos];
  debug("Instr::create, reading OPCODE: %hhx <-- %x\n", opcode, *pos);
  switch (opcode) {
    case 0x00:
      return Instr(new Unreachable(bytes, pos));
    case 0x01:
      return Instr(new Nop(bytes, pos));
    case 0x02:
      UNIMPLEMENTED;
    case 0x03:
      UNIMPLEMENTED;
    case 0x04:
      UNIMPLEMENTED;
    case 0x05:
      UNIMPLEMENTED;
    case 0x06:
      UNIMPLEMENTED;
    case 0x07:
      UNIMPLEMENTED;
    case 0x08:
      UNIMPLEMENTED;
    case 0x09:
      UNIMPLEMENTED;
    case 0x0a:
      UNIMPLEMENTED;
    case 0x0b:
      return Instr(new End(bytes, pos));
    case 0x0c:
      UNIMPLEMENTED;
    case 0x0d:
      UNIMPLEMENTED;
    case 0x0e:
      UNIMPLEMENTED;
    case 0x0f:
      return Instr(new Return(bytes, pos));
    case 0x10:
      return Instr(new Call(bytes, pos));
    case 0x11:
      UNIMPLEMENTED;
    case 0x12:
      UNIMPLEMENTED;
    case 0x13:
      UNIMPLEMENTED;
    case 0x14:
      UNIMPLEMENTED;
    case 0x15:
      UNIMPLEMENTED;
    case 0x16:
      UNIMPLEMENTED;
    case 0x17:
      UNIMPLEMENTED;
    case 0x18:
      UNIMPLEMENTED;
    case 0x19:
      UNIMPLEMENTED;
    case 0x1a:
      return Instr(new Drop(bytes, pos));
    case 0x1b:
      return Instr(new Select(bytes, pos));
    case 0x1c:
      UNIMPLEMENTED;
    case 0x1d:
      UNIMPLEMENTED;
    case 0x1e:
      UNIMPLEMENTED;
    case 0x1f:
      UNIMPLEMENTED;
    case 0x20:
      return Instr(new LocalGet(bytes, pos));
    case 0x21:
      UNIMPLEMENTED;
    case 0x22:
      UNIMPLEMENTED;
    case 0x23:
      UNIMPLEMENTED;
    case 0x24:
      UNIMPLEMENTED;
    case 0x25:
      UNIMPLEMENTED;
    case 0x26:
      UNIMPLEMENTED;
    case 0x27:
      UNIMPLEMENTED;
    case 0x28:
      UNIMPLEMENTED;
    case 0x29:
      UNIMPLEMENTED;
    case 0x2a:
      UNIMPLEMENTED;
    case 0x2b:
      UNIMPLEMENTED;
    case 0x2c:
      UNIMPLEMENTED;
    case 0x2d:
      UNIMPLEMENTED;
    case 0x2e:
      UNIMPLEMENTED;
    case 0x2f:
      UNIMPLEMENTED;
    case 0x30:
      UNIMPLEMENTED;
    case 0x31:
      UNIMPLEMENTED;
    case 0x32:
      UNIMPLEMENTED;
    case 0x33:
      UNIMPLEMENTED;
    case 0x34:
      UNIMPLEMENTED;
    case 0x35:
      UNIMPLEMENTED;
    case 0x36:
      UNIMPLEMENTED;
    case 0x37:
      UNIMPLEMENTED;
    case 0x38:
      UNIMPLEMENTED;
    case 0x39:
      UNIMPLEMENTED;
    case 0x3a:
      UNIMPLEMENTED;
    case 0x3b:
      UNIMPLEMENTED;
    case 0x3c:
      UNIMPLEMENTED;
    case 0x3d:
      UNIMPLEMENTED;
    case 0x3e:
      UNIMPLEMENTED;
    case 0x3f:
      UNIMPLEMENTED;
    case 0x40:
      UNIMPLEMENTED;
    case 0x41:
      UNIMPLEMENTED;
    case 0x42:
      UNIMPLEMENTED;
    case 0x43:
      UNIMPLEMENTED;
    case 0x44:
      UNIMPLEMENTED;
    case 0x45 ... 0xbf:
      return Instr(new Numeric(bytes, pos));
    default:
      UNIMPLEMENTED;
  }
}
