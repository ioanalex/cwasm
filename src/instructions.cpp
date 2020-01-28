#include "instructions.hpp"
#include "parse.hpp"

Call::Call(byte *bytes, u32 *pos) {
  funcidx idx(parse_idx(bytes, pos));
  imm = idx;
}

LocalGet::LocalGet(byte *bytes, u32 *pos) {
  localidx idx(parse_idx(bytes, pos));
  imm = idx;
}

Instr Instr::create(byte *bytes, u32 *pos) {
  InstrImpl *i = nullptr;  // TODO: the parsing goes here.
  debug("Entering Instr::create\n");
  byte opcode = bytes[(*pos)++];

  debug("Reading OPCODE: %hhx <-- %x\n", opcode, *pos - 1);
  switch (opcode) {
    case 0x0:
      i = new Unreachable(*pos - 1);
      break;
    case 0x1:
      i = new Nop(*pos - 1);
      break;
    case 0x2:

      break;
    case 0x3:

      break;
    case 0x4:

      break;
    case 0x5:

      break;
    case 0x6:

      break;

    case 0x7:

      break;

    case 0x8:

      break;

    case 0x9:

      break;

    case 0xa:

      break;

    case 0xb:

      break;

    case 0xc:

      break;

    case 0xd:

      break;

    case 0xe:

      break;

    case 0xf:
      i = new Return(*pos - 1);
      break;

    case 0x10:
      i = new Call(bytes, pos);
      break;

    case 0x11:

      break;

    case 0x12:

      break;

    case 0x13:

      break;

    case 0x14:

      break;

    case 0x15:

      break;

    case 0x16:

      break;

    case 0x17:

      break;

    case 0x18:

      break;

    case 0x19:

      break;

    case 0x1a:
      i = new Drop(*pos - 1);
      break;
    case 0x1b:
      i = new Select(*pos - 1);
      break;
    case 0x1c:

      break;

    case 0x1d:

      break;

    case 0x1e:

      break;

    case 0x1f:

      break;

    case 0x20:
      i = new LocalGet(bytes, pos);
      debug("OK\n");
      break;
    case 0x21:

      break;

    case 0x22:

      break;

    case 0x23:

      break;

    case 0x24:

      break;

    case 0x25:

      break;

    case 0x26:

      break;

    case 0x27:

      break;

    case 0x28:

      break;

    case 0x29:

      break;

    case 0x2a:

      break;

    case 0x2b:

      break;

    case 0x2c:

      break;

    case 0x2d:

      break;

    case 0x2e:

      break;

    case 0x2f:

      break;

    case 0x30:

      break;

    case 0x31:

      break;

    case 0x32:

      break;

    case 0x33:

      break;

    case 0x34:

      break;

    case 0x35:

      break;

    case 0x36:

      break;

    case 0x37:

      break;

    case 0x38:

      break;

    case 0x39:

      break;

    case 0x3a:

      break;

    case 0x3b:

      break;

    case 0x3c:

      break;

    case 0x3d:

      break;

    case 0x3e:

      break;

    case 0x3f:

      break;

    case 0x40:

      break;

    case 0x41:

      break;

    case 0x42:

      break;

    case 0x43:

      break;

    case 0x44:

      break;

    case 0x45 ... 0xbf:
      i = new Numeric(*pos - 1);
      break;
  }
  Instr ii(i);
  debug("Returning from Instr::create\n");
  return ii;
}
