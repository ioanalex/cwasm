#include <string>

#include "../types.hpp"
#include "../instructions.hpp"

string NumInstr::get_name(){
    string i_f = (type == i) ? "i" : "f";
    string x = (size == x32) ? "32" : "64";
    string res = i_f + x + "." + name;
    switch (sign){
        case u: res += "_u"; break;
        case s: res += "_s"; break;
        case n: res += "_n"; break;
    }
    return res;
}
