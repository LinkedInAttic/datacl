#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
export Q_RUN_TIME_CHECKS=TRUE
rm -r -f          $PWD/QD/
mkdir             $PWD/QD/
q init
q dld t2 meta2.csv data2.csv 'ignore_hdr=[true]'
# rm -r -f          $PWD/QD/
rslt=`q is_ddir $Q_DOCROOT | cut -f 1 -d ","`
if [ $rslt != 1 ]; then echo FAILURE; exit 1; fi 
tbl=lkp_fldSV

q regex_match $tbl txt ABC exact x 
rslt=`q f_to_s $tbl x sum | cut -f 1 -d ":"`
if [ $rslt != 1 ]; then echo FAILURE; exit 1; fi 

q regex_match $tbl txt DEFGHI exact x 
rslt=`q f_to_s $tbl x sum | cut -f 1 -d ":"`
if [ $rslt != 1 ]; then echo FAILURE; exit 1; fi 

echo ALL DONE;
