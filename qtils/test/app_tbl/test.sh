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
q dld t1 meta1.csv data1.csv 'ignore_hdr=[false]'
q get_nR t1
q pr_fld t1 fldI4:fldI8:fldF4:fldF8:fldI1:fldI2

echo "------------------------"
q app_tbl t1 t1 
q get_nR t1
q pr_fld t1 fldI4:fldI8:fldF4:fldF8:fldI1:fldI2
echo "------------------------"


echo ALL DONE;
rm -r -f          $PWD/QD/
