#include <iostream>

#include "InstrProfile.hpp"
#include "Reader.hpp"
#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"
#include "validate.hpp"
#include "values.hpp"

vec<InstrProfile> profiles;

int main(int argc, char *argv[]) {
  // create all instruction profiles
  for (byte i = 0; i < (0xBF + 0x01); ++i)
    profiles.emplace_back(InstrProfile(i));

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
