<?php

ini_set('display_errors', 'stderr');

//print error (to stderr)
function error_exit($error_type, $where, $code_line) {
    switch($error_type) {
        case 10: fprintf(STDERR, "Invalid argument or combination of arguments!\n");                            exit(10);
        case 21: fprintf(STDERR, "Missing or invalid header (line:%d)\n", $code_line);                          exit(21);
        case 22: fprintf(STDERR, "Invalid or unknown opcode on line %d: %s\n", $code_line, $where);             exit(22);
        case 23: fprintf(STDERR, "Invalid operand on line %d: %s\n", $code_line, $where);                       exit(23);
        case 24: fprintf(STDERR, "Invalid number of operands on line %d for opcode: %s\n", $code_line, $where); exit(23);
    }
}

//variable check
function var_check($string) {
    return preg_match("/(LF|TF|GF)@[a-zA-Z_\-\$&%\*!\?][a-zA-Z_\-\$&%\*!\?0-9]*/", $string) ? true : false;
}

//label check
function label_check($string) {
    return preg_match("/^[a-zA-Z_\-\$&%\*!\?][a-zA-Z_\-\$&%\*!\?0-9]*$/", $string) ? true : false;
}

//type check
function type_check($string) {
    if ($string == "int" || $string == "bool" || $string == "string") return 7;
    return 0;
}

//symb check (const or variable)
function symb_check($string) {
    if (preg_match("/int@(\-[0-9]+|[0-9]+|\+[0-9]+)$/", $string)) return 1;  //int check
    else if (preg_match("/bool@(true|false)$/", $string)) return 2;  //bool check
    else if (preg_match("/nil@nil$/", $string)) return 3;            //nil check
    else if (preg_match_all("/string@/", $string)) {                //string check
        if (substr_count($string, "\\") > preg_match_all("/(\\\\\\d{3})/", $string)) return 0;   //check if '\' occures more than '\xxx' (x - digit)
        return 4;
    }
    else if (var_check($string)) return 5;   //check variable
    else return 0;
}

//get operand type (used for instruction printing)
function get_type($type) {
    switch($type) {
        case 1: return "int";
        case 2: return "bool";
        case 3: return "nil";
        case 4: return "string";
        case 5: return "var";
        case 6: return "label";
        case 7: return "type";
    }
}

//print instruction
function print_instruction($order, $name, $types, $args) {
    echo '  <instruction order="', $order, '" opcode="', $name, '">', "\n"; //print instruction
    
    //go through arrays and print individual operands (if we can)
    for ($i = 0; $i < 3; $i++){
        if (!$types[$i] == 0) {
            if ($types[$i] < 5 && $types[$i] > 0 && strpos($args[$i], "@")) {   //if type is bool, int, nil or string
                $args[$i] = substr($args[$i], strpos($args[$i], '@')+1);        //remove the "prefix"
            }
            echo '    <arg', $i+1,' type="', get_type($types[$i]), '">', $args[$i], '</arg', $i+1,'>', "\n";  //print args
        }
    }
    echo '  </instruction>', "\n";  //print end of isntruction
}

//"global" variables
$header = false;
$order = 1;
$code_line = 0;

//argument check
if ($argc == 2 && $argv[1] == "--help") {
    printf("Usage: parse.php [OPTION] < input\n");
    printf("--help\t\t Display this message\n");
    exit(0);
}
else if ($argc > 2) error_exit(10, "", $code_line);

