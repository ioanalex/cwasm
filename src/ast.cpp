#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"
#include "binary.hpp"
#include "parse.hpp"

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
                warn("Parsing Type(1) section (length: 0x%x)\n", slen);
                parse_types(bytes, &pos, &types);
                warn("Parsing complete\n");
                for (int i = 0; i< types.size(); i++){
                    std::cout << types[i] << std::endl;
                }
                break;
            }
            case 2:
            {
                warn("Parsing Import(2) section (length: 0x%x)\n", slen);
                parse_imports(bytes, &pos, &imports);
                warn("Parsing Imports complete\n");
                #if DEBUG
                std::cout << "----- IMPORTS -----\n";
                for(int i = 0; i < imports.size(); i++){
                    std::cout << imports[i].module << "." << imports[i].name << " of type ";
                    switch(imports[i].desc.tag){
                        case importdesc::FUNC   : std::cout << "FUNC :: " << types[imports[i].desc.func] << std::endl; 
                                                  break;
                        case importdesc::TABLE  : std::cout << "TABLE :: from "  << imports[i].desc.table.limits.min \
                                                            << " to "            << imports[i].desc.table.limits.min \
                                                            << std::endl; 
                                                  break;
                        case importdesc::MEM    : std::cout << "MEM :: from " << imports[i].desc.mem.limits.min \
                                                            << " to "         << imports[i].desc.mem.limits.min \
                                                            << std::endl; 
                                                  break;
                        case importdesc::GLOBAL : std::cout << "GLOBAL :: " << imports[i].desc.global.value \
                                                            << ((imports[i].desc.global.mut) ? "(mut)" : "" ) \
                                                            << std::endl; 
                                                  break;
                    }
                }
                std::cout << "----- ------- -----\n";
                #endif
                break;
            }
            case 3:
            {
                warn("Parsing Function(3) section (length: 0x%x)\n", slen);
                parse_funcs(bytes, &pos, &funcs);
                warn("Parsing Functions complete\n");
                #if DEBUG
                std::cout << "----- FUNCTIONS -----\n";
                for (unsigned int i = 0; i < funcs.size(); i++){
                    std::cout << "FUNC :: " << types[funcs[i].type] << std::endl;
                }
                std::cout << "----- --------- -----\n";
                #endif
                break;
            }
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