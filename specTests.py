#! /usr/bin/python3

# This script takes all *.bin.wast files produced by the spec interpeter 
# and extracts the modules. 
# e.g file address.bin.wast will give address_module_<n>.wasm where
# n start from 0 and counts the amount of modules defined in the file
import os
import math

debug=False
root_dir = os.path.dirname(os.path.realpath(__file__))
tests_dir = os.path.join(root_dir, 'tests')
core_tests_dir = os.path.join(tests_dir, 'srcs/core-tests')
def parse_a_file(filename):
    if debug:
        print("Reading: " + filename)
    with open(filename, 'r') as file:
        i = 0
        line = file.readline()
        is_valid = True
        while True:      
            if "(assert_invalid" in line or "(assert_malformed" in line:
                is_valid = False      
            if "(module" in line:
                # this is the start of a module
                outputname = os.path.basename(filename).split('.')[0] + "_module_" + str(i) + ".wasm"
                if is_valid:
                    outputname = os.path.join(tests_dir, 'bins', outputname)
                else:
                    outputname = os.path.join(tests_dir, 'bins/fail', outputname)
                if debug:
                    print("-->" + outputname)
                i = i + 1
                # open the output file (create it if it does not exist),
                # delete the contents
                
                with open(outputname, 'wb') as output:
                    line = file.readline()
                    while ')' not in line:
                        line = line.strip()
                        
                        towrite = line.strip('\"')
                        towrite = "".join(towrite.split('\\'))
                        towrite = bytes.fromhex(towrite)
                    
                        output.write(towrite)
                        line = file.readline()
                    # it is possible to have: "\00...\f3..") so what we should do is
                    # strip the line of all whitespace
                    line = line.strip() # <hex>)<otherstuf>
                    if len(line) > 1:
                        # split at ) and keep the hex part
                        line = line.split(')')[0]
                        bins = line.split(' ') # it is possible to have 2 lines of hex in one.....
                        # print(bins)
                        for bin in bins:
                            # now just do what we always do                            
                            towrite = bin.strip('\"')
                            towrite = "".join(towrite.split('\\'))
                            towrite = bytes.fromhex(towrite)
                            output.write(towrite)
                    is_valid = True #reset is_valid for the next module

            line = file.readline()
            if not line:
                break

# run `parse_a_file` for all .bin.wast files
files = [f for f in os.listdir(core_tests_dir) if len(f.split('.')) > 1 and f.split('.')[1] == 'bin']
for f in files:
    parse_a_file(os.path.join(core_tests_dir, f))         