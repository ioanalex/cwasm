#include <iostream>

#include "InstrProfile.hpp"
#include "ast.hpp"
#include "global.hpp"
#include "types.hpp"
#include "util.hpp"
#include "validate.hpp"
#include "values.hpp"

byte *bytes;
long filelen;

vec<InstrProfile> profiles;

int main(int argc, char *argv[]) {
  // create all instruction profiles
  for (byte i = 0; i < (0xBF + 0x01); ++i)
    profiles.emplace_back(InstrProfile(i));

  // get the module as bytes in an array
  FILE *fileptr;

  if (argc != 2) {
    FATAL("give me a file\n");
  }

  char *filename = argv[1];

  fileptr = fopen(filename, "rb");
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);

  bytes = new byte[filelen + 1];
  fread(bytes, filelen, 1, fileptr);
  bytes[filelen] = '\0';
  fclose(fileptr);

  Module m;
  load_module(m, bytes, filelen);

  PrintModule(m);

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

  if (!res) FATAL("File: %s: VALIDATION FAILED\n", filename);
  // Validate::func(m.funcs[17]);

  // std::cout << "--------- COMMANDS THAT WERE NOT USED ---------\n";
  // for (auto instr : profiles) {
  //   if (!instr.is_used()) std::cout << instr << std::endl;
  // }
  delete[] bytes;
}
