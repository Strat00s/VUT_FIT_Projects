import argparse
import re
import xml.etree.ElementTree as ET
from sys import stderr, stdin
import operator

#TODO comments
#TODO change error_exit
def error_exit(err_code, spec_code, string1, string2 = "", string3 = ""):
    if spec_code == 0:
        stderr.write(string1)
    
    elif err_code == 31:
        spec_code = 1
        stderr.write("XML - "+string1+"\n")

    elif err_code == 32:
        if spec_code == 1:
            stderr.write("Invalid number of operands '{}' for opcode '{}'\n".format(string1, string2))
        if spec_code == 2:
            stderr.write("Invalid or unknown opcode '{}'\n".format(string1))
        if spec_code == 3:
            stderr.write("Invalid value '{}' for type '{}' at '{}'\n".format(string1, string2, string3))

    elif err_code == 52:
        if spec_code == 1:  #label
            stderr.write("Jump to unknown label '{}'\n".format(string1))
        if spec_code == 2:  #redefined var
            stderr.write("Variable '{}' already defined".format(string1))

    elif err_code == 53:
        if spec_code == 1:
            stderr.write("Invalid operand type '{}' for opcode '{}'\n".format(string1, string2))
    
    elif err_code == 54:
        spec_code = 1
        stderr.write("Undefined variable '{}'\n".format(string1))
    
    elif err_code == 55:
        if spec_code == 1:
            stderr.write("Invalid frame operation '{}'\n".format(string1))
        if spec_code == 2:
            stderr.write("Undefined frame for variable '{}'\n".format(string1))

    elif err_code == 56:
        if spec_code == 2:
            stderr.write("Missing value for opcode '{}'\n".format(string1))

    elif err_code == 57:
        if spec_code == 1:
            stderr.write("Invalid value '{}' of operand '{}' at '{}'\n".format(string1, string2, string3))
        if spec_code == 2:
            stderr.write("Invalid exit code '{}'\n".format(string1))
        if spec_code == 3:
            stderr.write("Invalid input value '{}' for type '{}' at '{}'\n".format(string1, string2, string3))
    
    elif err_code == 58:
        spec_code = 1
        stderr.write("Invalid string operation: ")
        stderr.write(string1)
    exit(err_code)

def get_frame_name(name):
    if not(re.match(r"^(GF|LF|TF)@", name)): error_exit(55, 2, name)
    global frame_cnt
    frame = name[:2]
    name = name[3:]
    if frame == "GF": frame = 0
    if frame == "TF": frame = 1
    if frame == "LF": frame = frame_cnt + 1
    return frame, name

def find_variable(name):
    global frames
    frame, var_name = get_frame_name(name)
    if frames[frame] == None:
        error_exit(55, 0, "Temporary frame no yet defined\n")
    var_data = frames[frame].get(var_name)
    if var_data == None:
        return frame, var_name, False 
    else:
        return var_data[0], var_data[1], True

def add_variable(name):
    global frames
    search = find_variable(name)
    if search[2] == False:
        frames[search[0]][search[1]] = ["", ""]
    else:
        error_exit(52, 2, name)

def edit_variable(name, edit):
    global frames
    frame, var_name = get_frame_name(name)
    frames[frame][var_name] = edit

def get_value(operand):
    if operand[1] == "var":
        return find_variable(operand[2])[0]
    else:
        return operand[2]

def get_type(operand):
    if operand[1] == "var":
        search = find_variable(operand[2])
        if search[2] == False:
            error_exit(54, 1, operand[2])
        op_type = search[1]
    else:
        op_type = operand[1]
    return op_type

def check_string(string):
    if string == None:
        return ""
    
    esq = re.findall(r"(\\\d{3})", string)
    
    if string.count("\\") > len(esq):
        error_exit(57, 0, "Invalid escape sequence in string '{}'\n".format(string))    
    
    esq = set(esq)
    
    for i in esq:
        unicode = int(i[1:])
        string = string.replace(i, chr(unicode))
    return string

