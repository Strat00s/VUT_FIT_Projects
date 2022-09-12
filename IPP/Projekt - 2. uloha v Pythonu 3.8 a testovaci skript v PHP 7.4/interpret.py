#imports
import argparse
import operator
import sys
import re
import os
import xml.etree.ElementTree as ET

#print error message and exit
#in: return code, message
def error_exit(return_code, message):
    sys.stderr.write(message)
    sys.exit(return_code)

#check string for correct escape sequences and perform them
#in: string
#out: string with replace escape sequences
def check_string(string):
    #might not be required anymore, not sure
    if string == None:
        return ""
    esq = re.findall(r"(\\\d{3})", string) #save how many times did escape sequence occure
    #exit if there are invalid escape sequences
    if string.count("\\") > len(esq):
        error_exit(57, "Invalid escape sequence in string '{}'\n".format(string))
    
    esq = set(esq)
    #go through escape sequences in current string and replace them
    for i in esq:
        unicode = int(i[1:])
        string = string.replace(i, chr(unicode))
    return string

#derive frame and name from entire variable name
#in: name of variable (FRAME@VAR)
#out: variable frame, variable name
def get_frame_name(name):
    #regex check for corrent name
    if not(re.match(r"^(GF|LF|TF)@", name)):
        error_exit(55, "Undefined frame for variable '{}'\n".format(name))

    global frame_cnt
    global frames
    frame = name[:2] #save frame name
    name = name[3:]  #save var name
    #save frame as index of 'frames'
    if frame == "GF":
        frame = 0
    if frame == "TF":
        #exit if TF does not exists yet
        if frames[1] == None:
            error_exit(55, "Temporary frame not defined\n")
        frame = 1
    if frame == "LF":
        #exit if there is no LF
        if frame_cnt == 0:
            error_exit(55, "Local frame not defined\n")
        frame = frame_cnt + 1
    return frame, name

#try to find a variable and return it's data
#in: name of variable (FRAME@VAR)
#out: if found: variable tpye, variable value, True
#     else: variable frame, variable name, False
def find_variable(name):
    global frames
    frame, var_name = get_frame_name(name) #get frame and var name
    var_data = frames[frame].get(var_name) #find it's data
    #if variable wasn't found
    if var_data == None:
        return frame, var_name, False
    else:
        return var_data[0], var_data[1], True

#check operand type, check type of value
#if variable, check that it exists and check type
#in: operand, opcode for error exit, check varialbe, check nil, check int, check string, check bool, does it exist
#out: variable or constant, type, value
def get_var_const(operand, opcode, var, nil, intt, string, boolt, is_set=1):
    match = False #set default match state
    var_const = "const" #set default type
    typet = ""
    value = ""

    #if we want variable
    if var and operand[1] == "var":
        var_const = "var"                  #set variable type
        search = find_variable(operand[2]) #search for it
    
        #exit if it wasn't found
        if search[2] == False:
            error_exit(54, "Undefined variable '{}'\n".format(operand[2]))
        
        value = search[0] #save var value
        typet = search[1] #save var type

        #exit on defined but unset variables (if they are supposed to be set)
        if is_set and typet == "":
            error_exit(56, "Missing value for opcode '{}'\n".format(opcode))
        
        match = True #set match

        #check and set type of variable
        if nil or intt or string or boolt:
            match = False                                 #set match
            if typet == "":                  match = True #set match
            if nil    and typet == "nil":    match = True #set match
            if intt   and typet == "int":    match = True #set match
            if string and typet == "string": match = True #set match
            if boolt  and typet == "bool":   match = True #set match

    #if we want nil constant
    if nil and operand[1] == "nil":
        #regex check name
        if not(re.match(r"^nil$", operand[2])):
            error_exit(32, "Invalid value '{}' for type '{}' at '{}'\n".format(operand[2], operand[1], opcode))

        operand[2] = ""    #edit value
        value = operand[2] #save value
        typet = "nil"      #save type
        match = True       #set match

    #if we want int constant
    if intt and operand[1] == "int":
        #regex check name
        if not(re.match(r"^[+-]?\d+$", str(operand[2]))):
            error_exit(32, "Invalid value '{}' for type '{}' at '{}'\n".format(operand[2], operand[1], opcode))

        operand[2] = int(operand[2]) #edit value as int
        value = operand[2]           #save value
        typet = "int"                #save type
        match = True                 #set match

    #if we want bool const
    if boolt and operand[1] == "bool":
        #regex check name
        if not(re.match(r"^(true|false)$", operand[2])):
                error_exit(32, "Invalid value '{}' for type '{}' at '{}'\n".format(operand[2], operand[1], opcode))

        value = operand[2] #save value
        typet = "bool"     #save type
        match = True       #set match

    #if we want string const
    if string and operand[1] == "string":
        value = check_string(operand[2]) #check and save string value
        typet = "string"                 #save type
        match = True                     #set match

    #on match
    if match == True:
        return var_const, typet, value
    else:
        error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(get_type(operand), opcode))

