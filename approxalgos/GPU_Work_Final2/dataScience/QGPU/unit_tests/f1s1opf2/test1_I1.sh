relpath=./unit_tests/data
file1=$relpath/1e5_incr_mod100_I1.bin

instr="gq load filesz=100000 nR=100000 fldtype=I1 filename=$file1 tbl=T1 h_fld=a d_fld=a"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq add_fld filesz=100000 nR=100000 fldtype=I1 filename=undef tbl=undef h_fld=b d_fld=b"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq list_used_registers"
outstr=`qtils urlencode "$instr"`
curl -s -o _x.html --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

op="=="
instr="gq f1s1opf2 d_fld1=a scalar=12 op=$op d_fld2=b"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

instr="gq f_to_s d_fld1=b op=sum"
outstr=`qtils urlencode "$instr"`
curl --url "localhost:8080/gq?COMMAND=$outstr"
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 