def get_var_const(operand, opcode, var, nil, intt, string, boolt, find = 1):
    match = False
    var_const = "const"
    typet = ""
    value = ""
    
    if var and operand[1] == "var":
        if not(re.match(r"^(GF|LF|TF)@", operand[2])):
                error_exit(55, 2, operand[2])
        match = True
        var_const = "var"
        if find:
            search = find_variable(operand[2])
            if search[2] == False:
                error_exit(54, 1, operand[2])
            value = search[0]
            typet = search[1]
        if nil or intt or string or boolt:
            match = False
            if typet == "":                  match = True
            if nil    and typet == "nil":    match = True
            if intt   and typet == "int":    match = True
            if string and typet == "string": match = True
            if boolt  and typet == "bool":   match = True
    
    if nil and operand[1] == "nil":
        if re.match(r"^nil$", operand[2]):
            operand[2] = ""
        else:
            error_exit(32, 3, operand[2], operand[1], opcode)
        typet = "nil"
        value = operand[2]
        match = True
    
    if intt and operand[1] == "int":
        if re.match(r"^[+-]?\d+$", str(operand[2])):
                operand[2] = int(operand[2])
        else:
                error_exit(32, 3, operand[2], operand[1], opcode)
        typet = "int"
        value = operand[2]
        match = True
    
    if boolt and operand[1] == "bool":
        if not(re.match(r"^(true|false)$", operand[2])):
                error_exit(32, 3, operand[2], operand[1], opcode)
        typet = "bool"
        value = operand[2]
        match = True
    
    if string and operand[1] == "string":
        typet = "string"
        value = check_string(operand[2])
        match = True

    if match == True:
        return var_const, typet, value
    else:
        error_exit(53, 1, get_type(operand), opcode)

def jump_if(op1_val, op1_type, op2_val, op2_type, label, opcode):
    if op1_type != op2_type and op1_type != "nil" and op2_type != "nil":
            error_exit(53, 1, op1_type+"' and '"+op2_type, opcode)
    
    jump_to = next((i for i, v in enumerate(instructions) if label in v), None)
    
    if jump_to == None:
        error_exit(52, 1, label)

    if op1_val == op2_val and opcode == "JUMPIFEQ":
        return jump_to + 1
    
    if op1_val != op2_val and opcode == "JUMPIFNEQ" :
        return jump_to + 1
    return -1

def basic_operations(op1_type, op1_val, op2_type, op2_val, opcode):
    if op1_type == "" or op2_type == "":
        error_exit(56, 2, opcode)
    
    if opcode == "AND":
            return "true" if op1_val == "true" and op2_val == "true" else "false"
    
    if opcode == "OR":
        return "true" if op1_val == "true" or op2_val == "true" else "false"
    
    if op1_type == "nil" or op2_type == "nil":
        if opcode !=  "EQ":
            error_exit(53, 1, op1_type+"' and '"+op2_type, opcode)
        return "true" if op1_val == op2_val else "false"
    
    if op1_type != op2_type:
        error_exit(53, 1, op1_type+"' and '"+op2_type, opcode)
    
    if opcode == "EQ":
        return "true" if op1_val == op2_val else "false"
    
    elif op1_type == "bool":
        if opcode == "LT":
            return "true" if op1_val == "false" and op2_val == "true" else "false"
        else:
            return "true" if op1_val == "true" and op2_val == "false" else "false"
    
    elif op1_type in ["int", "string"]:
        if opcode == "LT":
            return "true" if op1_val < op2_val else "false"
        else:
            return "true" if op1_val > op2_val else "false"

