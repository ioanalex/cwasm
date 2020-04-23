#include "ast.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>

#include "binary.hpp"
#include "global.hpp"
#include "parse.hpp"
#include "types.hpp"
#include "util.hpp"

u32 pos;
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
static bool sections[] = {
    false,  // Custom
    false,  // Type
    false,  // Import
    false,  // Function
    false,  // Table
    false,  // Memory
    false,  // Global
    false,  // Export
    false,  // Start
    false,  // Element
    false,  // Code
    false   // Data
};

void Module::Print() {
  std::cout << "Module:" << std::endl;
  std::cout << "  Types:" << std::endl;
  printvec(types, 2) std::cout << "  Funcs:" << std::endl;
  printvec(funcs, 2) std::cout << "  Tables:" << std::endl;
  printvec(tables, 2) std::cout << "  Memories:" << std::endl;
  printvec(mems, 2) std::cout << "  Globals" << std::endl;
  printvec(globals, 2)
}

void Module::Load(byte *bytes, u32 byte_count) {
  pos = 0;
  u32 word = read_u32(bytes, pos);
  ASSERT(word == 1836278016, "magic number is wrong\n");
  std::cout << "module magic word is " << word << std::endl;
  word = read_u32(bytes, pos);
  ASSERT(word == 1, "version is wrong\n");
  std::cout << "module version is " << word << std::endl;

  while (pos < byte_count) {
    u32 id = read_LEB(bytes, &pos, 7);
    u32 slen = read_LEB(bytes, &pos, 32);

    warn("Reading (%d) %s section at 0x%x, length %d\n", id, section_names[id],
         pos, slen);
    if (sections[id]) {
      FATAL("section repeats");
    }
    sections[id] = true;
    // pos += slen;
    switch (id) {
      case 0:
        // custom section, do nothing
        pos += slen;
        break;
      case 1: {
        // parse the list of types (m -> types) and return it
        warn("Parsing Type(1) section (length: 0x%x)\n", slen);
        parse_types(bytes, &pos, &types);
        warn("Parsing complete\n");
#if DEBUG
        printvec(types, 2);
#endif
        break;
      }
      case 2: {
        warn("Parsing Import(2) section (length: 0x%x)\n", slen);
        parse_imports(bytes, &pos, &imports);
        for (auto im : imports) {
          switch (im.desc.tag) {
            case importdesc::FUNC: {
              funcs.emplace_back(Func(im.desc.func));
              break;
            }
            case importdesc::TABLE: {
              tables.emplace_back(Table(im.desc.table));
              break;
            }
            case importdesc::MEM: {
              mems.emplace_back(Memory(im.desc.mem));
              break;
            }
            case importdesc::GLOBAL: {
              globals.emplace_back(Global(im.desc.global));
              break;
            }
          }
        }
        warn("Parsing Imports complete\n");
#if DEBUG
        std::cout << "----- IMPORTS -----\n";
        for (int i = 0; i < imports.size(); i++) {
          std::cout << imports[i].module << "." << imports[i].name
                    << " of type ";
          switch (imports[i].desc.tag) {
            case importdesc::FUNC:
              std::cout << "FUNC :: @" << imports[i].desc.func << std::endl;
              break;
            case importdesc::TABLE:
              std::cout << "TABLE :: " << imports[i].desc.table.limits
                        << std::endl;
              break;
            case importdesc::MEM:
              std::cout << "MEM :: " << imports[i].desc.mem.limits << std::endl;
              break;
            case importdesc::GLOBAL:
              std::cout << "GLOBAL :: " << imports[i].desc.global.value
                        << (imports[i].desc.global.mut ? "(mut)" : "")
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
        parse_funcs(bytes, &pos, &funcs);
        warn("Parsing Functions complete\n");
#if DEBUG
        std::cout << "----- FUNCTIONS -----\n";
        for (unsigned int i = 0; i < funcs.size(); i++)
          std::cout << "FUNC :: " << types[funcs[i].type] << std::endl;
        std::cout << "----- --------- -----\n";
#endif
        break;
      }
      case 4: {
        warn("Parsing Table(4) section (length: 0x%x)\n", slen);
        parse_tables(bytes, &pos, &tables);
        warn("Parsing Tables complete\n");
#if DEBUG
        std::cout << "----- TABLES -----\n";
        for (unsigned int i = 0; i < tables.size(); i++)
          std::cout << "TABLE :: " << tables[i].type.limits << std::endl;
        std::cout << "----- ------ -----\n";
#endif
        break;
      }
      case 5: {
        warn("Parsing Memory(5) section (length: 0x%x)\n", slen);
        parse_mems(bytes, &pos, &mems);
        warn("Parsing Memories complete\n");
#if DEBUG
        std::cout << "----- MEMORIES -----\n";
        for (unsigned int i = 0; i < mems.size(); i++)
          std::cout << "MEM :: " << mems[i].type.limits << std::endl;
        std::cout << "----- -------- -----\n";
#endif
        break;
      }
      case 6: {
        warn("Parsing Global(6) section (length: 0x%x)\n", slen);
        parse_globals(bytes, &pos, &globals);
        warn("Parsing Globals complete\n");
#if DEBUG
        std::cout << "----- GLOBALS -----\n";
        for (unsigned int i = 0; i < globals.size(); i++)
          std::cout << "GLOBAL ::  " << globals[i] << std::endl;
        std::cout << "----- ------- -----\n";
#endif
        break;
      }
      case 7: {
        warn("Parsing Export(7) section (length: 0x%x)\n", slen);
        parse_exports(bytes, &pos, &exports);
        warn("Parsing Exports complete\n");
#if DEBUG
        std::cout << "----- EXPORTS -----\n";
        for (unsigned int i = 0; i < exports.size(); i++) {
          std::cout << "EXPORT " << exports[i].name << " of type ";
          switch (exports[i].desc.tag) {
            case importdesc::FUNC:
              std::cout << "FUNC @ " << exports[i].desc.func << std::endl;
              break;
            case importdesc::TABLE:
              std::cout << "TABLE @ " << exports[i].desc.table << std::endl;
              break;
            case importdesc::MEM:
              std::cout << "MEM @ " << exports[i].desc.mem << std::endl;
              break;
            case importdesc::GLOBAL:
              // TODO: uncomment when globals are parsed (seg fault here)
              std::cout << "GLOBAL @ " << exports[i].desc.global << std::endl;
              break;
          }
        }
        std::cout << "----- ------- -----\n";
#endif
        break;
      }
      case 8: {
        warn("Parsing Start(8) section (length: 0x%x)\n", slen);
        start = funcidx(parse_idx(bytes, &pos));
        ASSERT((start < funcs.size()),
               "Start is a funcidx so it has to be < funcs.size()\n");
        warn("Parsing Start complete\n");
#if DEBUG
        std::cout << "START func is " << start.value()
                  << " :: " << types[funcs[start.value()].type] << std::endl;
#endif
        break;
      }
      case 9: {
        warn("Parsing Element(9) section (length: 0x%x)\n", slen);
        parse_elems(bytes, &pos, &elem, globals);
        warn("Parsing Elements complete\n");
#if DEBUG
        std::cout << "----- ELEMENTS -----\n";
        for (unsigned int i = 0; i < elem.size(); i++) {
          std::cout << "ELEM :: table " << elem[i].table << " offset (expr of #"
                    << elem[i].offset.size() << "): " << std::endl;
          for (auto &of : elem[i].offset)
            std::cout << "\t\t" << of << std::endl;
          std::cout << " has funcs:" << std::endl;
          for (unsigned int j = 0; j < elem[i].init.size(); j++)
            std::cout << "\t" << elem[i].init[j]
                      << " :: " << types[funcs[elem[i].init[j]].type]
                      << std::endl;
        }
        std::cout << "----- ------- -----\n";
#endif
        break;
      }
      case 10: {
        warn("Parsing Code(10) section (length: 0x%x)\n", slen);
#if WAIT
        WaitEnter();
#endif
        parse_codes(bytes, &pos, funcs);
        warn("Parsing Code complete\n");
        break;
      }
      case 11: {
        warn("Parsing Data(11)) section (length: 0x%x)\n", slen);
        parse_datas(bytes, &pos, &data, globals);
        warn("Parsing Datas complete\n");
#if DEBUG
        std::cout << "----- DATAS -----\n";
        for (unsigned int i = 0; i < data.size(); i++) {
          std::string content(data[i].init.begin(), data[i].init.end());
          std::cout << "DATA :: mem " << data[i].data << " offset (expr of #"
                    << data[i].offset.size() << ")" << std::endl;
          for (auto &of : data[i].offset)
            std::cout << "\t\t" << of << std::endl;

          std::cout << " has " << content.size() << " byte(s):" << std::hex
                    << std::endl;
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
