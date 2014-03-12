#/usr/local/bin/bash
set -e
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD
export Q_RUN_TIME_CHECKS=TRUE
rm -r -f $PWD/QD/
mkdir    $PWD/QD/
q init
#---------------------------------------------
N=10
q delete t2
q add_tbl t2 $N
q s_to_f t2 f1 "op=[seq]:start=[1]:incr=[1]:fldtype=[I4]" 
q f1s1opf2 t2 f1 $N "+" f3
q pr_fld t2 f1:f3
echo "====================================================="
N=10
q delete t2
q add_tbl t2 $N
q s_to_f t2 f1 "op=[seq]:start=[1]:incr=[1]:fldtype=[I4]" 
q f1s1opf2 t2 f1 '1:2:3' '==' x
q pr_fld t2 f1 'cond=[x]'
echo "====================================================="
