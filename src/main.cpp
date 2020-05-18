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

  std::cout << "running > " << argv[1] << std::endl;
  Reader reader(argv[1]);
  Module m;
  reader.parse_module(m);
  std::cout << m;

  Validator validator = Validator();
  validator.InitContext(m);

  // validate the module
  bool res = validator.validate_module(m);
  if (!res)
    FATAL("File: %s: VALIDATION FAILED\n", argv[1]);
  else
    std::cout << "Module is valid!\n";

  // std::cout << "--------- COMMANDS THAT WERE NOT USED ---------\n";
  // for (auto instr : profiles) {
  //   if (!instr.is_used()) std::cout << instr << std::endl;
  // }
}
