#ifndef __VALIDATE_HPP__
#define __VALIDATE_HPP__

#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"

struct Context {
  vec<type::Func> types;
  vec<type::Func> funcs;
  vec<type::Table> tables;
  vec<type::Memory> mems;
  vec<type::Global> globals;
  vec<type::Value> locals;
  vec<type::Result> labels;
  type::Result return_;
} context;

void InitContext(Module &mod) {
  context.types = mod.types;
  // each mod.func has a typeidx to types
  itloop(mod.funcs) {
    type::Func functype = mod.types[(*it).type];
    context.funcs.emplace_back(functype);
  }
  itloop(mod.tables) {
    type::Table tabletype = (*it).type;
    context.tables.emplace_back(tabletype);
  }
  itloop(mod.mems) {
    type::Memory memtype = (*it).type;
    context.mems.emplace_back(memtype);
  }
  itloop(mod.globals) {
    type::Global globaltype = (*it).type;
    context.globals.emplace_back(globaltype);
  }
}

void PrintContext() {
  std::cout << "Validation Context:" << std::endl;
  std::cout << "  Types:" << std::endl;
  printvec(context.types, 2) std::cout << "  Funcs:" << std::endl;
  printvec(context.funcs, 2) std::cout << "  Tables:" << std::endl;
  printvec(context.tables, 2) std::cout << "  Memories:" << std::endl;
  printvec(context.mems, 2) std::cout << "  Globals" << std::endl;
  printvec(context.globals, 2) std::cout << "  Locals:" << std::endl;
  printvec(context.locals, 2) std::cout << "  Labels:" << std::endl;
  printvec(context.labels, 2) std::cout << "  Return:" << std::endl << "\t\t";
  std::cout << context.return_ << std::endl;
}

#endif