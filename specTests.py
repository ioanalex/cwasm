#! /usr/bin/python3

# This script takes all *.bin.wast files produced by the spec interpeter 
# and extracts the modules. 
# e.g file address.bin.wast will give address_module_<n>.wasm where
# n start from 0 and counts the amount of modules defined in the file
import os
import math

root_dir = os.path.dirname(os.path.realpath(__file__))
tests_dir = os.path.join(root_dir, 'tests')
core_tests_dir = os.path.join(tests_dir, 'srcs/core-tests')
def parse_a_file(filename):
    with open(filename, 'r') as file:
        i = 0
        line = file.readline()
        while True:            
            if "(module" in line:
                # this is the start of a module
                outputname = os.path.basename(filename).split('.')[0] + "_module_" + str(i) + ".wasm"
                outputname = os.path.join(tests_dir, 'bins', outputname)
                i = i + 1
                # open the output file (create it if it does not exist),
                # delete the contents
                
                with open(outputname, 'wb') as output:
                    line = file.readline()
                    while ')' not in line:
                        towrite = line.strip().strip('\"')
                        towrite = "".join(towrite.split('\\'))
                        towrite = bytes.fromhex(towrite)
                        output.write(towrite)
                        line = file.readline()
            line = file.readline()
            if not line:
                break

# run `parse_a_file` for all .bin.wast files
files = [f for f in os.listdir(core_tests_dir) if f.split('.')[1] == 'bin']
for f in files:
    parse_a_file(os.path.join(core_tests_dir, f))         