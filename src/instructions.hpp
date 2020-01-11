#ifndef __INSTRUCTIONS_HPP__
#define __INSTRUCTIONS_HPP__

#include <iostream>

#include "types.hpp"
#include "values.hpp"

/*  types for instructions are:
        types (as defined)
        polymorphic types: t1, t2 etc

    instr_type ::= { args: vec(itype), ret: vec(itype) 

    itype::= type
            | ptype
    ptype::= 0 | 1 | 2 | ..
*/

class ptype {
    private:
        u32 id;
        bool star;
        bool maybe;
    public:
        ptype(u32, bool, bool);
    friend inline std::ostream &operator<<(std::ostream &os, const ptype &type){
        os << "t";
        if (type.id > 0) os << type.id; 
        else os << ""; 
        os << ((type.star)? "*" : "") << ((type.maybe)? "?" : "");
        return os;
    }
};

using itype = std::variant<type::Value, ptype>;

struct instr_type {
    vec<itype> args;
    vec<itype> ret;
};

std::ostream &operator<<(std::ostream &os, const instr_type &type);

class Instr {
    public:
        Instr(byte);
        string get_name();
        byte get_code();
        instr_type get_type();

        friend std::ostream &operator<<(std::ostream &, const Instr &);
    protected:
        byte code;
        string name;
        bool has_args;
        instr_type type;
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

#endif
