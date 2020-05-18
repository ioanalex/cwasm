#include "Reader.hpp"
#include "all_instructions.hpp"
#include "util.hpp"

#define UNIMPLEMENTED FATAL("Instruction is unimplemented, exiting\n")

InstrImpl::InstrImpl(Reader *reader)
    : pos_(reader->get_pos()), code_(reader->read_byte()) {}

using namespace Instruction;

Instr Instr::create(Reader *reader) {
  byte opcode = reader->peek_byte();
  debug("Instr::create, reading OPCODE: %hhx <-- %lx\n", opcode,
        reader->get_pos());
  profiles.at(opcode)->use();
  return Instr(profiles.at(opcode)->create(reader));
}

vec<std::unique_ptr<InstrProfile>> profiles;
vec<InstrProfile *> p_profiles = {
    new InstrFactory<0x00, Unreachable>("unreachable", {ptype(1, true, false)},
                                        {ptype(2, true, false)}),
    new InstrFactory<0x01, Nop>("nop", {}, {}),
    new InstrFactory<0x02, Block>("block [t?]", {}, {ptype(0, true, false)}),
    new InstrFactory<0x03, Loop>("loop [t?]", {}, {ptype(0, true, false)}),
    new InstrFactory<0x04, If>("if [t?]", {type::Value::i32},
                               {ptype(0, true, false)}),
    new InstrFactory<0x05, Nop>("else", {}, {}),
    new InstrFactory<0x0B, End>("end", {}, {}),
    new InstrFactory<0x0C, Br>("br l",
                               {ptype(1, true, false), ptype(0, false, true)},
                               {ptype(2, true, false)}),
    new InstrFactory<0x0D, Br_If>("br_if l",
                                  {ptype(0, false, true), type::Value::i32},
                                  {ptype(0, false, true)}),
    new InstrFactory<0x0E, Br_Table>(
        "br_table l∗ l",
        {ptype(1, true, false), ptype(0, false, true), type::Value::i32},
        {ptype(2, true, false)}),
    new InstrFactory<0x0F, Return>(
        "return", {ptype(1, true, false), ptype(0, false, true)},
        {ptype(2, true, false)}),
    new InstrFactory<0x10, Call>("call x", {ptype(1, true, false)},
                                 {ptype(2, true, false)}),
    new InstrFactory<0x11, CallIndirect>(
        "call_indirect x", {ptype(1, true, false), type::Value::i32},
        {ptype(2, true, false)}),
    new InstrFactory<0x1a, Drop>("drop", {ptype(0, false, false)}, {}),
    new InstrFactory<0x1b, Select>(
        "select",
        {ptype(0, false, false), ptype(0, false, false), type::Value::i32},
        {ptype(0, false, false)}),
    new InstrFactory<0x20, LocalGet>("local.get x", {},
                                     {ptype(0, false, false)}),
    new InstrFactory<0x21, LocalSet>("local.set x", {ptype(0, false, false)},
                                     {}),
    new InstrFactory<0x22, LocalTee>("local.tee x", {ptype(0, false, false)},
                                     {ptype(0, false, false)}),
    new InstrFactory<0x23, GlobalGet>("global.get x", {},
                                      {ptype(0, false, false)}),
    new InstrFactory<0x24, GlobalSet>("global.set x", {ptype(0, false, false)},
                                      {}),
    new InstrFactory<0x28, Load, type::Value>(
        "i32.load memarg", {type::Value::i32}, {type::Value::i32},
        type::Value::i32),
    new InstrFactory<0x29, Load, type::Value>(
        "i64.load memarg", {type::Value::i32}, {type::Value::i64},
        type::Value::i64),
    new InstrFactory<0x2A, Load, type::Value>(
        "f32.load memarg", {type::Value::i32}, {type::Value::f32},
        type::Value::f32),
    new InstrFactory<0x2B, Load, type::Value>(
        "f64.load memarg", {type::Value::i32}, {type::Value::f64},
        type::Value::f64),
    new InstrFactory<0x2C, Load, type::Value, opt_st_size>(
        "i32.load8_s memarg", {type::Value::i32}, {type::Value::i32},
        type::Value::i32, {_8, _s}),
    new InstrFactory<0x2D, Load, type::Value, opt_st_size>(
        "i32.load8_u memarg", {type::Value::i32}, {type::Value::i32},
        type::Value::i32, {_8, _u}),
    new InstrFactory<0x2E, Load, type::Value, opt_st_size>(
        "i32.load16_s memarg", {type::Value::i32}, {type::Value::i32},
        type::Value::i32, {_16, _s}),
    new InstrFactory<0x2F, Load, type::Value, opt_st_size>(
        "i32.load16_u memarg", {type::Value::i32}, {type::Value::i32},
        type::Value::i32, {_16, _u}),
    new InstrFactory<0x30, Load, type::Value, opt_st_size>(
        "i64.load8_s memarg", {type::Value::i32}, {type::Value::i64},
        type::Value::i64, {_8, _s}),
    new InstrFactory<0x31, Load, type::Value, opt_st_size>(
        "i64.load8_u memarg", {type::Value::i32}, {type::Value::i64},
        type::Value::i64, {_8, _u}),
    new InstrFactory<0x32, Load, type::Value, opt_st_size>(
        "i64.load16_s memarg", {type::Value::i32}, {type::Value::i64},
        type::Value::i64, {_16, _s}),
    new InstrFactory<0x33, Load, type::Value, opt_st_size>(
        "i64.load16_u memarg", {type::Value::i32}, {type::Value::i64},
        type::Value::i64, {_16, _u}),
    new InstrFactory<0x34, Load, type::Value, opt_st_size>(
        "i64.load32_s memarg", {type::Value::i32}, {type::Value::i64},
        type::Value::i64, {_32, _s}),
    new InstrFactory<0x35, Load, type::Value, opt_st_size>(
        "i64.load32_u memarg", {type::Value::i32}, {type::Value::i64},
        type::Value::i64, {_32, _u}),
    new InstrFactory<0x36, Store, type::Value>(
        "i32.store memarg", {type::Value::i32, type::Value::i32}, {},
        type::Value::i32),
    new InstrFactory<0x37, Store, type::Value>(
        "i64.store memarg", {type::Value::i32, type::Value::i64}, {},
        type::Value::i64),
    new InstrFactory<0x38, Store, type::Value>(
        "f32.store memarg", {type::Value::i32, type::Value::f32}, {},
        type::Value::f32),
    new InstrFactory<0x39, Store, type::Value>(
        "f64.store memarg", {type::Value::i32, type::Value::f64}, {},
        type::Value::f64),
    // when storing the sign doesn't matter
    new InstrFactory<0x3a, Store, type::Value, opt_st_size>(
        "i32.store8 memarg", {type::Value::i32, type::Value::i32}, {},
        type::Value::i32, {_8, _u}),
    new InstrFactory<0x3b, Store, type::Value, opt_st_size>(
        "i32.store16 memarg", {type::Value::i32, type::Value::i32}, {},
        type::Value::i32, {_16, _u}),
    new InstrFactory<0x3c, Store, type::Value, opt_st_size>(
        "i64.store8 memarg", {type::Value::i32, type::Value::i64}, {},
        type::Value::i64, {_8, _u}),
    new InstrFactory<0x3d, Store, type::Value, opt_st_size>(
        "i64.store16 memarg", {type::Value::i32, type::Value::i64}, {},
        type::Value::i64, {_16, _u}),
    new InstrFactory<0x3e, Store, type::Value, opt_st_size>(
        "i64.store32 memarg", {type::Value::i32, type::Value::i64}, {},
        type::Value::i64, {_32, _u}),
    new InstrFactory<0x3f, MemorySize>("memory.size", {}, {type::Value::i32}),
    new InstrFactory<0x40, MemoryGrow>("memory.grow", {type::Value::i32},
                                       {type::Value::i32}),

    new InstrFactory<0x41, Const, type::Value>(
        "i32.const i32", {}, {type::Value::i32}, type::Value::i32),
    new InstrFactory<0x42, Const, type::Value>(
        "i64.const i64", {}, {type::Value::i64}, type::Value::i64),
    new InstrFactory<0x43, Const, type::Value>(
        "f32.const f32", {}, {type::Value::f32}, type::Value::f32),
    new InstrFactory<0x44, Const, type::Value>(
        "f64.const f64", {}, {type::Value::f64}, type::Value::f64),

    new InstrFactory<0x45, Numeric>("i32.eqz", {type::Value::i32},
                                    {type::Value::i32}),
    new InstrFactory<0x46, Numeric>(
        "i32.eq", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x47, Numeric>(
        "i32.ne", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x48, Numeric>(
        "i32.lt_s", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x49, Numeric>(
        "i32.lt_u", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x4A, Numeric>(
        "i32.gt_s", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x4B, Numeric>(
        "i32.gt_u", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x4C, Numeric>(
        "i32.le_s", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x4D, Numeric>(
        "i32.le_u", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x4E, Numeric>(
        "i32.ge_s", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x4F, Numeric>(
        "i32.ge_u", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x50, Numeric>("i64.eqz", {type::Value::i64},
                                    {type::Value::i32}),
    new InstrFactory<0x51, Numeric>(
        "i64.eq", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x52, Numeric>(
        "i64.ne", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x53, Numeric>(
        "i64.lt_s", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x54, Numeric>(
        "i64.lt_u", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x55, Numeric>(
        "i64.gt_s", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x56, Numeric>(
        "i64.gt_u", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x57, Numeric>(
        "i64.le_s", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x58, Numeric>(
        "i64.le_u", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x59, Numeric>(
        "i64.ge_s", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x5A, Numeric>(
        "i64.ge_u", {type::Value::i64, type::Value::i64}, {type::Value::i32}),
    new InstrFactory<0x5B, Numeric>(
        "f32.eq", {type::Value::f32, type::Value::f32}, {type::Value::i32}),
    new InstrFactory<0x5C, Numeric>(
        "f32.ne", {type::Value::f32, type::Value::f32}, {type::Value::i32}),
    new InstrFactory<0x5D, Numeric>(
        "f32.lt", {type::Value::f32, type::Value::f32}, {type::Value::i32}),
    new InstrFactory<0x5E, Numeric>(
        "f32.gt", {type::Value::f32, type::Value::f32}, {type::Value::i32}),
    new InstrFactory<0x5F, Numeric>(
        "f32.le", {type::Value::f32, type::Value::f32}, {type::Value::i32}),
    new InstrFactory<0x60, Numeric>(
        "f32.ge", {type::Value::f32, type::Value::f32}, {type::Value::i32}),
    new InstrFactory<0x61, Numeric>(
        "f64.eq", {type::Value::f64, type::Value::f64}, {type::Value::i32}),
    new InstrFactory<0x62, Numeric>(
        "f64.ne", {type::Value::f64, type::Value::f64}, {type::Value::i32}),
    new InstrFactory<0x63, Numeric>(
        "f64.lt", {type::Value::f64, type::Value::f64}, {type::Value::i32}),
    new InstrFactory<0x64, Numeric>(
        "f64.gt", {type::Value::f64, type::Value::f64}, {type::Value::i32}),
    new InstrFactory<0x65, Numeric>(
        "f64.le", {type::Value::f64, type::Value::f64}, {type::Value::i32}),
    new InstrFactory<0x66, Numeric>(
        "f64.ge", {type::Value::f64, type::Value::f64}, {type::Value::i32}),
    new InstrFactory<0x67, Numeric>("i32.clz", {type::Value::i32},
                                    {type::Value::i32}),
    new InstrFactory<0x68, Numeric>("i32.ctz", {type::Value::i32},
                                    {type::Value::i32}),
    new InstrFactory<0x69, Numeric>("i32.popcnt", {type::Value::i32},
                                    {type::Value::i32}),
    new InstrFactory<0x6A, Numeric>(
        "i32.add", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x6B, Numeric>(
        "i32.sub", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x6C, Numeric>(
        "i32.mul", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x6D, Numeric>(
        "i32.div_s", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x6E, Numeric>(
        "i32.div_u", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x6F, Numeric>(
        "i32.rem_s", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x70, Numeric>(
        "i32.rem_u", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x71, Numeric>(
        "i32.and", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x72, Numeric>(
        "i32.or", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x73, Numeric>(
        "i32.xor", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x74, Numeric>(
        "i32.shl", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x75, Numeric>(
        "i32.shr_s", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x76, Numeric>(
        "i32.shr_u", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x77, Numeric>(
        "i32.rotl", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x78, Numeric>(
        "i32.rotr", {type::Value::i32, type::Value::i32}, {type::Value::i32}),
    new InstrFactory<0x79, Numeric>("i64.clz", {type::Value::i64},
                                    {type::Value::i64}),
    new InstrFactory<0x7A, Numeric>("i64.ctz", {type::Value::i64},
                                    {type::Value::i64}),
    new InstrFactory<0x7B, Numeric>("i64.popcnt", {type::Value::i64},
                                    {type::Value::i64}),
    new InstrFactory<0x7C, Numeric>(
        "i64.add", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x7D, Numeric>(
        "i64.sub", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x7E, Numeric>(
        "i64.mul", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x7F, Numeric>(
        "i64.div_s", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x80, Numeric>(
        "i64.div_u", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x81, Numeric>(
        "i64.rem_s", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x82, Numeric>(
        "i64.rem_u", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x83, Numeric>(
        "i64.and", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x84, Numeric>(
        "i64.or", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x85, Numeric>(
        "i64.xor", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x86, Numeric>(
        "i64.shl", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x87, Numeric>(
        "i64.shr_s", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x88, Numeric>(
        "i64.shr_u", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x89, Numeric>(
        "i64.rotl", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x8A, Numeric>(
        "i64.rotr", {type::Value::i64, type::Value::i64}, {type::Value::i64}),
    new InstrFactory<0x8B, Numeric>("f32.abs", {type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x8C, Numeric>("f32.neg", {type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x8D, Numeric>("f32.ceil", {type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x8E, Numeric>("f32.floor", {type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x8F, Numeric>("f32.trunc", {type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x90, Numeric>("f32.nearest", {type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x91, Numeric>("f32.sqrt", {type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x92, Numeric>(
        "f32.add", {type::Value::f32, type::Value::f32}, {type::Value::f32}),
    new InstrFactory<0x93, Numeric>(
        "f32.sub", {type::Value::f32, type::Value::f32}, {type::Value::f32}),
    new InstrFactory<0x94, Numeric>(
        "f32.mul", {type::Value::f32, type::Value::f32}, {type::Value::f32}),
    new InstrFactory<0x95, Numeric>(
        "f32.div", {type::Value::f32, type::Value::f32}, {type::Value::f32}),
    new InstrFactory<0x96, Numeric>(
        "f32.min", {type::Value::f32, type::Value::f32}, {type::Value::f32}),
    new InstrFactory<0x97, Numeric>(
        "f32.max", {type::Value::f32, type::Value::f32}, {type::Value::f32}),
    new InstrFactory<0x98, Numeric>("f32.copysign",
                                    {type::Value::f32, type::Value::f32},
                                    {type::Value::f32}),
    new InstrFactory<0x99, Numeric>("f64.abs", {type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0x9A, Numeric>("f64.neg", {type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0x9B, Numeric>("f64.ceil", {type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0x9C, Numeric>("f64.floor", {type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0x9D, Numeric>("f64.trunc", {type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0x9E, Numeric>("f64.nearest", {type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0x9F, Numeric>("f64.sqrt", {type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0xA0, Numeric>(
        "f64.add", {type::Value::f64, type::Value::f64}, {type::Value::f64}),
    new InstrFactory<0xA1, Numeric>(
        "f64.sub", {type::Value::f64, type::Value::f64}, {type::Value::f64}),
    new InstrFactory<0xA2, Numeric>(
        "f64.mul", {type::Value::f64, type::Value::f64}, {type::Value::f64}),
    new InstrFactory<0xA3, Numeric>(
        "f64.div", {type::Value::f64, type::Value::f64}, {type::Value::f64}),
    new InstrFactory<0xA4, Numeric>(
        "f64.min", {type::Value::f64, type::Value::f64}, {type::Value::f64}),
    new InstrFactory<0xA5, Numeric>(
        "f64.max", {type::Value::f64, type::Value::f64}, {type::Value::f64}),
    new InstrFactory<0xA6, Numeric>("f64.copysign",
                                    {type::Value::f64, type::Value::f64},
                                    {type::Value::f64}),
    new InstrFactory<0xA7, Numeric>("i32.wrap_i64", {type::Value::i64},
                                    {type::Value::i32}),
    new InstrFactory<0xA8, Numeric>("i32.trunc_f32_s", {type::Value::f32},
                                    {type::Value::i32}),
    new InstrFactory<0xA9, Numeric>("i32.trunc_f32_u", {type::Value::f32},
                                    {type::Value::i32}),
    new InstrFactory<0xAA, Numeric>("i32.trunc_f64_s", {type::Value::f64},
                                    {type::Value::i32}),
    new InstrFactory<0xAB, Numeric>("i32.trunc_f64_u", {type::Value::f64},
                                    {type::Value::i32}),
    new InstrFactory<0xAC, Numeric>("i64.extend_i32_s", {type::Value::i32},
                                    {type::Value::i64}),
    new InstrFactory<0xAD, Numeric>("i64.extend_i32_u", {type::Value::i32},
                                    {type::Value::i64}),
    new InstrFactory<0xAE, Numeric>("i64.trunc_f32_s", {type::Value::f32},
                                    {type::Value::i64}),
    new InstrFactory<0xAF, Numeric>("i64.trunc_f32_u", {type::Value::f32},
                                    {type::Value::i64}),
    new InstrFactory<0xB0, Numeric>("i64.trunc_f64_s", {type::Value::f64},
                                    {type::Value::i64}),
    new InstrFactory<0xB1, Numeric>("i64.trunc_f64_u", {type::Value::f64},
                                    {type::Value::i64}),
    new InstrFactory<0xB2, Numeric>("f32.convert_i32_s", {type::Value::i32},
                                    {type::Value::f32}),
    new InstrFactory<0xB3, Numeric>("f32.convert_i32_u", {type::Value::i32},
                                    {type::Value::f32}),
    new InstrFactory<0xB4, Numeric>("f32.convert_i64_s", {type::Value::i64},
                                    {type::Value::f32}),
    new InstrFactory<0xB5, Numeric>("f32.convert_i64_u", {type::Value::i64},
                                    {type::Value::f32}),
    new InstrFactory<0xB6, Numeric>("f32.demote_f64", {type::Value::f64},
                                    {type::Value::f32}),
    new InstrFactory<0xB7, Numeric>("f64.convert_i32_s", {type::Value::i32},
                                    {type::Value::f64}),
    new InstrFactory<0xB8, Numeric>("f64.convert_i32_u", {type::Value::i32},
                                    {type::Value::f64}),
    new InstrFactory<0xB9, Numeric>("f64.convert_i64_s", {type::Value::i64},
                                    {type::Value::f64}),
    new InstrFactory<0xBA, Numeric>("f64.convert_i64_u", {type::Value::i64},
                                    {type::Value::f64}),
    new InstrFactory<0xBB, Numeric>("f64.promote_f32", {type::Value::f32},
                                    {type::Value::f64}),
    new InstrFactory<0xBC, Numeric>("i32.reinterpret_f32", {type::Value::f32},
                                    {type::Value::i32}),
    new InstrFactory<0xBD, Numeric>("i64.reinterpret_f64", {type::Value::f64},
                                    {type::Value::i64}),
    new InstrFactory<0xBE, Numeric>("f32.reinterpret_i32", {type::Value::i32},
                                    {type::Value::f32}),
    new InstrFactory<0xBF, Numeric>("f64.reinterpret_i64", {type::Value::i64},
                                    {type::Value::f64}),

    // missing 0xC0-0xC4, extend
    // missing 0xFC 0x00-0x07, trunc_sat
};

#if 0
  switch (opcode) {
    default:
      FATAL("Instruction: %s(%x) is unimplemented\n",
            profiles.at(opcode).get_name().c_str(),
            profiles.at(opcode).get_code());
  }
}
#endif

#undef UNIMPLEMENTED
