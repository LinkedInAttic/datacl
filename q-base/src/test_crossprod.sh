set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
# make t1 
q add_tbl t1 12
q s_to_f t1 f1 'op=[seq]:start=[0]:incr=[1]:fldtype=[I4]'
q s_to_f t1 f2 'op=[seq]:start=[0]:incr=[10]:fldtype=[I4]'
# make t2 
q add_tbl taux 3
q s_to_f taux grp_cnt 'op=[seq]:start=[3]:incr=[1]:fldtype=[I4]'
# crossprod
q crossprod t1 f1 f2 t2 taux grp_cnt
