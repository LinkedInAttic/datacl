#/usr/local/bin/bash
set -e 
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/TEMP
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
#--------------------------------------------
nR1=1000000
nR2=1000
q add_tbl T1 $nR1
q mk_idx T1 f1 I4
q subsample T1 f1 $nR2 T2 f2
q pr_fld T2 f2 | head
#-------------------------------
# rm -r -f $Q_DATA_DIR
