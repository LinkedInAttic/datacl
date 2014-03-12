#/usr/local/bin/bash
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e 
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
export Q_RUN_TIME_CHECKS=TRUE
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
#---------------------------------------------
q dld t1 data.meta.csv data.csv ''

q bindmp t1 fldI4:fldF4:fldF8:fldI8 '' _tempfile $PWD
q binld  t2 fldI4:fldF4:fldF8:fldI8 I4:F4:F8:I8 _tempfile $PWD
q pr_fld t2 fldI4:fldF4:fldF8:fldI8 '' _out.csv
diff _out.csv data.csv

q s_to_f t1 id 'op=[seq]:start=[1]:incr=[1]:fldtype=[I4]'
q f1s1opf2 t1 id 3 '>' x 
q bindmp t1 fldI4:fldF4:fldF8:fldI8 x _tempfile2 $PWD
q binld  t2 fldI4:fldF4:fldF8:fldI8 I4:F4:F8:I8 _tempfile2 $PWD
q pr_fld t2 fldI4:fldF4:fldF8:fldI8 '' _out2.csv
diff _out2.csv chk2.csv

# cleanup
q delete t1:t2
rm -f _*
rm -r -f $Q_DATA_DIR
echo COMPLETED SUCCESSFULLY $0 in $PWD;
