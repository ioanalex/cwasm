#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"
#include "binary.hpp"

void parse_types  (byte *, u32 * , vec<type::Func> * );
void parse_imports(byte *, u32 * , vec<Import> *     );

// vec<Func>       parse_funcs  (byte *, u32 & );
// vec<Table>      parse_tables (byte *, u32 & );
// vec<Memory>     parse_mems   (byte *, u32 & );
// vec<Global>     parse_globals(byte *, u32 & );
// vec<Elem>       parse_elem   (byte *, u32 & );
// vec<Data>       parse_data   (byte *, u32 & );
// Start           parse_start  (byte *, u32 & );
void parse_exports(byte *, u32 * , vec<Export> *);

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

    // Module m;
    vec<type::Func> types;
    vec<Func> funcs;
    vec<Table> tables;
    vec<Memory> mems;
    vec<Global> globals;
    vec<Elem> elem;
    vec<Data> data;
    u32 startidx = 0; // TODO: this should be changed
    vec<Import> imports;
    vec<Export> exports;

    u32 pos = 0;
    u32 word = read_u32(bytes, pos);
    std::cout << "module magic word is " << word << std::endl;
    word = read_u32(bytes, pos);
    std::cout << "module version is " << word << std::endl;

    while (pos < byte_count) {
        u32 id = read_LEB(bytes, &pos, 7);
        u32 slen = read_LEB(bytes, &pos, 32);
        
        debug("Reading %s section at 0x%x, length %d\n", section_names[id], pos, slen);
        // pos += slen;
        switch(id){
            case 0:
                // custom section, do nothing
                pos += slen;
                break;
            case 1:
            {
                // parse the list of types (m -> types) and return it
                std::cout<<"size of m->types " << types.size() << std::endl;
                warn("Parsing Type(1) section (length: 0x%x)\n", slen);
                parse_types(bytes, &pos, &types);
                warn("Parsing complete\n");
                for (int i = 0; i< types.size(); i++){
                    std::cout << types[i] << std::endl;
                }
                break;
            }
            // case 2:
            // {
            //     warn("Parsing Import(2) section (length: 0x%x)\n", slen);
            //     parse_imports(bytes, &pos, &(m -> imports));
            //     warn("Parsing Imports complete\n");
            //     #if DEBUG
            //     std::cout << "----- IMPORTS -----\n";
            //     for(int i = 0; i < m -> imports.size(); i++){
            //         std::cout << m->imports[i].module << "." << m->imports[i].name << " of type ";
            //         switch(m->imports[i].desc.tag){
            //             case importdesc::FUNC   : std::cout << "FUNC :: " << m -> types[m -> imports[i].desc.func] << std::endl; break;
            //             case importdesc::TABLE  : std::cout << "TABLE :: from "  << m -> imports[i].desc.table.limits.min \
            //                                                 << " to "            << m -> imports[i].desc.table.limits.min \
            //                                                 << std::endl; 
            //                                       break;
            //             case importdesc::MEM    : std::cout << "MEM :: from " << m -> imports[i].desc.mem.limits.min \
            //                                                 << " to "         << m -> imports[i].desc.mem.limits.min \
            //                                                 << std::endl; 
            //                                       break;
            //             case importdesc::GLOBAL : std::cout << "GLOBAL :: " << m -> imports[i].desc.global.value \
            //                                                 << ((m -> imports[i].desc.global.mut) ? "(mut)" : "" ) \
            //                                                 << std::endl; 
            //                                       break;
            //         }
            //     }
            //     std::cout << "----- ------- -----\n";
            //     #endif
            //     break;
            // }
            // case 7:
            // {
            //     // parse list of exports (m -> exports)
            //     // std::cout << "about to parse exports" << std::endl;
            //     // parse_exports(bytes, &pos, &(m->exports));
            //     // std::cout << "parsing complete" << std::endl;
            //     // // for (int i = 0; i < m -> exports.size(); i++){
            //     //     std::cout << m -> exports[i] << std::endl;
            //     // }
            //     break;
            // }
            default: //unimplemented cases
                pos += slen;
                break;            
        }


    }
    Start start(startidx);
    return new Module(types, funcs, tables, mems, globals, elem, data, start, imports, exports);
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
        u32 kind = bytes[*pos];
        *pos = *pos + 1;

        debug("import: %d/%d, kind: %d, %s.%s\n",im+1,import_count, kind, import_module.c_str(), import_name.c_str());

        importdesc *desc;

        switch(kind){
            case 0:
            {
                u32 tidx = read_LEB(bytes, pos, 32); debug("typeidx is %d\n", tidx); 
                typeidx func(tidx);
                desc = new importdesc(func);
                break;
            }
            case 1:
            {
                type::Table table;
                ASSERT(read_LEB(bytes, pos, 7) == 0x70, "Table type can only be funcref\n");
                table.elem = type::Elem::funcref; 
                u32 has_max = read_u32(bytes, *pos); ASSERT(has_max <= 1, "has_max is either 0x00 or 0x01\n");
                table.limits.min = read_LEB(bytes, pos, 32);
                if (has_max){
                    table.limits.max = read_LEB(bytes, pos, 32);
                }
                desc = new importdesc(table);
                break;
            }
            case 3:
            {
                
            }
            default:
                FATAL("yolo \n");
        }

        imports -> push_back(Import(import_name, import_module, *desc));

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




// Constuctors

Module::Module( vec<type::Func> &types, vec<Func> &funcs, vec<Table> &tables, \
                vec<Memory> &mems, vec<Global> &globals, vec<Elem> &elem, \
                vec<Data> &data, Start &start, vec<Import> &imports, vec<Export> &exports \
            ): types(types), \
            funcs(funcs), \
            tables(tables), \
            mems(mems), \
            globals(globals), \
            elem(elem), \
            data(data), \
            start(start), \
            imports(imports), \
            exports(exports) {}

importdesc::importdesc(typeidx f): tag(FUNC), func(f){}
importdesc::importdesc(type::Table t): tag(TABLE), table(t){}
importdesc::importdesc(type::Memory m): tag(MEM), mem(m){}
importdesc::importdesc(type::Global g): tag(GLOBAL), global(g){}
Import::Import(type::Name name, type::Name mod, importdesc desc): module(mod), name(name), desc(desc) {}