def arithemtics(op1, op2, opcode):
    if opcode == "ADD":  return op1 + op2
    if opcode == "SUB":  return op1 - op2
    if opcode == "MUL":  return op1 * op2
    if opcode == "IDIV":
        if op2 == 0:
            error_exit(57, 0, "Division by zero\n")
        return int(op1 // op2)

def check_stack(size, operation):
    if len(var_stack) < size:
        error_exit(56, 0, "Not enought items on stack for '{}'\n".format(operation))

#TODO overhaul
#perform instructions
def check_ops(opcode, operands, labels, calls, index):
    types = ["int", "string", "bool", "nil"]
    symbols = types
    symbols.append("var")
    global frames
    global frame_cnt
    global var_stack

    #DONE? #TODO: BREAK
    if opcode in ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]:
        if len(operands) != 0:
            error_exit(32, 1, len(operands), opcode)
        if opcode == "CREATEFRAME":
            frames[1] = {}
        if opcode == "PUSHFRAME":
            if frames[1] == None:
                error_exit(55, 1, "PUSHFRAME")
            frame_cnt += 1
            if len(frames) - frame_cnt != 1:
                error_exit(55, 1, "PUSHFRAME")
            frames.append(frames[1])
            frames[1] = None
        if opcode == "POPFRAME":
            frame_cnt -= 1
            if frame_cnt < 0:
                error_exit(55, 1, "POPFRAME")
            if len(frames) - frame_cnt != 3:
                error_exit(55, 1, "POPFRAME")
            if len(frames) < 3:
                error_exit(55, 1, "POPFRAME")
            frames[1] = frames.pop(-1)
        if opcode == "RETURN":
            if len(calls) == 0: error_exit(56, 0, opcode)
            return calls.pop(-1) + 1
        return -1

    #DONE?
    if opcode == "DEFVAR":
        if len(operands) != 1:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)
        add_variable(operands[0][2])
        return -1

    #DONE?
    if opcode in ["WRITE", "EXIT", "DPRINT"]:
        if len(operands) != 1:
            error_exit(32, 1, len(operands), opcode)
        op0 = get_var_const(operands[0], opcode, 1, 1, 1, 1, 1)
        if opcode == "WRITE":
            if op0[1] == "":
                error_exit(56, 2, opcode)
            print(op0[2], end='')
        if opcode == "DPRINT":
            stderr.write(op0[2])
        if opcode == "EXIT":
            if op0[1] == "":
                error_exit(56, 2, opcode)
            if op0[1] != "int":
                error_exit(53, 1, op0[1], opcode)
            exit_code = op0[2]
            if isinstance(exit_code, int):
                if exit_code >= 0 and exit_code <= 49:
                    exit(exit_code)
            error_exit(57, 2, exit_code)
        return -1

    #DONE?
    if opcode == "PUSHS":
        if len(operands) != 1:
            error_exit(32, 1, len(operands), opcode)
        op0 = get_var_const(operands[0], opcode, 1, 1, 1, 1, 1)
        if op0[1] == "":
            error_exit(56, 2, opcode)
        var_stack.append([op0[2], op0[1]])
        return -1

    #DONE?
    if opcode == "POPS":
        if len(operands) != 1:
            error_exit(32, 1, len(operands), opcode)
        check_stack(1, opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        edit_variable(operands[0][2], var_stack.pop(-1))
        return -1

    #DONE?
    if opcode == "CLEARS":
        var_stack.clear()
        return -1

    #DONE?
    if opcode in ["ADDS", "SUBS", "MULS", "DIVS", "IDIVS"]:
        check_stack(2, opcode)
        if var_stack[-1][1] != "int" or var_stack[-2][1] != "int":
            error_exit(53, 0, "Invalid variable type on stack for '{}'\n".format(opcode))
        op1 = var_stack[-2]
        op2 = var_stack[-1]
        result = arithemtics(op1[0], op2[0], opcode[:-1])
        var_stack[-2] = [result, "int"]
        var_stack.pop(-1)
        return -1

    #DONE?
    if opcode in ["LTS", "GTS", "EQS"]:
        check_stack(2, opcode)
        op1 = var_stack[-2]
        op2 = var_stack[-1]
        result = basic_operations(op1[1], op1[0], op2[1], op2[0], opcode[:-1])
        var_stack[-2] = [result, "bool"]
        var_stack.pop(-1)
        return -1

    #DONE?
    if opcode in ["ANDS", "ORS"]:
        check_stack(2, opcode)
        if var_stack[-2][1] != "bool" or var_stack[-1][1] != "bool":
            error_exit(53, 0, "Invalid variable type on stack for '{}'\n".format(opcode))
        op1 = var_stack[-2]
        op2 = var_stack[-1]
        result = basic_operations(op1[1], op1[0], op2[1], op2[0], opcode[:-1])
        var_stack[-2] = [result, "bool"]
        var_stack.pop(-1)
        return -1

    #DONE?
    if opcode == "NOTS":
        check_stack(1, opcode)
        if var_stack[-1][1] != "bool":
            error_exit(53, 0, "Invalid variable type on stack for '{}'\n".format(opcode))
        var_stack[-1] = ["false", "bool"] if var_stack[-1][0] == "true" else ["true", "bool"]
        return -1
    
    #DONE?
    if opcode == "INT2CHARS":
        check_stack(1, opcode)
        if var_stack[-1][1] != "int":
            error_exit(53, 1, var_stack[-1][1], opcode)
        char = var_stack[-1][0]
        if char < 0 or char > 1114111:
            error_exit(58, 0, "Variable '{}'({}) out of range for '{}'\n".format(var_stack[-1][1], char, opcode))
        var_stack[-1] = [chr(char), "string"]
        return -1

    #DONE?
    if opcode == "STRI2INTS":
        check_stack(2, opcode)
        if var_stack[-2][1] != "string":
            error_exit(53, 1, var_stack[-2][1], opcode)
        if var_stack[-1][1] != "int":
            error_exit(53, 1, var_stack[-1][1], opcode)
        string = var_stack[-2][0]
        index = var_stack[-1][0]
        if len(string) - 1 < index or index < 0:
            error_exit(58, 0, "Index '{}' out of range for '{}'\n".format(index, string))
        char = string[index]
        var_stack[-2] = [ord(char), "int"]
        var_stack.pop(-1)
        return -1

    #DONE?
    if opcode in ["JUMPIFEQS", "JUMPIFNEQS"]:
        if len(operands) != 1:
            error_exit(32, 1, len(operands), opcode)
        if operands[0][1] != "label":
            error_exit(53, 1, operands[0][1], opcode)
        check_stack(2, opcode)
        op1 = var_stack[-2]
        op2 = var_stack[-1]
        return jump_if(op1[0], op1[1], op2[0], op2[1], operands[0][2], opcode[:-1])

    #DONE?
    if opcode in ["CALL", "LABEL", "JUMP"]:
        if opcode == "LABEL": return -1
        if len(operands) != 1:
            error_exit(32, 1, len(operands), opcode)
        if operands[0][1] != "label":
            error_exit(53, 1, operands[0][1], opcode)
        if not(operands[0][2] in labels):
            error_exit(52, 1, operands[0][2])
        jump_to = next((i for i, v in enumerate(instructions) if operands[0][2] in v), None)
        if opcode == "CALL":
            calls.append(index)
        return jump_to + 1

    #DONE?
    if opcode == "MOVE":
        if len(operands) != 2:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1)
        if op1[1] == "":
            error_exit(56, 2, opcode)
        edit_variable(operands[0][2], [op1[2],op1[1]])
        return -1

    #DONE?
    if opcode == "INT2CHAR":
        if len(operands) != 2:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 1, 0, 0)
        if op1[1] == "":
            error_exit(56, 2,opcode)
        char = op1[2]
        if char < 0 or char > 1114111:
            error_exit(58, 0, "Variable '{}'({}) out of range for '{}'\n".format(op1[1], char, opcode))
        edit_variable(operands[0][2], [chr(char), "string"])
        return -1

    #DONE?
    if opcode == "STRLEN":
        if len(operands) != 2:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 1, 0)
        if op1[1] != "string":
            error_exit(56, 2, opcode)
        edit_variable(operands[0][2], [len(op1[2]), "int"])
        return -1
    
    #DONE?
    if opcode == "TYPE":
        if len(operands) != 2:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1)
        edit_variable(operands[0][2], [op1[1], "string"])
        return -1

    #DONE?
    if opcode in ["AND", "OR"]:
        if len(operands) != 3:
            error_exit(32, 1, len(operands), opcode)
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 0, 1)
        op2 = get_var_const(operands[2], opcode, 1, 0, 0, 0, 1)
        result = basic_operations(op1[1], op1[2], op2[1], op2[2], opcode)
        edit_variable(operands[0][2], [result, "bool"])
        return -1

    #DONE?
    if opcode == "NOT":
        if len(operands) != 2:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 0, 1)
        if op1[1] != "bool":
            error_exit(56, 2, opcode)
        edit_variable(operands[0][2], ["false", "bool"] if op1[2] == "true" else ["true", "bool"])
        return -1

    #DONE?
    if opcode in ["ADD", "SUB", "MUL", "IDIV"]:
        if len(operands) != 3:
            error_exit(32, 1, len(operands), opcode)
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 1, 0, 0)
        op2 = get_var_const(operands[2], opcode, 1, 0, 1, 0, 0)
        if op1[1] != "int" or op2[1] != "int":
            error_exit(56, 2, opcode)
        result = arithemtics(op1[2], op2[2], opcode)
        edit_variable(operands[0][2], [result, "int"])
        return -1

    #DONE?
    if opcode in ["LT", "GT", "EQ"]:
        if len(operands) != 3:           error_exit(32, 1, len(operands), opcode)
        option = 0
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1)
        op2 = get_var_const(operands[2], opcode, 1, 1, 1, 1, 1)
        result = basic_operations(op1[1], op1[2], op2[1], op2[2], opcode)
        edit_variable(operands[0][2], [result, "bool"])
        return -1

    #DONE?
    if opcode == "CONCAT":
        if len(operands) != 3:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 1, 0)
        op2 = get_var_const(operands[2], opcode, 1, 0, 0, 1, 0)
        if op1[1] != "string" or op2[1] != "string":
            error_exit(56, 2, opcode)
        edit_variable(operands[0][2], [op1[2]+op2[2], "string"])
        return -1

    #DONE?
    if opcode == "SETCHAR":
        if len(operands) != 3:
            error_exit(32, 1, len(operands), opcode)
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 1, 0, 0)
        op2 = get_var_const(operands[2], opcode, 1, 0, 0, 1, 0)
        if op0[1] == "" or op1[1] == "" or op2[1] == "":
            error_exit(56, 2, opcode)
        if op0[1] != "string":
            error_exit(53, 1, op0[1], opcode)
        string = op0[2]
        index = op1[2]
        if len(op2[2]) == 0:
            error_exit(58, 1, "SETCHAR\n")
        replace = op2[2][0]
        if len(op0[2])-1 < op1[2] or op1[2] < 0:
            error_exit(58, 0, "Index '{}' out of range\n".format(operands[2][2]))
        string = string[:index] + replace + string[index+1:]
        edit_variable(operands[0][2], [string, "string"])
        return -1

    #DONE?
    if opcode in ["STRI2INT", "GETCHAR"]:
        if len(operands) != 3:
            error_exit(32, 1, len(operands), opcode)
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 1, 0)
        op2 = get_var_const(operands[2], opcode, 1, 0, 1, 0, 0)
        
        if op1[1] != "string" or op2[1] != "int":
            error_exit(56, 2, opcode)
        
        string = op1[2]
        index = op2[2]

        if len(string) - 1 < index or index < 0:
            error_exit(58, 0, "Index '{}' out of range for '{}'\n".format(index, string))
        char = string[index]

        if opcode == "STRI2INT":
            edit_variable(operands[0][2], [ord(char), "int"])
        else:
            edit_variable(operands[0][2], [char, "string"])
        return -1

    #DONE?
    if opcode == "READ":
        if len(operands) != 2:
            error_exit(32, 1, len(operands), opcode)
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0)
        #specific for read
        op1_type = get_type(operands[1])
        op1_value = get_value(operands[1])
        if op1_type != "type":
            error_exit(53, 1, operands[1][1], opcode)
        if not(op1_value in ["int", "string", "bool"]):
            error_exit(57, 1, op1_value, "type", opcode)
        inPut = input_f.readline()
        inPut = inPut.rstrip("\n")
        #inPut = check_string(inPut)
        if op1_value == "int":
            try:
                edit_variable(operands[0][2], [int(inPut), "int"])
            except:
                error_exit(57, 3, inPut, op1_value, opcode)
        if op1_value == "string":
            edit_variable(operands[0][2], [str(inPut), "string"])
        if op1_value == "bool":
            if inPut.lower() == "true":
                edit_variable(operands[0][2], ["true", "bool"])
            else:
                edit_variable(operands[0][2], ["false", "bool"])
        return -1

    #DONE?
    if opcode in ["JUMPIFEQ", "JUMPIFNEQ"]:
        if len(operands) != 3:
            error_exit(32, 1, len(operands), opcode)
        if operands[0][1] != "label":
            error_exit(53, 1, operands[0][1], opcode)
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1)
        op2 = get_var_const(operands[2], opcode, 1, 1, 1, 1, 1)
        if op1[1] == "" or op2[1] == "":
            error_exit(56, 2, opcode)
        return jump_if(op1[2], op1[1], op2[2], op2[1], operands[0][2], opcode)

    error_exit(32, 2, opcode)

