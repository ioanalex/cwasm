#include <iostream>

#include "InstrProfile.hpp"
#include "Reader.hpp"
#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"
#include "validate.hpp"
#include "values.hpp"

int main(int argc, char *argv[]) {
  // create all instruction profiles
  profiles.resize(0xBF + 1);
  for (InstrProfile *p : p_profiles)
    profiles.emplace(profiles.begin() + p->get_code(),
                     std::unique_ptr<InstrProfile>(p));

  if (argc != 2) {
    FATAL("give me a file\n");
  }

  // from here ---------------
  std::cout << "running > " << argv[1] << std::endl;
  Reader reader(argv[1]);
  Module m;
  reader.parse_module(m);
  std::cout << m;

  // from here: ----------------
  InitContext(m);
  // PrintContext();
  bool res = Validate::funcs(m);
  res &= Validate::tables(m);
  res &= Validate::mems(m);
  res &= Validate::globals(m);
  res &= Validate::elems(m);
  res &= Validate::datas(m);
  res &= Validate::start(m);
  res &= Validate::exports(m);
  res &= Validate::imports(m);

  if (!res) FATAL("File: %s: VALIDATION FAILED\n", argv[1]);

  //  to here ----------------
  // [after refactor]: just use:
  /*
  // get the validator
  Validator v = Validator.getInstance();

  // initialize it (this could be moved in m.Validate)
  v.Init(m) // this will initialize the context and the stacks

  // attach it to the module
  m.setValidator(&v);

  // validate the module
  m.Validate();

  */

  // Validate::func(m.funcs[17]);

  // std::cout << "--------- COMMANDS THAT WERE NOT USED ---------\n";
  // for (auto instr : profiles) {
  //   if (!instr.is_used()) std::cout << instr << std::endl;
  // }
}
