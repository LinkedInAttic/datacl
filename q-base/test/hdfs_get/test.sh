#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e

#---------------------------------------------------------
rm -r -f tempdir
mkdir tempdir
time qtils hdfs_get rsubramo eat1-magicnn01.grid.linkedin.com 50070 /user/rsubramo/edu_export DIR "I4:I4:I4"  tempdir DIR
ls -al tempdir
rm -r -f  tempdir
#---------------------------------------------------------
rm -f _tempfile
time qtils hdfs_get rsubramo eat1-magicnn01.grid.linkedin.com 50070 /user/rsubramo/edu_export/part-r-00000 FILE ""  _tempfile FILE
ls -l _tempfile
rm -f _tempfile
#---------------------------------------------------------
rm -r -f tempdir
mkdir tempdir
time qtils hdfs_get rsubramo eat1-magicnn01.grid.linkedin.com 50070 /user/rsubramo/edu_export/part-r-00000 FILE "I4:I4:I4"  tempdir DIR
ls -al tempdir
rm -r -f  tempdir

echo ALL DONE for $0 in $PWD
