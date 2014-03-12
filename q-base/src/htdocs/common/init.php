<?php
require_once "../common/get_param.php";
require_once "../common/getout.php";
function init_tbspc(
    $in_tbspc,
    &$token,
    &$q_docroot,
    &$dsk_data_dir,
    &$ram_data_dir,
    &$err_msg
    )
{
  if ( !is_null($in_tbspc) && ( $in_tbspc != "" ) ) {
    $tablespace = $in_tbspc;
  }
  else {
    $tablespace = get_param("TABLESPACE");
  }
  if ( $tablespace == "" ) { 
    $err_msg = "tablespace not defined\n"; return false; 
  }
  if ( !ctype_alnum($tablespace) ) {
    $err_msg = "invalid tablespace = $tablespace\n"; return false; 
  }
  // echo "tablespace = $tablespace \n";
  //-------------------------------------
  $ch = curl_init();
  $server = "localhost"; // HARD CODED
  $port   = 8000;        // HARD CODED
  $urlpre = "http://" . $server . ":" . $port . "/";
  $url = $urlpre . "init?TABLESPACE=$tablespace";
  curl_setopt($ch, CURLOPT_URL, $url);
  //return the transfer as a string
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
  // $output contains the output string
  $output = curl_exec($ch);
  // echo "<H2> $output </H2> \n";
  $X = json_decode($output);
  // var_dump($X);
  $token      = $X->TOKEN; 
  $q_docroot  = $X->Q_DOCROOT;
  $dsk_data_dir = $X->DSK_DATA_DIR;
  $ram_data_dir = $X->RAM_DATA_DIR;
  //---------------------------------------------------------
  //---------------------------------------------------------
  if ( is_null($token) || ( $token == "" ) ) { 
    $err_msg = "Null token"; return false; 
  }
  if ( is_null($q_docroot) || ( $q_docroot == "" ) ) { 
    $err_msg = "Null q_docroot"; return false; 
  }
  if ( is_null($dsk_data_dir) || ( $dsk_data_dir == "" ) ) { 
    $err_msg = "Null dsk_data_dir"; return false; 
  }
  //---------------------------------------------------------
  if ( !is_dir($dsk_data_dir) ) { 
    $err_msg = "missing dsk_data_dir [$dsk_data_dir] "; return false; 
  }
  if ( !is_dir($q_docroot)  ) { 
    $err_msg = "missing q_docroot [$q_docroot] "; return false; 
  }
  if ( !is_numeric($token) ) { 
    $err_msg = "not numeric token [ $token]"; return false; 
  }
  if ( !is_null($ram_data_dir) && ( $ram_data_dir != "" ) ) { 
    if ( !is_dir($ram_data_dir)  ) { 
      $err_msg = "missing ram_data_dir [$ram_data_dir]"; return false; 
    }
  }
  return true;
}

$is_cli = true; 
$tbspc = "";
if ( $is_cli ) { 
  if ( $argc != 2 ) { getout(""); }
  $tbspc = $argv[1];
$token = "";
$q_docroot = "";
$q_data_dir = "";
$err_msg = "";
$rslt = init_tbspc($tbspc, $token, $q_docroot, $dsk_data_dir,
    $ram_data_dir, $err_msg);
if ( $rslt === false ) { 
  getout($err_msg); 
}
echo "TOKEN=$token \n";
echo "Q_DOCROOT=$q_docroot \n";
echo "DSK_DATA_DIR=$dsk_data_dir \n";
echo "RAM_DATA_DIR=$ram_data_dir \n";
}

?>
