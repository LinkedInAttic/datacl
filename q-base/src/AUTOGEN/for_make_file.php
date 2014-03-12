<?php
// This needs more work but the aim is to take a list of C files and
// generate the appropriate lines for the Makefile automatically
require_once "str_between.php";

$infile = "_x";
$opfile = "_y";
$ifp = fopen($infile, "r");
$ofp = fopen($opfile, "w");

for ( $lno = 1; !feof($ifp) ; $lno++ ) {
  $line = trim(fgets($ifp));
  if ( $line == "" )  { break; }
  $line = str_replace('\\', '', $line);
  $x = php_str_between("GENFILES/", ".c", $line, "str");
  // echo "Line $lno = $x " . "\n";
  fwrite($ofp, "$x" . ".o : " . $line . "\n");
  fwrite($ofp, "\t " . '$(CC) $(CFLAGS) $(INCLUDES) -c ' . $line .  "\n");
}


?>

  
