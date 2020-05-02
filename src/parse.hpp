#include <fstream>
#include <vector>

#include "ast.hpp"
#include "types.hpp"

class Reader {
public:
  Reader(const char *fname) : filename(fname), pos(0) { load(); }

  u32 read_u32() {
    u32 u = *reinterpret_cast<u32 *>(&bytes[pos]);
    pos += sizeof(u32);
    return u;
  }

  byte peek_byte() { return bytes[pos]; }
  byte read_byte() { return bytes[pos++]; }

  u64 read_LEB(u32 maxbits, bool sign = false) {
    u64 result = 0;
    u32 shift = 0;
    u32 bcnt = 0;
    u32 startpos = pos;
    u64 byte;

    while (true) {
      byte = bytes[pos++];
      result |= ((byte & 0x7f) << shift);
      shift += 7;
      if ((byte & 0x80) == 0) break;
      bcnt += 1;
      if (bcnt > (maxbits + 7 - 1) / 7) {
        std::cerr << "Unsigned LEB at byte " << startpos << "overflow"
                  << std::endl;
      }
    }
    if (sign && (shift < maxbits) && (byte & 0x40)) {
      // Sign extend
      result |= -(1 << shift);
    }
    return result;
  }

  type::Name read_name() {
    u32 namelen = read_LEB(32);
    type::Name name(bytes.begin() + pos, bytes.begin() + pos + namelen);
    skip(namelen);
    return name;
  }

  void skip(u32 k) { pos += k; }
  std::streamsize get_pos() const { return pos; }

private:
  std::string filename;
  std::vector<byte> bytes;
  std::streamsize pos;
  std::streamsize length;

  void load() {
    std::ifstream f(filename, std::ios::binary);
    if (!f) return;
    f.seekg(0, f.end);
    length = f.tellg();
    f.seekg(0, f.beg);
    bytes.resize(length + 1);
    f.read(reinterpret_cast<char *>(&bytes[0]), length);
    bytes[length] = '\0';
    pos = 0;
  }

public:
  void parse_module(Module &m);

private:
  // Methods for parsing a Module
  void parse_types(vec<type::Func> &);
  void parse_imports(vec<Import> &);
  void parse_funcs(vec<Func> &);
  void parse_tables(vec<Table> &);
  void parse_mems(vec<Memory> &);
  void parse_globals(vec<Global> &);
  void parse_exports(vec<Export> &);
  void parse_elems(vec<Elem> &);
  void parse_codes(vec<Func> &);
  void parse_datas(vec<Data> &);

  // Methods for parsing other constructs
  void parse_expr(Expr &);
  u32 parse_idx();
  type::Value parse_valtype();
  type::Elem parse_elemtype();
  type::Limits parse_limits();
  type::Table parse_tabletype();
  type::Memory parse_memtype();
  type::Global parse_globaltype();
  type::Block parse_blocktype();
  Memarg parse_memarg();
};
