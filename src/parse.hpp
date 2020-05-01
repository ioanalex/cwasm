#include <fstream>

#include "ast.hpp"
#include "types.hpp"

void parse_types(byte *, u32 *, vec<type::Func> *);
void parse_imports(byte *, u32 *, vec<Import> *);
void parse_funcs(byte *, u32 *, vec<Func> *);
void parse_tables(byte *, u32 *, vec<Table> *);
void parse_mems(byte *, u32 *, vec<Memory> *);
void parse_globals(byte *, u32 *, vec<Global> *);
void parse_exports(byte *, u32 *, vec<Export> *);
void parse_elems(byte *, u32 *, vec<Elem> *, vec<Global> &);
void parse_codes(byte *, u32 *, vec<Func> &);
void parse_datas(byte *, u32 *, vec<Data> *, vec<Global> &);

u32 parse_idx(byte *, u32 *);
Memarg parse_memarg(byte *, u32 *);
type::Value parse_valtype(byte *, u32 *);

/* This class should be able to read a module from a file
 * It is also a singleton.
 *
 * How it should be used in my main:
 *
 * // get the object
 * Reader reader = Reader.getInstance();
 *
 * // initialize it
 * reader.Init(filename);
 *
 * // when you want to Load a module just use
 * reader.ReadModule(module);
 *
 */
class Reader {
public:
  // with this function we construct the singleton
  static Reader &getInstance();
  // opens the file and does all error checking and setting up for the reading
  // to start
  void Init(std::string filename);

private:
  // Constuctor
  Reader();

  // the file
  std::ifstream file;
  std::string filename;

  // the position in the file
  int pos;

public:
  // Read a module
  // Takes an empty module and feels it
  void ReadModule(Module *mod);

public:
  /* Delete copy constructor and asignment operator so that no copying is
   * happening. Since this is a singleton we need to tell the compiler that if
   * at any point this object is copied, something is wrong. This provides
   * safety and alerts us if anything unexpected is happening.
   */
  Reader(Reader const &) = delete;
  void operator=(Reader const &) = delete;
};