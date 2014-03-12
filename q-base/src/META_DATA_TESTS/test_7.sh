#/usr/local/bin/bash
set -e 

##C \begin{invariant}
##C \label{inv_7}
##C If field is a nn field, then nn of parent must be same as itself.
##C \end{invariant}

# Let x identify nn fields 
q f1s1opf2 Tflds parent_id 0 '>=' x
q delete tempt
q copy_fld Tflds id        x tempt 
q copy_fld Tflds parent_id x tempt 
q rename tempt id fld_id 
q sortf1f2 tempt parent_id fld_id A_
q is_a_in_b Tflds id tempt parent_id '' fld_id chk_nn_fld_id
q f1f2opf3 Tflds nn_fld_id chk_nn_fld_id '!=' x
n=`q f_to_s Tflds x sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then echo FAILURE; exit 1; fi

q delete Tflds x:y:z:tempf
echo "Completed $0 in $PWD"
