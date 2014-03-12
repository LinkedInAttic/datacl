#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/_datadir
export Q_DATA_DIR=$PWD/_datadir
rm -r -f $PWD/_datadir
mkdir $PWD/_datadir
q init
set -e 
#---------------------------------------------
q delete t1
q dld t1 meta1.csv data1.csv '' 
