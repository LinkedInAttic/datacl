relpath=./unit_tests/data
file1=$relpath/1e5_incr_mod100_I1.bin
file2=$relpath/1e5_evenones_I1.bin

instr="gq load filesz=100000 nR=100000 fldtype=I1 filename=$file1 tbl=T1 h_fld=a d_fld=a"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq load filesz=100000 nR=100000 fldtype=I1 filename=$file2 tbl=T1 h_fld=b d_fld=b"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq f1f2_to_s d_fld1=b d_fld2=a op=sum"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

