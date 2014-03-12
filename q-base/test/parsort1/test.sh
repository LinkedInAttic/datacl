#/usr/local/bin/bash
set -e 
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/mnt/u001/TEMP/
q init
#--------------------------------------------

q del_tbl Tsrc
nR=100000000
q add_tbl Tsrc $nR
q s_to_f Tsrc fsrc "op=seq:start=$nR:incr=-1:fldtype=int"
iter=1
while [ $iter -le 4 ]; do 
  time q parsort1 Tsrc fsrc fdst asc
  q pr_fld Tsrc fdst | head
  exit;
  time q fop Tsrc fsrc sortA
  echo "=============== Iter = $iter ==================";
  iter=`expr $iter + 1 `
done
#-------------------------------
# q del_tbl Tsrc
# q del_tbl Tdst
