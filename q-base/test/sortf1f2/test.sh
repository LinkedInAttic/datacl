#/usr/local/bin/bash
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
set -e 
q init
#---------------------------------------------
rm -f _meta.csv
echo "i1,int,4" >> _meta.csv
echo "i2,int,4" >> _meta.csv
q dld Tin _meta.csv in1.csv '' ''
q sortf1f2 Tin i1 i2 A_
q pr_flds Tin i1:i2 '' _out1.csv
diff _out1.csv out1.csv
#---------------------------------------------
q dld Tin _meta.csv in1.csv '' ''
q f1s1opf2 Tin i1 '-1' '*' minus_i1
q sortf1f2 Tin minus_i1 i2 A_
q pr_flds  Tin minus_i1:i2 '' _out1.csv
diff _out1.csv out2.csv
