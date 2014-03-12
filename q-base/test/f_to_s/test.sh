#/usr/local/bin/bash
set -e
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/TEMPM
export Q_DSK_DATA_DIR=$PWD/TEMPD/
export Q_RAM_DATA_DIR=$PWD/TEMPR/
rm -r -f $Q_DSK_DATA_DIR; mkdir    $Q_DSK_DATA_DIR
rm -r -f $Q_RAM_DATA_DIR; mkdir    $Q_RAM_DATA_DIR
rm -r -f $Q_DOCROOT;      mkdir    $Q_DOCROOT
export Q_RUN_TIME_CHECKS=TRUE
q init "$Q_DSK_DATA_DIR" "$Q_RAM_DATA_DIR"
#---------------------------------------------
# cleanup
q delete t1
echo "fI4,I4,"       > _meta.csv
echo "fI8,I8," >> _meta.csv
echo "fI1,I1,"      >> _meta.csv
q dld t1 _meta.csv data1.csv ""
rslt=`q f_to_s t1 fI4 max`; echo $rslt; 
rslt=`q f_to_s t1 fI4 min`; echo $rslt; 
rslt=`q f_to_s t1 fI4 sum`; echo $rslt; 
echo "============================";
#--------------------------------------
rslt=`q f_to_s t1 fI8 max`; echo $rslt; 
rslt=`q f_to_s t1 fI8 min`; echo $rslt; 
rslt=`q f_to_s t1 fI8 sum`; echo $rslt; 
echo "============================";
#--------------------------------------
rslt=`q f_to_s t1 fI1 max`; echo $rslt; 
rslt=`q f_to_s t1 fI1 min`; echo $rslt; 
rslt=`q f_to_s t1 fI1 sum`; echo $rslt; 
echo "============================";
#--------------------------------------
q f1s1opf2 t1 fI4 3 '>' x
chk=`q f_to_s t1 x 'op=[get_idx]:val=[1]'`
if [ $chk != 3 ]; then echo FAILURE; exit 1; fi 
chk=`q f_to_s t1 x 'op=[get_idx]:val=[0]'`
if [ $chk != 0 ]; then echo FAILURE; exit 1; fi 
chk=`q f_to_s t1 fI4 'op=[get_idx]:val=[4]'`
if [ $chk != 3 ]; then echo FAILURE; exit 1; fi 
#=================================
q add_tbl t2 50 
q mk_idx t2 f1 I8
q f1s1opf2 t2 f1 50 '+' f1 

idx=`q f_to_s t2 f1 'op=[get_idx]:val=[75]'`
if [ $idx -ne 25 ]; then echo FAILURE; exit 1; fi 
idx=`q f_to_s t2 f1 'op=[get_idx]:val=[0]'`
if [ $idx -ge 0 ]; then echo FAILURE; exit 1; fi 
#-----------------------------------------------
q f1opf2 t2 f1 'op=[conv]:newtype=[I4]' f1 
idx=`q f_to_s t2 f1 'op=[get_idx]:val=[75]'`
if [ $idx -ne 25 ]; then echo FAILURE; exit 1; fi 
idx=`q f_to_s t2 f1 'op=[get_idx]:val=[0]'`
if [ $idx -ge 0 ]; then echo FAILURE; exit 1; fi 
#-----------------------------------------------
q f1opf2 t2 f1 'op=[conv]:newtype=[I2]' f1 
idx=`q f_to_s t2 f1 'op=[get_idx]:val=[75]'`
if [ $idx -ne 25 ]; then echo FAILURE; exit 1; fi 
idx=`q f_to_s t2 f1 'op=[get_idx]:val=[0]'`
if [ $idx -ge 0 ]; then echo FAILURE; exit 1; fi 
#-----------------------------------------------
q mk_idx t2 f1 I4
q f1s1opf2 t2 f1 50 '+' f1 
q f1opf2 t2 f1 'op=[conv]:newtype=[I1]' f1 
idx=`q f_to_s t2 f1 'op=[get_idx]:val=[75]'`
if [ $idx -ne 25 ]; then echo FAILURE; exit 1; fi 
idx=`q f_to_s t2 f1 'op=[get_idx]:val=[0]'`
if [ $idx -ge 0 ]; then echo FAILURE; exit 1; fi 
#-----------------------------------------------
val=`q f_to_s t1 fI4 'op=[get_val]:idx=[1]'`
if [ $val != 2 ]; then echo FAILURE: $LINENO; exit 1; fi 
q describe t1
q describe t2
echo "Completed $0 in $PWD"
