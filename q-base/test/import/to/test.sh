#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/_datadir
export Q_DATA_DIR=$PWD/_datadir
rm -r -f $PWD/_datadir
mkdir    $PWD/_datadir
q init
set -e 
#---------------------------------------------
q delete t1
q add_tbl t1 10
q s_to_f  t1 id 'op=[seq]:start=[10]:incr=[100]:fldtype=[I4]'
from_docroot=$HOME/WORK/Q/test/import/from/_datadir/
from_data_dir=$HOME/WORK/Q/test/import/from/_datadir/
q import "$from_docroot" "$from_data_dir" lkp_XXX lkp_XXX
q import "$from_docroot" "$from_data_dir" t1 t2
q pr_fld t2 x:y:z:by:bz:fk_lkp_XXX
# THINGS TO DO MANUALLY
# TEST 1 
# q del_fld t2 x
# Then q pr_fld t1 x in FROM DOCROOT SHOULD BE UNAFFECTED
# TEST 2 
# q fop t2 y sortA should fail
# TEST 3 
# q import "$from" t1 t2
# Then q pr_fld t1 x in FROM DOCROOT SHOULD BE UNAFFECTED
