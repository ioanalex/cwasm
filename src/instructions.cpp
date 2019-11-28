#include <cassert>


#include "types.h"
#include "instructions.h"

Instr::Instr(byte opcode) {
    pname res = code_to_name(opcode);
    assert(!res.has_args);
    code = opcode;
    name = res.name;
    args = vec<Value>();
}

Instr::Instr(byte opcode, vec<Value> params){
    pname res = code_to_name(opcode);
    assert(res.has_args);
    code = opcode;
    name = res.name;
    args = vec<Value> (params);
}

string Instr::get_name(){ return name;}
byte Instr::get_code(){return code;}

pname Instr::code_to_name(byte opcode){
    switch (opcode){
        case 0x00: return {"unreachable", false};
        default : return {"nop", false}; 
    }
}