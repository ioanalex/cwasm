#ifndef __INSTRUCTIONS_HPP__
#define __INSTRUCTIONS_HPP__
#include <memory>

#include "InstrProfile.hpp"
#include "global.hpp"
#include "types.hpp"

// This is the abstract class for all different types of instructions.
class InstrImpl {
public:
  explicit InstrImpl(u32 pos) : pos_(pos) {}
  virtual ~InstrImpl() {}

  u32 pos() const { return pos_; }
  u32 code() const { return bytes[pos_]; }

  // TODO: takes a Store.
  virtual void run() = 0;
  // TODO: takes a Context.
  virtual bool validate() = 0;

private:
  u32 pos_;  // Index in bytecode
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
  // TODO: takes a Context.
  virtual bool validate() { impl->validate(); }

private:
  std::unique_ptr<InstrImpl> impl;
};

#endif
