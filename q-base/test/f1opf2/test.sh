#/usr/local/bin/bash
set -e 
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q '
export    Q_DOCROOT=$PWD/TEMPM/
export    Q_DSK_DATA_DIR=$PWD/TEMPD/
export    Q_RAM_DATA_DIR=$PWD/TEMPR/
rm -r -f $Q_DOCROOT
rm -r -f $Q_DSK_DATA_DIR
rm -r -f $Q_RAM_DATA_DIR
mkdir    $Q_DOCROOT
mkdir    $Q_DSK_DATA_DIR
mkdir    $Q_RAM_DATA_DIR
q init $Q_DSK_DATA_DIR $Q_RAM_DATA_DIR
#---------------------------------------------
# cleanup
echo "f0,I4,"         > _meta.csv
echo "f1,I8,"         >> _meta.csv
q dld t1 _meta.csv data1.csv ""
q f1opf2 t1 f1 "op=[cum]:newtype=[I4]" f2 
q f1opf2 t1 f1 "op=[shift]:shift=[1]:newval=[9999]"  f2 
q f1opf2 t1 f1 "op=[shift]:shift=[-1]:newval=[8888]" f3
q f1opf2 t1 f2 "op=[cum]:newtype=[I8]" f4
q pr_fld t1 f1:f2:f3:f4
echo "----00000000000000000-------";
q mk_idx t1 idx I8
q pr_fld t1 f0:idx
echo "----00000000000000000-------";

q f1opf2 t1 f2 "op=[cum]:newtype=[I4]" if4
q f1opf2 t1 f2 "op=[cum]:newtype=[I8]" llf5
q pr_fld t1 f2:if4:llf5
echo "----=================-------";

#-------------------------------
# echo PREMATURE; exit 1; 
set +e
q f1opf2 t1 f2   "op=[conv]:newtype=[I8]" f1I8
if [ $? = 0 ]; then echo FAILURE; exit 1; fi 
set -e 
q f1opf2 t1 f2     "op=[conv]:newtype=[F4]" f2F4
q f1opf2 t1 f2     "op=[conv]:newtype=[I4]" f2I4
q f1opf2 t1 f2F4   "op=[conv]:newtype=[I4]" f2F4I4
q f1opf2 t1 f2F4   "op=[conv]:newtype=[F8]" f2F4F8
# NOT READY q f1opf2 t1 f2F4F8 "op=[conv]:newtype=[I8]" f2F4F8I8

# NOT READY q pr_fld t1 f2:f2F4:f2I4:f2F4I4:f2F4F8:f2F4F8I8
q pr_fld t1 f2:f2F4:f2I4:f2F4I4:f2F4F8
echo "----11111111111111111-------";
#--------------

N=2000000
q delete t2
q add_tbl t2 $N
q s_to_f t2 if1 "op=[seq]:start=[1]:incr=[1]:fldtype=[I4]" 
q f1opf2 t2 if1 "op=[++]" if2
q f1opf2 t2 if1 "op=[--]" if3
q pr_fld t2 if1:if2:if3 | head
q pr_fld t2 if1:if2:if3 | tail
#--------------------------------------
q s_to_f t2 llf1 "op=[seq]:start=[1]:incr=[2]:fldtype=[I8]" 
q f1opf2 t2 llf1 "op=[++]" llf2
q f1opf2 t2 llf1 "op=[--]" llf3
q pr_fld t2 llf1:llf2:llf3  | head
q pr_fld t2 llf1:llf2:llf3  | tail
echo "----------------------"

q delete t1
q add_tbl t1 64
q mk_idx t1 f1 I4
q f1s1opf2 t1 f1 8 '%' f2
q f1s1opf2 t1 f2 0 '==' x
q f1opf2   t1 x 'op=[idx_with_reset]' f3
q pr_fld t1 x:f3 | head -18
echo "ALL DONE for $0 in $PWD"

rm -r -f $Q_DOCROOT
rm -r -f $Q_DSK_DATA_DIR
rm -r -f $Q_RAM_DATA_DIR
