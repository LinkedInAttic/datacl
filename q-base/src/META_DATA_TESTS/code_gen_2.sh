#!/usr/local/bin/bash
if [ $# != 1 ]; then echo FAILURE; exit 1; fi 
outfile=$1
while read testnum 
do 
  echo "%%------- START CODE Test $testnum -----------" >> $outfile
  echo "\\subsection{Invariant $testnum}" >> $outfile
  echo "\\label{code_$testnum} " >> $outfile
  echo "\\begin{verbatim}" >> $outfile 
  grep -v "^##C " test_$testnum.sh > _tempf
  cat _tempf >> $outfile 
  echo "\\end{verbatim}"   >> $outfile 
  rm -f _tempf
  echo "%%------- STOP SPEC Test $testnum -----------" >> $outfile
done < tests.csv



