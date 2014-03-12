#/usr/local/bin/bash
set -e 

##C \begin{invariant}
##C \label{inv_3}
##C Relationship between min/max/sum and fldtype 
##C \begin{enumerate}
##C \item If min is defined, then fldtype must be one of I1, I2, I4, I8, F4, F8. 
##C \item If max is defined, then fldtype must be one of I1, I2, I4, I8, F4, F8. 
##C \item If sum is defined, then fldtype must be one of B, I1, I2, I4, I8, F4, F8. 
##C \end{enumerate}
##C \end{invariant}


function foo()
{
  fldtype=$1
  q regex_match lkp_fldtype txt $fldtype exact x
  idx=`q f_to_s lkp_fldtype x 'op=[get_idx]:val=[1]'`
  q f1s1opf2 Tflds fldtype $idx '==' tempf
  q f1f2opf3 Tflds tempf y '||' y
}

q s_to_f Tflds y 'op=[const]:val=[0]:fldtype=[I1]'
foo I1
foo I2
foo I4
foo I8
foo F4
foo F8

function bar() 
{
  x=$1
  q f1s1opf2 Tflds is_${x}_nn 1 '==' x
  q f1f2opf3 Tflds x y '&&!' z
  n=`q f_to_s Tflds z sum | cut -f 1 -d ":"`
  if [ $n != 0 ]; then exit 1; fi 
}
bar min
bar max

foo B
bar sum

q delete Tflds x:y:z:tempf
echo "Completed $0 in $PWD"
