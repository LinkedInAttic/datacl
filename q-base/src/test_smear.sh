set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
# make t1 
q add_tbl t1 100
q mk_idx t1 f1 I4
q f1s1opf2 t1 f1 10 '%' f2
q f1s1opf2 t1 f2 0 '==' x
q f1opf2 t1 x 'op=[smear]:plus=[0]:minus=[0]' y
q pr_fld t1 f1:x:y
