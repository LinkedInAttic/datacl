#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
export Q_RUN_TIME_CHECKS=TRUE
rm -r -f          $PWD/QD/
mkdir             $PWD/QD/
q init $Q_DATA_DIR ""
#-----------------------------------------------------------
q dld tv meta11.csv val1.csv ''
q dld tp meta2.csv pat1.csv ''
q patmatch tv val '' fout tp val lb ub 
chk=`q pr_fld tv val 'cond=[fout]' | grep '^1$' | wc -l`
if [ $chk != 2 ]; then echo FAILURE: $LINENO; exit 1; fi
q pr_fld tv val 'cond=[fout]' > _I4

q f1opf2 tv val 'op=[conv]:newtype=[I2]' valI2
q f1opf2 tp val 'op=[conv]:newtype=[I2]' valI2
q patmatch tv valI2 '' fout tp valI2 lb ub 
q pr_fld tv valI2 'cond=[fout]' > _I2
diff _I2 _I4

q f1opf2 tv val 'op=[conv]:newtype=[I1]' valI1
q f1opf2 tp val 'op=[conv]:newtype=[I1]' valI1
q patmatch tv valI1 '' fout tp valI1 lb ub 
q pr_fld tv valI1 'cond=[fout]' > _I1
diff _I1 _I4

#-----------------------------------------------------------

q dld tv meta12.csv val2.csv ''
q dld tp meta2.csv  pat2.csv ''
q patmatch tv val fb fout tp val lb ub 
rslt=`q f_to_s tv fout sum`
if [ "$rslt" != "27:1024" ]; then echo FAILURE: $LINENO; exit 1; fi 

#-----------------------------------------------------------
q dld tv meta11.csv val3.csv ''
q dld tp meta2.csv pat3.csv ''
q patmatch tv val '' fout tp val lb ub 
chk=`q pr_fld tv val 'cond=[fout]' | grep '^1$' | wc -l`
if [ $chk != 2 ]; then echo FAILURE: $LINENO; exit 1; fi
#-----------------------------------------------------------

echo "Successfully completed $0 in $PWD "
