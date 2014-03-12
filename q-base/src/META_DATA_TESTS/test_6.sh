#/usr/local/bin/bash
set -e 

function foo_fld()
{
  fldtype=$1
  q regex_match lkp_fldtype txt $fldtype exact x
  idx=`q f_to_s lkp_fldtype x 'op=[get_idx]:val=[1]'`
  q f1s1opf2 Tflds fldtype $idx '==' tempf
  q f1f2opf3 Tflds tempf y '||' y
}

##C \begin{invariant}
##C \label{inv_6}
##C If field is a nn field, then fldtype must be I1 or B
##C \end{invariant}

# Let x identify nn fields 
q f1s1opf2 Tflds parent_id 0 '>=' x
n=`q f_to_s Tflds x sum | cut -f 1 -d ":"`
if [ $n = 0 ]; then echo Test not executed: $0 in $PWD; exit 0; fi

# Let y identify fields that are I1 or B
q s_to_f Tflds y 'op=[const]:val=[0]:fldtype=[I1]'
foo_fld I1 y
foo_fld B  y
# Let z = x and not y
q f1f2opf3 Tflds x y '&&!' z
n=`q f_to_s Tflds z sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then exit 1; fi 

q delete Tflds x:y:z:tempf
echo "Completed $0 in $PWD"