#add variable to frame
#in: name of variable (FRAME@VAR)
def add_variable(name):
    global frames
    search = find_variable(name) #find variable

    #add variable if none was found
    if search[2] == False:
        frames[search[0]][search[1]] = ["", ""]
    else:
        error_exit(52, "Variable '{}' already defined".format(name))

#edit given variable
#in: name of variable (FRAME@VAR), value and type which to write as array
def edit_variable(name, edit):
    global frames
    frame, var_name = get_frame_name(name) #get frame and name
    frames[frame][var_name] = edit         #edit variable

#return operand value
#in: operand
#out: operand value
def get_value(operand):
    #return value if var or const
    if operand[1] == "var":
        return find_variable(operand[2])[0]
    else:
        return operand[2]

#return operand type
#in: operand
#out: operand type (int, bool, string, nil)
def get_type(operand):
    #return type depending on var or const
    if operand[1] == "var":
        search = find_variable(operand[2]) #find variable
        
        #exit on undefined variable
        if search[2] == False:
            error_exit(54, "Undefined variable '{}'\n".format(operand[2]))
        
        op_type = search[1] #set type
    else:
        op_type = operand[1] #set type
    return op_type

#find label index on JUMPIFEQ and JUMPIFNEQ
#in: first operand type, first operand value, second operand type, second operand value, opcode for error exit
#out: where to jump
def jump_if(op1_type, op1_val, op2_type, op2_val, label, opcode):
    global instructions

    #exit if neither is nil
    if op1_type != op2_type and op1_type != "nil" and op2_type != "nil":
        error_exit(53, "Invalid operand type '{}' and '{}' for opcode '{}'\n".format(op1_type, op2_type, opcode))

    #find first occurance of 'label' in instructions list and get it's index
    #taken from: https://stackoverflow.com/a/25398310
    jump_to = next((i for i, instr in enumerate(instructions) if label in instr), None)
    
    #exit on unknown label (might not be needed)
    if jump_to == None:
        error_exit(52, "Jump to unknown label '{}'\n".format(label))

    #compare and jump
    if op1_val == op2_val and opcode == "JUMPIFEQ":
        return jump_to + 1

    #compare and jump
    if op1_val != op2_val and opcode == "JUMPIFNEQ" :
        return jump_to + 1
    return -1

