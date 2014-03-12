#/usr/local/bin/bash
set -e 

##C \begin{invariant}
##C \label{inv_5}
##C Name of field is unique within a table 
##C \end{invariant}

q dup_fld Tflds fk_lkp_flds x
q drop_nn_fld Tflds x  # TODO P1 Should not be needed
q f1s1opf2 Tflds tbl_id 16 '<<' y
q f1f2opf3 Tflds y x '|' z 
q fop Tflds z sortA
q count_vals Tflds z '' tempt z cnt
n1=`q get_nR Tflds`
n2=`q get_nR tempt`
if [ $n1 != $n2 ]; then echo FAILURE; exit 1; fi 
q delete Tflds x:y:z
q delete tempt
echo "Completed $0 in $PWD"
