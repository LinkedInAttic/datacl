<?php

function
extract_fns_from_hdr_file(
    $dot_h_file, 
    $ofp=stdout
    )
{
  $fp = fopen($dot_h_file, "r");
  if ( !$fp ) { echo "File $dot_h_file not found \n"; exit; }
  $to_pr = false;
  for ( ; !feof($fp); ) { 
    $line = fgets($fp);
    if ( feof($fp) ) { break; }
    $line = trim($line);
    if ( $line == "" ) { break; }
    $n = preg_match("/^extern /", $line);
    if ( $n > 0 ) {
      $to_pr = true;
      continue; 
    }
    $fn = str_replace("(", "", $line);
    if ( $to_pr == true ) { 
      fprintf($ofp, "%s,%s\n", $dot_h_file, $fn);
      $to_pr = false;
    }
  }
  fclose($fp);
}

// Look for a function call 
function pr_func_call(
    $file,
    $line,
    $ofp=stdout
    )
{
  $alt_line = str_replace(" ", "", $line);
  $x = str_between("=", "(", $line);
  if ( !isset($x) ) { return; }
  $x = trim($x);
  $y = strtolower($x);
  $Y = str_split($y);
  $to_pr = true;
  for ( $i = 0; $i < count($Y); $i++ ) { 
    if ( ( ctype_alnum($Y[$i]) === true ) || 
         ( $Y[$i] == "_") ) {
      /* all is well */
    }
    else {
      $to_pr = false;
      break;
    }
  }
  if ( $to_pr === true ) { 
    fprintf($ofp, "%s,%s\n", $file, $x);
  }
}

require_once "str_between.php";
$pwd = getcwd();
$F = scandir($pwd);
$i = 0;

$ofp1 = fopen("file_to_function.txt", "w");
if ( !$ofp1 ) { echo "unable to open file"; exit; }
$ofp2 = fopen("functions_in_dot_h_file.txt", "w");
if ( !$ofp2 ) { echo "unable to open file"; exit; }
$cc_pattern = '/.cc$/';
$pattern = '/.c$/';
foreach ( $F as $filename ) {
  $n = preg_match($pattern, $filename);
  $n_cc = preg_match($cc_pattern, $filename);
  if ( ( $n == 1 ) && ( $n_cc == 0 ) ) { 
    $X[$i++] = $filename;
  }
}
unset($F);
$num_c_files = $i;
echo "Number of C files = $num_c_files\n";

$j = 0;
for ( $i = 0; $i < $num_c_files; $i++ ) { 
  unset($str);
  $str = file_get_contents($X[$i]);
  $n1 = preg_match('/START FUNC DECL/', $str);
  $n2 = preg_match('/STOP FUNC DECL/',  $str);
  $n3 = preg_match('/DO NOT AUTO GENERATE HEADER FILE/',  $str);
  if ( $n3 > 1 ) { 
    echo $X[$i] . "\n"; exit; 
  }
  if ( $n3 > 0 ) {
    // Okay for this file to not have auto-generated header file 
    continue; 
  }
  if ( ( $n1 == 0 ) && ( $n2 == 0 ) ) {
    echo $X[$i] . " does not have auto-generated header file\n";
    continue; 
  }
  if ( $n1 != $n2 ) { echo "file = " . $X[$i] . "\n"; exit; }
  $F[$j++] = $X[$i];
}
$num_c_files = $j;
echo "Number of C files = $num_c_files\n";
for ( $i = 0; $i < $num_c_files; $i++ ) { 
  $prefix = strstr($F[$i], ".", true);
  $opfile = $prefix . ".h";
  // echo "Reading from " . $F[$i] . "\n";
  // echo "Writing to    $opfile \n";
  $ofp = fopen($opfile, "w");
  $ifp = fopen($F[$i],  "r");
  $to_copy = false;
  $num_balanced = 0;
  for ( ; ; ) {
    $line = fgets($ifp);
    if ( feof($ifp) ) { break; }
    $alt_line = trim($line);
    $alt_line = str_replace(" ", "", $alt_line);
    $alt_line = str_replace("/", "", $alt_line);
    $alt_line = str_replace("*", "", $alt_line);
    $alt_line = strtolower($alt_line);
    if ( $alt_line == "startfuncdecl" ) {
      $num_balanced++;
      $to_copy = true;
      fprintf($ofp, "extern ");
    }
    if ( $alt_line == "stopfuncdecl" ) {
      $num_balanced--;
      fprintf($ofp, ";\n//----------------------------\n"); 
      $to_copy = false;
    }
    if ( ( $to_copy == true ) && ( $alt_line != "startfuncdecl" ) ) { 
      fprintf($ofp, "%s", $line);
    }
    pr_func_call($prefix, $line, $ofp1);
  }
  extract_fns_from_hdr_file($opfile, $ofp2);
  if ( $num_balanced != 0 ) { 
    echo "Unbalanced start/stop in $prefix while creating $opfile \n";
    exit;
  }
  fclose($ofp);
  fclose($ifp);
  // echo "Created $opfile \n";
}
?>

