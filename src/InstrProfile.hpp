#ifndef __INSTRPROFILE_HPP__
#define __INSTRPROFILE_HPP__

#include <iostream>

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

class InstrProfile {
public:
  InstrProfile(byte);
  string get_name();
  byte get_code();
  instr_type get_type();

  friend std::ostream &operator<<(std::ostream &, const InstrProfile &);

protected:
  byte code;
  string name;
  bool has_args;
  instr_type type;
};

#endif
