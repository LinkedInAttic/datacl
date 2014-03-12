#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/

q init
q del_tbl t1
q dld t1 meta.csv data.csv 1:1:1 ''
q f1opf2 t1 ifld 'op=conv:newtype=float' ffld
q f1opf2 t1 ifld 'op=conv:newtype=long long' lfld
q f1opf2 t1 ffld 'op=conv:newtype=double' dfld
#-------------------------------
q dup_fld t1 ifld i2
q dup_fld t1 bfld b2
q dup_fld t1 ffld f2
q dup_fld t1 lfld l2
q dup_fld t1 dfld d2
q dup_fld t1 sfld s2
#----------------------------------
q pr_flds t1 ifld:bfld:ffld:lfld:dfld:sfld
echo "============================";
q pr_flds t1 i2:b2:f2:l2:d2:s2
