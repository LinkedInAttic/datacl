set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
# make t1 
q add_tbl t1 10
q s_to_f t1 f1 'op=[seq]:start=[0]:incr=[1]:fldtype=[I4]'
q f1s1opf2 t1 f1 5 '>=' x
q f1f2_to_s t1 x f1 sum
# make t2 
