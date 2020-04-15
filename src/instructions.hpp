#ifndef __INSTRUCTIONS_HPP__
#define __INSTRUCTIONS_HPP__
#include <memory>

#include "InstrProfile.hpp"
#include "global.hpp"
#include "types.hpp"

// This is the abstract class for all different types of instructions.
class InstrImpl {
public:
  explicit InstrImpl(u32 pos) : pos_(pos), code_(bytes[pos]) {}

  virtual ~InstrImpl() {}

  u32 pos() const { return pos_; }
  u32 code() const { return code_; }
  bool is_const() const { return is_const_; }
  void set_const() { is_const_ = true; }

  // TODO: takes a Store.
  virtual void run() = 0;
  // TODO: takes a Context.
  virtual bool validate() = 0;

private:
  u32 pos_;  // Index in bytecode
  byte code_;
  bool is_const_ = false;
};

class Instr {
public:
  Instr() : impl(nullptr) {}
  explicit Instr(InstrImpl *i) : impl(i) {}
  Instr(const Instr &i) = delete;
  Instr(Instr &&i) = default;

  // static factory method
  // checks byte[*pos]
  // desides which sub-class to use as an InstrImpl
  // initializes the impl onject
  // initializes the instr object using basic constructor
  static Instr create(byte *, u32 *);  // called when parsing
  u32 pos() const { return impl->pos(); }
  u32 code() const { return impl->code(); }
  // TODO: takes a Store.
  void run() { impl->run(); }
  bool is_const() { return impl->is_const(); }
  // TODO: takes a Context.
  bool validate() {
    std::cout << "(0x" << std::hex << code() << std::dec << ") "
              << profiles[code()].get_name() << std::endl;
    bool b = impl->validate();
    return b;
  }

private:
  std::unique_ptr<InstrImpl> impl;
};

inline std::ostream &operator<<(std::ostream &os, const Instr &i) {
  return os << "(0x" << std::hex << i.code() << std::dec << ") "
            << profiles[i.code()].get_name();
}

#endif
