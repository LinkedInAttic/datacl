#/usr/local/bin/bash
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
set -e 
q init
#---------------------------------------------
rm -f _meta.csv
echo "f1,I4," >> _meta.csv
q dld T1 _meta.csv in1.csv '' 

rm -f _meta.csv
echo "lb,I4," >> _meta.csv
echo "ub,I4," >> _meta.csv
q dld T2 _meta.csv in2.csv '' 

q f1T1opf2 T1 f1 T2 bin f2
q pr_fld T1 f1:f2
