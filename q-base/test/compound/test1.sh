#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e 
export Q_DOCROOT=$PWD/TEMPM/
export Q_DSK_DATA_DIR=$PWD/TEMPD/
export Q_RAM_DATA_DIR=$PWD/TEMPR/
export Q_RUN_TIME_CHECKS=TRUE

rm -r -f  $Q_DOCROOT
rm -r -f  $Q_DSK_DATA_DIR
rm -r -f  $Q_RAM_DATA_DIR

mkdir $Q_DOCROOT
mkdir $Q_DSK_DATA_DIR
mkdir $Q_RAM_DATA_DIR

echo "f1,I4,"  > _meta.csv
echo "f2,I4," >> _meta.csv
q init $Q_DSK_DATA_DIR $Q_RAM_DATA_DIR
q dld t1 _meta.csv data.csv ''
nR2=4
q add_tbl t2 $nR2
# done inside count: q s_to_f t2 cnt 'op=[const]:val=[0]:fldtype=[I8]'
q start_compound 
prime="'"
q f1s1opf2 t1 f1         $nR2 '/' "g4$prime"
q f1opf2   t1 f1         'op=[conv]:newtype=[F4]' "g1$prime"
q f1opf2   t1 f2         'op=[conv]:newtype=[F4]' "g2$prime"
q f1opf2   t1 f1         'op=[++]'                "t$prime"
q f1opf2   t1 "t$prime"  'op=[conv]:newtype=[F4]' "g3$prime"
q f1s1opf2 t1 "g1$prime" 10 '*' "g11$prime"
q f1s1opf2 t1 "g2$prime" 99 '/' "g22$prime"
q f1f2opf3 t1 "g11$prime" "g2$prime" '-' g3_yes_null
q f1f2opf3 t1 "g1$prime"  "g3$prime" '*' g3_no_null
q f_to_s   t1 g3_yes_null sum SUM_WITH_NULL
q f_to_s   t1 g3_no_null  sum SUM_WITHOUT_NULL
q f_to_s   t1 f1  max MAX_F1
q count    t1 "g4$prime" "" t2 cnt
# echo PREMATURE; exit 1; 
q stop_compound "" ""
isfld=`q is_fld t1 g3_yes_null | cut -f 1 -d ":"`
if [ $isfld != 1 ]; then echo FAILURE; exit 1; fi 
isfld=`q is_fld t1 g3_no_null | cut -f 1 -d ":"`
if [ $isfld != 1 ]; then echo FAILURE; exit 1; fi 
q pr_fld t1 f1:f2:g3_no_null:g3_yes_null
echo "=====";
q pr_fld t2 cnt
echo ALL DONE
