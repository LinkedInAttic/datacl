#/usr/local/bin/bash
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
rm -r -f  $Q_DATA_DIR
mkdir     $Q_DATA_DIR
set -e 
q init
#---------------------------------------------
rm -f _meta.csv
echo "f1,I4," > _meta.csv
q dld T1 _meta.csv in1.csv '' 
q fop T1 f1 sortA
q dld T2 _meta.csv in2.csv '' 
q fop T2 f1 sortA
q t1f1t2f2opt3f3 T1 f1 T2 f1 intersection '' T3 f1
q pr_fld T3 f1
