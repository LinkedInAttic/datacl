#!/usr/local/bin/bash
set -e 
if [ $# != 3 ]; then echo FAILURE: $#: $0: $LINENO; exit 1; fi 
incl_rslt=$1
incl_code=$2
outfile=$3
while read testnum 
do 
  echo "%%------- START SPEC Test $testnum -----------" >> $outfile
  grep '^##C' test_$testnum.sh | sed s'/##C//'g >> $outfile 
  if [ $incl_code = 1 ]; then 
    echo "Invariant~\\ref{inv_$testnum} implemented in \
    Section~\\ref{code_$testnum}." >> $outfile 
  fi
  echo "%%------- STOP  SPEC Test $testnum -----------" >> $outfile
  if [ $incl_rslt = 1 ]; then 
    echo "%%------- START RSLTS Test $testnum -----------" >> $outfile
    test -f test_$testnum.sh
    set +e
    bash test_$testnum.sh
    status=$?
    if [ $status = 0 ]; then 
      echo '\textcolor{green}{TEST PASSED}' >> $outfile
    else 
      echo '\textcolor{red}{TEST FAILED}' >> $outfile 
    fi
    set -e
    echo "%%------- STOP  RSLTS Test $testnum -----------" >> $outfile
  fi
done < tests.csv
