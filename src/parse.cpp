#include "types.hpp"
#include "ast.hpp"
#include "binary.hpp"
#include "util.hpp"

inline u32 parse_byte(byte *bytes, u32 *pos) {
    u32 b = bytes[*pos];
    *pos = *pos + 1;
    return b;
}

u32 parse_idx(byte *bytes, u32 *pos){ 
    return read_LEB(bytes, pos, 32);
}

type::Elem parse_elemtype(byte *bytes, u32 *pos){
    ASSERT(read_LEB(bytes, pos, 7) == 0x70, "elemtype can only be funcref\n");
    return type::Elem::funcref; 
}

type::Limits parse_limits(byte *bytes, u32 *pos){
    type::Limits limits;
    u32 has_max = bytes[*pos]; *pos = *pos + 1; ASSERT(has_max <= 1, "has_max is either 0x00 or 0x01\n");
    limits.min = read_LEB(bytes, pos, 32);
    if (has_max){
        limits.max = read_LEB(bytes, pos, 32);
    }
    return limits;
}

type::Table parse_tabletype(byte *bytes, u32 *pos){
    type::Table table;
    table.elem = parse_elemtype(bytes, pos);
    table.limits = parse_limits(bytes, pos);
    return table;
}

type::Memory parse_memtype(byte *bytes, u32 *pos){
    type::Memory mem;
    mem.limits = parse_limits(bytes, pos);
    return mem;
}

type::Global parse_globaltype(byte *bytes, u32 *pos){
    type::Global global;
    u32 encoded_type = read_LEB(bytes, pos, 32);
    global.value = decode_type(encoded_type);
    global.mut = (bytes[*pos] == 0x01);
    *pos = *pos + 1;
    return global;
}

void parse_types(byte *bytes, u32 *pos , vec<type::Func> *types){   
    u32 type_count = read_LEB(bytes, pos, 32);
    for (u32 t = 0; t < type_count; t++){
        type::Func newtype;
        ASSERT(read_LEB(bytes, pos, 7) == 0x60, "Functions are encoded with the 0x60 code\n");
        u32 arg_count = read_LEB(bytes, pos, 32);
        for (u32 a = 0; a < arg_count; a++){
            u32 encoded_type = read_LEB(bytes, pos, 32);
            newtype.args.push_back(decode_type(encoded_type));
        }
        u32 res_count = read_LEB(bytes, pos, 32);
        for (u32 r = 0; r < res_count; r++){
            u32 encoded_type =  read_LEB(bytes, pos, 32); 
            newtype.result.push_back(decode_type(encoded_type));
        }
        // std::cout << types[t] << std::endl;
        types -> push_back(newtype);
    }
}

void parse_imports(byte *bytes, u32 *pos , vec<Import> *imports){
    u32 import_count = read_LEB(bytes, pos, 32);
    for (u32 im = 0; im < import_count; im++){
        // parse each import
        // import ::= {mod : Name, name : Name, d : importdesc}
        type::Name import_module = read_name(bytes, pos);
        type::Name import_name   = read_name(bytes, pos);
        // importdesc :: = kind : {0,1,2,3}, content: depends on kind
        u32 kind = parse_byte(bytes, pos);
        // debug("import: %d/%d, kind: %d, %s.%s\n",im+1,import_count, kind, import_module.c_str(), import_name.c_str());

        importdesc *desc;
        switch(kind){
            case 0:
            {
                typeidx func(parse_idx(bytes, pos));
                desc = new importdesc(func);
                break;
            }
            case 1:
            {
                type::Table table;
                table = parse_tabletype(bytes, pos);                
                desc = new importdesc(table);
                break;
            }
            case 2:
            {
                type::Memory mem;
                mem = parse_memtype(bytes, pos);
                desc = new importdesc(mem);
                break;
            }
            case 3:
            {
                type::Global global;
                global = parse_globaltype(bytes, pos);
                desc = new importdesc(global);
                break;
            }
            default:
                FATAL("You imported something with kind %d, that I cannot understand\n", kind);
        }
        imports -> push_back(Import(import_name, import_module, *desc));
    }
}

void parse_funcs(byte *bytes, u32 *pos, vec<Func> *funcs){
    u32 func_count = read_LEB(bytes, pos, 32);
    for (unsigned int i = 0; i < func_count; i++){
        typeidx tid(parse_idx(bytes, pos));
        Func func(tid);
        funcs->push_back(func);
    }
}

void parse_exports(byte *bytes, u32 *pos , vec<Export> *exports){
    // u32 export_count = read_LEB(bytes, pos, 32);
    
    // for (u32 t = 0; t < export_count; t++){
    //     exports->push_back( Export() );
    //     type::Name name = read_name(bytes, pos);
    //     (*exports)[t].name = name;
    //     u32 kind  = read_LEB(bytes, pos, 32);
    //     u32 index = read_LEB(bytes, pos, 32);
    //     std::cout << name << " " << kind << " " << index << std::endl;
    // }
}