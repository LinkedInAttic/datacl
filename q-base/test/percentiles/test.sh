#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
set -e 
#---------------------------------------------
q add_tbl t1 100
q mk_idx t1 i4 I4
q mk_idx t1 i8 I8
q f1opf2 t1 i4 'op=[conv]:newtype=[F4]' f4
q f1opf2 t1 i4 'op=[conv]:newtype=[F8]' f8

q percentiles t1 i4 tout 6
q pr_fld tout min:max:avg:cnt
