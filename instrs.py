#!/usr/bin/python3
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
inp = os.path.join(dir_path, 'ops')

ops_file = open(inp, 'r')
ops_lines = ops_file.readlines()

ops_lines = list(map(lambda x: x.strip(), ops_lines))

i = 0
while i < len(ops_lines):
    # reserved opcodes are skipped (default case of the switch)
    if "(reserved)" in ops_lines[i]:
        i += 2
        continue

    name = str(ops_lines[i])
    opcode = str(ops_lines[i+1])

    # else and end commands have no type so we are done with them
    if(not name == "else" and not name == "end"):
        # now we have to parse the type signatures that appear on line i+2
        # so let's define a function that take the line and gives back two strings
        # one with the args and one with the ret

        def parse_args(line):
            if line == "":
                return "",""
            args, ret = line.split('→');
            args = args.strip("[]")
            ret = ret.strip("[]")
            return args, ret

        args, ret = parse_args(ops_lines[i+2])

        # Now that we have args and ret as strings with no brackets etc we need to map
        # them to push_back statements. Let's write a function that take a type string
        # and returns a list of itypes

        def get_itypes(args):
            types = args.split(" ")
            # Now we need a functions that reads a type and return the push_back statement
            def statement_for_type(type):
                if type == "":
                    return ""
                if type[0] == 't':
                    # we have a polymorphic type
                    if len(type) > 1:
                        if type[1] == '∗':
                            if len(type) == 3:
                                return "ptype({}, true, false)".format(type[2])
                            return "ptype(0, true, false)"
                        if type[1] == '?':
                            if len(type) == 3:
                                return "ptype({}, false, true)".format(type[2])
                            return "ptype(0, false, true)"
                        return "ptype({}, false, false)".format(type[1])
                    return "ptype(0, false, false)"
                return "type::Value::{}".format(type)
            return list(map(statement_for_type, types))
            # for type in types:
            #     print(statement_for_type(type))
        i+=4
    else:
        args = ""
        ret = ""
        i+=3

    args = get_itypes(args)
    ret = get_itypes(ret)

    args = "{" + ", ".join(args) + "}"
    ret = "{" + ", ".join(ret) + "}"

    print("""\
  new InstrFactory<{}, Numeric>("{}",
    {}, {}
  ),""".format(opcode, name, args, ret))
