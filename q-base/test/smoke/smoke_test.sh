#!/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'

set -e 
rm -r -f TEMP
mkdir TEMP
export Q_DOCROOT=$PWD/TEMP
export Q_DATA_DIR=$PWD/TEMP
q init
q delete T1
q delete T2
q add_tbl T1 10
q s_to_f T1 f1 'op=[const]:val=[10]:fldtype=[I4]'
q s_to_f T1 f2 'op=[seq]:start=[1]:incr=[10]:fldtype=[I4]'
q describe T1
q describe T1 f1
q describe T1 f2
q f_to_s T1 f1  min; echo "";
q f_to_s T1 f2  min; echo "";
q f_to_s T1 f1  max; echo "";
q f_to_s T1 f2  max; echo "";
q f_to_s T1 f1  sum; echo "";
q f_to_s T1 f2  sum; echo "";
q delete T1 
q dld T1 test1.meta.csv data1.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
# echo PREMATURE; exit 1; 
q pr_fld T1 f4
set +e
q pr_fld T1 i1
q pr_fld T1 i2
if [ $? = 0 ]; then echo "FAILURE $LINENO"; exit 1; fi 
q pr_fld T1 i4
if [ $? = 0 ]; then echo "FAILURE $LINENO"; exit 1; fi 
set -e
q pr_fld T1 i8
q pr_fld T1 f4
q pr_fld T1 f8
q pr_fld T1 i1:i8:f4:f8
q dld T2 test2.meta.csv data2.csv 'ignore_hdr=[true]:fld_sep=[comma]' 
q pr_fld T2 i1
q dup_fld T2 i1 dup_i1
q dup_fld T2 f4 dup_f4
# TODO q list_files
# TODO q orphan_files
#------------
q delete T3
q add_tbl T3 10000000
q s_to_f T3 f1 "op=[seq]:fldtype=[I4]:start=[0]:incr=[1]"
q bindmp T3 f1 ''  _xx
fileno=`q describe T3 f1 fileno | cut -f 3 -d "," | sed s'/"//'g`
binfile="${Q_DATA_DIR}/_$fileno"
cmp $binfile _xx
rm -f _xx
q set_meta T3 f1 disp_name "abc"
dispname=`q describe T3 f1 | cut -f 4 -d "," | sed s'/"//'g`
if [ "$dispname" != "abc" ]; then echo FAILURE; exit 1; fi 
q set_meta T3 "" disp_name "abc"

q bindmp T1 f8 _nn_i8 _xxx
q binld  T4 f8 F8 _xxx
q pr_fld T1 f8 'cond=[_nn_i8]' _yyy
q pr_fld T4 f8 ''              _zzz
diff _yyy _zzz
rm -f _xxx _yyy _zzz

q dld T3  test3.meta.csv  data3.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
q pr_dict dict1 csv ""
q dld T4  test4.meta.csv  data4.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
q pr_dict dict1 csv ""
set +e
q dld T3  test3.meta.csv  data3.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
if [ $? = 0 ]; then echo "FAILURE: $LINENO"; exit 1; fi 
set -e
# testing sortf1f2
rm -f _data.csv
n=10
while [ $n -gt 0 ]; do
  echo $n >> _data.csv
  n=`expr $n - 1 `
done
echo "i4,I4,\"is_load=[true]:is_all_def=[true]\"" > _meta.csv
q dld tempt _meta.csv _data.csv '' 
q s_to_f tempt id 'op=[seq]:start=[0]:incr=[1]:fldtype=[I4]'
q sortf1f2 tempt i4 id 
q pr_fld tempt i4:id 

#------------------------------------------------------
q dld T3  test31.meta.csv  data3.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
q dld T3S test3S.meta.csv data3.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
echo "-----------------------------";
q dld T3  test31.meta.csv  data3.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
q dld T3S test3S.meta.csv data4.csv 'ignore_hdr=[false]:fld_sep=[comma]' 

q dld T3  test31.meta.csv  data3.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
q dld T3Q test3Q.meta.csv data4.csv 'ignore_hdr=[false]:fld_sep=[comma]' 

q dld T3  test31.meta.csv  data3.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
set +e
q dld T3R test3R.meta.csv data4.csv 'ignore_hdr=[false]:fld_sep=[comma]' 
if [ $? = 0 ]; then echo "FAILURE: $LINENO"; exit 1; fi 
set -e 
# Incorrect use of safe mode tested above
# START testing fb
set -e 
rm -r -f $Q_DATA_DIR/_fb_T5
q dld T5 meta5.csv data5.csv ''
q describe T5
q pr_fld T5 i1:i2:i4:i8:f4:f8
qfb init_fb_idx T5 i1:i2:i4:i8:f4:f8
qfb mk_fb_idx T5 i1 ""
qfb mk_fb_idx T5 i2 ""
qfb mk_fb_idx T5 i4 ""
qfb mk_fb_idx T5 i8 ""
qfb mk_fb_idx T5 f4 ""
qfb mk_fb_idx T5 f8 ""
qfb query_fb_idx T5 i1 " ( f4 >= 15  ) and ( f8 <= 25 ) " _tempf_I1
qfb query_fb_idx T5 i2 " ( f4 >= 15  ) and ( f8 <= 25 ) " _tempf_I2
qfb query_fb_idx T5 i4 " ( f4 >= 15 )  and ( f8 <= 25 ) " _tempf_I4
qfb query_fb_idx T5 i8 " ( f4 >= 15 )  and ( f8 <= 25 ) " _tempf_I8
# STOP testing fb
echo ALL DONE
