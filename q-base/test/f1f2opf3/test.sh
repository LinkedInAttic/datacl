#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
set -e 
#---------------------------------------------
TEMPDIR=XXX
rm -r -f $TEMPDIR
mkdir $TEMPDIR
q del_tbl t1
echo "f1,int,4" > _meta.csv
q dld t1 _meta.csv t1.csv 1 ""
q dup_fld t1 f1 f2
q f1f2opf3 t1 f1 f2 "+" f3
#--------------
N=10
q del_tbl t2
q add_tbl t2 $N
q s_to_f t2 f1 "op=seq:start=1:incr=1:fldtype=int" 
q s_to_f t2 f2 "op=seq:start=1:incr=2:fldtype=int" 
q f1f2opf3 t2 f1 f2 "+" "f3"
#----------------------
export Q_TEMP_DATA_DIR=$PWD/$TEMPDIR
q f1f2opf3 t2 f1 f2 "+" "ephemeral:f4"
q fld_meta t2 f4 | grep filename | grep $TEMPDIR
if [ $? != 0 ]; then echo "FAILURE: $LINENO"; exit 1; fi

q f1f2opf3 t2 f1 f2 "+" "f5"
q fld_meta t2 f5 | grep filename | grep $TEMPDIR
if [ $? = 0 ]; then echo "FAILURE: $LINENO"; exit 1; fi
#----------------------
rm -f _meta.csv
rm -r -f $TEMPDIR
