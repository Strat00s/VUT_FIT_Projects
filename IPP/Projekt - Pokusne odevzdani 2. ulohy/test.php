<?php

ini_set('display_errors', 'stderr');

//print error and exit with corrent return code
function error_exit($error_type, $arg1, $arg2) {
    switch($error_type) {
        case 10: fprintf(STDERR, "Invalid argument or combination of arguments: %s %s\n", $arg1, $arg2); exit(10);
        case 41: fprintf(STDERR, "Path/file specified is invalid/has invalid permission: %s\n", $arg1);  exit(41);
        default: fprintf(STDERR, "INTERNAL ERROR: %s\n", $arg1);                                         exit(99);
    }
}

//get/crteate absolute path
function get_abs_path($path) {
    if ($path[0] != '/') $path = getcwd() . '/' . $path;    //check if absolute path -> make it absolute
    return $path;
}

//print HTML
function print_html() {
    global $test_type, $tests_total, $passed, $failed, $html_table;

    printf("<!doctype html>\n");
    printf("<html lang='cz'>\n");
    printf("  <head>\n");
    printf("    <h1><strong>Test Summary</strong></h1>\n");
    printf("    <h2>Type: %s</h2>\n", $test_type);
    printf("    <h2>Total number of tests: %d</h2>\n", $tests_total);
    printf("    <h2><span data-darkreader-inline-color='' style='--darkreader-inline-color:#47d583; color:#2ecc71'>Passed: %d</span></h3>\n", $passed);
    printf("    <h2><span data-darkreader-inline-color='' style='--darkreader-inline-color:#e95849; color:#e74c3c'>Failed: %d</span></h3>\n", $failed);
    printf("  </head>\n");
    printf("  <body>\n");
    printf("    <h2>Detailed Results</h2>\n");
    printf("    <table border='1' cellpadding='1' cellspacing='1' style='width:100%%'>\n");
    printf("      <tbody>\n");
    printf("        <tr>\n");
    printf("          <td style='text-align:center; width:100px'>Test Number</td>\n");
    printf("          <td style='text-align:center; width:100px'>Status</td>\n");
    printf("          <td style='text-align:center'>Path</td>\n");
    printf("          <td style='text-align:center; width:150px'>Return Code (got/ref)</td>\n");
    printf("          <td style='text-align:center; width:100px'>Output Diff</td>\n");
    printf("        </tr>\n");

    //change colors depending on invalid return code and XML difference
    for ($i = 0; $i < sizeof($html_table); $i++){
        printf("        <tr>\n");
        printf("          <td style='text-align:center'>%d</td>\n", $html_table[$i][0]);
        if (!$html_table[$i][5]) printf("          <td style='background-color:#009900; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $html_table[$i][1]);
        else printf("          <td style='background-color:#ff3300; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $html_table[$i][1]);
        printf("          <td style='text-align:center'>%s</td>\n", $html_table[$i][2]);
        if ($html_table[$i][5] == 3 || $html_table[$i][5] == 1) printf("          <td style='background-color:#ff3300; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $html_table[$i][3]);
        else printf("          <td style='background-color:#009900; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $html_table[$i][3]);
        if ($html_table[$i][5] == 3 || $html_table[$i][5] == 2) printf("          <td style='background-color:#ff3300; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $html_table[$i][4]);
        else printf("          <td style='background-color:#009900; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $html_table[$i][4]);
        printf("        </tr>\n");
    }   
    printf("      </tbody>\n");
    printf("    </table>\n");
    printf("  </body>\n");
    printf("</html>\n");
}

function run_tests($test_type, $parse_script, $int_script, $jexamcfg, $jexamxml, $path) {

    //using global variables here, just to make stuff easier
    global $html_table, $passed, $failed;

    $full_path = $path;                                         //save full path for HTML
    $file_path = preg_replace("/\.[^.\s]{2,3}$/", "", $path);   //get file path without extension

    //check .in, .out and .rc files and create them if required
    if (!is_file($file_path . ".in")) touch($file_path . ".in");
    if (!is_file($file_path . ".out")) touch($file_path . ".out");
    if (!is_file($file_path . ".rc"))  {
        touch($file_path . ".rc");
        file_put_contents($file_path . ".rc", "0");
    }

    //parser
    $return_code;
    $def_rc;
    if ($test_type == "Parse only") {
        $def_rc = file_get_contents($file_path . ".rc");    //save example return code
        exec("php7.4 ". $parse_script . " < " . $full_path . " > parse.out 2> /dev/null", $output, $return_code);
    }

    if ($test_type == "Interpret only") {
        $def_rc = file_get_contents($file_path . ".rc");    //save example return code
        exec("python3.8 " . $int_script . " --source=" . $full_path . " --input=" . $file_path . ".in > int.out 2> /dev/null", $output, $return_code);

    }
    if ($test_type == "Both") {
        exec("php7.4 ". $parse_script . " < " . $full_path . " > parse.out 2> /dev/null", $output, $return_code); 
        $def_rc = file_get_contents($file_path . ".rc");    //save example return code
        exec("python3.8 " . $int_script . " --source=parse.out" . " --input=" . $file_path . ".in > int.out 2> /dev/null", $output, $return_code);
    }
    
    //on same return code
    if($return_code == $def_rc){
        //return code is different than 0
        if ($return_code){
            //push HTML table arguments onto the 2D array
            array_push($html_table[$passed+$failed], "SUCCESS");
            array_push($html_table[$passed+$failed], $full_path);
            array_push($html_table[$passed+$failed], $return_code . "/" . $def_rc);
            array_push($html_table[$passed+$failed], "UNKNOWN");   //we don't care what is the output, as we have the same error reurn code
            array_push($html_table[$passed+$failed], 0);   //0 - good, 1 - rc bad, 2 - XML bad, 3 - both
            $passed++;
            //skip jexam
        }
        //return code is same and 0
        else {
            //run jexamxml with parser
            if ($test_type == "Parse only") {
                exec("java -jar " . $jexamxml . " " . $file_path . ".out " . "parse.out " . "diffs.xml " . "/D " . $jexamcfg, $diff_output, $diff_retun_code);  //run jexam here, as we wan't to know if the XML is same or not
            }
            //run diff with interpreter and both
            else {
                exec("diff " . $file_path . ".out " . "int.out", $diff_output, $diff_retun_code);
            }
            //if files are different
            if ($diff_retun_code) {
                array_push($html_table[$passed+$failed], "FAIL");
                array_push($html_table[$passed+$failed], $full_path);
                array_push($html_table[$passed+$failed], $return_code . "/" . $def_rc);
                array_push($html_table[$passed+$failed], "DIFF");
                array_push($html_table[$passed+$failed], 2);
                $failed++;
            }
            //if they are same
            else {
                array_push($html_table[$passed+$failed], "SUCCESS");
                array_push($html_table[$passed+$failed], $full_path);
                array_push($html_table[$passed+$failed], $return_code . "/" . $def_rc);
                array_push($html_table[$passed+$failed], "SAME");
                array_push($html_table[$passed+$failed], 0);
                $passed++;
            }
        }
    }
    //on different return code
    else {
        if ($test_type == "Parse only") {
            exec("java -jar " . $jexamxml . " " . $file_path . ".out " . "parse.out " . "diffs.xml " . "/D " . $jexamcfg, $diff_output, $diff_retun_code);
        }
        //run diff with interpreter and both
        else {
            exec("diff " . $file_path . ".out " . "int.out", $diff_output, $diff_retun_code);
        }  //running it here as well just to see if return code is the only problem
        //if they are different
        if ($diff_retun_code) {
            array_push($html_table[$passed+$failed], "FAIL");
            array_push($html_table[$passed+$failed], $full_path);
            array_push($html_table[$passed+$failed], $return_code . "/" . $def_rc);
            array_push($html_table[$passed+$failed], "DIFF");
            array_push($html_table[$passed+$failed], 3);
            $failed++;
        }
        //if they are different (just bad return code)
        else {
            array_push($html_table[$passed+$failed], "FAIL");
            array_push($html_table[$passed+$failed], $full_path);
            array_push($html_table[$passed+$failed], $return_code . "/" . $def_rc);
            array_push($html_table[$passed+$failed], "SAME");
            array_push($html_table[$passed+$failed], 1);
            $failed++;
        }
    }
}

//arguments
$long_opt = ["help", "directory", "recursive", "parse-script", "int-script", "parse-only", "int-only", "jexamxml", "jexamcfg"];

//argument check
for ($i = 1; $i < $argc; $i++) {
    $argv[$i] = preg_replace("/=.*$/", "", $argv[$i]);
    if (!in_array(str_replace("--", "", $argv[$i]), $long_opt)) error_exit(10, $argv[$i], "");
}

$long_opt = ["help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only", "jexamxml:", "jexamcfg:"];
$options = getopt("", $long_opt, $i);

//default variables for arguments
$directory =    getcwd() . '/';
$parse_script = $directory . "parse.php";
$int_script =   $directory . "interpret.py";
$recursive =  false;
$parse_only = false;
$int_only =   false;
$jexamxml = "/pub/courses/ipp/jexamxml/jexamxml.jar";
$jexamcfg = "/pub/courses/ipp/jexamxml/options";

//HTML variables
$test_type = "Both";
$passed = 0;
$failed = 0;

//help
if (isset($options["help"])) {
    if ($argc != 2) error_exit(10, "--help", "is a standalone argument");
    printf("Usage: parse.php [OPTIONS]...\n");
    printf("\t--help\t\t Display this help message\n");
    printf("\t--directory=DIR\t Search tests in 'DIR' directory\n");
    printf("\t--recursive\t\t Search tests recursively\n");
    printf("\t--parse-script=FILE\t PHP Parser script file\n");
    printf("\t--int-script=FILE\t Python interpreter file\n");
    printf("\t--parse-only\t Only test the parser script\n");
    printf("\t--int-only\t\t Only test the interpreter\n");
    printf("\t--jexamxml\t\t A7Soft JExamXML file\n");
    printf("\t--jexamcfg\t\t A7Soft JExamXML config file\n");
    exit(0);
}

//set recursive test
if (isset($options["recursive"])) $recursive = true;

//set parser only
if(isset($options["parse-only"])) {
    $parse_only = true;
    $test_type = "Parse only";
}

//set interpreter only
if(isset($options["int-only"])) {
    $int_only = true;
    $test_type = "Interpret only";
}

//check that either parse or interpret is set
if ($int_only == true && $parse_only == true) error_exit(10, "--parse-only", "--int-only");

//set directory
if (isset($options["directory"])) $directory = get_abs_path($options["directory"]);

//set parse script
if (isset($options["parse-script"])) {
    if ($int_only) error_exit(10, "--parse-script", "--int-only");  //check that int-only is not set
    $parse_script = get_abs_path($options["parse-script"]);
}

//set interpret script
if (isset($options["int-script"])) {
    if ($parse_only) error_exit(10, "--int-script", "--parse-only");    //check that parse-only is not set
    $int_script = get_abs_path($options["int-script"]);
}
//set jexam...
if (/*$test_type == "Both" or */$test_type == "Parse only"){
    if (isset($options["jexamxml"])) $jexamxml = get_abs_path($options["jexamxml"]);
    if (isset($options["jexamcfg"])) $jexamcfg = get_abs_path($options["jexamcfg"]);
    if (!is_file($jexamcfg) || !is_readable($jexamcfg)) error_exit(41, $jexamcfg, "");
    if (!is_file($jexamxml) || !is_readable($jexamxml)) error_exit(41, $jexamxml, "");
}

//check directories and files and their permissions
if (!is_dir($directory)     || !is_readable($directory) || !is_writeable($directory)) error_exit(41, $directory, "");
if ((!is_file($parse_script)|| !is_readable($parse_script)) && !$int_only)            error_exit(41, $parse_script, "");
if ((!is_file($int_script)  || !is_readable($int_script)  ) && !$parse_only)          error_exit(41, $int_script, "");
//if (!is_file($jexamxml)     || !is_readable($jexamxml))                               error_exit(41, $jexamxml, "");
//if (!is_file($jexamcfg)     || !is_readable($jexamcfg))                               error_exit(41, $jexamcfg, "");

//get files
$files;
if ($recursive) exec("find " . $directory . " -type f -name '*.src'", $files);  //for recursive testing
else $files = glob($directory . "/*.src");                                      //for current directory

$tests_total = sizeof($files);                  //save total number of tests
if ($parse_only) $parse_total = $tests_total;   //will probably change once both are implemented. Not sure yet
else $int_total = $tests_total;

//HTML table array
$html_table = array();

//loop thought files and test them
for ($i = 0; $i < sizeof($files); $i++) {
    $html_table[$passed+$failed][0] = $passed+$failed+1;   //itterate tests accordingly to parse and interpret
    run_tests($test_type, $parse_script, $int_script, $jexamcfg, $jexamxml, $files[$i]);
}

touch ("int.out"); unlink("int.out");       //touch and remove int.out, if it wasn't created
touch ("parse.out"); unlink("parse.out");       //touch and remove int.out, if it wasn't created
touch("diffs.xml"); unlink("diffs.xml");    //touch diffs.ximl (if it somehow wasn't create) and remove it

print_html($test_type, $html_table);   //print HTML

?>