#ifndef __INSTRUCTION_HPP__
#define __INSTRUCTION_HPP__

#include "InstrProfile.hpp"
#include "named.hpp"
#include "types.hpp"

// This is the abstract class for all different types of instructions.
class InstrImpl {
public:
  explicit InstrImpl(u32 pos) : pos_(pos) {}
  virtual ~InstrImpl() {}

  u32 pos() const { return pos_; }

  // TODO: takes a Store.
  virtual void run();
  // TODO: takes a Context.
  virtual void verify();

private:
  u32 pos_;  // Index in bytecode
};

class Instr {
public:
  Instr() { this->impl = nullptr; }
  explicit Instr(InstrImpl *i) : impl(i) {}
  Instr(const Instr &i) = delete;
  Instr(Instr &&i) = default;
  ~Instr() { delete impl; }

  // static factory method
  // checks byte[*pos]
  // desides which sub-class to use as an InstrImpl
  // initializes the impl onject
  // initializes the instr object using basic constructor
  static Instr create(byte *, u32 *);  // called when parsing

  // TODO: takes a Store.
  void run() { impl->run(); }
  // TODO: takes a Context.
  void verify() { impl->verify(); }

protected:
  InstrImpl *impl;
};

class Unreachable : public InstrImpl {
  using InstrImpl::InstrImpl;
};

class Nop : public InstrImpl {
  using InstrImpl::InstrImpl;
};

class Return : public InstrImpl {
  using InstrImpl::InstrImpl;
};

class Drop : public InstrImpl {
  using InstrImpl::InstrImpl;
};

class Select : public InstrImpl {
  using InstrImpl::InstrImpl;
};

// TODO: maybe add some abstraction here
class Numeric : public InstrImpl {
  using InstrImpl::InstrImpl;
};

template <typename T>
class ImmediateImpl : public InstrImpl {
protected:
  T imm;
};
#endif
