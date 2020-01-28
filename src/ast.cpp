#include "ast.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>

#include "binary.hpp"
#include "parse.hpp"
#include "types.hpp"
#include "util.hpp"

static const char *section_names[] = {
    "Custom",    // 0
    "Type",      // 1
    "Import",    // 2
    "Function",  // 3
    "Table",     // 4
    "Memory",    // 5
    "Global",    // 6 !
    "Export",    // 7
    "Start",     // 8
    "Element",   // 9 !
    "Code",      // 10 !
    "Data"       // 11 !
};

void load_module(Module &mod, byte *bytes, u32 byte_count) {
  u32 pos = 0;
  u32 word = read_u32(bytes, pos);
  std::cout << "module magic word is " << word << std::endl;
  word = read_u32(bytes, pos);
  std::cout << "module version is " << word << std::endl;

  while (pos < byte_count) {
    u32 id = read_LEB(bytes, &pos, 7);
    u32 slen = read_LEB(bytes, &pos, 32);

    debug("Reading %s section at 0x%x, length %d\n", section_names[id], pos,
          slen);
    // pos += slen;
    switch (id) {
      case 0:
        // custom section, do nothing
        pos += slen;
        break;
      case 1: {
        // parse the list of types (m -> types) and return it
        warn("Parsing Type(1) section (length: 0x%x)\n", slen);
        parse_types(bytes, &pos, &mod.types);
        warn("Parsing complete\n");
        for (int i = 0; i < mod.types.size(); i++)
          std::cout << mod.types[i] << std::endl;
        break;
      }
      case 2: {
        warn("Parsing Import(2) section (length: 0x%x)\n", slen);
        parse_imports(bytes, &pos, &mod.imports);
        for (auto im : mod.imports) {
          switch (im.desc.tag) {
            case importdesc::FUNC: {
              mod.funcs.emplace_back(Func(im.desc.func));
              break;
            }
            case importdesc::TABLE: {
              mod.tables.emplace_back(Table(im.desc.table));
              break;
            }
            case importdesc::MEM: {
              mod.mems.emplace_back(Memory(im.desc.mem));
              break;
            }
            case importdesc::GLOBAL: {
              mod.globals.emplace_back(Global(im.desc.global));
              break;
            }
          }
        }
        warn("Parsing Imports complete\n");
#if DEBUG
        std::cout << "----- IMPORTS -----\n";
        for (int i = 0; i < mod.imports.size(); i++) {
          std::cout << mod.imports[i].module << "." << mod.imports[i].name
                    << " of type ";
          switch (mod.imports[i].desc.tag) {
            case importdesc::FUNC:
              std::cout << "FUNC :: " << mod.types[mod.imports[i].desc.func]
                        << std::endl;
              break;
            case importdesc::TABLE:
              std::cout << "TABLE :: from "
                        << mod.imports[i].desc.table.limits.min << " to "
                        << mod.imports[i].desc.table.limits.max << std::endl;
              break;
            case importdesc::MEM:
              std::cout << "MEM :: from " << mod.imports[i].desc.mem.limits.min
                        << " to " << mod.imports[i].desc.mem.limits.max
                        << std::endl;
              break;
            case importdesc::GLOBAL:
              std::cout << "GLOBAL :: " << mod.imports[i].desc.global.value
                        << (mod.imports[i].desc.global.mut ? "(mut)" : "")
                        << std::endl;
              break;
          }
        }
        std::cout << "----- ------- -----\n";
#endif
        break;
      }
      case 3: {
        warn("Parsing Function(3) section (length: 0x%x)\n", slen);
        parse_funcs(bytes, &pos, &mod.funcs);
        warn("Parsing Functions complete\n");
#if DEBUG
        std::cout << "----- FUNCTIONS -----\n";
        for (unsigned int i = 0; i < mod.funcs.size(); i++)
          std::cout << "FUNC :: " << mod.types[mod.funcs[i].type] << std::endl;
        std::cout << "----- --------- -----\n";
#endif
        break;
      }
      case 4: {
        warn("Parsing Table(4) section (length: 0x%x)\n", slen);
        parse_tables(bytes, &pos, &mod.tables);
        warn("Parsing Tables complete\n");
#if DEBUG
        std::cout << "----- TABLES -----\n";
        for (unsigned int i = 0; i < mod.tables.size(); i++)
          std::cout << "TABLE :: from " << mod.tables[i].type.limits.min
                    << " to " << mod.tables[i].type.limits.max << std::endl;
        std::cout << "----- ------ -----\n";
#endif
        break;
      }
      case 5: {
        warn("Parsing Memory(5) section (length: 0x%x)\n", slen);
        parse_mems(bytes, &pos, &mod.mems);
        warn("Parsing Memories complete\n");
#if DEBUG
        std::cout << "----- MEMORIES -----\n";
        for (unsigned int i = 0; i < mod.mems.size(); i++)
          std::cout << "MEM :: from " << mod.mems[i].type.limits.min << " to "
                    << mod.mems[i].type.limits.max << std::endl;
        std::cout << "----- -------- -----\n";
#endif
        break;
      }
      case 6: {
        warn("Parsing Global(6) section (length: 0x%x)\n", slen);
        parse_globals(bytes, &pos, &mod.globals);
        warn("Parsing Globals complete\n");
#if DEBUG
        std::cout << "----- GLOBALS -----\n";
        for (unsigned int i = 0; i < mod.globals.size(); i++)
          std::cout << "GLOBAL ::  " << mod.globals[i].type.value
                    << (mod.globals[i].type.mut ? " (mut)" : "") << " init to "
                    << mod.globals[i].init << std::endl;
        std::cout << "----- ------- -----\n";
#endif
        break;
      }
      case 7: {
        warn("Parsing Export(7) section (length: 0x%x)\n", slen);
        parse_exports(bytes, &pos, &mod.exports);
        warn("Parsing Exports complete\n");
#if DEBUG
        std::cout << "----- EXPORTS -----\n";
        for (unsigned int i = 0; i < mod.exports.size(); i++) {
          std::cout << "EXPORT " << mod.exports[i].name << " of type ";
          switch (mod.exports[i].desc.tag) {
            case importdesc::FUNC:
              std::cout << "FUNC :: "
                        << mod.types[mod.funcs[mod.exports[i].desc.func].type]
                        << std::endl;
              break;
            case importdesc::TABLE:
              std::cout << "TABLE :: from "
                        << mod.tables[mod.exports[i].desc.table].type.limits.min
                        << " to "
                        << mod.tables[mod.exports[i].desc.table].type.limits.max
                        << std::endl;
              break;
            case importdesc::MEM:
              std::cout << "MEM :: from "
                        << mod.mems[mod.exports[i].desc.mem].type.limits.min
                        << " to "
                        << mod.mems[mod.exports[i].desc.mem].type.limits.max
                        << std::endl;
              break;
            case importdesc::GLOBAL:
              // TODO: uncomment when globals are parsed (seg fault here)
              std::cout << "GLOBAL :: "
                        << mod.globals[mod.exports[i].desc.global].type.value
                        << (mod.globals[mod.exports[i].desc.global].type.mut
                                ? "(mut)"
                                : "")
                        << mod.globals[mod.exports[i].desc.global].init
                        << std::endl;
              break;
          }
        }
        std::cout << "----- ------- -----\n";
#endif
        break;
      }
      case 8: {
        warn("Parsing Start(8) section (length: 0x%x)\n", slen);
        mod.start = funcidx(parse_idx(bytes, &pos));
        ASSERT((mod.start < mod.funcs.size()),
               "Start is a funcidx so it has to be < funcs.size()\n");
        warn("Parsing Start complete\n");
#if DEBUG
        std::cout << "START func is " << mod.start.value()
                  << " :: " << mod.types[mod.funcs[mod.start.value()].type]
                  << std::endl;
#endif
        break;
      }
      case 9: {
        warn("Parsing Element(9) section (length: 0x%x)\n", slen);
        parse_elems(bytes, &pos, &mod.elem, mod.globals);
        warn("Parsing Elements complete\n");
#if DEBUG
        std::cout << "----- ELEMENTS -----\n";
        for (unsigned int i = 0; i < mod.elem.size(); i++) {
          std::cout << "ELEM :: table " << mod.elem[i].table << " at "
                    << mod.elem[i].offset << " has funcs:" << std::endl;
          for (unsigned int j = 0; j < mod.elem[i].init.size(); j++)
            std::cout << "\t" << mod.elem[i].init[j] << " :: "
                      << mod.types[mod.funcs[mod.elem[i].init[j]].type]
                      << std::endl;
        }
        std::cout << "----- ------- -----\n";
#endif
        break;
      }
      case 10: {
        warn("Parsing Code(10) section (length: 0x%x)\n", slen);
        parse_codes(bytes, &pos, mod.funcs);
        warn("Parsing Code complete\n");
        break;
      }
      case 11: {
        warn("Parsing Data(11)) section (length: 0x%x)\n", slen);
        parse_datas(bytes, &pos, &mod.data, mod.globals);
        warn("Parsing Datas complete\n");
#if DEBUG
        std::cout << "----- DATAS -----\n";
        for (unsigned int i = 0; i < mod.data.size(); i++) {
          std::string content(mod.data[i].init.begin(), mod.data[i].init.end());
          std::cout << "DATA :: mem " << mod.data[i].data << " at "
                    << mod.data[i].offset << " has " << content.size()
                    << " byte(s):" << std::hex << std::endl;
          char old_fill = std::cout.fill('0');
          for (int n = 0; n < content.size(); n += 20) {
            std::cout << "  ";
            for (int k = n; k < n + 20; k++)
              if (k < content.size())
                std::cout << std::setw(2)
                          << int(static_cast<unsigned char>(content[k]));
              else
                std::cout << "  ";
            std::cout << "    ";
            for (int k = n; k < n + 20; k++)
              if (k < content.size())
                std::cout << (std::isprint(content[k]) ? content[k] : '.');
              else
                std::cout << " ";
            std::cout << std::endl;
          }
          std::cout << std::dec;
          std::cout.fill(old_fill);
        }
        std::cout << "----- ----- -----\n";
#endif
        break;
      }
      default:  // unimplemented cases
        pos += slen;
        break;
    }
  }
}
