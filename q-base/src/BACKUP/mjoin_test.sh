#!/bin/sh
DB=db1.sq3
VG="valgrind --leak-check=full --track-fds=yes --show-reachable=yes --track-origins=yes"
VG=""
pid=$PPID
x=`date`
suff=`cli_hash_string "$x" I | cut -f 1 -d ','`
tempf="$PWD/_tempf_${suff}_${pid}"
echo '"a"' >   $tempf
echo '"a"'  >>   $tempf
echo '"aa"' >>  $tempf
echo '"aa"' >>  $tempf
echo '"aa"' >>  $tempf
echo '"aaa"' >>  $tempf
echo '"aaa"' >>  $tempf
echo '"aaa"' >>  $tempf
echo '"aaa"' >>  $tempf
rslt=`str2bin $tempf`
nR=`echo $rslt | cut -f 1 -d ':' `
txtfile=`echo $rslt | cut -f 2 -d ':' `
szfile=`echo $rslt | cut -f 3 -d ':' `
nnfile=`echo $rslt | cut -f 4 -d ':' `
echo "txtfile = $txtfile";
echo "szfile = $szfile";
echo "nnfile = $nnfile";

$VG q del_tbl     t1 
$VG q add_tbl     t1 $nR
$VG q add_fld     t1 f1 "fldtype=char string:n_sizeof=0:filename=$txtfile"
$VG q add_aux_fld t1 f1 $szfile sz
$VG q add_aux_fld t1 f1 $nnfile nn


$VG q del_tbl t2 
$VG q add_tbl t2 $nR
$VG q s_to_f  t2 id "fldtype=int:op=seq:start=0:incr=1" 
$VG q s_to_f  t1 one "fldtype=int:op=seq:start=1:incr=0" 
$VG q s_to_f  t1 ramp "fldtype=int:op=seq:start=1:incr=1" 

$VG q pr_flds     t1 f1:_sz_f1:ramp "" ""  #-- for debugging
stop=`expr $nR - 1`
rm -f $tempf
#---- Now we have finished setting up the data
$VG q mjoin t1 _sz_f1 one t2 id cnt sum
$VG q pr_flds t2 "id:cnt" "" "" 

$VG q mjoin t1 ramp one t2 id test1 sum
$VG q mjoin t1 ramp one t2 id test1 min
$VG q mjoin t1 ramp one t2 id test1 max

$VG q mjoin t1 _sz_f1 ramp t2 id cnt2 sum
$VG q pr_flds t2 "id:cnt2" "" "" 


# 
rm -f _tempf1 _tempf1.meta.csv
echo "1,2" >> _tempf1
echo "1,3" >> _tempf1
echo "2,4" >> _tempf1
echo "2,5" >> _tempf1
echo "3,6" >> _tempf1
echo "3,7" >> _tempf1
echo "3,"  >> _tempf1
echo "f1,int,4" >> _tempf1.meta.csv
echo "f2,int,4" >> _tempf1.meta.csv
q dld t1 _tempf1.meta.csv _tempf1 1:1
# echo PREMATURE EXIT; exit 1; 
#---------------------------------------
rm -f _tempf2 _tempf2.meta.csv
echo "0,2" >> _tempf2
echo ",3"  >> _tempf2
echo "2,4" >> _tempf2
echo "3,5" >> _tempf2
echo "4,6" >> _tempf2
echo "5,7" >> _tempf2
echo "6,"  >> _tempf2
echo "f1,int,4" >> _tempf2.meta.csv
echo "f2,int,4" >> _tempf2.meta.csv
q dld t2 _tempf2.meta.csv _tempf2 1:1
#---------------------------------------
$VG q mjoin t1 f1 f2 t2 f1 f3 sum
$VG q mjoin t1 f1 f2 t2 f1 f3 min
$VG q mjoin t1 f1 f2 t2 f1 f3 max
$VG q mjoin t1 f1 f2 t2 f1 f3 reg


echo "ALL DONE"; exit 0;
