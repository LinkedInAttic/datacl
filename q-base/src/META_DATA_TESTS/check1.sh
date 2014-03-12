#/usr/local/bin/bash
set -e
if [ $# != 3 ]; then echo FAILURE; exit 1; fi
Tdict=$1
fldname=$2
fldtype=$3

nR2=`q get_nR $Tdict`
if [ $nR2 == 0 ]; then echo FAILURE; exit 1; fi
# x identifies fields that are in some dict_tbl
q is_a_in_b Tflds tbl_id $Tdict dict_tbl_id x '' ''
# y identifies fields that have name "$fldname"
q regex_match lkp_flds txt $fldname exact x
val=`q pr_fld lkp_flds idx 'cond=[x]'`
q f1s1opf2 Tflds fk_lkp_flds $val '==' y
# z identifies fields that have type SV
q regex_match lkp_fldtype txt $fldtype exact x
val=`q pr_fld lkp_fldtype idx 'cond=[x]'`
q f1s1opf2 Tflds fldtype $val '==' z
# create a table that contains dict_tbl_id for x and y and z
q f1f2opf3 Tflds x y '&&' w
q f1f2opf3 Tflds w z '&&' w
q drop_nn_fld Tflds w # TODO P1 Should not be needed. 
n=`q f_to_s Tflds w sum | cut -f 1 -d ":" `
if [ $n = 0 ]; then echo FAILURE; exit 1; fi 
q delete tempt
q copy_fld Tflds tbl_id w tempt
q fop tempt tbl_id sortA
q is_a_in_b $Tdict dict_tbl_id tempt tbl_id x '' ''
chkn=`q f_to_s $Tdict x sum | cut -f 1 -d ":"`
if [ $chkn != $nR2 ]; then echo FAILURE; exit 1; fi 
q delete tempt
