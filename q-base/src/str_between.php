<?php
  /**P: Given a target string (the haystack) and two delimters, a start
   string and a stop string, it finds the first string that exists
   between the two delimiters, assuming that one exists at all in the
   first place. 

   The haystack can be either a file or a string and hence we need to tell
   the function which one it is.

   If the stop string is null, then the result string is everything from
   the end of the first occurrence of the start string to the end of the
   haystack.


  **/

function str_between(
		     $startstr,
		     $stopstr,
		     $infile,
		     $is_file_or_str="str"
		     )
{
  if ( ( $is_file_or_str == "file" ) && ( !is_file($infile) ) ) {
    getout("infile = $infile"); 
  }
  if ( ( $is_file_or_str != "file" ) && ( $is_file_or_str != "str" ) ) {
    getout("is_file_or_str = $is_file_or_str");
  }
  if ( $is_file_or_str == "file" ) { 
    $filestr = file_get_contents($infile);
  }
  else if ( $is_file_or_str == "str" ) { 
    $filestr = $infile;
  }
  $X = strstr($filestr, $startstr);
  $X = substr($X, strlen($startstr), strlen($X));
  if ( $X == "" ) { // Start string not found  
    return null;
  }
  if ( $stopstr != "" ) { 
    $pos = strpos($X, $stopstr);
    if ( !is_numeric($pos) ) {
      // echo "\nFound startstr but not stopstr \n";
      return null;
    }
    else {
      $Y = substr($X, 0, $pos);
    }
  }
  else {
    $Y = $X;
  }
  return $Y;
}
?>
