<?php
/*
   http://esv4-analytics01.linkedin.biz:8080/pulse/q1.php?QUERY=q1&ACTION=Ad%20Impression&DISTINCT=true
   http://esv4-analytics01.linkedin.biz:8080/pulse/q1.php?QUERY=q1&ACTION=Ad%20Impression&DISTINCT=false
   http://esv4-analytics01.linkedin.biz:8080/pulse/q1.php?QUERY=q2&ACTION=Ad%20Impression
   */

require_once "../common/init.php";
require_once "../common/free.php";
require_once "json_to_q.php";
require_once "json_to_q_O3.php";

$HOME=getenv("HOME");

$is_err = false;
$glbl_docroot      = "/dev/shm/METAQ/PLATO";    /* HARD CODED */
$glbl_dsk_data_dir = "/home/rsubramo/Q/PLATO";  /* HARD CODED */
$glbl_ram_data_dir = "/dev/shm/Q/PLATO";        /* HARD CODED */

if ( !is_dir($glbl_docroot) ) { 
  $err_msg = "glbl docroot missing"; $is_err = true; 
}
if ( !is_dir($glbl_dsk_data_dir) ) { 
  $err_msg = "glbl dsk_data_dir missing"; $is_err = true; 
}
if ( !is_dir($glbl_ram_data_dir) ) { 
  $err_msg = "glbl ram_data_dir missing"; $is_err = true; 
}
//-----------------------------------------------------------
// Get the parameters
$optflag  = get_param("OPTFLAG"); 

$query = get_param("QUERY"); 
if ( $query == "" ) { 
  $is_err = true;
  $err_msg = "Query not specified\n";
}

if ( $is_err === true ) { 
  header("HTTP/1.1 500 Internal Server Error");
  echo "<H1> Problem with Inputs </H1> \n";
  echo "<H2> Error = [$err_msg] </H2> \n";
  exit;
}

$qfile   = tempnam($ramfsdir, "_tempf_");
if ( $optflag == "O3" ) { 
  $rslt = json_to_q_O3($query, $qfile);
}
else if ( $optflag == "GPU" ) { 
/*
$rslt = json_to_q_GPU($query, $qfile);
$rslt = false; 
*/
}
else {
  $rslt = json_to_q($query, $qfile);
}
if ( $rslt === false ) {
  header("HTTP/1.1 500 Internal Server Error");
  echo "<H2> Error = [Malformed json query ] </H2> \n";
  exit;
}

$suffix = "/" . microtime() . getmypid(); 
$suffix = str_replace(" ", "", $suffix); 
$suffix = str_replace(".", "", $suffix); 
$lcl_docroot      = $glbl_docroot      . $suffix;
$lcl_dsk_data_dir = $glbl_dsk_data_dir . $suffix;
$lcl_ram_data_dir = $glbl_ram_data_dir . $suffix;


if ( !mkdir($lcl_docroot) ) { $is_err = true; }
if ( !mkdir($lcl_dsk_data_dir) ) { $is_err = true; }
if ( !mkdir($lcl_ram_data_dir) ) { $is_err = true; }
if ( $is_err === true ) {
  header("HTTP/1.1 500 Internal Server Error");
  echo "<H2> Error = [Unable to create local tablespace] </H2> \n";
  exit;
}

//-----------------------------------------------------------
$opfile   = tempnam($ramfsdir, "_tempf_");
$errfile  = tempnam($ramfsdir, "_tempf_");
$batfile1 = tempnam($ramfsdir, "_tempf_");
$batfile2 = tempnam($ramfsdir, "_tempf_");

$command  = " export Q_DOCROOT=$lcl_docroot; ";
$command .= " export Q_USE_RAM_DIR=TRUE; ";
$command .= " q bootstrap $lcl_docroot $glbl_docroot $lcl_dsk_data_dir $lcl_ram_data_dir; "; 
$command .= " bash $qfile 1>$opfile 2>$errfile; ";
$command .= " rm -r -f $lcl_docroot ; "; 
$command .= " rm -r -f $lcl_dsk_data_dir ; "; 
$command .= " rm -r -f $lcl_ram_data_dir ; "; 
$errfile  = tempnam($ramfsdir, "_tempf_");

$rslt = system($command, $status);
if ( ( $status == 0 ) && ( filesize($errfile) == 0 ) ) {
  header("HTTP/1.1 200 OK ");
  $outstr = file_get_contents($opfile);
}
else {
  header("HTTP/1.1 500 Internal Server Error");
  $outstr = file_get_contents($errfile); 
}
echo $outstr;
unlink($qfile);
unlink($opfile);
unlink($errfile);
// echo "<H1> Released token $token </H1> \n";
?>
