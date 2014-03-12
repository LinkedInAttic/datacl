set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
# make t1 
rm -f _data.csv
echo "2" >> _data.csv
echo "3" >> _data.csv
echo "4" >> _data.csv
echo "4" >> _data.csv
echo "3" >> _data.csv
echo "2" >> _data.csv
echo "4" >> _data.csv
echo "2" >> _data.csv
echo "3" >> _data.csv
echo "f1,I4," > _meta.csv
q dld t1 _meta.csv _data.csv '' 
q add_tbl t2 3
q s_to_f t2 lb 'op=[seq]:start=[0]:incr=[3]:fldtype=[I4]'
q s_to_f t2 cnt 'op=[const]:val=[3]"fldtype=[I4]'
# make t2 
q rng_sort t1 f1 A t2 lb cnt 
# crossprod
q pr_fld t1 f1 
