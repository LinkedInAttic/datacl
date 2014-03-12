#/usr/local/bin/bash
set -e 

##C \begin{definition}
##C A table, \(i\), is said to be a ``dictionary'', if there is some
##C field, \(f\) such that \(f.dict\_tbl\_id = i\)
##C \end{definition}
##C \begin{invariant}
##C \label{inv_1}
##C If a table is a dictionary, then it must have the following fields 
##C \begin{enumerate}
##C \item len with fldtype = I4 
##C \item off with fldtype = I4 
##C \item idx with fldtype = I4 
##C \item txt with fldtype = SV 
##C \end{enumerate}
##C \end{invariant}
q f1s1opf2 Tflds dict_tbl_id 0 '>=' x
n=`q f_to_s Tflds x sum | cut -f 1 -d ":"`
if [ $n -gt 0 ]; then 
  q delete T1
  q copy_fld Tflds dict_tbl_id x T1
  q fop T1 dict_tbl_id sortA
  q count_vals T1 dict_tbl_id '' T2 dict_tbl_id cnt
# T2 is set of tables that are dictionaries for some field
  bash check1.sh T2 "txt" "SV"
  bash check1.sh T2 "key" "I8"
  bash check1.sh T2 ".len.txt" "I4"
  bash check1.sh T2 ".off.txt" "I4"
  bash check1.sh T2 "idx" "I4"
  echo "Completed $0 in $PWD"
else 
  echo "Test $0 in $PWD not executed "
fi
q delete T1:T2
q delete Tflds x
