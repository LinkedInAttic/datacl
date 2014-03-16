relpath=./unit_tests/data
file1=$relpath/1.8M_mult3_I4.bin
file2=$relpath/1.8M_val_I4.bin
file3=$relpath/1.8M_mult2_I4.bin

instr="gq load filesz=7200000 nR=1800000 fldtype=I4 filename=$file1 tbl=T1 h_fld=a d_fld=a"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq load filesz=7200000 nR=1800000 fldtype=I4 filename=$file2 tbl=T1 h_fld=b d_fld=b"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq load filesz=7200000 nR=1800000 fldtype=I4 filename=$file3 tbl=T1 h_fld=c d_fld=c"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=7200000 nR=1800000 fldtype=I4 filename=qweq tbl=T1 h_fld=d d_fld=d"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=1800000 nR=1800000 fldtype=I1 filename=asdasf tbl=T1 h_fld=e d_fld=e"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq join d_ls=a d_vs=b d_ld=c d_vd=d d_vd_nn=e op=reg"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq f_to_s d_fld1=d op=sum"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

