set -e 
INDIR=/home/rsubramo/DATA/
INFILE=_tempf_a
sz=`stat --format=%s ${INDIR}${INFILE}`
nR=`echo "$sz / 4" | bc `
rm -r -f TEMP; mkdir TEMP
q init
q add_tbl T1 $nR
q add_fld T1 f1 "file=[$INFILE]:fldtype=[I4]:dir=[$INDIR]"
maxval=`q f_to_s T1 f1 max | cut -f 1 -d ":"`
minval=`q f_to_s T1 f1 min | cut -f 1 -d ":"`
echo "maxval = $maxval, minval = $minval ";
if [ $minval -lt 0 ]; then 
  q f1s1opf2 T1 f1 $minval '-' f1 
fi
q dup_fld T1 f1 srt_f2
q dup_fld T1 f1 srt_f3
# echo PREMATURE; exit 1; 
q fop T1 srt_f3 'op=[sort]:order=[asc]'
q fop T1 srt_f2 'op=[sort]:order=[asc]:distribution=[uniform_random]' 
q f1f2opf3 T1 srt_f2 srt_f3 '!=' x
n=`q f_to_s T1 x sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

#--- Repeat but with I8
q f1opf2 T1 f1 'op=[conv]:newtype=[I8]' f1 
q dup_fld T1 f1 srt_f2
q dup_fld T1 f1 srt_f3
q fop T1 srt_f3 'op=[sort]:order=[asc]'
q fop T1 srt_f2 'op=[sort]:order=[asc]:distribution=[uniform_random]' 
q f1f2opf3 T1 srt_f2 srt_f3 '!=' x
n=`q f_to_s T1 x sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

echo "ALL DONE $0 in $PWD"
