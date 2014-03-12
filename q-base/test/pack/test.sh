set -e 
export Q_DOCROOT=$PWD/TEMP/
export Q_DATA_DIR=$PWD/TEMP/
rm -r -f $PWD/TEMP; mkdir $PWD/TEMP; q init
q init
q add_tbl t1 8 
q s_to_f t1 f1 'op=[seq]:start=[0]:incr=[1]:fldtype=[I1]'
q s_to_f t1 f2 'op=[seq]:start=[0]:incr=[1]:fldtype=[I2]'
q s_to_f t1 f3 'op=[seq]:start=[0]:incr=[1]:fldtype=[I4]'
q pack t1 f1:f2:f3 "6:3:0" I8 fout
q pr_fld t1 fout '' _out1
echo "0,73,146,219,292,365,438,511" > _out2
qtils replace_char _out2 "," bslashn
diff _out1 _out2
q unpack t1 fout "6:3:0" "3:3:3" I1:I2:I4 g1:g2:g3 
# q pr_fld t1 f1:f2:g1:g2:f3:g3
q f1f2opf3 t1 f1 g1 '!=' x 
n=`q f_to_s t1 x sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then echo "FAILURE: $LINENO"; exit 1; fi 

q f1f2opf3 t1 f2 g2 '!=' x 
n=`q f_to_s t1 x sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then echo "FAILURE: $LINENO"; exit 1; fi 

q f1f2opf3 t1 f3 g3 '!=' x 
n=`q f_to_s t1 x sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then echo "FAILURE: $LINENO"; exit 1; fi 
echo ALL DONE
