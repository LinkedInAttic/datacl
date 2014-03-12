<?php

if ( $argc != 4 ) { exit("Usage is $argv[0] infile replfile, outfile \n"); }
$infile   = $argv[1];
$replfile = $argv[2];
$outfile  = $argv[3];

if ( !is_file($infile) ) { exit(""); }
$ifp = fopen($infile, "r");
if ( !$ifp ) { exit(""); }

if ( !is_file($replfile) ) { exit(""); }
$rfp = fopen($replfile, "r");
if ( !$rfp ) { exit(""); }

$ofp = fopen($outfile, "w");
if ( !$ofp ) { exit(""); }

$n_repl_pat = 0;
for ( ; !feof($rfp); ) { 
  $line = trim(fgets($rfp));
  if ( $line == "" ) { break; }
  $X = explode(",", $line);
  if ( count($X) != 2 ) { exit("Line = " . __LINE__ ); }
  $repl_pat[$n_repl_pat]['in'] = $X[0];
  $repl_pat[$n_repl_pat]['out'] = $X[1];
  $n_repl_pat++;
}
fclose($rfp);
if ( $n_repl_pat == 0 ) { exit("Line = " . __LINE__ ); }


for ( $lno = 1; !feof($ifp); $lno++ ) { 
  $line = fgets($ifp);
  $str = $line;
  for ( $i = 0; $i < $n_repl_pat; $i++ ) { 
    $str = preg_replace('/' . $repl_pat[$i]['in'] . '/', 
	$repl_pat[$i]['out'], $str);
  }
  fwrite($ofp, $str);
}
echo "DBG: Processed $lno lines \n";

fclose($ifp);
fclose($ofp);

?>

