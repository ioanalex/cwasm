#ifndef __INSTRUCTION_HPP__
#define __INSTRUCTION_HPP__

#include "InstrProfile.hpp"
#include "indices.hpp"
#include "named.hpp"
#include "types.hpp"

// This is the abstract class for all different types of instructions.
class InstrImpl {
public:
  InstrImpl() {}
  explicit InstrImpl(u32 pos) : pos_(pos) {}
  virtual ~InstrImpl() {}

  u32 pos() const { return pos_; }

  // TODO:
  //  * make virtual
  //  * takes a Store.
  void run();
  // TODO:
  //  * make virtual
  //  * takes a Context.
  void verify();

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

private:
  InstrImpl *impl = nullptr;
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

// TODO: add some abstraction here
class Numeric : public InstrImpl {
  using InstrImpl::InstrImpl;
};

template <typename T>
class ImmediateImpl : public InstrImpl {
public:
  ImmediateImpl() {}

protected:
  T imm;
};

class Call : public ImmediateImpl<funcidx> {
public:
  Call(byte *bytes, u32 *pos);
};

class LocalGet : public ImmediateImpl<localidx> {
public:
  LocalGet(byte *bytes, u32 *pos);
};
#endif
