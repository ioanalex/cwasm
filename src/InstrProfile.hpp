#ifndef __INSTRPROFILE_HPP__
#define __INSTRPROFILE_HPP__

#include <iostream>
#include <tuple>

#include "types.hpp"
#include "values.hpp"

/*  types for instructions are:
        types (as defined)
        polymorphic types: t1, t2 etc

    instr_type ::= { args: vec(itype), ret: vec(itype)

    itype::= type
            | ptype
    ptype::= 0 | 1 | 2 | ..
*/

class ptype {
private:
  u32 id;
  bool star;
  bool maybe;

public:
  ptype(u32, bool, bool);
  friend inline std::ostream &operator<<(std::ostream &os, const ptype &type) {
    os << "t";
    if (type.id > 0)
      os << type.id;
    else
      os << "";
    os << ((type.star) ? "*" : "") << ((type.maybe) ? "?" : "");
    return os;
  }
};

using itype = std::variant<type::Value, ptype>;

struct instr_type {
  vec<itype> args;
  vec<itype> ret;
};

std::ostream &operator<<(std::ostream &os, const instr_type &type);

class InstrImpl;
class Reader;

class InstrProfile {
public:
  InstrProfile(byte c, const char *n, const vec<itype> &args,
               const vec<itype> &ret)
      : code(c),
        name(n),
        has_args(args.size() > 0),
        type{args, ret},
        used(false) {}
  virtual ~InstrProfile() {}

  string get_name() { return name; }
  byte get_code() { return code; }
  instr_type get_type() { return type; }
  bool is_used() { return used; }
  void use() { used = true;
  }
  virtual InstrImpl *create(Reader *reader) const = 0;

  friend std::ostream &operator<<(std::ostream &, const InstrProfile &);

protected:
  byte code;
  string name;
  bool has_args;
  instr_type type;
  bool used;
};

template <byte opcode, typename T, typename... Extra>
class InstrFactory : public InstrProfile {
public:
  InstrFactory(const char *name, const vec<itype> &args, const vec<itype> &ret,
               Extra... extra)
      : InstrProfile(opcode, name, args, ret), extra_(extra...) {}
  InstrImpl *create(Reader *reader) const override {
    return std::apply(
        [reader](Extra... extra) { return new T(reader, extra...); }, extra_);
  }

private:
  std::tuple<Extra...> extra_;
};

#endif
