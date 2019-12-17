#ifndef __INSTRUCTIONS_HPP__
#define __INSTRUCTIONS_HPP__

#include <iostream>

#include "types.hpp"
#include "values.hpp"

class Instr {
    public:
        Instr(byte);
        string get_name();
        byte get_code();

        friend string get_type(const Instr&);
        friend std::ostream &operator<<(std::ostream &, const Instr &);
    protected:
        byte code;
        string name;
        bool has_args;
        vec<type::Value> args;
        type::Value ret;
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