#perform "basic" (and, or, <, >, =) operations
#in: first operand type, first operand value, second operand type, second operand value, opcode for error exit
#out: result of the operation
def basic_operations(op1_type, op1_val, op2_type, op2_val, opcode):
    #perform AND
    if opcode == "AND":
            return "true" if op1_val == "true" and op2_val == "true" else "false"

    #perform OR
    if opcode == "OR":
        return "true" if op1_val == "true" or op2_val == "true" else "false"

    #nil only with EQ
    if op1_type == "nil" or op2_type == "nil":
        if opcode !=  "EQ":
            error_exit(53, "Invalid operand type '{}' and '{}' for opcode '{}'\n".format(op1_type, op2_type, opcode))
        return "true" if op1_val == op2_val else "false"

    #exit on different types
    if op1_type != op2_type:
        error_exit(53, "Invalid operand type '{}' and '{}' for opcode '{}'\n".format(op1_type, op2_type, opcode))

    #perform EQ
    if opcode == "EQ":
        return "true" if op1_val == op2_val else "false"

    #on bool
    elif op1_type == "bool":
        #perform LT, else GT
        if opcode == "LT":
            return "true" if op1_val == "false" and op2_val == "true" else "false"
        else:
            return "true" if op1_val == "true" and op2_val == "false" else "false"

    #on string or int
    elif op1_type in ["int", "string"]:
        #perform LT, else GT
        if opcode == "LT":
            return "true" if op1_val < op2_val else "false"
        else:
            return "true" if op1_val > op2_val else "false"

