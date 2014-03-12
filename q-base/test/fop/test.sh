#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
set -e 
#---------------------------------------------
q del_tbl tempt
q add_tbl tempt 10 
q s_to_f tempt id 'op=const:val=1:fldtype=int'
q fld_meta tempt id 
q fop tempt id 'op=cast:newtype=float'
q fld_meta tempt id 

