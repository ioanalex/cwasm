#ifndef __INSTRUCTION_HPP__
#define __INSTRUCTION_HPP__

#include "InstrProfile.hpp"
#include "types.hpp"

class InstrImpl {
    private:
        byte opcode; 
        u32 pos; // Index in bytecode
    // this is the abstract class for all different types of instructions
    public:
    
        virtual void run   (/* takes a Store   */)=0;
        virtual void verify(/* takes a Context */)=0;
};


class Instruction {
    public:        
        InstrImpl *impl;  
        // Constructor
        Instruction(InstrProfile *, InstrImpl *, u32);

        // static factory method
        // checks byte[*pos]
        // desides which sub-class to use as an InstrImpl
        // initializes the impl onject
        // initializes the instr object using basic constructor
        static Instruction createInstr(byte *, u32 *); // called when parsing

        void run   (/* takes a Store   */); // calls impl -> run()
        void verify(/* takes a Context */); // calls impl -> verify()
};

template<class t>
class ImmediateImpl: public InstrImpl {
    T imm;
};



#endif