#perform arithmetics operations
#in: first operand value, second operand value, opcode for error exit
#out: result
def arithmetics(op1, op2, opcode):
    if opcode == "ADD":  return op1 + op2 #perform ADD
    if opcode == "SUB":  return op1 - op2 #perform SUB
    if opcode == "MUL":  return op1 * op2 #perform MUL
    if opcode == "IDIV":                  #perform IDIV
        if op2 == 0:
            error_exit(57, "Division by zero\n")
        return int(op1 // op2)

#check stack size
#in: size we need, opcode for error exit
def check_stack(size, opcode):
    global var_stack

    #exit if stack is not big enough
    if len(var_stack) < size:
        error_exit(56, "Not enought items on stack for '{}'\n".format(opcode))

#check operand count
#in: actual operand count, opcode for error exit, count we want
def check_operands_cnt(op_cnt, opcode, cnt):
    #exit if not enough operands
    if op_cnt != cnt:
        error_exit(32, "Invalid number of operands '{}' for opcode '{}'\n".format(op_cnt, opcode))

#perform and check all instructions
#in: opcode, operands array, label array, call array, index of current instruction
#out: index where to jump
def instr_exec(opcode, operands, labels, calls, index):
    #get global variables
    global frames
    global frame_cnt
    global var_stack
    global instructions

    if opcode in ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]:
        check_operands_cnt(len(operands), opcode, 0) #check number of operands

        if opcode == "CREATEFRAME":
            frames[1] = {} #create temporary frame (as dict)

        if opcode == "PUSHFRAME":
            #check for temporary frame
            if frames[1] == None:
                error_exit(55, "Invalid frame operation '{}'\n".format(opcode))
            frame_cnt += 1           #increase frame counter
            frames.append(frames[1]) #move variables to LF
            frames[1] = None         #reset TF

        if opcode == "POPFRAME":
            frame_cnt -= 1 #decrease frame counter
            #exit if pop on empty frame
            if frame_cnt < 0:
                error_exit(55, "Invalid frame operation '{}'\n".format(opcode))
            if len(frames) - frame_cnt != 3:
                error_exit(55, "Invalid frame operation '{}'\n".format(opcode))
            if len(frames) < 3:
                error_exit(55, "Invalid frame operation '{}'\n".format(opcode))
            frames[1] = frames.pop(-1) #pop variables from LF to TF

        if opcode == "RETURN":
            #exit if there is nowhere to jump
            if len(calls) == 0:
                error_exit(56, "Missing value for opcode '{}'\n".format(opcode))
            return calls.pop(-1) + 1 #return to call instruction +1

        if opcode == "BREAK":
            return -2 #return BREAK index
        return -1

    if opcode == "DEFVAR":
        check_operands_cnt(len(operands), opcode, 1) #check operand count
        add_variable(operands[0][2])                 #try and add a variable
        return -1

    if opcode in ["WRITE", "EXIT", "DPRINT"]:
        check_operands_cnt(len(operands), opcode, 1)            #check operand count
        op0 = get_var_const(operands[0], opcode, 1, 1, 1, 1, 1) #check and save operand

        if opcode == "WRITE":
            print(op0[2], end='') #print operand value

        if opcode == "DPRINT":
            sys.stderr.write(op0[2]) #print operand value to stderr

        if opcode == "EXIT":

            #exit on invalid type
            if op0[1] != "int":
                error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(op0[1], opcode))
            return_code = op0[2]
            #check return code range, otherwise exit with 57
            if return_code >= 0 and return_code <= 49:
                sys.exit(return_code)
            error_exit(57, "Invalid exit code '{}'\n".format(return_code))
        return -1

    if opcode == "PUSHS":
        check_operands_cnt(len(operands), opcode, 1)            #check operand count
        op0 = get_var_const(operands[0], opcode, 1, 1, 1, 1, 1) #check and save operand
        var_stack.append([op0[2], op0[1]])                      #push variable to stack
        return -1

    if opcode == "POPS":
        check_operands_cnt(len(operands), opcode, 1)         #check operand count
        check_stack(1, opcode)                               #check stack size for operation
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0) #check operand
        edit_variable(operands[0][2], var_stack.pop(-1))     #edit variable with value from stack
        return -1

    if opcode == "CLEARS":
        var_stack.clear() #clear stack
        return -1

    if opcode in ["ADDS", "SUBS", "MULS", "DIVS", "IDIVS"]:
        check_stack(2, opcode) #check stack size
        #exit on invalid types
        if var_stack[-1][1] != "int" or var_stack[-2][1] != "int":
            error_exit(53, "Invalid variable type on stack for '{}'\n".format(opcode))

        op1 = var_stack[-2]                               #save second var on stack
        op2 = var_stack[-1]                               #save first var on stack
        result = arithmetics(op1[0], op2[0], opcode[:-1]) #perform operation
        var_stack[-2] = [result, "int"]                   #save result to second var
        var_stack.pop(-1)                                 #pop fisr var
        return -1

    if opcode in ["LTS", "GTS", "EQS"]:
        check_stack(2, opcode)                                                 #check stack size
        op1 = var_stack[-2]                                                    #save second var from stack
        op2 = var_stack[-1]                                                    #save first var from stack
        result = basic_operations(op1[1], op1[0], op2[1], op2[0], opcode[:-1]) #perform operation
        var_stack[-2] = [result, "bool"]                                       #save result to second var
        var_stack.pop(-1)                                                      #pop first var
        return -1

    if opcode in ["ANDS", "ORS"]:
        check_stack(2, opcode) #check stack size
        #exit on invalid types
        if var_stack[-2][1] != "bool" or var_stack[-1][1] != "bool":
            error_exit(53, "Invalid variable type on stack for '{}'\n".format(opcode))
        
        op1 = var_stack[-2]                                                    #save second var from stack
        op2 = var_stack[-1]                                                    #save first var from stack
        result = basic_operations(op1[1], op1[0], op2[1], op2[0], opcode[:-1]) #perform operation
        var_stack[-2] = [result, "bool"]                                       #save result to second var
        var_stack.pop(-1)                                                      #pop first var
        return -1

    if opcode == "NOTS":
        check_stack(1, opcode) #check stack size
        #exit on invalid types
        if var_stack[-1][1] != "bool":
            error_exit(53, "Invalid variable type on stack for '{}'\n".format(opcode))

        var_stack[-1] = ["false", "bool"] if var_stack[-1][0] == "true" else ["true", "bool"] #flip bool value
        return -1
    
    if opcode == "INT2CHARS":
        check_stack(1, opcode) #check stack size
        #exit on invalid type
        if var_stack[-1][1] != "int":
            error_exit(53, "Invalid variable type on stack for '{}'\n".format(opcode))

        char = var_stack[-1][0]
        #check character range (Python3 chr())
        if char < 0 or char > 1114111:
            error_exit(58, "Variable '{}' out of range for '{}'\n".format(char, opcode))

        var_stack[-1] = [chr(char), "string"] #save var to stack
        return -1

    if opcode == "STRI2INTS":
        check_stack(2, opcode) #check stack size
        #exit on invalid type
        if var_stack[-2][1] != "string":
            error_exit(53, "Invalid variable type on stack for '{}'\n".format(opcode))
        if var_stack[-1][1] != "int":
            error_exit(53, "Invalid variable type on stack for '{}'\n".format(opcode))
        
        string = var_stack[-2][0] #save value
        index = var_stack[-1][0]  #save value
        #check index range
        if len(string) - 1 < index or index < 0:
            error_exit(58, "Index '{}' out of range for '{}'\n".format(index, string))
        char = string[index]               #get single character on index
        var_stack[-2] = [ord(char), "int"] #save result to second var
        var_stack.pop(-1)                  #pop first var
        return -1

    if opcode in ["JUMPIFEQS", "JUMPIFNEQS"]:
        check_operands_cnt(len(operands), opcode, 1) #check operand count
        #exit if not label
        if operands[0][1] != "label":
            error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(operands[0][1], opcode))

        check_stack(2, opcode)                                                      #check stack size
        op1 = var_stack[-2]                                                         #save second stack var
        op2 = var_stack[-1]                                                         #save first stack var
        return jump_if(op1[1], op1[0], op2[1], op2[0], operands[0][2], opcode[:-1]) #return jump index

    if opcode in ["CALL", "LABEL", "JUMP"]:
        #skip LABEL opcode
        if opcode == "LABEL":
            return -1

        check_operands_cnt(len(operands), opcode, 1) #check operand count
        #exit if operand is not label
        if operands[0][1] != "label":
            error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(operands[0][1], opcode))
        #exit on unknown label
        if not(operands[0][2] in labels):
            error_exit(52, "Jump to unknown label '{}'\n".format(operands[0][2]))

        #find first occurance of 'label' in instructions list and get it's index
        #taken from: https://stackoverflow.com/a/25398310
        jump_to = next((i for i, instr in enumerate(instructions) if operands[0][2] in instr), None)
        #save current index on CALL
        if opcode == "CALL":
            calls.append(index)
        return jump_to + 1 #return where to jump

    if opcode == "MOVE":
        check_operands_cnt(len(operands), opcode, 2)            #check operand count
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)    #check that first operand is a variable
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1) #check second operand
        edit_variable(operands[0][2], [op1[2],op1[1]])          #write value to variable
        return -1

    if opcode == "INT2CHAR":
        check_operands_cnt(len(operands), opcode, 2)            #check number of operands
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)    #check that first operand is a var
        op1 = get_var_const(operands[1], opcode, 1, 0, 1, 0, 0) #check second operand type
        char = op1[2]
        #exit on invalid range
        if char < 0 or char > 1114111:
            error_exit(58, "Variable '{}'({}) out of range for '{}'\n".format(op1[1], char, opcode))

        edit_variable(operands[0][2], [chr(char), "string"]) #save char
        return -1

    if opcode == "STRLEN":
        check_operands_cnt(len(operands), opcode, 2)            #check number of operands
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)    #check that first operands is variable
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 1, 0) #check second operand type
        edit_variable(operands[0][2], [len(op1[2]), "int"])     #save result
        return -1

    if opcode == "TYPE":
        check_operands_cnt(len(operands), opcode, 2)               #check operand count
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)       #check first operand is var
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1, 0) #check second operand type
        edit_variable(operands[0][2], [op1[1], "string"])          #save result
        return -1

    if opcode in ["AND", "OR"]:
        check_operands_cnt(len(operands), opcode, 3)                      #check operand count
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)        #check if var
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 0, 1)           #check operand type
        op2 = get_var_const(operands[2], opcode, 1, 0, 0, 0, 1)           #check operand type
        result = basic_operations(op1[1], op1[2], op2[1], op2[2], opcode) #perform operation
        edit_variable(operands[0][2], [result, "bool"])                   #save result
        return -1

    if opcode == "NOT":
        check_operands_cnt(len(operands), opcode, 2)                                               #check operand count
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)                                       #check if var
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 0, 1)                                    #check type
        edit_variable(operands[0][2], ["false", "bool"] if op1[2] == "true" else ["true", "bool"]) #save fliped value
        return -1

    if opcode in ["ADD", "SUB", "MUL", "IDIV"]:
        check_operands_cnt(len(operands), opcode, 3)               #check operand count
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0) #check if var
        op1 = get_var_const(operands[1], opcode, 1, 0, 1, 0, 0)    #check type
        op2 = get_var_const(operands[2], opcode, 1, 0, 1, 0, 0)    #check type
        result = arithmetics(op1[2], op2[2], opcode)               #perform operation
        edit_variable(operands[0][2], [result, "int"])             #save result
        return -1

    if opcode in ["LT", "GT", "EQ"]:
        check_operands_cnt(len(operands), opcode, 3)                      #check operand count
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)        #check if var
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1)           #check type
        op2 = get_var_const(operands[2], opcode, 1, 1, 1, 1, 1)           #check type
        result = basic_operations(op1[1], op1[2], op2[1], op2[2], opcode) #perform operation
        edit_variable(operands[0][2], [result, "bool"])                   #save result
        return -1

    if opcode == "CONCAT":
        check_operands_cnt(len(operands), opcode, 3)             #check operand count
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0)     #check if var
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 1, 0)  #check type
        op2 = get_var_const(operands[2], opcode, 1, 0, 0, 1, 0)  #check type
        edit_variable(operands[0][2], [op1[2]+op2[2], "string"]) #save result
        return -1

    if opcode == "SETCHAR":
        check_operands_cnt(len(operands), opcode, 3)            #check operand count
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0) #check if var
        op1 = get_var_const(operands[1], opcode, 1, 0, 1, 0, 0) #check type
        op2 = get_var_const(operands[2], opcode, 1, 0, 0, 1, 0) #check type
        #check that first operand is a string
        if op0[1] != "string":
            error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(op0[1], opcode))

        string = op0[2]
        index = op1[2]
        #exit on invalid string
        if len(op2[2]) == 0:
            error_exit(58, "Invalid string operation: SETCHAR\n")

        replace = op2[2][0] #save first character of string
        #exit on invalid range
        if len(op0[2])-1 < op1[2] or op1[2] < 0:
            error_exit(58, "Index '{}' out of range\n".format(operands[2][2]))

        string = string[:index] + replace + string[index+1:] #replace char on index
        edit_variable(operands[0][2], [string, "string"])    #save result
        return -1

    if opcode in ["STRI2INT", "GETCHAR"]:
        check_operands_cnt(len(operands), opcode, 3)               #check operand count
        op0 = get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0) #check if var
        op1 = get_var_const(operands[1], opcode, 1, 0, 0, 1, 0)    #check type
        op2 = get_var_const(operands[2], opcode, 1, 0, 1, 0, 0)    #check type
        
        string = op1[2]
        index = op2[2]
        #exit on invalid index
        if len(string) - 1 < index or index < 0:
            error_exit(58, "Index '{}' out of range for '{}'\n".format(index, string))

        char = string[index]

        #save result depending on opcode
        if opcode == "STRI2INT":
            edit_variable(operands[0][2], [ord(char), "int"])
        else:
            edit_variable(operands[0][2], [char, "string"])
        return -1

    if opcode == "READ":
        check_operands_cnt(len(operands), opcode, 2)         #check operand type
        get_var_const(operands[0], opcode, 1, 0, 0, 0, 0, 0) #check if var
        
        #specific for read
        op1_type = get_type(operands[1])
        op1_value = get_value(operands[1])
        #exit on invalid type and value
        if op1_type != "type":
            error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(operands[1][1], opcode))
        if not(op1_value in ["int", "string", "bool"]):
            error_exit(57, "Invalid value '{}' of operand '{}' at '{}'\n".format(op1_value, "type", opcode))
        inPut = input_f.readline() #read from input
        inPut = inPut.rstrip("\n") #remove new line if possible
        #inPut = check_string(inPut)

        #if int, try to convert it, otherwise exit
        if op1_value == "int":
            try:
                edit_variable(operands[0][2], [int(inPut), "int"])
            except:
                error_exit(57, "Invalid input value '{}' for type '{}' at '{}'\n".format(inPut, op1_value, opcode))
        #if string, save result
        if op1_value == "string":
            edit_variable(operands[0][2], [str(inPut), "string"])
        #if bool, check fo 'true', otherwise 'false'
        if op1_value == "bool":
            if inPut.lower() == "true":
                edit_variable(operands[0][2], ["true", "bool"])
            else:
                edit_variable(operands[0][2], ["false", "bool"])
        return -1

    if opcode in ["JUMPIFEQ", "JUMPIFNEQ"]:
        check_operands_cnt(len(operands), opcode, 3) #check operand count
        #exit if not label
        if operands[0][1] != "label":
            error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(operands[0][1], opcode))
        
        op1 = get_var_const(operands[1], opcode, 1, 1, 1, 1, 1) #check type
        op2 = get_var_const(operands[2], opcode, 1, 1, 1, 1, 1) #check type
        return jump_if(op1[1], op1[2], op2[1], op2[2], operands[0][2], opcode) #jump to result

    error_exit(32, "Invalid or unknown opcode '{}'\n".format(opcode)) #exit if no valid opcode was found

