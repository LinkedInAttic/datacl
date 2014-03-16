# relpath=./unit_tests/data
relpath=./unit_tests/datagen2/data
file1=$relpath/same_session_I1.bin
file2=$relpath/pageid_I4.bin

instr="gq load filesz=16 nR=16 fldtype=I1 filename=$file1 tbl=T1 h_fld=a d_fld=a"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq load filesz=64 nR=16 fldtype=I4 filename=$file2 tbl=T1 h_fld=b d_fld=b"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=16 nR=16 fldtype=I1 filename=dummy tbl=T1 h_fld=c1 d_fld=c1"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=16 nR=16 fldtype=I1 filename=dummy tbl=T1 h_fld=c2 d_fld=c2"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=16 nR=16 fldtype=I1 filename=dummy tbl=T1 h_fld=d1 d_fld=d1"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=16 nR=16 fldtype=I1 filename=dummy tbl=T1 h_fld=d2 d_fld=d2"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq f1s1opf2 d_fld1=b scalar=1 op=== d_fld2=c1"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq f1s1opf2 d_fld1=b scalar=2 op=== d_fld2=c2"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq funnel d_key=b d_value=2 d_same_session=a d_prev_funnel=c1 d_result=d1"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq funnel d_key=b d_value=1 d_same_session=a d_prev_funnel=c2 d_result=d2"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=40 nR=10 fldtype=I4 filename=dummy tbl=T1 h_fld=s1 d_fld=s1"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=40 nR=10 fldtype=I4 filename=dummy tbl=T1 h_fld=s2 d_fld=s2"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq funnel_count d_key=b d_distance=2 d_same_session=a d_curr_funnel=c1 d_result=s1"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=a n=16 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=b n=16 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=c1 n=16 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=s1 n=10 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=d1 n=16 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=c2 n=16 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=d2 n=16 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 
