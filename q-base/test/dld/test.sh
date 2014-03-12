#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export Q_DOCROOT=$PWD/QD/
export Q_DSK_DATA_DIR=$PWD/QD/
export Q_RAM_DATA_DIR=$PWD/QD/
export Q_RUN_TIME_CHECKS=TRUE
rm -r -f          $PWD/QD/
mkdir             $PWD/QD/
q init $Q_DSK_DATA_DIR ""
q dld t1 meta1.csv data1.csv 'ignore_hdr=[false]'
q describe t1
q pr_fld t1 fldI1:fldI2:fldI4:fldI8:fldF4:fldF8
echo "-- PRINTED t1 ----------"
q dld t2 meta2.csv data2.csv 'ignore_hdr=[true]'
q describe t2
echo "-- DESCRIBED t2 ----------"
q pr_fld lkp_SV idx:key:txt:.len.txt:.off.txt
echo "--- PRINTED lkp_SV------"
q pr_fld t2 fldI1:fldI2:fldI4:fldI8:fldF4:fldF8:fldSC:fk_lkp_SV
echo "--- PRINTED t2 ---------"
# rm -r -f          $PWD/QD/
rslt=`q is_ddir $Q_DOCROOT | cut -f 1 -d ":"`
if [ $rslt != 1 ]; then echo FAILURE; exit 1; fi 

q copy_tbl t2 t3
n3=`q describe t3 | cut -f 3 -d "," | sort | uniq | wc -l`
n2=`q describe t2 | cut -f 3 -d "," | sort | uniq | wc -l`
# Currently, we do not copy SC and SV fields 
exp_n3=` echo "$n2 -2 " | bc `
if [ $exp_n3 != $n3 ]; then echo FAILURE: $LINENO; exit 1; fi 

echo ALL DONE;
echo PREMATURE; exit 1; 
echo "NOW TESTING BATCH FILE "
rm -f _batfile
echo "q is_tbl lkp_SV " >> _batfile
echo "q describe t2 "    >> _batfile
q batch _batfile

