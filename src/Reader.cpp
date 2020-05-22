#include "Reader.hpp"

#include <cstring>
#include <iomanip>

#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"
#include "values.hpp"

u32 Reader::parse_idx() { return read_LEB(32); }

Value Reader::parse_value(type::Value vt) {
  Value v;
  switch (vt) {
    case type::Value::i32:
      v = from_i32(read_LEB(32));
      break;
    case type::Value::i64:
      v = from_i64(read_LEB(64));
      break;
    case type::Value::f32: {
      f32 f;
      auto p = skip(4);
      std::copy(bytes.begin() + p, bytes.begin() + p + 4,
                reinterpret_cast<byte *>(&f));
      v = from_f32(f);
      break;
    }
    case type::Value::f64: {
      f64 f;
      auto p = skip(8);
      std::copy(bytes.begin() + p, bytes.begin() + p + 8,
                reinterpret_cast<byte *>(&f));
      v = from_f64(f);
      break;
    }
  }
  return v;
}

type::Value Reader::parse_valtype() {
  u32 encoded_type = read_byte();  // TODO: this is a byte, dont use u32
  return decode_type(encoded_type);
}

type::Elem Reader::parse_elemtype() {
  // TODO: use read_byte
  ASSERT(read_LEB(7) == 0x70, "elemtype can only be funcref\n");
  return type::Elem::funcref;
}

type::Limits Reader::parse_limits() {
  type::Limits limits;
  byte has_max = read_byte();
  ASSERT(has_max <= 0x01, "has_max is either 0x00 or 0x01\n");
  limits.min = read_LEB(32);
  if (has_max) {
    limits.max = read_LEB(32);
  }
  return limits;
}

type::Table Reader::parse_tabletype() {
  type::Table table;
  table.elem = parse_elemtype();
  table.limits = parse_limits();
  return table;
}

type::Memory Reader::parse_memtype() {
  type::Memory mem;
  mem.limits = parse_limits();
  return mem;
}

type::Global Reader::parse_globaltype() {
  type::Global global;
  global.value = parse_valtype();
  ASSERT(peek_byte() < 0x02, "mut can be either 0 or 1, malformed\n");
  global.mut = (read_byte() == 0x01);
  return global;
}

type::Block Reader::parse_blocktype() {
  type::Block block;
  if (peek_byte() == 0x40) {
    skip();
    // by default empty type
    return block;
  } else {
    block.has_type = true;
    block.type = parse_valtype();
    return block;
  }
}

Memarg Reader::parse_memarg() {
  Memarg memarg;
  memarg.align = read_LEB(32);
  memarg.offset = read_LEB(32);
  return memarg;
}

void Reader::parse_expr(Expr &e) {
  unsigned int instr_count = 0;
  while (true) {
    warn("Parsing instr[%d] at %lx\n", instr_count, pos);
    e.emplace_back(Instr::create(this));
    if (e.back().code() == 0x0B) break;
    instr_count++;
#if WAIT
    WaitEnter();
#endif
  }
}

void Reader::parse_types(vec<type::Func> &types) {
  u32 type_count = read_LEB(32);
  for (u32 t = 0; t < type_count; t++) {
    type::Func newtype;
    auto check = read_LEB(7);
    ASSERT(check == 0x60,
           "Functions are encoded with the 0x60 code\n");  // TODO: use
                                                           // read_byte here
    u32 arg_count = read_LEB(32);
    for (u32 a = 0; a < arg_count; a++)
      newtype.args.emplace_back(parse_valtype());
    u32 res_count = read_LEB(32);
    ASSERT(res_count <= 1, "invalid result arity\n");
    for (u32 r = 0; r < res_count; r++)
      newtype.result.emplace_back(parse_valtype());
    // std::cout << types[t] << std::endl;
    types.push_back(newtype);
  }
}

void Reader::parse_imports(vec<Import> &imports) {
  u32 import_count = read_LEB(32);
  for (u32 im = 0; im < import_count; im++) {
    // parse each import
    // import ::= {mod : Name, name : Name, d : importdesc}
    type::Name import_module = read_name();
    type::Name import_name = read_name();
    // importdesc :: = kind : {0,1,2,3}, content: depends on kind
    u32 kind = read_byte();
    // debug("import: %d/%d, kind: %d, %s.%s\n",im+1,import_count, kind,
    // import_module.c_str(), import_name.c_str());

    importdesc *desc;
    switch (kind) {
      case 0: {
        typeidx func(parse_idx());
        desc = new importdesc(func);
        break;
      }
      case 1: {
        type::Table table;
        table = parse_tabletype();
        desc = new importdesc(table);
        break;
      }
      case 2: {
        type::Memory mem;
        mem = parse_memtype();
        desc = new importdesc(mem);
        break;
      }
      case 3: {
        type::Global global;
        global = parse_globaltype();
        desc = new importdesc(global);
        break;
      }
      default:
        FATAL("You imported something with kind %d, that I cannot understand\n",
              kind);
    }
    imports.emplace_back(Import(import_name, import_module, *desc));
    delete desc;
  }
}

