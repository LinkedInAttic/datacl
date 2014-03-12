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

echo "cnt,I8,"        > _meta.csv
q dld t2 _meta.csv data2.csv ""

q lkp_sort t1 f1 t2 cnt idxf1 srtf1
q pr_fld t1 f1:idxf1:srtf1
q fop t1 idxf1 sortA
q mk_idx t1 idx I8
q f1f2opf3 t1 idx idxf1 '!=' x
n=`q f_to_s t1 x sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then echo FAILURE; exit 1; fi 

q delete t1 x:idxf1:idx
q lkp_sort t1 f1 t2 cnt "" srtf1
q describe t1 
echo "ALL DONE for $0 in $PWD"
