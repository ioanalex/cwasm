#include <iostream>

#include "InstrProfile.hpp"
#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"
#include "values.hpp"

int main(int argc, char *argv[]) {
  // create all instruction profiles
  vec<InstrProfile> profiles;
  for (byte i = 0; i < (0xBF + 0x01); i++) {
    InstrProfile instr(i);
    profiles.push_back(instr);
  }

  // for (auto instr : profiles) {
  //   std::cout << instr << std::endl;
  // }

  // get the module as bytes in an array
  FILE *fileptr;
  byte *buffer;
  long filelen;

  if (argc != 2) {
    FATAL("give me a file\n");
  }

  char *filename = argv[1];

  fileptr = fopen(filename, "rb");
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);

  buffer = new byte[filelen + 1];
  fread(buffer, filelen, 1, fileptr);
  buffer[filelen] = '\0';
  fclose(fileptr);

  Module m;
  load_module(m, buffer, filelen);
  delete[] buffer;
}
