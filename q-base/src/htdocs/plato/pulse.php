<?php
/*
   http://esv4-analytics01.linkedin.biz:8080/pulse/q1.php?QUERY=q1&ACTION=Ad%20Impression&DISTINCT=true
   http://esv4-analytics01.linkedin.biz:8080/pulse/q1.php?QUERY=q1&ACTION=Ad%20Impression&DISTINCT=false
   http://esv4-analytics01.linkedin.biz:8080/pulse/q1.php?QUERY=q2&ACTION=Ad%20Impression
   */

require_once "../common/init.php";
require_once "../common/free.php";

$HOME=getenv("HOME");
$ramfsdir   = "/dev/shm/";       // HARD CODED
$tbspc      = "plato";           // HARD CODED
//-----------------------------------------------------------
// Get the parameters
$is_err = false;
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
//--------------------------------------------------------
$token      = "";
$q_docroot  = "";
$dsk_data_dir = "";
$ram_data_dir = "";
$token      = "";
$err_msg    = "";
$rslt = init_tbspc($tbspc, $token, $q_docroot, $dsk_data_dir,
    $ram_data_dir, $err_msg);
if ( $rslt === false ) {
  header("HTTP/1.1 500 Internal Server Error");
  echo "<H1> Unable to reserve a tablespace </H1> \n";
  echo "<H2> Error = [$err_msg] </H2> \n";
  exit;
}
// echo "<H1> Acquired token $token </H1> \n";
// echo "<H2> $qscriptdir = $qscriptdir </H2> \n";
//-----------------------------------------------------------
$qfile   = tempnam($ramfsdir, "_tempf_");
$opfile  = tempnam($ramfsdir, "_tempf_");
$errfile = tempnam($ramfsdir, "_tempf_");
json_to_q($query, $qfile);
putenv("Q_DOCROOT=$q_docroot"); 

$command = "bash $qfile 1>$opfile 2>$errfile \n";
$errfile  = tempnam($ramfsdir, "_tempf_");

$rslt = system($command, $status);
if ( ( $status == 0 ) && ( filesize($errfile) == 0 ) ) {
  header("HTTP/1.1 200 OK ");
  switch ( $query ) { 
    case "q1" : 
    $str = trim(file_get_contents($tempfile));
    $X = array('n' => $str);
    break;
    case "q2" : 
      $fp = fopen($tempfile, "r");
      if ( !$fp ) { getout(""); }
      $n1 = trim(fgets($fp)); 
      $n2 = trim(fgets($fp)); 
      fclose($fp); 
      $X = array('n1' => $n1, 'n2' => $n2);
      break;
    default : 
    getout(""); 
    break;
  }
  $Y = json_encode($X);
  echo $Y;
}
else {
  header("HTTP/1.1 500 Internal Server Error");
  $str = file_get_contents($errfile); echo $str;
}
unlink($errfile);
unlink($tempfile);
free_tbspc($token);
// echo "<H1> Released token $token </H1> \n";

