#include "types.h"
#include "instructions.h"

Instr::Instr(byte opcode) {
    bool error = false;
    
    switch (opcode){
        case 0x01:  
            name = "unreachable";
            args = vec<type::Value>(type::Value::any);
            ret  = type::Value::any;
            break;
        default :
            error = true;
    }
    
    if(!error){
        has_args = (args.size > 0);
    }
    
}

string Instr::get_name(){ return name;}
byte Instr::get_code(){return code;}
