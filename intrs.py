#!/usr/bin/python3
import os

def get_args(line):

    args = []
    ret = ""
    deliminer = "→"

    _l = line.split('→')
    
    print(_l[0] + " -> " + _l[1])

    return (args, ret)

def result_str(name, opcode, has_args=false, args=[], ret=""):

    # TODO format args

    ret = "case {opcode}: return { \"{name}\", {has_args}, [{args}], {ret} }"\
        .format(name=name, opcode=opcode, has_args=has_args, args=fargs, ret=ret)

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
    if name == "else":
        print(result_str(name,opcode))
    num_args=0
    types = []
    return_type = ""
    if "else" not in ops_lines[i]:
        types, return_type = get_args(ops_lines[i+2])
    # print(ops_lines[i+2].split('→'))
    # print("case " + opcode + ":    return {\"" + name + "\", false}")
        i+=4
        continue
    i += 3
# for i in range(ops.)
#     if "reserved" in l:


