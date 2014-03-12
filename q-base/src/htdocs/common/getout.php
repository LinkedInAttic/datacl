<?
function getout(
                $str,
                $status='-1' /* optional */
                )
{
  $W = debug_backtrace();
  for ( $i = 0; $i < count($W); $i++ ) {
    echo "F:[" . $W[$i]['file'] . "] L:[" . $W[$i]['line'] .
      "] FN:[" .  $W[$i]['function'] . "]\n";
  }
  echo("\n[$str]\n");
  exit(1);
}
?>