source_f = stdin
input_f = stdin

parser = argparse.ArgumentParser()
parser.add_argument('--source', metavar='file', required=False, help='Source XML file to be interpreted')
parser.add_argument('--input',  metavar='file', required=False, help='Input arguments for source')
args = parser.parse_args()
if not (args.source or args.input):
    stderr.write("At least 1 argument (--source or --input) is required\n")
    exit(10)
try:
    if args.source:
        source_f = open(args.source, 'r')
    if args.input: 
        input_f = open(args.input, 'r')
except:
    stderr.write("Invalid input/source file\n")
    exit(11)


#XML check
try:
    root = ET.fromstring(source_f.read())
except Exception as e:
    #print(e.code, e.msg, e.text)    #TODO remove
    error_exit(31, 0, e.msg)

if root.tag != "program" or root.attrib.get("language") != "IPPcode21": #header check
    stderr.write("XML: Invalid or missing header\n")
    exit(32)

instructions = []
labels = []
duplicates = []

#TODO case sensitivity
#TODO clean
for elem in root:
    if elem.tag != "instruction":
        error_exit(32, 0, "XML: Unknown tag {}\n".format(elem.tag))
    att = list(elem.attrib.keys())
    
    if not("order" in att) or not("opcode" in att):
        error_exit(32, 0, "Missing 'order' or 'opcode' attribute(s)\n")
    try:
        order = int(elem.attrib.get("order"))
    except:
        error_exit(32, 0, "Invalid instruction indexing\n")
    if order <= 0:             error_exit(32, 0, "Negative or zero instruction indexing\n")
    if order in duplicates:    error_exit(32, 0, "Duplicit instruction index\n")
    
    duplicates.append(order)
    
    arguments = []
    arg_duplicates = []

    for subelem in elem:
        if not(re.match(r"arg[123]", subelem.tag)):    error_exit(32, 0, "Invalid argument number\n")
        if not("type" in list(subelem.attrib.keys())): error_exit(32, 0, "Argument missing 'type'\n")
        arg_order = int(re.sub(r"[^0-9]", "", subelem.tag))
        if arg_order in arg_duplicates:                error_exit(32, 0, "Duplicit argument number\n")
        arg_duplicates.append(arg_order)
    
        arguments.append([arg_order, subelem.attrib.get("type"), subelem.text])
    
    arguments.sort(key=operator.itemgetter(0))
    if len(arguments) > 0 and arguments[0][0] != 1: error_exit(32, 0, "Invalid argument number\n")
    if len(arguments) > 1 and arguments[1][0] != 2: error_exit(32, 0, "Invalid argument number\n")
    if len(arguments) > 2 and arguments[2][0] != 3: error_exit(32, 0, "Invalid argument number\n")

    #save labels on the first run
    if elem.attrib.get("opcode") == "LABEL":
        if len(arguments) != 1:        error_exit(32, 1, len(arguments), "LABEL")
        if arguments[0][1] != "label": error_exit(53, 1, arguments[0][1], "LABEL")
        if arguments[0][2] in labels:  error_exit(52, 0, "Duplicit label\n")
        labels.append(arguments[0][2])
        instructions.append([int(elem.attrib.get("order")), "LABEL", arguments[0][2]])
    else:
        instructions.append([int(elem.attrib.get("order")), elem.attrib.get("opcode").upper(), arguments])

instructions.sort(key=operator.itemgetter(0))

#global variables (bad pactice)
frame_cnt = 0
frames = [{}, None]    #[{GF}, {TF}, {frame1} {frame2}, {...}]
calls = []
var_stack = []

i = 0
while i < len(instructions):
    found_index = check_ops(instructions[i][1], instructions[i][2], labels, calls, i)
    if found_index < 0:
        i += 1
    else:
        i = found_index

if source_f != stdin:
    source_f.close()
if input_f != stdin:
    input_f.close()