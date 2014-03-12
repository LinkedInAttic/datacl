#/usr/local/bin/bash
set -e 
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/mnt/u001/TEMP/
q init
#--------------------------------------------

q del_tbl Tsrc
nR=100
q add_tbl Tsrc $nR
q s_to_f Tsrc fsrc "op=seq:start=1:incr=1:fldtype=int"

q del_tbl Trng
nR=10
incr=10
incrplusone=`expr $incr + 1`
q add_tbl Trng $nR
q s_to_f Trng lb  "op=seq:start=1:incr=$incr:fldtype=int"
q s_to_f Trng ub  "op=seq:start=$incrplusone:incr=$incr:fldtype=int"
q s_to_f Trng cnt "op=const:val=$incr:fldtype=long long"
q mv_range Tsrc fsrc fdst Trng lb ub cnt
