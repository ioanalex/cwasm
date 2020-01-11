#include "instructions.hpp"
#include "types.hpp"
#include "values.hpp"
#include "util.hpp"
#include "ast.hpp"
#include <iostream>

int main() {
  // Value v1 = from_i32(42);
  // Value v2 = from_i64(42);

  // std::cout << (v1 == v2 ? "v1 == v2" : "v1 != v2") << std::endl;
  // std::cout << v1 << std::endl;
  // std::cout << v2 << std::endl;

  
  // for (int i = 0x00; i < 0xbf; i++){
  //   std::cout << Instr(i) << std::endl;
  // }

  // get the module as bytes in an array
  FILE *fileptr;
  byte *buffer;
  long filelen;

  fileptr = fopen("hello.wasm", "rb");
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);

  buffer = (byte *)malloc( (filelen+1) * sizeof(byte));
  fread(buffer, filelen, 1, fileptr);
  buffer[filelen] = '\0';
  fclose(fileptr);

  // print the module
  // for (long i = 0; i < filelen; i++){
  //   printf("%hhx", buffer[i]);
  // }
  // printf("\n");
  // printf("%s\n", buffer);

  load_module(buffer, filelen);
}
