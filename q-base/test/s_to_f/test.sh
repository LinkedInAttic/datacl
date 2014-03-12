#/usr/local/bin/bash
set -e
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
#---------------------------------------------
# cleanup
q del_tbl t1
q add_tbl t1 10 
q s_to_f t1 f1 'op=const:nfldtype=char string:val=ABC';
q pr_fld t1 f1
