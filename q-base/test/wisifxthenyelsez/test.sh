#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
set -e 
#---------------------------------------------
$VG q del_tbl t1
$VG q dld t1 meta1.csv data1.csv 1:1:1:1:1 ""
$VG q wisifxthenyelsez t1 w x y z 
q pr_flds t1 w:x:y:z
echo "=========================="
q f1opf2 t1 y 'op=conv:newtype=long long' y
q f1opf2 t1 z 'op=conv:newtype=long long' z
$VG q wisifxthenyelsez t1 w x y z 
q pr_flds t1 w:x:y:z
echo "=========================="
$VG q wisifxthenyelsez t1 w x by bz 
q pr_flds t1 w:x:by:bz
echo "=========================="
