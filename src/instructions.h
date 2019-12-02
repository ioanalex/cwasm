#ifndef INSTR_H
#define INSTR_H

#include "types.h"
#include "values.h"

// this helps when constracting a new Instr object
struct pname {
    string name;
    bool has_args;
};

class Instr {
    public:
        Instr(byte);
        Instr(byte, vec<Value>);
        string get_name();
        byte get_code();
    protected:
        string name;
        byte code;
        vec<Value> args;
    private:
        pname code_to_name(byte);
};


// ignore this
/*
    Number instructions:
        * Unop   = iunop  | funop
        * binop  = ibinop | fbinop
        * testop = itestop
        * relop  = irelop | frelop
        * cvtop = wrap | extend | trunc | convert | demote | promote | reinterpret
*/
// class NumInstr: public Instr{
//     public:
//         string get_name();
//         byte get_code();
//     protected:
//         enum {i, f} type; //integer or float
//         enum {x32, x64} size; // 32 or 64 bit operation
//         enum {u, s, n} sign      // signed or unsigned or doesn't matter
// };

// class ParamInstr: public Instr{

// };

// class VarInstr: public Instr{

// };

// class MemInstr: public Instr{

// };

// class ControlInstr: public Instr{

// };

typedef vec<Instr> Expr;


#endif
