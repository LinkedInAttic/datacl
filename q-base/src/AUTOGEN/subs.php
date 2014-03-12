<?php

if ( $argc != 6 ) {
  exit("Usage is $argv[0] infile label replfile outdir outfile \n"); 
}
$infile   = $argv[1];
$label    = $argv[2];
$replfile = $argv[3];
$outdir   = $argv[4];
$outfile  = $argv[5];

if ( !is_file($infile) ) { echo "File not found [$infile]\n";  exit(""); }
/* infile must start with tmpl_ */
$pos = strpos($infile, "tmpl_");
if ( $pos !== 0 ) { echo "infile should start with tmpl_"; exit(__LINE__); }
$ifp = fopen($infile, "r");
if ( !$ifp ) { exit("Line " . __LINE__ ); }

if ( !is_file($replfile) ) { exit("Line " . __LINE__); }
$rfp = fopen($replfile, "r");
if ( !$rfp ) { exit("Line " . __LINE__ ); }

if ( $label == "" ) { exit(__LINE__); }

if ( $outfile == "" ) { 
  $search = "tmpl";
  $outfile = str_replace($search, $label, $infile);
}
if ( $outfile == "" ) {  exit("Line = " . __LINE__ . "\n"); }
if ( $outdir == "" ) { 
  $ofp = fopen($outfile, "w");
  // echo "Opened $outfile for writing \n";
}
else {
  $ofp = fopen("$outdir/$outfile", "w");
  // echo "Opened $outdir/$outfile for writing \n";
}
if ( !$ofp )  {  exit("Line = " . __LINE__ . "\n"); }

/* Read the replacement file focusing on lines that have desired label */
$n_repl_pat = 0;
for ( ; !feof($rfp); ) { 
  $line = trim(fgets($rfp));
  if ( $line == "" ) { break; }
  $X = explode(",", $line);
  if ( count($X) != 3 ) { exit("line=[$line], Line = " . __LINE__ ); }
  for ( $i = 0; $i < 3; $i++ ) { $X[$i] = str_replace('"', '', $X[$i]); }
  if ( $X[0] == $label ) { 
    $repl_pat[$n_repl_pat]['in'] = "__" . $X[1] . "__";
    $repl_pat[$n_repl_pat]['out'] = $X[2];
    $n_repl_pat++;
  }
}
fclose($rfp);
if ( $n_repl_pat == 0 ) { 
  echo "Unable to find any patterns for replacement\n";
  echo "infile   = [$infile] \n";
  echo "replfile = [$replfile] \n";
  echo "label    = [$label] \n";
  echo "Line = " . __LINE__ . "\n";
  exit(1);
}
// for ( $i = 0; $i < $n_repl_pat; $i++ ) { 
//   echo "in = " . $repl_pat[$i]['in'] . "\n";
// }


for ( $lno = 1; !feof($ifp); $lno++ ) { 
  $line = fgets($ifp);
  $str = $line;
  for ( $i = 0; $i < $n_repl_pat; $i++ ) { 
    $str = preg_replace('/' . $repl_pat[$i]['in'] . '/', 
	$repl_pat[$i]['out'], $str);
  }
  fwrite($ofp, $str);
}
// echo "DBG: Processed $lno lines \n";
fclose($ifp);
fclose($ofp);
?>
