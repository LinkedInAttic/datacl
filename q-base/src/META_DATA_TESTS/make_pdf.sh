#!/usr/local/bin/bash
#----------------------------------------------------------------
set -e
if [ $# != 2 ]; then echo FAILURE: $0 : $LINENO; exit 1; fi 
incl_rslt=$1
incl_code=$2
if [ "$Q_DOCROOT"  = "" ]; then echo FAILURE: $LINENO; exit 1; fi 
if [ "$Q_DATA_DIR" = "" ]; then echo FAILURE: $LINENO; exit 1; fi 
test -d $Q_DOCROOT
test -d $Q_DATA_DIR
TST_DOCROOT=$Q_DOCROOT/debug
TST_DATA_DIR=$Q_DATA_DIR/debug
export Q_DOCROOT=$TST_DOCROOT
export Q_DATA_DIR=$TST_DOCROOT
#----------------------------------------------------------------
rm -f _meta_tests.pdf _meta_tests.tex
touch .meta
eval `../../latex/tools/setenv`
set +e
echo "\\startreport{Meta Data Tests for Q}" >> _meta_tests.tex
echo "\\reportauthor{Ramesh Subramonian}" >> _meta_tests.tex
echo "\\newcommand{\\IncludeResults}{$incl_rslt}" >> _meta_tests.tex
echo "\\section{Tests}" >> _meta_tests.tex

bash code_gen_1.sh $incl_rslt $incl_code _meta_tests.tex
if [ $incl_code = 1 ]; then 
  echo "\\section{Code Listing}" >> _meta_tests.tex
  bash code_gen_2.sh             _meta_tests.tex
fi
#----------------------------------------------
make -f ../../latex/tools/docdir.mk _meta_tests.pdf

