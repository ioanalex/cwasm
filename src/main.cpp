#include <iostream>

#include "values.h"

int main()
{

    i32 num = 42;
    Value v1 = from_i32(num);
    Value v2 = from_i64(num);

    std::cout << ((v1 == v2) ? "v1 == v2" : "v1 != v2") << std::endl;
    std::cout << v1 << std::endl;
    std::cout << v2 << std::endl;
    return 0;
}