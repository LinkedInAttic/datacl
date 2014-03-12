#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q '
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
set -e 
#---------------------------------------------
# cleanup
echo "f1,I4,"         > _meta.csv
q dld t1 _meta.csv data1.csv ""
q dup_fld t1 f1 srt_A_f1
q dup_fld t1 f1 srt_D_f1

echo "lb,I8,"         > _meta.csv
echo "ub,I8,"        >> _meta.csv
q dld t2 _meta.csv data2.csv ""

q rng_sort t1 srt_A_f1 "A" t2 lb ub
q rng_sort t1 srt_D_f1 "D" t2 lb ub
q pr_fld t1 f1:srt_A_f1:srt_D_f1
echo "ALL DONE for $0 in $PWD"
