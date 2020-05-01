#include <cstring>

#include "ast.hpp"
#include "binary.hpp"
#include "global.hpp"
#include "types.hpp"
#include "util.hpp"
#include "values.hpp"

u32 parse_idx(byte *bytes, u32 *pos) { return read_LEB(bytes, pos, 32); }

type::Value parse_valtype(byte *bytes, u32 *pos) {
  u32 encoded_type = read_byte(bytes, pos);
  // debug("valtype at addr:%x -> |%x|\n", *pos - 1, encoded_type);
  return decode_type(encoded_type);
}

type::Elem parse_elemtype(byte *bytes, u32 *pos) {
  ASSERT(read_LEB(bytes, pos, 7) == 0x70, "elemtype can only be funcref\n");
  return type::Elem::funcref;
}

type::Limits parse_limits(byte *bytes, u32 *pos) {
  type::Limits limits;
  u32 has_max = bytes[*pos];
  *pos = *pos + 1;
  ASSERT(has_max <= 1, "has_max is either 0x00 or 0x01\n");
  limits.min = read_LEB(bytes, pos, 32);
  if (has_max) {
    limits.max = read_LEB(bytes, pos, 32);
  }
  return limits;
}

type::Table parse_tabletype(byte *bytes, u32 *pos) {
  type::Table table;
  table.elem = parse_elemtype(bytes, pos);
  table.limits = parse_limits(bytes, pos);
  return table;
}

type::Memory parse_memtype(byte *bytes, u32 *pos) {
  type::Memory mem;
  mem.limits = parse_limits(bytes, pos);
  return mem;
}

type::Global parse_globaltype(byte *bytes, u32 *pos) {
  type::Global global;
  // debug(">>>>>>>>>>>>>>>>>\n");
  // for(int i = 0; i < 20; i++){
  //      debug("addr: %x -> |%x|\n", *pos + i, bytes[*pos + i]);
  // }
  // debug("<<<<<<<<<<<<<<<<\n");
  global.value = parse_valtype(bytes, pos);
  ASSERT(bytes[*pos] < 0x02, "mut can be either 0 or 1, malformed\n");
  global.mut = (bytes[*pos] == 0x01);
  *pos = *pos + 1;
  return global;
}

type::Block parse_blocktype(byte *bytes, u32 *pos) {
  type::Block block;
  byte check = bytes[*pos];
  if (check == 0x40) {
    *pos = *pos + 1;
    // by default empty type
    return block;
  } else {
    block.has_type = true;
    block.type = parse_valtype(bytes, pos);
    return block;
  }
}

Memarg parse_memarg(byte *bytes, u32 *pos) {
  Memarg memarg;
  memarg.align = read_LEB(bytes, pos, 32);
  memarg.offset = read_LEB(bytes, pos, 32);
  return memarg;
}

void parse_expr(Expr &e, byte *bytes, u32 *pos) {
  warn("Enter -\n");
  unsigned int instr_count = 0;
  while (true) {
    warn("Parsing instr[%d] at %x\n", instr_count, *pos);
    e.emplace_back(Instr::create(bytes, pos));
    warn("emplaced\n");
    if (e.back().code() == 0x0B) break;
    instr_count++;
#if WAIT
    WaitEnter();
#endif
  }
  warn("Exit -\n");
  ASSERT(bytes[*pos - 1] == 0x0b, "Expressions end with the 0x0b code\n");
}

void parse_types(byte *bytes, u32 *pos, vec<type::Func> *types) {
  u32 type_count = read_LEB(bytes, pos, 32);
  for (u32 t = 0; t < type_count; t++) {
    type::Func newtype;
    ASSERT(read_LEB(bytes, pos, 7) == 0x60,
           "Functions are encoded with the 0x60 code\n");
    u32 arg_count = read_LEB(bytes, pos, 32);
    for (u32 a = 0; a < arg_count; a++)
      newtype.args.emplace_back(parse_valtype(bytes, pos));
    u32 res_count = read_LEB(bytes, pos, 32);
    for (u32 r = 0; r < res_count; r++)
      newtype.result.emplace_back(parse_valtype(bytes, pos));
    // std::cout << types[t] << std::endl;
    types->push_back(newtype);
  }
}

void parse_imports(byte *bytes, u32 *pos, vec<Import> *imports) {
  u32 import_count = read_LEB(bytes, pos, 32);
  for (u32 im = 0; im < import_count; im++) {
    // parse each import
    // import ::= {mod : Name, name : Name, d : importdesc}
    type::Name import_module = read_name(bytes, pos);
    type::Name import_name = read_name(bytes, pos);
    // importdesc :: = kind : {0,1,2,3}, content: depends on kind
    u32 kind = read_byte(bytes, pos);
    // debug("import: %d/%d, kind: %d, %s.%s\n",im+1,import_count, kind,
    // import_module.c_str(), import_name.c_str());

    importdesc *desc;
    switch (kind) {
      case 0: {
        typeidx func(parse_idx(bytes, pos));
        desc = new importdesc(func);
        break;
      }
      case 1: {
        type::Table table;
        table = parse_tabletype(bytes, pos);
        desc = new importdesc(table);
        break;
      }
      case 2: {
        type::Memory mem;
        mem = parse_memtype(bytes, pos);
        desc = new importdesc(mem);
        break;
      }
      case 3: {
        type::Global global;
        global = parse_globaltype(bytes, pos);
        desc = new importdesc(global);
        break;
      }
      default:
        FATAL("You imported something with kind %d, that I cannot understand\n",
              kind);
    }
    imports->emplace_back(Import(import_name, import_module, *desc));
    delete desc;
  }
}

