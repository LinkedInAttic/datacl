<?php

$pwd = getcwd();
$F = scandir($pwd);
$i = 0;

$pattern = '/.c$/';
foreach ( $F as $filename ) {
  $n = preg_match($pattern, $filename);
  if ( $n == 1 ) { 
    $X[$i++] = $filename;
  }
}
unset($F);
$num_c_files = $i;
echo "Number of C files = $num_c_files\n";

$j = 0;
for ( $i = 0; $i < $num_c_files; $i++ ) { 
  unset($str);
  $c_file = $X[$i];
  $str = file_get_contents($c_file);
  $n1 = preg_match('/START FUNC DECL/', $str);
  $n2 = preg_match('/STOP FUNC DECL/',  $str);
  if ( ( $n1 == 0 ) && ( $n2 == 0 ) ) { 
    echo "File " . $X[$i] . " can be ignored\n";
    continue; 
  }
  if ( $n1 != $n2 ) { echo "file = " . $X[$i] . "\n"; exit; }
  $F[$j++] = $X[$i];
  $h_file = str_replace(".c", ".h", $c_file);
  $command = " cat header.txt " . $h_file . " >> _tempfile ; " ;
  $command .= " mv _tempfile " . $h_file . " \n";
  // echo $command;
  system($command, $status); if ( $status != 0 ) { echo $command; break; }
}
$num_c_files = $j;
echo "Number of C files = $num_c_files\n";
exit;
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
  }
  if ( $num_balanced != 0 ) { 
    echo "Unbalanced start/stop in $prefix while creating $opfile \n";
    exit;
  }
  fclose($ofp);
  fclose($ifp);
  echo "Created $opfile \n";
}
?>

