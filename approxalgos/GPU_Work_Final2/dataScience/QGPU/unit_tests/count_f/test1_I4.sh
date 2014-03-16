relpath=./unit_tests/data
file1=$relpath/1e5_histdata_I4.bin
file2=$relpath/1e5_evenones_I1.bin
file3=$relpath/1e5_all10s_I4.bin

instr="gq load filesz=400000 nR=100000 fldtype=I4 filename=$file1 tbl=T1 h_fld=a d_fld=a"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq load filesz=100000 nR=100000 fldtype=I1 filename=$file2 tbl=T1 h_fld=b d_fld=b"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq load filesz=400000 nR=100000 fldtype=I4 filename=$file3 tbl=T1 h_fld=c d_fld=c"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=80 nR=10 fldtype=I8 filename=qwe tbl=T1 h_fld=d d_fld=d"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq count_f d_fld1=a d_cfld=b d_fld2=c d_out=d"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=d n=10 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 
