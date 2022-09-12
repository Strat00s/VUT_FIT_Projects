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
function print_html($parse_table, $int_table) {
    global $test_type, $tests_total, $parse_total, $parse_passed, $parse_failed, $int_total, $int_passed, $int_failed;

    printf("<!doctype html>\n");
    printf("<html lang='cz'>\n");
    printf("  <head>\n");
    printf("    <h1><strong>Test Summary</strong></h1>\n");
    printf("    <h2>Type: %s</h2>\n", $test_type);
    printf("    <h2>Total number of tests: %d</h2>\n", $tests_total);
    printf("  </head>\n");
    printf("  <body>\n");
    printf("    <hr />\n");
    printf("    <h2>Parser</h2>\n");
    printf("    <h3>Number of tests: %d</h3>\n", $parse_total);
    printf("    <h3><span data-darkreader-inline-color='' style='--darkreader-inline-color:#47d583; color:#2ecc71'>Passed: %d</span></h3>\n", $parse_passed);
    printf("    <h3><span data-darkreader-inline-color='' style='--darkreader-inline-color:#e95849; color:#e74c3c'>Failed: %d</span></h3>\n", $parse_failed);
    printf("    <hr />\n");
    printf("    <h2>Interpreter</h2>\n");
    printf("    <h3>Number of tests: %d</h3>\n", $int_total);
    printf("    <h3><span data-darkreader-inline-color='' style='--darkreader-inline-color:#47d583; color:#2ecc71'>Passed: %d</span></h3>\n", $int_passed);
    printf("    <h3><span data-darkreader-inline-color='' style='--darkreader-inline-color:#e95849; color:#e74c3c'>Failed: %d</span></h3>\n", $int_failed);
    printf("    <hr />\n");
    printf("    <h2>Parser - Detailed Results</h2>\n");
    printf("    <table border='1' cellpadding='1' cellspacing='1' style='width:100%%'>\n");
    printf("      <tbody>\n");
    printf("        <tr>\n");
    printf("          <td style='text-align:center; width:100px'>Test Number</td>\n");
    printf("          <td style='text-align:center; width:100px'>Status</td>\n");
    printf("          <td style='text-align:center'>Path</td>\n");
    printf("          <td style='text-align:center; width:150px'>Return Code (got/ref)</td>\n");
    printf("          <td style='text-align:center; width:100px'>XML Diff</td>\n");
    printf("        </tr>\n");

    //change colors depending on invalid return code and XML difference
    for ($i = 0; $i < sizeof($parse_table); $i++){
        printf("        <tr>\n");
        printf("          <td style='text-align:center'>%d</td>\n", $parse_table[$i][0]);

        //RED:   printf("<td style='background-color:#ff3300; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][1]);
        //GREEN: printf("<td style='background-color:#009900; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][1]);


        if (!$parse_table[$i][5]) printf("          <td style='background-color:#009900; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][1]);
        else printf("          <td style='background-color:#ff3300; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][1]);
        printf("          <td style='text-align:center'>%s</td>\n", $parse_table[$i][2]);
        if ($parse_table[$i][5] == 3 || $parse_table[$i][5] == 1) printf("          <td style='background-color:#ff3300; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][3]);
        else printf("          <td style='background-color:#009900; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][3]);
        if ($parse_table[$i][5] == 3 || $parse_table[$i][5] == 2) printf("          <td style='background-color:#ff3300; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][4]);
        else printf("          <td style='background-color:#009900; text-align:center'><span style='color:#000000'><strong>%s</strong></span></td>\n", $parse_table[$i][4]);
        printf("        </tr>\n");

        //if ($parse_table[$i][5]) {
        //    printf("<tr>\n");
        //    printf("<td colspan='5' style='text-align:center'>%s</td>\n", "'" . str_replace('"', "&quot;", trim(file_get_contents("diffs.xml"))) . "'");
        //    printf("</tr>\n");
        //}
    }   
    printf("      </tbody>\n");
    printf("    </table>\n");

    printf("    <hr />\n");

    printf("    <h2>Interpreter - Detailed Results</h2>\n");
    printf("    <table border='1' cellpadding='1' cellspacing='1' style='width:100%%'>\n");
    printf("      <tbody>\n");
    printf("        <tr>\n");
    printf("          <td style='text-align:center; width:100px'>Test Number</td>\n");
    printf("          <td style='text-align:center; width:100px'>Status</td>\n");
    printf("          <td style='text-align:center'>Path</td>\n");
    printf("          <td style='text-align:center; width:150px'>Return Code (got/ref)</td>\n");
    printf("          <td style='text-align:center; width:100px'>XML Diff</td>\n");
    printf("        </tr>\n");
    printf("      </tbody>\n");
    printf("    </table>\n");
    printf("  </body>\n");
    printf("</html>\n");
}

function run_tests($parse_only, $int_only, $parse_script, $int_script, $jexamcfg, $jexamxml, $path) {

    //using global variables here, just to make stuff easier
    global $parse_table, $parse_passed, $parse_failed, $int_passed, $int_failed;

    $full_path = $path;                                         //save full path for HTML
    $file_path = preg_replace("/\.[^.\s]{2,3}$/", "", $path);   //get file path without extension

    //check .in, .out and .rc files and create them if required
    if (!is_file($file_path . ".in")) touch($file_path . ".in");
    if (!is_file($file_path . ".out")) touch($file_path . ".out");
    if (!is_file($file_path . ".rc"))  {
        touch($file_path . ".rc");
        file_put_contents($file_path . ".rc", "0");
    }

    //for parser only as of now -> will probably change for both
    if($parse_only || ($parse_only == $int_only)) {
        $def_rc = file_get_contents($file_path . ".rc");    //save example return code
        exec("php7.4 ". $parse_script . " < " . $full_path . " > tmp.out 2> /dev/null", $output, $return_code); //run php script and save output to tmp.out (silent)
        
        //on same return code
        if($return_code == $def_rc){
            //other than 0
            if ($return_code){
                //push HTML table arguments onto the 2D array
                array_push($parse_table[$parse_passed+$parse_failed], "SUCCESS");
                array_push($parse_table[$parse_passed+$parse_failed], $full_path);
                array_push($parse_table[$parse_passed+$parse_failed], $return_code . "/" . $def_rc);
                array_push($parse_table[$parse_passed+$parse_failed], "UNKNOWN");   //we don't care what is the output, as we have the same error reurn code
                array_push($parse_table[$parse_passed+$parse_failed], 0);   //0 - good, 1 - rc bad, 2 - XML bad, 3 - both
                $parse_passed++;
                //skip jexam
            }
            else {
                exec("java -jar " . $jexamxml . " " . $file_path . ".out " . "tmp.out " . "diffs.xml " . "/D " . $jexamcfg, $j_output, $j_retun_code);  //run jexam here, as we wan't to know if the XML is same or not
                if ($j_retun_code) {
                    array_push($parse_table[$parse_passed+$parse_failed], "FAIL");
                    array_push($parse_table[$parse_passed+$parse_failed], $full_path);
                    array_push($parse_table[$parse_passed+$parse_failed], $return_code . "/" . $def_rc);
                    array_push($parse_table[$parse_passed+$parse_failed], "DIFF");
                    array_push($parse_table[$parse_passed+$parse_failed], 2);
                    $parse_failed++;
                }
                else {
                    array_push($parse_table[$parse_passed+$parse_failed], "SUCCESS");
                    array_push($parse_table[$parse_passed+$parse_failed], $full_path);
                    array_push($parse_table[$parse_passed+$parse_failed], $return_code . "/" . $def_rc);
                    array_push($parse_table[$parse_passed+$parse_failed], "SAME");
                    array_push($parse_table[$parse_passed+$parse_failed], 0);
                    $parse_passed++;
                }
            }
        }
        else {
            exec("java -jar " . $jexamxml . " " . $file_path . ".out " . "tmp.out " . "diffs.xml " . "/D " . $jexamcfg, $j_output, $j_retun_code);  //running it here as well just to see if return code is the only problem
                if ($j_retun_code) {
                    array_push($parse_table[$parse_passed+$parse_failed], "FAIL");
                    array_push($parse_table[$parse_passed+$parse_failed], $full_path);
                    array_push($parse_table[$parse_passed+$parse_failed], $return_code . "/" . $def_rc);
                    array_push($parse_table[$parse_passed+$parse_failed], "DIFF");
                    array_push($parse_table[$parse_passed+$parse_failed], 3);
                    $parse_failed++;
                }
                else {
                    array_push($parse_table[$parse_passed+$parse_failed], "FAIL");
                    array_push($parse_table[$parse_passed+$parse_failed], $full_path);
                    array_push($parse_table[$parse_passed+$parse_failed], $return_code . "/" . $def_rc);
                    array_push($parse_table[$parse_passed+$parse_failed], "SAME");
                    array_push($parse_table[$parse_passed+$parse_failed], 1);
                    $parse_failed++;
                }
        }
    }

    if($int_only || ($parse_only == $int_only)) {
        //TODO once we are there
        error_exit(99, "Python script not yet implemented...", "");
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
$parse_total = 0;
$parse_passed = 0;
$parse_failed = 0;
$int_total = 0;
$int_passed = 0;
$int_failed = 0;

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
if (isset($options["jexamxml"])) $jexamxml = get_abs_path($options["jexamxml"]);
if (isset($options["jexamcfg"])) $jexamcfg = get_abs_path($options["jexamcfg"]);

//check directories and files and their permissions
if (!is_dir($directory)     || !is_readable($directory) || !is_writeable($directory)) error_exit(41, $directory, "");
if ((!is_file($parse_script)|| !is_readable($parse_script)) && !$int_only)            error_exit(41, $parse_script, "");
if ((!is_file($int_script)  || !is_readable($int_script)  ) && !$parse_only)          error_exit(41, $int_script, "");
if (!is_file($jexamxml)     || !is_readable($jexamxml))                               error_exit(41, $jexamxml, "");
if (!is_file($jexamcfg)     || !is_readable($jexamcfg))                               error_exit(41, $jexamcfg, "");

touch("tmp.out");   //create tmp file for storing parse output

//get files
$files;
if ($recursive) exec("find " . $directory . " -type f -name '*.src'", $files);  //for recursive testing
else $files = glob($directory . "/*.src");                                      //for current directory

$tests_total = sizeof($files);                  //save total number of tests
if ($parse_only) $parse_total = $tests_total;   //will probably change once both are implemented. Not sure yet
else $int_total = $tests_total;

//HTML table array
$parse_table = array();
$int_table = array(); 

//loop thought files and test them
for ($i = 0; $i < sizeof($files); $i++) {
    $parse_table[$parse_passed+$parse_failed][0] = $parse_passed+$parse_failed+1;   //itterate tests accordingly to parse and interpret
    $int_table[$int_passed+$int_failed][0] = $int_passed+$int_failed+1;
    run_tests($parse_only, $int_only, $parse_script, $int_script, $jexamcfg, $jexamxml, $files[$i]);
}

unlink("tmp.out");                          //remove tmp.out
touch("diffs.xml"); unlink("diffs.xml");    //touch diffs.ximl (if it somehow wasn't create) and remove it

print_html($parse_table, $int_table);   //print HTML

?>