#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
export Q_RUN_TIME_CHECKS=TRUE
rm -r -f          $PWD/QD/
mkdir             $PWD/QD/
qtils mk_ldb_dict data.csv comma xyz "write" _xxx _yyy ""
od -l -v _xxx > _asc_xxx
od -i -v _yyy > _asc_yyy
diff _asc_xxx good_xxx
diff _asc_yyy good_yyy
#-- Testing read 
qtils mk_ldb_dict data.csv comma xyz "read" _x2 _y2 "num_cols=[2]"
cmp _xxx _x2
cmp _yyy _y2
#-- Testing append 
# echo PREMATURE; exit 1; 
qtils mk_ldb_dict data2.csv comma xyz "append" _x3 _y3 ""
od -l -v _x3 > _chkx3
od -i -v _y3 > _chky3
diff _chkx3 goodx3
diff _chky3 goody3
echo "ALL DONE for $0 in $PWD"

rm -f _*
rm -r -f $Q_DATA_DIR
