#include "types.h"
#include "instructions.h"




Instr::Instr(byte opcode) {
    bool error = false;
    
    switch (opcode){
        case 0x01:  
            name = "unreachable";
            type::Value types[2] = {type::Value::any, type::Value::I32};
            args = vec<type::Value> (types, types+2);
            ret  = type::Value::any;
            break;
        default :
            error = true;
    }
    
    if(!error){
        has_args = (args.size() > 0);
    }
    
}

string Instr::get_name(){ return name;}
byte   Instr::get_code(){return code;}

string get_type(const Instr &instr){
    string ret = "[";

    std::cout << "has args " << instr.has_args << std::endl;

    for(unsigned int i = 0; i < instr.args.size(); i++){
        switch(instr.args.at(i)){
            case type::Value::I32: ret += "i32"; break;
            case type::Value::I64: ret += "i64"; break;
            case type::Value::F32: ret += "f32"; break;
            case type::Value::F64: ret += "f64"; break;
            case type::Value::any: ret += " t "; break;
        }
        ret += " ";
    }

    ret += "] -> [";

    switch(instr.ret){
        case type::Value::I32: ret += "i32"; break;
        case type::Value::I64: ret += "i64"; break;
        case type::Value::F32: ret += "f32"; break;
        case type::Value::F64: ret += "f64"; break;
        case type::Value::any: ret += " t "; break;
    };

    ret += "]";

    return ret;
}

std::ostream &operator<<(std::ostream &os, const Instr &i){
    std::cout << i.name \
              << "(0x" << std::hex << (int) (i.code) << ")" \
              << " :: " << get_type(i);
    return os;
}
