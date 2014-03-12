set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
# make t1 
q add_tbl t1 7
q s_to_f t1 f1 'op=[seq]:start=[0]:incr=[10]:fldtype=[I8]'
# make t2 
q add_tbl t2 56
q s_to_f t2 f2 'op=[seq]:start=[1]:incr=[1]:fldtype=[I8]'
q mk_idx t2 idx I8
# say page size is 8
q add_tbl t3 7
q s_to_f t3 lb_val 'op=[seq]:start=[1]:incr=[8]:fldtype=[I8]'
q s_to_f t3 ub_val 'op=[seq]:start=[8]:incr=[8]:fldtype=[I8]'
q s_to_f t3 lb_idx 'op=[seq]:start=[0]:incr=[8]:fldtype=[I8]'
q s_to_f t3 ub_idx 'op=[seq]:start=[8]:incr=[8]:fldtype=[I8]'
