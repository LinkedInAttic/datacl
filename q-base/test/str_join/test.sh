#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
set -e 
#---------------------------------------------

echo "lnk,char string,0"  > _meta.csv
echo "val,int,4" >> _meta.csv
q dld Tsrc _meta.csv src.csv 1:1 ''

echo "lnk,char string,0"  > _meta.csv
q dld Tdst _meta.csv dst.csv 1 ''

q str_join Tsrc lnk val Tdst lnk val
