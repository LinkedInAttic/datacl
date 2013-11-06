#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
q dld t1 meta1.csv data1.csv 'is_all_def=[true]'
q pr_fld lkp_fldSV idx:txt
q pr_fld t1 fI1:fI2:fI4:fI8:fF4:fF8:fk_lkp_fldSV
q dld t2 meta2.csv data2.csv 'ignore_hdr=[true]'
q dld t3 meta3.csv data3.csv 'ignore_hdr=[true]:fld_sep=[tab]'
echo ALL DONE;

rm -r -f $Q_DATA_DIR
