#!/usr/bin/python3
import os

code_with_args = """
        case {code}:  
            name = "{name}";
            type::Value types[{size}] = {args};
            args = vec<type::Value> (types, types+{size});
            ret  = type::Value::{ret};
            break;
       """
code_no_args  = """
        case {code}:  
            name = "{name}";
            args = vec<type::Value> ();
            ret  = type::Value::{ret};
            break;
       """

def get_args(line):

    args = []
    ret = ""
    deliminer = "→"

    _l = line.split('→')
    
    print(_l[0] + " -> " + _l[1])

    return (fargs, size, ret)


dir_path = os.path.dirname(os.path.realpath(__file__))
inp = os.path.join(dir_path, 'ops')

ops_file = open(inp, 'r')
ops_lines = ops_file.readlines()

ops_lines = list(map(lambda x: x.strip(), ops_lines))

i = 0
while i < len(ops_lines):
    if "(reserved)" in ops_lines[i]:
        i += 2
        continue

    name = str(ops_lines[i])
    opcode = str(ops_lines[i+1])
    
    size = 0;
    ret = [];
    if "else" not in ops_lines[i+2]:
        fargs, size, ret = get_args(ops_lines[i+2])

    if 

    # print(ops_lines[i+2].split('→'))
    # print("case " + opcode + ":    return {\"" + name + "\", false}")
    i+=4
    
    
    #    ret = code.format(name=name, opcode=opcode, size=size, args=fargs, ret=ret)


