set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
# make t1  and t2
# 000001  000001   1   1  ==> 000010 (2)
# 001000           8      ==> 001000 (8)
#         010010      18  ==> 010010 (18)
# 011000  011000   24 24  ==> 011000 (24)
# 100000  100011   32 35  ==> 100011 (35)
# 101000  101001   40 41  ==> 101001 (41)
# 110010  110010   50 50  ==> 110100 (52)
# 111001  111001   57 57  ==> 111010 (58)
echo "1,8,24,32,40,50,57" > _1.csv
qtils replace_char _1.csv ',' bslashn
echo "1,18,24,35,41,50,57" > _2.csv
qtils replace_char _2.csv ',' bslashn
echo "f1,I4," > _meta.csv
q dld t1 _meta.csv _1.csv ''
rslt=`q f_to_s t1 f1 is_sorted`
if [ "$rslt" != "ascending" ]; then echo FAILURE; exit 1; fi 
echo "f2,I4," > _meta.csv
q dld t2 _meta.csv _2.csv ''
rslt=`q f_to_s t2 f2 is_sorted`
if [ "$rslt" != "ascending" ]; then echo FAILURE; exit 1; fi 
q t1f1t2f2opt3f3 t1 f1 t2 f2 pvalcalc 'mask=[7]' t3 f3 

echo "2,8,18,24,35,41,52,58" > _3.csv
qtils replace_char _3.csv ',' bslashn
q pr_fld t3 f3 '' _alt_3.csv
diff _3.csv _alt_3.csv
echo ALL DONE;
