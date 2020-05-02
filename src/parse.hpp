#include <fstream>
#include <vector>

#include "ast.hpp"
#include "types.hpp"

void parse_types(byte *, u32 *, vec<type::Func> *);
void parse_imports(byte *, u32 *, vec<Import> *);
void parse_funcs(byte *, u32 *, vec<Func> *);
void parse_tables(byte *, u32 *, vec<Table> *);
void parse_mems(byte *, u32 *, vec<Memory> *);
void parse_globals(byte *, u32 *, vec<Global> *);
void parse_exports(byte *, u32 *, vec<Export> *);
void parse_elems(byte *, u32 *, vec<Elem> *, vec<Global> &);
void parse_codes(byte *, u32 *, vec<Func> &);
void parse_datas(byte *, u32 *, vec<Data> *, vec<Global> &);

u32 parse_idx(byte *, u32 *);
Memarg parse_memarg(byte *, u32 *);
type::Value parse_valtype(byte *, u32 *);

class Reader {
public:
  Reader(const char *fname) : filename(fname), pos(0) { load(); }

  u32 read_u32() {
    u32 u = *reinterpret_cast<u32 *>(&bytes[pos]);
    pos += sizeof(u32);
    return u;
  }

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

  void skip(u32 k) { pos += k; }
  std::streamsize get_pos() const { return pos; }

private:
  std::string filename;
  std::vector<byte> bytes;
  std::streamsize pos;

  void load() {
    std::ifstream f(filename, std::ios::binary);
    if (!f) return;
    f.seekg(0, f.end);
    std::streamsize length = f.tellg();
    f.seekg(0, f.beg);
    bytes.resize(length + 1);
    f.read(reinterpret_cast<char *>(&bytes[0]), length);
    bytes[length] = '\0';
    pos = 0;
  }
};
