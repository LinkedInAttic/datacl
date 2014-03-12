#/usr/local/bin/bash
set -e
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
q init
#---------------------------------------------
# cleanup
q del_tbl t1
q del_tbl t2
echo "f,int,4" > _meta.csv
q dld t1 _meta.csv t1.csv 1 "" 
q dld t2 _meta.csv t2.csv 1 "" 
q fop t1 f sortA
q fop t2 f sortA
q is_a_in_b t1 f t2 f x '' ''
q is_a_in_b t2 f t1 f x '' ''

# cleanup
q del_tbl t1
q del_tbl t2

#--------------------------------------------------
echo "f,int,4" > _meta.csv
q dld t1 _meta.csv t1.csv 1 ""
q dld t2 _meta.csv t2.csv 1 ""
q fop t2 f sortA
q is_a_in_b_2 t1 f t2 f x '' ''

# cleanup
q del_tbl t1
q del_tbl t2

echo "f,int,4" > _meta.csv
q dld t1 _meta.csv t1.csv 1 ""
q dld t2 _meta.csv t2.csv 1 ""
q f1opf2 t1 f "op=conv:newtype=long long" lf
q f1opf2 t2 f "op=conv:newtype=long long" lf
q fop t2 lf sortA
q is_a_in_b t1 lf t2 lf x '' ''

# cleanup
q del_tbl t1
q del_tbl t2
