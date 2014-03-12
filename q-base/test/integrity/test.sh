#/usr/local/bin/bash
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
if [ $# != 2 ]; then echo FAILURE; exit 1; fi 
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
tblfile=$1
fldfile=$2
test -f $tblfile
test -f $fldfile
#--------------------------------------------
q dld alltbl tbl.meta.csv $tblfile '' ''
q s_to_f alltbl zid 'op=seq:start=0:incr=1:fldtype=int'

q f1s1opf2 alltbl nR 1 '>=' x
n=`q f_to_s alltbl x sum`

q del_tbl tbl
q add_tbl tbl $n
q copy_fld alltbl zid    x tbl
q rename tbl zid alltbl_id
q copy_fld alltbl tbl_id x tbl
q copy_fld alltbl nR     x tbl
q copy_fld alltbl is_lkp x tbl
q copy_fld alltbl is_map x tbl
q xfer alltbl name tbl alltbl_id name
q xfer alltbl dispname tbl alltbl_id dispname

q dld allfld fld.meta.csv $fldfile '' ''
q s_to_f allfld zid 'op=seq:start=0:incr=1:fldtype=int'

q f1s1opf2 allfld tbl_id 0 '>=' x
n=`q f_to_s allfld x sum`

q del_tbl fld
q add_tbl fld $n
q copy_fld allfld zid    x fld
q rename fld zid allfld_id
q copy_fld allfld fld_id      x fld
q copy_fld allfld tbl_id      x fld 
q copy_fld allfld is_external x fld 
q copy_fld allfld is_lkp_id   x fld 
q copy_fld allfld is_lkp_val  x fld 
q copy_fld allfld lkp_tbl_id  x fld 
q copy_fld allfld sorttype    x fld 
q copy_fld allfld n_sizeof    x fld 
q copy_fld allfld auxtype     x fld 
q copy_fld allfld parent_id   x fld 
q copy_fld allfld nn_fld_id   x fld 
q copy_fld allfld sz_fld_id   x fld 
q copy_fld allfld filesize    x fld 

q xfer allfld fldtype fld allfld_id fldtype
q xfer allfld filename fld allfld_id filename
q xfer allfld name fld allfld_id name
q xfer allfld dispname fld allfld_id dispname
echo DATA LOADED