while ($line = fgets(STDIN)) {  //read line by line
    $code_line++;                                                           //increase current line in source code (for errors)
    if (strlen(trim($line)) == 0) continue;                                 //skip empty lines
    if (strpos($line, "#") === 0) continue;                                 //skip comments (# has to be first character on line -> indented comments go through)
    if (strpos($line, "#")) $line = substr($line, 0, strpos($line, '#'));   //remove comments
    $line = str_replace("&", "&amp;", $line);                                //replace problem chars
    $line = str_replace("<", "&lt;", $line);                                //replace problem chars
    $line = str_replace(">", "&gt;", $line);                                //replace problem chars
    $split = preg_split("/\s+/", $line, -1, PREG_SPLIT_NO_EMPTY);           //split lines using any white space and remove empty parts

    //header check
    if (!$header) {
        if (strtoupper($split[0]) == ".IPPCODE21") {
            $header = true;
            echo '<?xml version="1.0" encoding="utf-8"?>', "\n";
            echo '<program language="IPPcode21">', "\n";
        }
        else error_exit(21, "", $code_line);
        continue;
    }

    //fix for indented comments
    $opcode = "";
    if (sizeof($split) != 0) $opcode = strtoupper($split[0]);

    //opcode check
    switch ($opcode) {
        /*--------(0)-------- */
        case "CREATEFRAME":
        case "PUSHFRAME":
        case "POPFRAME":
        case "RETURN":
        case "BREAK":
            if (sizeof($split) != 1) error_exit(24, $split[0], $code_line);         //operand count check
            print_instruction($order, $opcode, array(0, 0, 0), array("", "", ""));  //print instruction
            break;
        /*--------(1 <var>)-------- */
        case "DEFVAR":
        case "POPS":
            if (sizeof($split) != 2)   error_exit(24, $split[0], $code_line);
            if (!var_check($split[1])) error_exit(23, $split[1], $code_line);   //check operand(s)
            print_instruction($order, $opcode, array(5, 0, 0,), array($split[1], "", ""));
            break;
        /*--------(1 <symb>)-------- */
        case "PUSHS":
        case "WRITE":
        case "EXIT":
        case "DPRINT":
            if (sizeof($split) != 2) error_exit(24, $split[0], $code_line);
            $symb_type = symb_check($split[1]);                                 //using extra variables to not call symb_check twice :shrunk:
            if (!$symb_type)         error_exit(23, $split[1], $code_line);
            print_instruction($order, $opcode, array($symb_type, 0, 0), array($split[1], "", ""));
            break;
        /*--------(1 <label>)-------- */
        case "CALL":
        case "LABEL":
        case "JUMP":
            if (sizeof($split) != 2)     error_exit(24, $split[0], $code_line);
            if (!label_check($split[1])) error_exit(23, $split[1], $code_line);
            print_instruction($order, $opcode, array(6, 0, 0), array($split[1], "", ""));
            break;
        /*--------(2 <var> <symb>)-------- */
        case "MOVE":
        case "NOT": //special
        case "INT2CHAR":
        case "STRLEN":
        case "TYPE":
            if (sizeof($split) != 3)   error_exit(24, $split[0], $code_line);
            $symb_type = symb_check($split[2]);
            if (!var_check($split[1])) error_exit(23, $split[1], $code_line);
            if (!$symb_type)           error_exit(23, $split[2], $code_line);
            print_instruction($order, $opcode, array(5, $symb_type, 0), array($split[1], $split[2], ""));
            break;
        /*--------(3 <var> <symb1> <symb2>)-------- */
        case "ADD":
        case "SUB":
        case "MUL":
        case "IDIV":
        case "LT":
        case "GT":
        case "EQ":
        case "AND":
        case "OR":
        case "STRI2INT":
        case "CONCAT":
        case "GETCHAR":
        case "SETCHAR":
            if (sizeof($split) != 4)   error_exit(24, $split[0], $code_line);
            $symb_type1 = symb_check($split[2]);
            $symb_type2 = symb_check($split[3]);
            if (!var_check($split[1])) error_exit(23, $split[1], $code_line);
            if (!$symb_type1)          error_exit(23, $split[2], $code_line);
            if (!$symb_type2)          error_exit(23, $split[3], $code_line);
            print_instruction($order, $opcode, array(5, $symb_type1, $symb_type2), array($split[1], $split[2], $split[3]));
            break;
        /*--------(2 <var> <type>)-------- */
        case "READ":
            if (sizeof($split) != 3)   error_exit(24, $split[0], $code_line);
            $type_type = type_check($split[2]);
            if (!var_check($split[1])) error_exit(23, $split[1], $code_line);
            if (!$type_type)           error_exit(23, $split[2], $code_line);
            print_instruction($order, $opcode, array(5, $type_type, 0), array($split[1], $split[2], ""));
            break;
        /*--------(3 <label> <symb1> <symb2>)-------- */
        case "JUMPIFEQ":
        case "JUMPIFNEQ":
            if (sizeof($split) != 4)     error_exit(24, $split[0], $code_line);
            $symb_type1 = symb_check($split[2]);
            $symb_type2 = symb_check($split[3]);
            if (!label_check($split[1])) error_exit(23, $split[1], $code_line);
            if (!$symb_type1)            error_exit(23, $split[2], $code_line);
            if (!$symb_type2)            error_exit(23, $split[3], $code_line);
            print_instruction($order, $opcode, array(6, $symb_type1, $symb_type2), array($split[1], $split[2], $split[3]));
            break;
        case "": break; //indented comments "fix"/skip
        default: error_exit(22, $split[0], $code_line);
    }
    $order++;
}
if (!$header) error_exit(21, "", $code_line); //exit(21);
echo "</program>\n";
exit(0)
?>