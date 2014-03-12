#!/bin/sh
if [ $# != 4 ]; then echo FAILURE; exit 1; fi
IN_TBL=$1
INFLD=$2
OUT_TBL=$3
SORT_ORDR=$4

if [ "$SORT_ORDR" != "A" ] && [ "$SORT_ORDR" != "D" ]; then 
  echo "Sort order = [$SORT_ORDR}. Must be A or D "; exit 1; 
fi

VG="$valgrind"
VG=""

pid=$PPID
x=`date`
suff=`cli_hash_string "$x" | cut -f 1 -d ','`
tempf="$PWD/_tempf_${suff}_${pid}"

# cleanup prior
$VG q del_tbl "" tempt
$VG q del_tbl "" tempt2
$VG q del_tbl "" $OUT_TBL

$VG q s_to_f  "" $IN_TBL "fldtype=int:op=seq:start=0:incr=1" id
$VG q bindmp  "" $IN_TBL "$INFLD:id" $tempf
$VG rsutils sortbindmp $tempf II AA
$VG q binld   ""  tempt "$INFLD:id" "I:I" $tempf

$VG q f1s1opf2 "" tempt $INFLD 0 is_prev_diff c1 
$VG q del_tbl  "" $OUT_TBL
$VG q copy_fld "" tempt id     c1 tempt2
$VG q copy_fld "" tempt $INFLD c1 tempt2

$VG q s_to_f "" tempt "fldtype=int:op=seq:start=1:incr=0" one
$VG q mjoin  "" tempt $INFLD one   tempt2 $INFLD cnt    sum

# Now sort tempt on cnt as specified by sort order
$VG q bindmp  "" tempt2 "cnt:id" $tempf
$VG rsutils sortbindmp $tempf II ${SORT_ORDR}${SORT_ORDR}
$VG q binld   ""  tempt3 "cnt:id" "I:I" $tempf
$VG q join    "" tempt2 id $INFLD tempt3 id $INFLD

$VG q rename "" tempt3 $OUT_TBL

#-- cleanup
$VG q del_tbl "" tempt
$VG q del_tbl "" tempt2
$VG q del_fld "" $IN_TBL id
rm -f $tempf
