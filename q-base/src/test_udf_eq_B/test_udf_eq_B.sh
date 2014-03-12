export Q_DOCROOT=$PWD/TEMP
export Q_DATA_DIR=$PWD/TEMP
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
q add_tbl t1 4097
q s_to_f t1 f1 'op=[seq]:start=[0]:incr=[1]:fldtype=[I4]'
q f1s1opf2 t1 f1 8 '%' f2
q udf_eq_B t1 f2 0:1 x
q f_to_s t1 x sum
