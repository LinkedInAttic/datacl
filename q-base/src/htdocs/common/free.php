<?php
require_once "../common/str_between.php";
require_once "../common/getout.php";
function free_tbspc(
    $token
    )
{
  if ( ( is_null($token) ) || ( $token != "" ) && ( !is_numeric($token)
	) ) { 
    getout("");
  }
  //-------------------------------------
  $ch = curl_init();
  $server = "localhost"; // HARD CODED
  $port   = 8000;        // HARD CODED
  $urlpre = "http://" . $server . ":" . $port . "/";
  // free tablespace
  $url = $urlpre . "free?TOKEN=$token";
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_URL, $url);
  $output = curl_exec($ch);
  // close curl resource to free up system resources
  curl_close($ch);      
}

$is_cli = false; 
if ( $is_cli ) { 
  if ( $argc != 2 )  { getout(""); }
  $token = $argv[1];
  $token = free_tbspc($token);
}
?>
