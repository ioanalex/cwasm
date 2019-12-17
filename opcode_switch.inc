case 0x00:
name = "unreachable";
type.args.push_back(ptype(1, true, false));
type.ret.push_back(ptype(2, true, false));
break;
case 0x01:
name = "nop";


break;
case 0x02:
name = "block [t?]";

type.ret.push_back(ptype(0, true, false));
break;
case 0x03:
name = "loop [t?]";

type.ret.push_back(ptype(0, true, false));
break;
case 0x04:
name = "if [t?]";
type.args.push_back(type::Value::i32);
type.ret.push_back(ptype(0, true, false));
break;
case 0x05:
name = "else";
case 0x0B:
name = "end";
case 0x0C:
name = "br l";
type.args.push_back(ptype(1, true, false));
type.args.push_back(ptype(0, false, true));
type.ret.push_back(ptype(2, true, false));
break;
case 0x0D:
name = "br_if l";
type.args.push_back(ptype(0, false, true));
type.args.push_back(type::Value::i32);
type.ret.push_back(ptype(0, false, true));
break;
case 0x0E:
name = "br_table l∗ l";
type.args.push_back(ptype(1, true, false));
type.args.push_back(ptype(0, false, true));
type.args.push_back(type::Value::i32);
type.ret.push_back(ptype(2, true, false));
break;
case 0x0F:
name = "return";
type.args.push_back(ptype(1, true, false));
type.args.push_back(ptype(0, false, true));
type.ret.push_back(ptype(2, true, false));
break;
case 0x10:
name = "call x";
type.args.push_back(ptype(1, true, false));
type.ret.push_back(ptype(2, true, false));
break;
case 0x11:
name = "call_indirect x";
type.args.push_back(ptype(1, true, false));
type.args.push_back(type::Value::i32);
type.ret.push_back(ptype(2, true, false));
break;
case 0x1A:
name = "drop";
type.args.push_back(ptype(0, false, false));

break;
case 0x1B:
name = "select";
type.args.push_back(ptype(0, false, false));
type.args.push_back(ptype(0, false, false));
type.args.push_back(type::Value::i32);
type.ret.push_back(ptype(0, false, false));
break;
case 0x20:
name = "local.get x";

type.ret.push_back(ptype(0, false, false));
break;
case 0x21:
name = "local.set x";
type.args.push_back(ptype(0, false, false));

break;
case 0x22:
name = "local.tee x";
type.args.push_back(ptype(0, false, false));
type.ret.push_back(ptype(0, false, false));
break;
case 0x23:
name = "global.get x";

type.ret.push_back(ptype(0, false, false));
break;
case 0x24:
name = "global.set x";
type.args.push_back(ptype(0, false, false));

break;
case 0x28:
name = "i32.load memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x29:
name = "i64.load memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0x2A:
name = "f32.load memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::f32);
break;
case 0x2B:
name = "f64.load memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::f64);
break;
case 0x2C:
name = "i32.load8_s memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x2D:
name = "i32.load8_u memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x2E:
name = "i32.load16_s memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x2F:
name = "i32.load16_u memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x30:
name = "i64.load8_s memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0x31:
name = "i64.load8_u memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0x32:
name = "i64.load16_s memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0x33:
name = "i64.load16_u memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0x34:
name = "i64.load32_s memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0x35:
name = "i64.load32_u memarg";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0x36:
name = "i32.store memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);

break;
case 0x37:
name = "i64.store memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i64);

break;
case 0x38:
name = "f32.store memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::f32);

break;
case 0x39:
name = "f64.store memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::f64);

break;
case 0x3A:
name = "i32.store8 memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);

break;
case 0x3B:
name = "i32.store16 memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);

break;
case 0x3C:
name = "i64.store8 memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i64);

break;
case 0x3D:
name = "i64.store16 memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i64);

break;
case 0x3E:
name = "i64.store32 memarg";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i64);

break;
case 0x3F:
name = "memory.size";

type.ret.push_back(type::Value::i32);
break;
case 0x40:
name = "memory.grow";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x41:
name = "i32.const i32";

type.ret.push_back(type::Value::i32);
break;
case 0x42:
name = "i64.const i64";

type.ret.push_back(type::Value::i64);
break;
case 0x43:
name = "f32.const f32";

type.ret.push_back(type::Value::f32);
break;
case 0x44:
name = "f64.const f64";

