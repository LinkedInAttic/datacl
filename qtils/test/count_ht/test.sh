#/usr/local/bin/bash
set -e
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/TEMP
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
#---------------------------------------------
echo "f1,I4,"       > _meta.csv
q dld t1 _meta.csv data1.csv ""
q mk_idx t1 idx I4
# q s_to_f   t1 x 'op=[const]:val=[1]:fldtype=[I1]'
q f1s1opf2 t1 idx 2 '%' xx
q f1s1opf2 t1 xx  1 '==' x 

q count_ht t1 f1 t2 
q sortf1f2 t2 value count A_
q pr_fld t2 value:count  | sed s'/"//'g > _out1
rm -f _good1
echo "1,4" >> _good1
echo "2,3" >> _good1
echo "3,2" >> _good1
echo "4,1" >> _good1
echo "5,1" >> _good1
echo "6,1" >> _good1
echo "7,1" >> _good1
diff _out1 _good1

# cleanup
rm -f _*
rm -r -f ./TEMP/

echo "Successfully completed $0 in $PWD"
