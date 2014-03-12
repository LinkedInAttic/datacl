#/usr/local/bin/bash
set -e
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
#---------------------------------------------
$VG q del_tbl t1
$VG q dld     t1 region.meta.csv region.csv '' ''
$VG q pr_fld  t1 description
$VG q fop     t1 description lcase
$VG q pr_fld  t1 description
