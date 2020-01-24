#ifndef __INSTRUCTION_HPP__
#define __INSTRUCTION_HPP__

#include "InstrProfile.hpp"
#include "types.hpp"

class InstrImpl {
    // this is the abstract class for all different types of instructions
    public:
        virtual void run   (/* takes a Store   */)=0;
        virtual void verify(/* takes a Context */)=0;
};


class Instruction {
    public:
        InstrProfile *profile;
        InstrImpl *impl; 
        u32 pos; // Index in bytecode

        // Constructor
        Instruction(InstrProfile *, InstrImpl *, u32);

        // static factory method
        // checks byte[*pos]
        // desides which sub-class to use as an InstrImpl
        // initializes the impl onject
        // initializes the instr object using basic constructor
        static Instruction createInstr(byte *, u32 *);

        void run   (/* takes a Store   */); // calls impl -> run()
        void verify(/* takes a Context */); // calls impl -> verify()
};



#endif