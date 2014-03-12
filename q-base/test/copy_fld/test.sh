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
#--------------------------------------------

q delete t1
q dld t1 meta.csv data.csv ''
q f1opf2 t1 ifld 'op=[conv]:newtype=[F4]' ffld
q f1opf2 t1 ifld 'op=[conv]:newtype=[I8]' lfld
q f1opf2 t1 ffld 'op=[conv]:newtype=[F8]' dfld
#-------------------------------
n=`q get_nR t1`
q delete t2
q add_tbl t2 $n
q copy_fld t1 ifld '' t2
q copy_fld t1 ffld '' t2
q copy_fld t1 lfld '' t2
q copy_fld t1 dfld '' t2
q pr_fld t2 ifld:ffld:lfld:dfld
echo "=================================="
#-------------------------------
q f1s1opf2 t1 ifld 3 '>=' x
n=`q f_to_s t1 x sum | cut -f 1 -d ":"`
q delete t2
q add_tbl t2 $n
q copy_fld t1 ifld x t2
q copy_fld t1 ffld x t2
q copy_fld t1 lfld x t2
q copy_fld t1 dfld x t2
q pr_fld t2 ifld:ffld:lfld:dfld
echo "=================================="

# cleanup
rm -r -f $Q_DATA_DIR
echo SUCCESSFULLY COMPLETED $0 in $PWD