void Reader::parse_funcs(vec<Func> &funcs) {
  u32 func_count = read_LEB(32);
  for (unsigned int i = 0; i < func_count; i++) {
    typeidx tid(parse_idx());
    funcs.emplace_back(Func(tid));
  }
}

void Reader::parse_tables(vec<Table> &tables) {
  u32 table_count = read_LEB(32);
  for (unsigned int i = 0; i < table_count; i++)
    tables.emplace_back(parse_tabletype());
}

void Reader::parse_mems(vec<Memory> &mems) {
  u32 table_count = read_LEB(32);
  for (unsigned int i = 0; i < table_count; i++)
    mems.emplace_back(parse_memtype());
}

void Reader::parse_globals(vec<Global> &globals) {
  u32 global_count = read_LEB(32);
  for (unsigned int i = 0; i < global_count; i++) {
    type::Global globaltype = parse_globaltype();
    globals.emplace_back(Global(globaltype));
    parse_expr(globals.back().init);
  }
}

void Reader::parse_exports(vec<Export> &exports) {
  u32 export_count = read_LEB(32);

  for (unsigned int i = 0; i < export_count; i++) {
    type::Name export_name = read_name();

    byte kind = read_byte();
    exportdesc *desc;
    u32 x = parse_idx();
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
    exports.emplace_back(Export(export_name, *desc));
    delete desc;
  }
}

void Reader::parse_elems(vec<Elem> &elems) {
  u32 elem_count = read_LEB(32);
  for (unsigned int i = 0; i < elem_count; i++) {
    tableidx x(parse_idx());
    elems.emplace_back(Elem(x));

    parse_expr(elems.back().offset);

    u32 func_count = read_LEB(32);
    vec<funcidx> &funcs = elems.back().init;
    for (unsigned int j = 0; j < func_count; j++)
      funcs.emplace_back(funcidx(parse_idx()));
  }
}

void Reader::parse_codes(vec<Func> &funcs, int imported_func_count) {
  u32 code_count = read_LEB(32);
  debug("imported: %u    code for: %u    total: %lu\n", imported_func_count,
        code_count, funcs.size());
  ASSERT(code_count == funcs.size() - imported_func_count,
         "Mismatch in number of codes: %lu vs %u\n", funcs.size(), code_count);

  for (unsigned int i = 0; i < code_count; i++) {
    u32 _size = read_LEB(32);
    USE(_size);
    u32 locals_count = read_LEB(32);
    warn("%d locals\n", locals_count);
    for (unsigned int j = 0; j < locals_count; j++) {
      u32 n = read_LEB(32);
      ASSERT((int)n >= 0, "Overflow will not kill me!\n");
      type::Value t(parse_valtype());
      for (unsigned int k = 0; k < n; k++) {
        funcs[i + imported_func_count].locals.push_back(t);
      }
    }

#if DEBUG
    for (unsigned int i = imported_func_count; i < funcs.size(); ++i) {
      for (const auto &l : funcs[i].locals) {
        std::cout << l << std::endl;
      }
    }
#endif
    debug("About to parse_expr\n");
    parse_expr(funcs[i + imported_func_count].body);
    debug("OK parse_expr\n");
  }
}

void Reader::parse_datas(vec<Data> &datas) {
  u32 data_count = read_LEB(32);
  ASSERT((int)data_count >= 0, "Overflow will not kill me!\n");
  for (unsigned int i = 0; i < data_count; i++) {
    memidx x(parse_idx());
    datas.emplace_back(Data(x));

    parse_expr(datas.back().offset);

    u32 byte_count = read_LEB(32);
    ASSERT((int)byte_count >= 0, "Overflow will not kill me!\n");
    vec<byte> bs(byte_count);
    int p = skip(byte_count);
    std::copy(bytes.begin() + p, bytes.begin() + pos, bs.begin());
    datas.back().init = bs;
  }
}

#if DEBUG
#define debugVec(some_vec) \
  for (const auto &it : some_vec) std::cout << it.sDebug() << std::endl;
#else
#define debugVec(some_vec)
#endif

