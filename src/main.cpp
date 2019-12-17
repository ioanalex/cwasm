#include "instructions.hpp"
#include "types.hpp"
#include "values.hpp"
#include <iostream>

int main() {
  Value v1 = from_i32(42);
  Value v2 = from_i64(42);

  std::cout << (v1 == v2 ? "v1 == v2" : "v1 != v2") << std::endl;
  std::cout << v1 << std::endl;
  std::cout << v2 << std::endl;

  
  for (int i = 0x00; i < 0xbf; i++){
    std::cout << Instr(i) << std::endl;
  }
}
