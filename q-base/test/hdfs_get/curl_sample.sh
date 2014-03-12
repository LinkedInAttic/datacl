#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e

#---------------------------------------------------------
ODIR=tempdir
tempf=_tempfile
rm -r -f $ODIR
mkdir $ODIR

mode=2
case $mode in 
1) 
user=rsubramo
url=/user/rsubramo/edu_export
flds="I4:I4:I4"
;;
2)
user=rsubramo
url="/jobs/dsciuser/Plato/Q/ProfileCompleteness/Data"
flds="I4:I1:I2:I1:I1:I1:I1:I1:I1:I1:I1:I1:I1:I1"
#      1  2  3  4  5  6  7  8  9 10 11 12 13 14
;;
*)
echo ERROR; exit 1;
;;
esac

time qtils hdfs_get $user eat1-magicnn01.grid.linkedin.com 50070 \
	$url DIR "$flds"  $ODIR DIR
ls -al $ODIR
rm -r -f  $ODIR
#---------------------------------------------------------
rm -f $tempf
time qtils hdfs_get $user eat1-magicnn01.grid.linkedin.com 50070 \
       $url/part-r-00000 FILE ""  $tempf FILE
ls -l $tempf
rm -f $tempf
#---------------------------------------------------------
rm -r -f $ODIR
mkdir $ODIR
time qtils hdfs_get $user eat1-magicnn01.grid.linkedin.com 50070 \
       $url/part-r-00000 FILE "$flds"  $ODIR DIR
ls -al $ODIR
rm -r -f  $ODIR

echo ALL DONE for $0 in $PWD
