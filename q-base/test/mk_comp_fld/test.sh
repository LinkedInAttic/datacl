#/usr/local/bin/bash
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
set -e 
q init
#---------------------------------------------
q add_tbl tempt 10 
q s_to_f tempt f1 'op=seq:start=1:incr=10:fldtype=int'
q s_to_f tempt f2 'op=seq:start=1:incr=10:fldtype=int'