type.ret.push_back(type::Value::f64);
break;
case 0x45:
name = "i32.eqz";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x46:
name = "i32.eq";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x47:
name = "i32.ne";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x48:
name = "i32.lt_s";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x49:
name = "i32.lt_u";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x4A:
name = "i32.gt_s";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x4B:
name = "i32.gt_u";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x4C:
name = "i32.le_s";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x4D:
name = "i32.le_u";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x4E:
name = "i32.ge_s";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x4F:
name = "i32.ge_u";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x50:
name = "i64.eqz";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x51:
name = "i64.eq";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x52:
name = "i64.ne";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x53:
name = "i64.lt_s";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x54:
name = "i64.lt_u";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x55:
name = "i64.gt_s";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x56:
name = "i64.gt_u";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x57:
name = "i64.le_s";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x58:
name = "i64.le_u";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x59:
name = "i64.ge_s";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x5A:
name = "i64.ge_u";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0x5B:
name = "f32.eq";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0x5C:
name = "f32.ne";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0x5D:
name = "f32.lt";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0x5E:
name = "f32.gt";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0x5F:
name = "f32.le";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0x60:
name = "f32.ge";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0x61:
name = "f64.eq";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0x62:
name = "f64.ne";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0x63:
name = "f64.lt";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0x64:
name = "f64.gt";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0x65:
name = "f64.le";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0x66:
name = "f64.ge";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0x67:
name = "i32.clz";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x68:
name = "i32.ctz";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x69:
name = "i32.popcnt";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x6A:
name = "i32.add";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x6B:
name = "i32.sub";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x6C:
name = "i32.mul";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x6D:
name = "i32.div_s";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x6E:
name = "i32.div_u";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x6F:
name = "i32.rem_s";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x70:
name = "i32.rem_u";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x71:
name = "i32.and";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x72:
name = "i32.or";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x73:
name = "i32.xor";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x74:
name = "i32.shl";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x75:
name = "i32.shr_s";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x76:
name = "i32.shr_u";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x77:
name = "i32.rotl";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x78:
name = "i32.rotr";
type.args.push_back(type::Value::i32);
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i32);
break;
case 0x79:
name = "i64.clz";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x7A:
name = "i64.ctz";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x7B:
name = "i64.popcnt";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x7C:
name = "i64.add";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x7D:
name = "i64.sub";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x7E:
name = "i64.mul";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x7F:
name = "i64.div_s";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x80:
name = "i64.div_u";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x81:
name = "i64.rem_s";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x82:
name = "i64.rem_u";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x83:
name = "i64.and";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x84:
name = "i64.or";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x85:
name = "i64.xor";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x86:
name = "i64.shl";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x87:
name = "i64.shr_s";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x88:
name = "i64.shr_u";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x89:
name = "i64.rotl";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x8A:
name = "i64.rotr";
type.args.push_back(type::Value::i64);
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i64);
break;
case 0x8B:
name = "f32.abs";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x8C:
name = "f32.neg";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x8D:
name = "f32.ceil";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x8E:
name = "f32.floor";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x8F:
name = "f32.trunc";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x90:
name = "f32.nearest";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x91:
name = "f32.sqrt";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x92:
name = "f32.add";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x93:
name = "f32.sub";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x94:
name = "f32.mul";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x95:
name = "f32.div";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x96:
name = "f32.min";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x97:
name = "f32.max";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x98:
name = "f32.copysign";
type.args.push_back(type::Value::f32);
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f32);
break;
case 0x99:
name = "f64.abs";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0x9A:
name = "f64.neg";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0x9B:
name = "f64.ceil";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0x9C:
name = "f64.floor";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0x9D:
name = "f64.trunc";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0x9E:
name = "f64.nearest";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0x9F:
name = "f64.sqrt";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA0:
name = "f64.add";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA1:
name = "f64.sub";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA2:
name = "f64.mul";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA3:
name = "f64.div";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA4:
name = "f64.min";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA5:
name = "f64.max";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA6:
name = "f64.copysign";
type.args.push_back(type::Value::f64);
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f64);
break;
case 0xA7:
name = "i32.wrap_i64";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::i32);
break;
case 0xA8:
name = "i32.trunc_f32_s";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0xA9:
name = "i32.trunc_f32_u";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0xAA:
name = "i32.trunc_f64_s";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0xAB:
name = "i32.trunc_f64_u";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i32);
break;
case 0xAC:
name = "i64.extend_i32_s";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0xAD:
name = "i64.extend_i32_u";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::i64);
break;
case 0xAE:
name = "i64.trunc_f32_s";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i64);
break;
case 0xAF:
name = "i64.trunc_f32_u";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i64);
break;
case 0xB0:
name = "i64.trunc_f64_s";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i64);
break;
case 0xB1:
name = "i64.trunc_f64_u";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i64);
break;
case 0xB2:
name = "f32.convert_i32_s";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::f32);
break;
case 0xB3:
name = "f32.convert_i32_u";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::f32);
break;
case 0xB4:
name = "f32.convert_i64_s";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::f32);
break;
case 0xB5:
name = "f32.convert_i64_u";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::f32);
break;
case 0xB6:
name = "f32.demote_f64";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::f32);
break;
case 0xB7:
name = "f64.convert_i32_s";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::f64);
break;
case 0xB8:
name = "f64.convert_i32_u";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::f64);
break;
case 0xB9:
name = "f64.convert_i64_s";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::f64);
break;
case 0xBA:
name = "f64.convert_i64_u";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::f64);
break;
case 0xBB:
name = "f64.promote_f32";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::f64);
break;
case 0xBC:
name = "i32.reinterpret_f32";
type.args.push_back(type::Value::f32);
type.ret.push_back(type::Value::i32);
break;
case 0xBD:
name = "i64.reinterpret_f64";
type.args.push_back(type::Value::f64);
type.ret.push_back(type::Value::i64);
break;
case 0xBE:
name = "f32.reinterpret_i32";
type.args.push_back(type::Value::i32);
type.ret.push_back(type::Value::f32);
break;
case 0xBF:
name = "f64.reinterpret_i64";
type.args.push_back(type::Value::i64);
type.ret.push_back(type::Value::f64);
break;
