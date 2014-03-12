set -e 
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
q add_tbl t1 100000000
q mk_idx  t1 f1 I4
q mk_idx  t1 f2 I4
q f1s1opf2 t1 f2 2 '%' ix
q f1opf2  t1 ix 'op=[conv]:newtype=[I1]' x
q mk_nn_fld t1 x t1 f2
q delete t1 ix

q start_compound 
prime="'"
q f1opf2   t1 f1 'op=[++]' "g3$prime"
q f1opf2   t1 f2 'op=[conv]:newtype=[F4]' "g2$prime"
q f1opf2   t1 f1 'op=[conv]:newtype=[F4]' "g1$prime"
q f1s1opf2 t1 "g1$prime" 10 '*' "g11$prime"
q f1s1opf2 t1 "g2$prime" 99 '/' "g22$prime"
q f1f2opf3 t1 "g11$prime" "g2$prime" '-' g3_yes_null
q f1f2opf3 t1 "f1"   "g3$prime" '*' g3_no_null
q f_to_s   t1 g3_yes_null sum SUM_WITH_NULL
q f_to_s   t1 g3_no_null  sum SUM_WITHOUT_NULL
q f_to_s   t1 "g22$prime" sum JUNK
# echo PREMATURE; exit 1; 
# valgrind $valgrind_options q stop_compound
q stop_compound
isfld=`q is_fld t1 g3_yes_null | cut -f 1 -d ":"`
if [ $isfld != 1 ]; then echo FAILURE; exit 1; fi 
isfld=`q is_fld t1 g3_no_null | cut -f 1 -d ":"`
if [ $isfld != 1 ]; then echo FAILURE; exit 1; fi 
q pr_fld t1 f1:f2:g3_no_null:g3_yes_null | head
echo ALL DONE
