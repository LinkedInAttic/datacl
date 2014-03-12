#/usr/local/bin/bash
set -e 
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
#---------------------------------------------
rm -f _meta.csv
echo "f11,I4," >> _meta.csv
echo "f12,I4," >> _meta.csv
q dld t1 _meta.csv t1.csv ''

mode=complete
echo "mode = $mode"
q crossprod t1 f11 f12 t2 f21 f22 "" "" $mode
q pr_fld t2 f21:f22

mode=upper_triangular
echo "mode = $mode"
q crossprod t1 f11 f12 t2 f21 f22 "" "" $mode
q pr_fld t2 f21:f22

mode=upper_triangular_minus_diagonal
echo "mode = $mode"
q crossprod t1 f11 f12 t2 f21 f22 "" "" $mode
q pr_fld t2 f21:f22

#-------
# q delete t1:t2
# od -i $Q_DOCROOT/docroot.aux
echo "--------------------"
rm -f _meta.csv
echo "f11,I4," >> _meta.csv
echo "f12,I4," >> _meta.csv
q dld t1 _meta.csv t2.csv ''

q add_tbl taux 3
q s_to_f taux batchsize 'op=[seq]:start=[4]:incr=[-1]:fldtype=[I4]'
mode=complete
q crossprod t1 f11 f12 tX fX1 fX2 taux batchsize $mode
q pr_fld tX fX1:fX2 '' _out1.csv
diff _out1.csv good1.csv

mode=upper_triangular
q crossprod t1 f11 f12 tX fX1 fX2 taux batchsize $mode
q pr_fld tX fX1:fX2 '' _out2.csv
diff _out2.csv good2.csv

mode=upper_triangular_minus_diagonal
q crossprod t1 f11 f12 tX fX1 fX2 taux batchsize $mode
q pr_fld tX fX1:fX2 '' _out3.csv
diff _out3.csv good3.csv

echo "ALL DONE"
