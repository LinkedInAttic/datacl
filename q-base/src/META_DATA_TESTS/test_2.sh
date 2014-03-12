#/usr/local/bin/bash
set -e 

##C \begin{invariant}
##C \label{inv_2}
##C If min is not defined, then min val must be 0. Same for sum, max
##C \end{invariant}
function foo()
{
  x=$1
  q f1s1opf2 Tflds is_${x}_nn 0 '==' x
  q f1s1opf2 Tflds ${x}I8 0 '!=' y 
  q f1f2opf3 Tflds x y '&&' z
  n=`q f_to_s Tflds z sum | cut -f 1 -d ":"`
  if [ $n -ne 0 ]; then exit 1; fi 
}
foo min
foo max
foo sum


q delete Tflds x:y:z
echo "Completed $0 in $PWD"
