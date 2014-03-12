<?php
function json_to_q_O3(
		   $instr,
		   $opfile
		   )
{
  $ofp = fopen($opfile, "w");
  if ( !$ofp ) { return false; }

  $is_filter = false;
  $is_grpby  = false;

  $dbg = 1;
  $tempfnum = 1;

  $ramfsdir = "/dev/shm/";        /* HARD CODED */
  $batfile1 = tempnam($ramfsdir, " _tempf_");
  $batfile2 = tempnam($ramfsdir, " _tempf_");
  $bfp1 = fopen($batfile1, "w");
  if ( !$bfp1 ) { return false; }
  $bfp2 = fopen($batfile2, "w");
  if ( !$bfp2 ) { return false; }


  if ( $instr == "" ) { return false; }
  $X = json_decode($instr);
  if ( is_null($X) ) { return false; }
  $F = $X->{'filters'};
  if ( is_null($F) ) { 
    $is_filter = false;
  }
  else {
    $is_filter = true;
    // because we deal with only one filter
    $F0 = $F[0]; 
    $fiter = 0;
    $tbl ="TM";

    fprintf($ofp, "set -e \n"); 
    fprintf($ofp, "q start_compound $batfile1 '' \n");
    fprintf($ofp, "q batch          $batfile2 '' \n");
    while ( list($key, $value) = each ($F0) ) {
      // echo "Attribute = $key \n";
      $operator = $value->{'operator'};
      switch ( $operator ) { 
      case "equals"     : $op = "=="; break;
      case "not_equals" : $op = "!="; break;
      default : echo "ERROR \n"; exit; break;
      }
      $values   = $value->{'values'};
      // echo "operator = $operator \n";
      $valstr = "";
      $viter=1;
      foreach ( $values as $v ) { 
	if ( $viter == 1 ) { $valstr = $v; } else { $valstr = $valstr . ":" .  $v; }
	$viter++;
      }
      // echo "valstr = $valstr\n";
      if ( $fiter == 0 ) { 
	fprintf($bfp1, "q start_compound \n");
	$xfld = "tempfld$tempfnum"; 
	fprintf($bfp1, "q f1s1opf2 $tbl $key $valstr $op $xfld \n"); 
      }
      else { 
	$xfld = "tempfld$tempfnum"; $tempfnum++;
	$yfld = "tempfld$tempfnum"; $tempfnum++;
	$zfld = "tempfld$tempfnum"; 
	fprintf($bfp1, "q f1s1opf2 $tbl $key $valstr $op $yfld \n"); 
	fprintf($bfp1, "q f1f2opf3 $tbl $xfld $yfld '&&' $zfld \n");
      }
      $fiter++;
    }
  }

  $G = $X->{'group_by'};
  if ( is_null($G) ) { 
    $is_grpby = false;
  }
  else { 
    $is_grpby = true;
    $G0 = $G[0];
    $topn  = $G0->{'limit'};
    if ( is_null($topn) ) { $topn = 10; }
    if ( ( $topn <= 0 ) || ( $topn >= 100 ) ) {
      $topn = 10;
    }
    $grpby = $G0->{'dimension'};
    if ( !is_null($grpby) ) { 
      $grpbyfld = $grpby . "Id";
      $grpbytbl = "T" . $grpby;
    }
  }

  if ( $is_filter ) { 
    $cfld = "tempfld$tempfnum";
  }
  else { 
    $cfld = "";
  }

  if ( $is_grpby ) { 
    $cwd = getcwd();
    $tempf = tempnam($cwd, "_tempf_");
    fprintf($bfp1, "q count $tbl $grpbyfld '$cfld' $grpbytbl cnt \n");
    fprintf($bfp1, "q stop_compound '' '' \n");
    fprintf($bfp2, "q f1opf2 $grpbytbl cnt 'op=[conv]:newtype=[I4]' cnt\n");
    fprintf($bfp2, "q dup_fld $grpbytbl idx xidx \n"); 
    fprintf($bfp2, "q sortf1f2 $grpbytbl cnt xidx D_ \n");
    if ( $dbg ) { 
      fprintf($bfp2, "q set_meta $grpbytbl xidx dict_tbl $grpbytbl \n");
    }
    fprintf($bfp2, "q pr_fld $grpbytbl xidx:cnt 'lb=[0]:ub=[$topn]' \n");
  }
  else {
    if ( $is_filter ) {
      fprintf($bfp2, "q f_to_s $tbl x sum SUM \n");
      fprintf($bfp2, "q stop_compound '' '' \n");
    }
    else {
      fprintf($bfp2, "q get_nR $tbl\n");
    }
  }
  if ( $is_filter ) { 
    $tempflds = tempfld1;
    for ( $i = 2; $i <= $tempfnum; $i++ ) { 
      $tempflds = $tempflds . ":tempfld$i";
    }
    if ( $tempflds == "" ) { echo "Error on Line __LINE__\n"; exit; }
    fprintf($ofp, "q delete $tbl $tempflds \n");
  }
  if ( $is_grpby ) { 
    fprintf($ofp, "q delete $grpbytbl cnt:xidx \n");
  }
  fclose($ofp);
  return true; 
}

$is_cli = true;
if ( $is_cli ) { 
  if ( $argc != 3 ) { 
    echo "Usage is " . $argv[0] . " <in.json> <out.q> \n"; exit; 
  }
  $infile = $argv[1];
  $str = file_get_contents($infile);
  if ( !is_file($infile) ) { echo "file not found [$infile]\n"; exit; }
  $opfile = $argv[2];
  $instr = file_get_contents($infile);
  $rslt = json_to_q_O3($instr, $opfile);
  if ( $rslt === true ) { 
    echo "Created $opfile from $infile \n";
  }
  else {
    echo "Error in infile \n";
  }
}
?>
