#include "validate.hpp"

Context context;

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

bool Validate::funcs(Module &m) {
  itloop(m.funcs) {
    bool is_ok = Validate::func(*it);
    if (!is_ok) return false;
  }
  return true;
}

bool Validate::func(Func &f) {
  type::Func functype(context.types.at(f.type));  // TODO: add exception
                                                  // handling
  context.locals = functype.args + f.locals;
  context.labels.resize(functype.result.size());
  std::copy(functype.result.begin(), functype.result.end(),
            context.labels.begin());
  if (functype.result.empty())
    context.return_ = type::Result();
  else
    context.return_ = type::Result(functype.result.front());

  std::cout << "changed context to:" << std::endl;
  PrintContext();

  return Validate::expr(f.body);
}

bool Validate::expr(Expr &ex) { itloop(ex) if (it->validate()) return false; }