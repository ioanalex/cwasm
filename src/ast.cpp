#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"

vec<type::Func> parse_types  (byte *, u32 & , u32 );
vec<Func>       parse_funcs  (byte *, u32 & , u32 );
vec<Table>      parse_tables (byte *, u32 & , u32 );
vec<Memory>     parse_mems   (byte *, u32 & , u32 );
vec<Global>     parse_globals(byte *, u32 & , u32 );
vec<Elem>       parse_elem   (byte *, u32 & , u32 );
vec<Data>       parse_data   (byte *, u32 & , u32 );
Start           parse_start  (byte *, u32 & , u32 );
vec<Import>     parse_imports(byte *, u32 & , u32 );
vec<Export>     parse_exports(byte *, u32 & , u32 );

const char* section_names [] = {"custom",   // 0
                                "type",     // 1 
                                "import",   // 2
                                "function", // 3
                                "table",    // 4
                                "memory",   // 5
                                "global",   // 6 
                                "export",   // 7
                                "start",    // 8
                                "element",  // 9
                                "code",     // 10
                                "data"      // 11
                                };

Module * load_module(byte* bytes, u32 byte_count){

    Module *m = (Module *) malloc (sizeof(Module));

    u32 pos = 0;
    u32 word = read_u32(bytes, pos);
    std::cout << "module magic word is " << word << std::endl;
    word = read_u32(bytes, pos);
    std::cout << "module version is " << word << std::endl;

    while (pos < byte_count) {
        u32 id = read_LEB(bytes, &pos, 7);
        u32 slen = read_LEB(bytes, &pos, 32);
        
        printf("Reading %s section at 0x%x, length %d\n", section_names[id], pos, slen);
        // pos += slen;

        switch(id){
            case 0:
                // custom section, do nothing
                pos += slen;
                break;
            case 1:
                // parse the list of types (m -> types) and return it
                m -> types = parse_types(bytes, pos, slen);
                std::cout<<"hi"<<std::endl;
                for (int i = 0; i< m -> types.size(); i++){
                    std::cout << m -> types[i] << std::endl;
                }
                break;
            default: //unimplemented cases
                pos += slen;
                break;
            
        }


    }

    return m;
}

inline type::Value decode_type(u32 v){
    switch(v){
        case 0x7F: return type::Value::i32;
        case 0x7E: return type::Value::i64;
        case 0x7D: return type::Value::f32;
        case 0x7FC: return type::Value::f64;
        default: std::cout<<"invalid value " << v << "not a valtype" << std::endl;
    }
}

vec<type::Func> parse_types(byte *bytes, u32 &pos, u32 slen){
    u32 type_count = read_LEB(bytes, &pos, 32); 
    vec<type::Func> types(type_count, type::Func());

    for (u32 t = 0; t < type_count; t++){
        read_LEB(bytes, &pos, 7);
        vec<type::Value> arg_types;
        u32 arg_count = read_LEB(bytes, &pos, 32); 
        for (u32 a = 0; a < arg_count; a++){
            u32 encoded_type = read_LEB(bytes, &pos, 32); 
            types[t].args.push_back(decode_type(encoded_type));
        }
        vec<type::Value> res_types;
        u32 res_count = read_LEB(bytes, &pos, 32); 
        for (u32 r = 0; r < res_count; r++){
            u32 encoded_type =  read_LEB(bytes, &pos, 32); 
            types[t].result.push_back(decode_type(encoded_type));
        }
        // types.push_back(type::Func());
        // types[t].args = arg_types;
        // types[t].result = res_types;

        std::cout << types[t] << std::endl;
    }
    
    return types;
}