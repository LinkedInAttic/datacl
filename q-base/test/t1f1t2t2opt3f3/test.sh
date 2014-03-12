#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
#--------------------------------------------

q del_tbl t1
q add_tbl t1 20
q s_to_f t1 f1 'op=seq:start=1:incr=2:fldtype=int'
q fop t1 f1 sortA

q del_tbl t2
q add_tbl t2 20
q s_to_f t2 f2 'op=seq:start=1:incr=3:fldtype=int'
q fop t2 f2 sortA

q t1f1t2f2opt3f3 t1 f1 t2 f2 intersection t3 f3 
q t1f1t2f2opt3f3 t1 f1 t2 f2 union        t4 f4
