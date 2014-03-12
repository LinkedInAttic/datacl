#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
set -e 
#---------------------------------------------

echo "str,char string,0" > _meta.csv
q dld tempt _meta.csv data2.csv 1 ''
q f1opf2 tempt str 'op=hash:hash_algo=addchars:len=8' hash
q pr_flds tempt str:hash