void parse_funcs(byte *bytes, u32 *pos, vec<Func> *funcs) {
  u32 func_count = read_LEB(bytes, pos, 32);
  for (unsigned int i = 0; i < func_count; i++) {
    typeidx tid(parse_idx(bytes, pos));
    funcs->emplace_back(Func(tid));
  }
}

void parse_tables(byte *bytes, u32 *pos, vec<Table> *tables) {
  u32 table_count = read_LEB(bytes, pos, 32);
  for (unsigned int i = 0; i < table_count; i++)
    tables->emplace_back(parse_tabletype(bytes, pos));
}

void parse_mems(byte *bytes, u32 *pos, vec<Memory> *mems) {
  u32 table_count = read_LEB(bytes, pos, 32);
  for (unsigned int i = 0; i < table_count; i++)
    mems->emplace_back(parse_memtype(bytes, pos));
}

void parse_globals(byte *bytes, u32 *pos, vec<Global> *globals) {
  u32 global_count = read_LEB(bytes, pos, 32);
  for (unsigned int i = 0; i < global_count; i++) {
    type::Global globaltype = parse_globaltype(bytes, pos);

    // we donot const_eval anything, just store the instructions
    // after the validation we can const eval (don't use the function
    // above, write a better one)
    globals->emplace_back(Global(globaltype));
    parse_expr(globals->back().init, bytes, pos);
  }
}

void parse_exports(byte *bytes, u32 *pos, vec<Export> *exports) {
  u32 export_count = read_LEB(bytes, pos, 32);

  for (unsigned int i = 0; i < export_count; i++) {
    type::Name export_name = read_name(bytes, pos);

    byte kind = read_byte(
        bytes,
        pos);  // TODO: this should be in the binary file as read_byte(..)
    exportdesc *desc;
    u32 x = parse_idx(bytes, pos);
    switch (kind) {
      case 0x00: {
        funcidx func(x);
        desc = new exportdesc(func);
        break;
      }
      case 0x01: {
        tableidx table(x);
        desc = new exportdesc(table);
        break;
      }
      case 0x02: {
        memidx mem(x);
        desc = new exportdesc(mem);
        break;
      }
      case 0x03: {
        globalidx global(x);
        desc = new exportdesc(global);
        break;
      }
      default:
        FATAL(
            "You are exporting something with kind %d, that I cannot "
            "understand\n",
            kind);
    }
    exports->emplace_back(Export(export_name, *desc));
    delete desc;
  }
}

void parse_elems(byte *bytes, u32 *pos, vec<Elem> *elems,
                 vec<Global> &globals) {
  u32 elem_count = read_LEB(bytes, pos, 32);
  for (unsigned int i = 0; i < elem_count; i++) {
    tableidx x(parse_idx(bytes, pos));
    elems->emplace_back(Elem(x));

    parse_expr(elems->back().offset, bytes, pos);

    u32 func_count = read_LEB(bytes, pos, 32);
    vec<funcidx> &funcs = elems->back().init;
    for (unsigned int j = 0; j < func_count; j++)
      funcs.emplace_back(funcidx(parse_idx(bytes, pos)));
  }
}

void parse_codes(byte *bytes, u32 *pos, vec<Func> &funcs) {
  u32 code_count = read_LEB(bytes, pos, 32);
  u32 imported_func_count = funcs.size() - code_count;
  debug("imported: %u    code for: %u    total: %lu\n", imported_func_count,
        code_count, funcs.size());
  ASSERT(code_count <= funcs.size(), "Mismatch in number of codes: %lu vs %u\n",
         funcs.size(), code_count);

  for (unsigned int i = 0; i < code_count; i++) {
    u32 size = read_LEB(bytes, pos, 32);
    USE(size);  // TODO: verification
    u32 locals_count = read_LEB(bytes, pos, 32);
    warn("%d locals\n", locals_count);
    for (unsigned int j = 0; j < locals_count; j++) {
      u32 n = read_LEB(bytes, pos, 32);
      ASSERT((int)n >= 0, "Overflow will not kill me!\n");
      type::Value t(parse_valtype(bytes, pos));
      // warn("n = %d\n", (int)n);
      for (unsigned int k = 0; k < n; k++) {
        funcs[i + imported_func_count].locals.push_back(t);
      }
    }

#if DEBUG
    for (unsigned int i = imported_func_count; i < funcs.size(); ++i) {
      for (auto l : funcs[i].locals) {
        std::cout << l << std::endl;
      }
    }
#endif
    debug("About to parse_expr\n");
    parse_expr(funcs[i + imported_func_count].body, bytes, pos);
    debug("OK parse_expr\n");
  }
}

void parse_datas(byte *bytes, u32 *pos, vec<Data> *datas,
                 vec<Global> &globals) {
  u32 data_count = read_LEB(bytes, pos, 32);
  ASSERT((int)data_count >= 0, "Overflow will not kill me!\n");
  for (unsigned int i = 0; i < data_count; i++) {
    memidx x(parse_idx(bytes, pos));
    datas->emplace_back(Data(x));

    parse_expr(datas->back().offset, bytes, pos);
    // warn("Expr, ok!\n");

    u32 byte_count = read_LEB(bytes, pos, 32);
    ASSERT((int)byte_count >= 0, "Overflow will not kill me!\n");
    vec<byte> bs(bytes + *pos, bytes + *pos + byte_count);
    *pos += byte_count;
    datas->back().init = bs;
  }
}
