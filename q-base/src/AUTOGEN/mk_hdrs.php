<?php

if ( $argc != 3 ) { exit("Usage is $argv[0] outdir infile \n"); }
$outdir   = $argv[1];
$infile   = $argv[2];
$outfile = str_replace(".c", ".h", $infile);
if ( $outfile == "" ) { exit("Line = " . __LINE__ ); }
// echo "Producing $outfile from $infile\n";

$cwd = getcwd();
if ( $outdir != "" ) { chdir($outdir);  }

if ( !is_file($infile) ) { exit("File not found [$infile]"); }
$ifp = fopen($infile, "r");
if ( !$ifp ) { exit(""); }

$ofp = fopen($outfile, "w");
if ( !$ofp )  {  exit("Line = " . __LINE__ . "\n"); }

$num_lines = count(file($infile));
$is_to_pr = false;
$is_hdr_seen = false;
for ( $lno = 1; $lno <= $num_lines; $lno++ ) { 
  $orig_line = fgets($ifp);
  $line = trim($orig_line);
  // Strip white space and tabs 
  $line = str_replace(" ", "", $line);
  $line = str_replace("\t", "", $line);
  $line = strtolower($line);
  if ( !$is_hdr_seen ) { 
    $cptr = strstr($line, "starthdrfiles");
    if ( $cptr != "" ) { 
      for ( ; $lno <= $num_lines; $lno++ ) { 
	$orig_line = fgets($ifp);
	$line = trim($orig_line);
        $line = str_replace(" ", "", $line);
        $line = str_replace("\t", "", $line);
	$line = strtolower($line);
	$cptr = strstr($line, "stophdrfiles");
	if ( $cptr != "" ) { 
	  break;
	}
	fwrite($ofp, $orig_line);
	// echo "Lno = $lno, $line = $line \n";
      }
    }
    $is_hdr_seen = true;
  }
  $cptr = strstr($line, "startfuncdecl");
  if ( $cptr != "" ) {
    fwrite($ofp, "extern ");
    $is_to_pr = true;
    continue;
  }
  $cptr = strstr($line, "stopfuncdecl");
  if ( $cptr != "" ) {
    fwrite($ofp, ";\n"); 
    $is_to_pr = false;
  }
  if ( $is_to_pr ) {
    fwrite($ofp, $orig_line);
  }
 }
fwrite($ofp, "\n");
fclose($ifp);
fclose($ofp);
if ( $outdir != "" ) { chdir($cwd);  }
?>
