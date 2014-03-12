#/usr/local/bin/bash
set -e 

##C \begin{invariant}
##C \label{inv_8}
##C fileno is unique and positive and defined
##C \end{invariant}

# Let x identify nn fields 
min=`q f_to_s Tflds fileno min | cut -f 1 -d ":"`
if [ $min -lt 0 ]; then echo FAILURE; exit 1; fi 
q dup_fld Tflds fileno x
q fop Tflds x sortA
q delete tempt
q count_vals Tflds x '' tempt fileno cnt
nR1=`q get_nR Tflds`
nR2=`q get_nR tempt`
if [ $nR1 -ne $nR2 ]; then echo FAILURE; exit 1; fi 

q delete Tflds x
q delete tempt
echo "Completed $0 in $PWD"
