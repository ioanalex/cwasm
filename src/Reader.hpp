#ifndef __PARSE_HPP__
#define __PARSE_HPP__

#include <fstream>
#include <vector>

#include "ast.hpp"
#include "types.hpp"
#include "values.hpp"

class Reader {
public:
  Reader(const char *fname) : filename(fname), pos(0) { load(); }

  // Use this method to advance the position on the file
  std::streamsize skip(u32 k = 1) {
    std::streamsize p = pos;
    pos += k;
    if (pos > length) FATAL("trying to skip outside of the buffer");
    return p;
  }

  void expect(byte b) {
    byte next = bytes[pos++];
    ASSERT(next == b, "Expected 0x%02x instead of 0x%02x\n", b, next);
  }

  bool upto(byte b) { return peek_byte() != b; }
  bool upto(byte b1, byte b2) { return peek_byte() != b1 && peek_byte() != b2; }

  bool maybe(byte b) {
    if (bytes[pos] != b) return false;
    ++pos;
    return true;
  }
  bool unless(byte b) { return !maybe(b); }

  // Use this method if you want the value pos.
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
  // This is the method used for reading a module
  void parse_module(Module &m);

  // Methods for parsing the insides of a Module
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

  // Methods for parsing other, more basic constructs
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
  Value parse_value(type::Value);
  // methods that actually read from the binary file
  u32 read_u32() {
    u32 u = *reinterpret_cast<u32 *>(&bytes[pos]);
    pos += sizeof(u32);
    return u;
  }

  byte peek_byte(int offset = 0) const { return bytes.at(pos + offset); }
  byte read_byte() { return bytes.at(pos++); }

  u64 read_LEB(u32 maxbits, bool sign = false) {
    u64 result = 0;
    u32 shift = 0;
    u32 bcnt = 0;
    u32 startpos = pos;
    u64 byte;

    while (true) {
      byte = bytes.at(pos++);
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
    int p = skip(namelen);
    type::Name name(bytes.begin() + p, bytes.begin() + pos);
    if (!is_utf8(name.c_str())) {
      FATAL("Malformed utf8 string\n");
    }
    return name;
  }

  // helper function to check for utf8 encoded strings
  // https://stackoverflow.com/questions/1031645/how-to-detect-utf-8-in-plain-c
  bool is_utf8(const char *string) {
    if (!string) return false;

    const unsigned char *bytes = (const unsigned char *)string;
    while (*bytes) {
      if (  // ASCII
            // use bytes[0] <= 0x7F to allow ASCII control characters
          bytes[0] <= 0x7F) {
        bytes += 1;
        continue;
      }

      if ((  // non-overlong 2-byte
              (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
              (0x80 <= bytes[1] && bytes[1] <= 0xBF))) {
        bytes += 2;
        continue;
      }

      if ((  // excluding overlongs
              bytes[0] == 0xE0 && (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
              (0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
          (  // straight 3-byte
              ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) || bytes[0] == 0xEE ||
               bytes[0] == 0xEF) &&
              (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
              (0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
          (  // excluding surrogates
              bytes[0] == 0xED && (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
              (0x80 <= bytes[2] && bytes[2] <= 0xBF))) {
        bytes += 3;
        continue;
      }

      if ((  // planes 1-3
              bytes[0] == 0xF0 && (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
              (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
              (0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
          (  // planes 4-15
              (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
              (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
              (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
              (0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
          (  // plane 16
              bytes[0] == 0xF4 && (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
              (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
              (0x80 <= bytes[3] && bytes[3] <= 0xBF))) {
        bytes += 4;
        continue;
      }
      std::cout << "bytes[0] = " << std::hex << (int)bytes[0] << std::dec
                << std::endl;
      return false;
    }

    return true;
  }
};

#endif
