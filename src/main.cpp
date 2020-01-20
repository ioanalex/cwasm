#include "instructions.hpp"
#include "types.hpp"
#include "values.hpp"
#include "util.hpp"
#include "ast.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  // get the module as bytes in an array
  FILE *fileptr;
  byte *buffer;
  long filelen;

  if(argc != 2) {
    FATAL("give me a file\n");
  }

  char *filename = argv[1];

  fileptr = fopen(filename, "rb");
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
  Module * m = load_module(buffer, filelen);
  delete m;
}