#default file settings (both as stdin)
source_f = sys.stdin
input_f = sys.stdin

#argument parsing
parser = argparse.ArgumentParser()
parser.add_argument('--source', metavar='file', required=False, help='Source XML file to be interpreted')
parser.add_argument('--input',  metavar='file', required=False, help='Input arguments for source')
args = parser.parse_args()

#if no argument is given
if not (args.source or args.input):
    sys.stderr.write("At least 1 argument (--source or --input) is required\n")
    sys.exit(10)

#try to open the files
try:
    if args.source:
        source_f = open(args.source, 'r')
    if args.input: 
        input_f = open(args.input, 'r')
except:
    error_exit(11, "Invalid input/source file\n")


#XML check
try:
    root = ET.fromstring(source_f.read())
except:
    error_exit(31, "Invalid XML formating")

#check root for program tag and correct language
if root.tag != "program" or root.attrib.get("language").upper() != "IPPCODE21": #header check
    sys.stderr.write("XML: Invalid or missing header\n")
    sys.exit(32)

#set variables
instructions = []   #instructions array
labels = []         #labels array
duplicates = []     #instruction index array

#go through xml
for elem in root:
    #exit on invalid element
    if elem.tag != "instruction":
        error_exit(32, "XML: Unknown tag {}\n".format(elem.tag))
    
    att = list(elem.attrib.keys())
    #exit on invalid tags
    if not("order" in att) or not("opcode" in att):
        error_exit(32, "Missing 'order' or 'opcode' attribute(s)\n")
    #get instruction index if possible
    try:
        order = int(elem.attrib.get("order"))
    except:
        error_exit(32, "Invalid instruction indexing\n")
    #check for negative and duplicit indexing
    if order <= 0:
        error_exit(32, "Negative or zero instruction indexing\n")
    if order in duplicates:
        error_exit(32, "Duplicit instruction index\n")

    duplicates.append(order)    #add index to duplicates

    arguments = []      #arguments array
    arg_duplicates = [] #argument index array

    #go through arguments
    for subelem in elem:
        #regex check for argument tag
        if not(re.match(r"arg[123]", subelem.tag)):
            error_exit(32, "Invalid argument number\n")
        #check for type tag
        if not("type" in list(subelem.attrib.keys())):
            error_exit(32, "Argument missing 'type'\n")

        arg_order = int(re.sub(r"[^0-9]", "", subelem.tag)) #save argument number (index)

        #exit on duplicit index
        if arg_order in arg_duplicates:
            error_exit(32, "Duplicit argument number\n")

        arg_duplicates.append(arg_order)                                        #add index tu diplicates
        arguments.append([arg_order, subelem.attrib.get("type"), subelem.text]) #add argument to array

    arguments.sort(key=operator.itemgetter(0)) #sort arguments by index

    #check correct arguments indexing
    if len(arguments) > 0 and arguments[0][0] != 1:
        error_exit(32, "Invalid argument number\n")
    if len(arguments) > 1 and arguments[1][0] != 2:
        error_exit(32, "Invalid argument number\n")
    if len(arguments) > 2 and arguments[2][0] != 3:
        error_exit(32, "Invalid argument number\n")

    #save labels on the first run
    if elem.attrib.get("opcode") == "LABEL":
        check_operands_cnt(len(arguments), "LABEL", 1)  #check number of arguments
        #exit if type is not label
        if arguments[0][1] != "label":
            error_exit(53, "Invalid operand type '{}' for opcode '{}'\n".format(arguments[0][1], "LABEL"))
        #exit on duplicit label
        if arguments[0][2] in labels:
            error_exit(52, "Duplicit label\n")

        labels.append(arguments[0][2])  #add label to array
        instructions.append([int(elem.attrib.get("order")), "LABEL", arguments[0][2]]) #add label to instructuion array, so that we can search for it later
    else:
        instructions.append([int(elem.attrib.get("order")), elem.attrib.get("opcode").upper(), arguments]) #add instructuin to array

