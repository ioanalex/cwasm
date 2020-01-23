#include "types.hpp"

void parse_types  (byte *, u32 * , vec<type::Func> *);
void parse_imports(byte *, u32 * , vec<Import> *);
void parse_funcs  (byte *, u32 * , vec<Func> *);
void parse_tables (byte *, u32 * , vec<Table> *);
void parse_mems   (byte *, u32 * , vec<Memory> *);
void parse_globals(byte *, u32 * , vec<Global> *);
void parse_elems   (byte *, u32 * , vec<Elem> *, vec<Global> &);
void parse_datas   (byte *, u32 * , vec<Data> *, vec<Global> &);
void parse_exports(byte *, u32 * , vec<Export> *);

u32 parse_idx(byte *, u32 *);

