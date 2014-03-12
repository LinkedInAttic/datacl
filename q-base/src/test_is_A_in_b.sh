export Q_DOCROOT=$PWD/TEMP
export Q_DATA_DIR=$PWD/TEMP
q init
export PATH=$PWD:$PATH
q init
q add_tbl t1 100
q s_to_f t1 f1 'op=[seq]:start=[0]:incr=[1]:fldtype=[I4]'
q add_tbl t2 20
q s_to_f t2 f2 'op=[seq]:start=[-10]:incr=[10]:fldtype=[I4]'
q is_A_in_b t1 f1 t2 f2 x
q f_to_s t1 x sum
q pr_fld t1 f1 'cond=[x]'