instructions.sort(key=operator.itemgetter(0)) #sort instruction

#global variables
frame_cnt = 0       #local frame counter
frames = [{}, None] #[{GF}, {TF}, {frame1} {frame2}, {...}]
calls = []          #call "stack"
var_stack = []      #stack

i = 0

#go through the instructions array
while i < len(instructions):
    #execute instractions and save "new" index (for label jumps)
    found_index = instr_exec(instructions[i][1], instructions[i][2], labels, calls, i)

    #ob BREAK index, print interpret information
    if found_index == -2:
        width = os.get_terminal_size()[0]
        sys.stderr.write("-" * width)
        sys.stderr.write("\nStatus:\nInstruction (BREAK) {} out of {}\n".format(i+1, len(instructions)))
        sys.stderr.write("Last instruction ({}):\t".format(i))
        if i == 0:
            sys.stderr.write("None")
        else:
            arg_cnt = len(instructions[i-1][2])
            if arg_cnt >= 0:
                sys.stderr.write("{} ".format(instructions[i-1][1]))
            if arg_cnt >= 1:
                sys.stderr.write("{} ".format(instructions[i-1][2][0][2]))
            if arg_cnt >= 2:
                sys.stderr.write("{} ".format(instructions[i-1][2][1][2]))
            if arg_cnt >= 3:
                sys.stderr.write("{} ".format(instructions[i-1][2][2][2]))
        sys.stderr.write("\n")
        sys.stderr.write("Frame state:\n")
        sys.stderr.write("\tGF:  {}\n".format(frames[0]))
        sys.stderr.write("\tTF:  {}\n".format(frames[1]))
        sys.stderr.write("\tLFs: {}\n".format(frames[2:]))
        sys.stderr.write("Stack state:\n")
        sys.stderr.write("\t{}\n".format(var_stack))
        sys.stderr.write("-" * width)
        sys.stderr.write("\n")

    #continue in instruction on negative index, otherwise jump to new index
    if found_index < 0:
        i += 1
    else:
        i = found_index

#close files if possible
if source_f != sys.stdin:
    source_f.close()
if input_f != sys.stdin:
    input_f.close()