set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
# make t1 
q add_tbl t1 10
q s_to_f t1 t1f1 'op=[seq]:start=[10]:incr=[1]:fldtype=[I4]'
q f_to_s t1 t1f1 is_sorted

q add_tbl t2 30
q mk_idx  t2 t2f1 I4

q mark_pos t1 t1f1 "" t2 t1f2
