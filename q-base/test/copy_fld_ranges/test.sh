#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export    Q_DOCROOT=$PWD/TEMPM
export    Q_DSK_DATA_DIR=$PWD/QD/TEMPD
export    Q_RAM_DATA_DIR=$PWD/QD/TEMPR
export    Q_RUN_TIME_CHECKS=TRUE
rm -r -f $Q_DOCROOT
rm -r -f $Q_DSK_DATA_DIR
rm -r -f $Q_RAM_DATA_DIR
mkdir    $Q_DOCROOT
mkdir    $Q_DSK_DATA_DIR
mkdir    $Q_RAM_DATA_DIR

q init $Q_DSK_DATA_DIR $Q_RAM_DATA_DIR
q add_tbl t1 100
q s_to_f t1 f1 'op=[seq]:start=[1]:incr=[1]:fldtype=[I4]'

q add_tbl tr 8
q s_to_f tr lb 'op=[seq]:start=[10]:incr=[10]:fldtype=[I8]'
q s_to_f tr ub 'op=[seq]:start=[12]:incr=[10]:fldtype=[I8]'


q copy_fld_ranges t1 f1 tr lb ub t2
q rename t2 f1 f2
q copy_fld_ranges t1 f1 tr lb ub t2

echo "f1,I4," > _meta.csv
rm -f _data.csv
iter=1;
while [ $iter -le 100 ]; do 
  echo $iter >> _data.csv
  echo ""    >> _data.csv
  iter=`expr $iter + 2`
done
  
q delete t1:t2
q dld t1 _meta.csv _data.csv ''
q copy_fld_ranges t1 f1 tr lb ub t2

q delete t2
q copy_fld_ranges t1 f1 "" 2 12 t2

echo "Succesfully Completed $0 in $PWD"
rm -f _*
rm -r -f $Q_DOCROOT
rm -r -f $Q_DSK_DATA_DIR
rm -r -f $Q_RAM_DATA_DIR
