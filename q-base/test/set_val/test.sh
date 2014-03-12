#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
set -e
t1=t1
export Q_DATA_DIR=/tmp/
export Q_DOCROOT=$PWD
q init 
#---------------------------------------------
q del_tbl t1
q dld     t1 meta.csv data.csv 1:1:1:1 ""
q pr_flds t1 bfld:ifld:llfld:ffld '' > _before.csv

q set_val t1 bfld 0 true
q set_val t1 bfld 1 false
q set_val t1 bfld 2 true

q set_val t1 ifld 0 100
q set_val t1 ifld 1 200
q set_val t1 ifld 2 300

q set_val t1 llfld 0 10000
q set_val t1 llfld 1 20000
q set_val t1 llfld 2 30000

q set_val t1 ffld 0 100000
q set_val t1 ffld 1 200000
q set_val t1 ffld 2 300000

q pr_flds t1 bfld:ifld:llfld:ffld '' > _after.csv

diff before.csv _before.csv
diff after.csv  _after.csv
