relpath=./unit_tests/data
file1=$relpath/1e5_incr_mod100_I1.bin

instr="gq load filesz=100000 nR=100000 fldtype=I1 filename=$file1 tbl=T1 d_fld=a h_fld=a"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=100000 nR=100000 fldtype=I1 filename=qwer tbl=T1 h_fld=c d_fld=c"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq f1opf2 d_fld1=a d_fld2=c op=shift val=3 newval=-1"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

mode="head"
instr="gq print_reg_data d_fld=c n=10 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=100000 nR=100000 fldtype=I1 filename=qwer tbl=T1 h_fld=d d_fld=d"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq f1opf2 d_fld1=c d_fld2=d op=shift val=-2 newval=0"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq print_reg_data d_fld=d n=10 mode=$mode"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 