void Reader::parse_module(Module &m) {
  u32 word = read_u32();
  ASSERT(word == 1836278016, "magic number is wrong\n");
  std::cout << "module magic word is " << word << std::endl;
  word = read_u32();
  ASSERT(word == 1, "version is wrong\n");
  std::cout << "module version is " << word << std::endl;
  int imported_funcs = 0;
  while (get_pos() < length) {
    u32 id = read_LEB(7);  // TODO: change that to read_byte
    u32 slen = read_LEB(32);
    ASSERT(slen > 0, "Sections with 0 bytes are not allowed\n");
    std::streamsize old_pos = get_pos();

    switch (id) {
      case 0: {
        // custom section
        // read the name and then skip the rest of the bytes
        type::Name theName = read_name();
        skip(slen - theName.size());
        break;
      }
      case 1: {
        // parse the list of types (m -> types) and return it
        warn("Parsing Type(1) section (length: 0x%x)\n", slen);
        parse_types(m.types);
        warn("Parsing complete\n");
#if DEBUG
        // TODO:
        // for (const auto &it : types) std::cout << it.sDebug() <<
        // std::endl;
#endif
        break;
      }
      case 2: {
        warn("Parsing Import(2) section (length: 0x%x)\n", slen);
        parse_imports(m.imports);
        for (auto im : m.imports) {
          switch (im.desc.tag()) {
            case importdesc::FUNC: {
              m.funcs.emplace_back(Func(im.desc.func()));
              break;
            }
            case importdesc::TABLE: {
              m.tables.emplace_back(Table(im.desc.table()));
              break;
            }
            case importdesc::MEM: {
              m.mems.emplace_back(Memory(im.desc.mem()));
              break;
            }
            case importdesc::GLOBAL: {
              m.globals.emplace_back(Global(im.desc.global()));
              break;
            }
          }
        }
        warn("Parsing Imports complete\n");
        imported_funcs = m.funcs.size();
        debugVec(m.imports);
        break;
      }
      case 3: {
        warn("Parsing Function(3) section (length: 0x%x)\n", slen);
        parse_funcs(m.funcs);
        warn("Parsing Functions complete\n");
        debugVec(m.funcs);
        break;
      }
      case 4: {
        warn("Parsing Table(4) section (length: 0x%x)\n", slen);
        parse_tables(m.tables);
        warn("Parsing Tables complete\n");
        debugVec(m.tables);
        break;
      }
      case 5: {
        warn("Parsing Memory(5) section (length: 0x%x)\n", slen);
        parse_mems(m.mems);
        warn("Parsing Memories complete\n");
        debugVec(m.mems);
        break;
      }
      case 6: {
        warn("Parsing Global(6) section (length: 0x%x)\n", slen);
        parse_globals(m.globals);
        warn("Parsing Globals complete\n");
        debugVec(m.globals);
        break;
      }
      case 7: {
        warn("Parsing Export(7) section (length: 0x%x)\n", slen);
        parse_exports(m.exports);
        warn("Parsing Exports complete\n");
        debugVec(m.exports);
        break;
      }
      case 8: {
        warn("Parsing Start(8) section (length: 0x%x)\n", slen);
        m.start = funcidx(parse_idx());
        ASSERT((m.start < m.funcs.size()),
               "Start is a funcidx so it has to be < funcs.size()\n");
        warn("Parsing Start complete\n");
#if DEBUG
        // this can cause a segmentation fault!
        std::cout << "START func is " << m.start.value()
                  << " :: " << m.types[m.funcs[m.start.value()].type]
                  << std::endl;
#endif
        break;
      }
      case 9: {
        warn("Parsing Element(9) section (length: 0x%x)\n", slen);
        parse_elems(m.elem);
        warn("Parsing Elements complete\n");
        debugVec(m.elem);
        break;
      }
      case 10: {
        warn("Parsing Code(10) section (length: 0x%x)\n", slen);
        parse_codes(m.funcs, imported_funcs);
        warn("Parsing Code complete\n");
        break;
      }
      case 11: {
        warn("Parsing Data(11)) section (length: 0x%x)\n", slen);
        parse_datas(m.data);
        warn("Parsing Datas complete\n");

// TODO: move this in ast.hpp -> Dat::sDebug()
#if DEBUG
        std::cout << "----- DATAS -----\n";
        for (unsigned int i = 0; i < m.data.size(); i++) {
          std::string content(m.data[i].init.begin(), m.data[i].init.end());
          std::cout << "DATA :: mem " << m.data[i].data << " offset (expr of #"
                    << m.data[i].offset.size() << ")" << std::endl;
          for (auto &of : m.data[i].offset)
            std::cout << "\t\t" << of << std::endl;

          std::cout << " has " << content.size() << " byte(s):" << std::hex
                    << std::endl;
          char old_fill = std::cout.fill('0');
          for (unsigned n = 0; n < content.size(); n += 20) {
            std::cout << "  ";
            for (unsigned k = n; k < n + 20; k++)
              if (k < content.size())
                std::cout << std::setw(2)
                          << int(static_cast<unsigned char>(content[k]));
              else
                std::cout << "  ";
            std::cout << "    ";
            for (unsigned k = n; k < n + 20; k++)
              if (k < content.size())
                std::cout << (std::isprint(content[k]) ? content[k] : '.');
              else
                std::cout << " ";
            std::cout << std::endl;
          }
          std::cout << std::dec;
          std::cout.fill(old_fill);
        }
        std::cout << "----- ----- -----\n";
#endif
        break;
      }
      default:  // unimplemented cases
        pos += slen;
        break;
    }
    if (get_pos() != old_pos + slen) FATAL("section size mismatch\n");
  }
  if (get_pos() < length) FATAL("junk after last section\n");
  if (get_pos() > length) FATAL("unexpected end\n");
}
