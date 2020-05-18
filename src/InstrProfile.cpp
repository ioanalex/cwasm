#include "InstrProfile.hpp"

#include "types.hpp"

std::ostream &operator<<(std::ostream &os, const InstrProfile &i) {
  std::cout << i.name << "(0x" << std::hex << (int)(i.code) << std::dec << ")"
            << " :: " << i.type;
  return os;
}

ptype::ptype(u32 id, bool star, bool maybe)
    : id(id), star(star), maybe(maybe){};

std::ostream &operator<<(std::ostream &os, const instr_type &type) {
  // print the [
  os << "[ ";
  // print the args types
  for (auto const &type : type.args) {
    std::visit([&os](auto t) { os << t << " "; }, type);
  }
  // print the ] - > [
  os << "] → [ ";
  // print the ret
  for (auto const &type : type.ret) {
    std::visit([&os](auto t) { os << t; }, type);
  }
  // print the  ]
  os << " ]";
  return os;
}
