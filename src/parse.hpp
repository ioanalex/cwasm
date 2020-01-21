#include "types.hpp"

void parse_types  (byte *, u32 * , vec<type::Func> *);
void parse_imports(byte *, u32 * , vec<Import> *);
void parse_funcs  (byte *, u32 * , vec<Func> *);
void parse_tables (byte *, u32 * , vec<Table> *);
void parse_mems   (byte *, u32 * , vec<Memory> *);
void parse_globals(byte *, u32 * , vec<Global> *);
void parse_elem   (byte *, u32 * , vec<Elem> *);
void parse_data   (byte *, u32 * , vec<Data> *);
void parse_start  (byte *, u32 * , Start *);
void parse_exports(byte *, u32 * , vec<Export> *);

