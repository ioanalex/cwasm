#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"
#include "binary.hpp"
#include "parse.hpp"

const char* section_names [] = {"Custom",   // 0
                                "Type",     // 1 
                                "Import",   // 2
                                "Function", // 3
                                "Table",    // 4
                                "Memory",   // 5
                                "Global",   // 6 !
                                "Export",   // 7
                                "Start",    // 8
                                "Element",  // 9 !
                                "Code",     // 10 !
                                "Data"      // 11 !
                                };

Module * load_module(byte* bytes, u32 byte_count){

    // Module m;
    vec<type::Func> types;
    vec<Import> imports;
    vec<Func> funcs;
    vec<Table> tables;
    vec<Memory> mems;
    vec<Global> globals;
    vec<Elem> elems;
    vec<Data> datas;
    u32 startidx = 0; // TODO: this should be changed
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
                for(auto im: imports){
                    switch(im.desc.tag){
                        case importdesc::FUNC:
                        {
                            Func f(im.desc.func);
                            funcs.push_back(f);
                            break;

                        }
                        case importdesc::TABLE:
                        {
                            Table t(im.desc.table);
                            tables.push_back(t);
                            break;
                        }
                        case importdesc::MEM:
                        {
                            Memory m(im.desc.mem);
                            mems.push_back(m);
                            break;
                        }   
                        case importdesc::GLOBAL:
                        {
                            Global g(im.desc.global);
                            globals.push_back(g);
                            break;
                        }
                    }
                }
                warn("Parsing Imports complete\n");
                #if DEBUG
                std::cout << "----- IMPORTS -----\n";
                for(int i = 0; i < imports.size(); i++){
                    std::cout << imports[i].module << "." << imports[i].name << " of type ";
                    switch(imports[i].desc.tag){
                        case importdesc::FUNC   : std::cout << "FUNC :: " << types[imports[i].desc.func] << std::endl; 
                                                  break;
                        case importdesc::TABLE  : std::cout << "TABLE :: from "  << imports[i].desc.table.limits.min \
                                                            << " to "            << imports[i].desc.table.limits.max \
                                                            << std::endl; 
                                                  break;
                        case importdesc::MEM    : std::cout << "MEM :: from " << imports[i].desc.mem.limits.min \
                                                            << " to "         << imports[i].desc.mem.limits.max \
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
            case 4:
            {
                warn("Parsing Table(4) section (length: 0x%x)\n", slen);
                parse_tables(bytes, &pos, &tables);
                warn("Parsing Tables complete\n");
                #if DEBUG
                std::cout << "----- TABLES -----\n";
                for (unsigned int i = 0; i < tables.size(); i++){
                    std::cout << "TABLE :: from "  << tables[i].type.limits.min \
                              << " to "            << tables[i].type.limits.max \
                              << std::endl;
                }
                std::cout << "----- ------ -----\n";
                #endif
                break;   
            }
            case 5:
            {
                warn("Parsing Memory(5) section (length: 0x%x)\n", slen);
                parse_mems(bytes, &pos, &mems);
                warn("Parsing Memories complete\n");
                #if DEBUG
                std::cout << "----- MEMORIES -----\n";
                for (unsigned int i = 0; i < mems.size(); i++){
                    std::cout << "MEM :: from " << mems[i].type.limits.min \
                              << " to "         << mems[i].type.limits.max \
                              << std::endl;
                }
                std::cout << "----- -------- -----\n";
                #endif
                break;
            }
            case 6:
            {
                warn("Parsing Global(6) section (length: 0x%x)\n", slen);
                parse_globals(bytes, &pos, &globals);
                warn("Parsing Globals complete\n");
                #if DEBUG
                std::cout << "----- GLOBALS -----\n";
                for (unsigned int i = 0; i < globals.size(); i++){
                    std::cout << "GLOBAL ::  " << globals[i].type.value << ((globals[i].type.mut) ? " (mut)" : "" ) \
                              << " init to "   << globals[i].init << std::endl;
                }
                std::cout << "----- ------- -----\n";
                #endif
                break;
            }
            case 7:
            {
                warn("Parsing Export(7) section (length: 0x%x)\n", slen);
                parse_exports(bytes, &pos, &exports);
                warn("Parsing Exports complete\n");
                #if DEBUG
                std::cout << "----- EXPORTS -----\n";
                for (unsigned int i = 0; i < exports.size(); i++){
                    std::cout << "EXPORT " << exports[i].name << " of type ";
                    switch(exports[i].desc.tag){
                        case importdesc::FUNC   : std::cout << "FUNC :: " << types[funcs[exports[i].desc.func].type] << std::endl; 
                                                  break;
                        case importdesc::TABLE  : std::cout << "TABLE :: from "  << tables[exports[i].desc.table].type.limits.min \
                                                            << " to "            << tables[exports[i].desc.table].type.limits.max \
                                                            << std::endl; 
                                                  break;
                        case importdesc::MEM    : std::cout << "MEM :: from " << mems[exports[i].desc.mem].type.limits.min \
                                                            << " to "         << mems[exports[i].desc.mem].type.limits.max \
                                                            << std::endl; 
                                                  break;
                        case importdesc::GLOBAL : 
                        // TODO: uncomment when globals are parsed (seg fault here)
                        // std::cout << "GLOBAL :: " << globals[exports[i].desc.global].type.value \
                        //                                     << ((globals[exports[i].desc.global].type.mut) ? "(mut)" : "" ) \
                        //                                     << globals[exports[i].desc.global].init \
                        //                                     << std::endl; 
                                                  break;
                    }
                }
                std::cout << "----- ------- -----\n";
                #endif
                break;
            }
            case 8:
            {
                warn("Parsing Start(8) section (length: 0x%x)\n", slen);
                startidx = parse_idx(bytes, &pos); 
                ASSERT((startidx < funcs.size()), "Start is a funcidx so it has to be < funcs.size()\n");
                warn("Parsing Start complete\n");
                #if DEBUG
                    std::cout << "START func is " << startidx << " :: " << types[funcs[startidx].type] << std::endl;
                #endif
                break;
            }
            case 9:
            {
                warn("Parsing Element(9) section (length: 0x%x)\n", slen);
                parse_elems(bytes, &pos, &elems, globals); 
                warn("Parsing Elements complete\n");
                #if DEBUG
                std::cout << "----- ELEMENTS -----\n";
                for (unsigned int i = 0; i < elems.size(); i++){
                    std::cout << "ELEM :: table " << elems[i].table \
                              << " at " << elems[i].offset \
                              << " has funcs:" << std::endl;
                    for (unsigned int j = 0; j < elems[i].init.size(); j++){
                        std::cout << "\t" << elems[i].init[j] << " :: " << types[funcs[elems[i].init[j]].type] << std::endl;
                    }
                }
                std::cout << "----- ------- -----\n";    
                #endif
                break;
            }
            case 11:
            {
                warn("Parsing Data(11)) section (length: 0x%x)\n", slen);
                parse_datas(bytes, &pos, &datas, globals); 
                warn("Parsing Datas complete\n");
                #if DEBUG
                std::cout << "----- DATAS -----\n";
                for (unsigned int i = 0; i < datas.size(); i++){
                    std::string content(datas[i].init.begin(), datas[i].init.end());
                    std::cout << "DATA :: mem " << datas[i].data \
                              << " at " << datas[i].offset \
                              << " has bytes:" << content << std::endl;
                }
                std::cout << "----- ----- -----\n";    
                #endif
                break;
            }
            default: //unimplemented cases
                pos += slen;
                break;            
        }


    }
    Start start(startidx);
    return new Module(types, funcs, tables, mems, globals, elems, datas, start, imports, exports);
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

importdesc::importdesc(typeidx &f): tag(FUNC), func(f){}
importdesc::importdesc(type::Table &t): tag(TABLE), table(t){}
importdesc::importdesc(type::Memory &m): tag(MEM), mem(m){}
importdesc::importdesc(type::Global &g): tag(GLOBAL), global(g){}
Import::Import(type::Name &name, type::Name &mod, importdesc &desc): module(mod), name(name), desc(desc) {}

exportdesc::exportdesc(funcidx &f): tag(FUNC), func(f) {}
exportdesc::exportdesc(tableidx &t): tag(TABLE), table(t) {} 
exportdesc::exportdesc(memidx &m): tag(MEM), mem(m) {}
exportdesc::exportdesc(globalidx &g): tag(GLOBAL), global(g) {}
Export::Export(type::Name &name, exportdesc &desc): name(name), desc(desc) {}