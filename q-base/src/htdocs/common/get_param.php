<?php
function get_param(
    $param_name
    )
{
  $param_val = "";
  if ( !is_null($_GET[$param_name]) ) { 
    $param_val = trim($_GET[$param_name]);
  }
  if ( !is_null($_POST[$param_name]) ) { 
    $param_val = trim($_POST[$param_name]);
  }
  return $param_val;
}
?>
