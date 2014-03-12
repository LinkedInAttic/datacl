#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
set -e 
#---------------------------------------------
echo "lnk,int,4"  > _meta.csv
echo "val,int,4" >> _meta.csv
q dld Tsrc _meta.csv src.csv 1:1 ''
echo "lnk,int,4"  > _meta.csv
q dld Tdst _meta.csv dst.csv 1 ''
# sort on lnk
q fop Tdst lnk sortA
q sortf1f2 Tsrc lnk val A_

echo "Regular join"
q srt_join Tsrc lnk val Tdst lnk val reg
q pr_flds Tdst lnk:val 
#----------------------------------------------
echo "Min join"
q srt_join Tsrc lnk val Tdst lnk val min
q pr_flds Tdst lnk:val 
#----------------------------------------------
echo "Max join"
q srt_join Tsrc lnk val Tdst lnk val max
q pr_flds Tdst lnk:val 
#----------------------------------------------
echo "lnk,int,4"  > _meta.csv
echo "val,int,4" >> _meta.csv
q dld Tsrc _meta.csv src2.csv 1:1 ''
q sortf1f2 Tsrc lnk val A_
#----------------------------------------------
echo "AND join"
q srt_join Tsrc lnk val Tdst lnk val and
q pr_flds Tdst lnk:val 
#----------------------------------------------
echo "OR join"
q srt_join Tsrc lnk val Tdst lnk val or
q pr_flds Tdst lnk:val 
echo "CNT join"
q srt_join Tsrc lnk '' Tdst lnk val cnt
q pr_flds Tdst lnk:val 
