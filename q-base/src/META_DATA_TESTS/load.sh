#/usr/local/bin/bash
set -e 

if [ "$Q_DOCROOT"  = "" ]; then echo FAILURE: $LINENO; exit 1; fi 
if [ "$Q_DATA_DIR" = "" ]; then echo FAILURE: $LINENO; exit 1; fi 
test -d $Q_DOCROOT
test -d $Q_DATA_DIR
TST_DOCROOT=$Q_DOCROOT/debug
TST_DATA_DIR=$Q_DATA_DIR/debug
rm -r -f  $TST_DOCROOT
mkdir $TST_DOCROOT
if [ "$TST_DOCROOT" != "$TST_DATA_DIR" ]; then 
  rm -r -f  $TST_DATA_DIR
  mkdir  $TST_DATA_DIR
fi

cd $TST_DATA_DIR
q dump _tbl.csv _fld.csv _fld_info.csv "mc_readable=[true]"

export Q_DOCROOT=$TST_DOCROOT
export Q_DATA_DIR=$TST_DATA_DIR
q init
rm -f _tbl_meta.csv
echo "tbl_id,I2," >> _tbl_meta.csv
echo "xtbltype,LKP,'is_dict_old=[false]:dict=[lkp_tbltype]" >> _tbl_meta.csv
echo "xname,LKP,'is_dict_old=[false]:dict=[lkp_tbls]" >> _tbl_meta.csv
echo "nR,I8,"     >> _tbl_meta.csv
q dld Ttbls _tbl_meta.csv _tbl.csv 'ignore_hdr=[true]'

#----------------------------------------------------
rm -f _meta.csv
echo "xxx,SV,'is_dict_old=[false]:dict=[lkp_srttype]" >> _meta.csv
rm -f _data.csv
echo "unknown" >> _data.csv
echo "ascending" >> _data.csv
echo "descending" >> _data.csv
echo "unsorted" >> _data.csv
q dld tempt _meta.csv _data.csv ''
q delete tempt
#-----------------------------------------------------
rm -f _meta.csv _data.csv
echo "xxx,SV,'is_dict_old=[false]:dict=[lkp_fldtype]" >> _meta.csv
echo "undef_fldtype" >> _data.csv
echo "B" >> _data.csv
echo "I1" >> _data.csv
echo "I2" >> _data.csv
echo "I4" >> _data.csv
echo "I8" >> _data.csv
echo "F4" >> _data.csv
echo "F8" >> _data.csv
echo "SC" >> _data.csv
echo "SV" >> _data.csv
q dld tempt _meta.csv _data.csv ''
q delete tempt
#-----------------------------------------------------
rm -f _meta.csv _data.csv
echo "xxx,SV,'is_dict_old=[false]:dict=[lkp_auxtype]" >> _meta.csv
echo "primary" >> _data.csv
echo "nn"            >> _data.csv
q dld tempt _meta.csv _data.csv ''
q delete tempt
#-----------------------------------------------------
rm -f _fld_meta.csv
echo "id,I4,"           >> _fld_meta.csv
echo "tbl_id,I4,"       >> _fld_meta.csv
echo "ddir_id,I4,"      >> _fld_meta.csv
echo "parent_id,I4,"    >> _fld_meta.csv
echo "nn_fld_id,I4,"    >> _fld_meta.csv
echo "len,I4,"          >> _fld_meta.csv
echo "dict_tbl_id,I4,"  >> _fld_meta.csv
echo "len_fld_id,I4,"   >> _fld_meta.csv
echo "off_fld_id,I4,"   >> _fld_meta.csv
echo "filesz,I8,"       >> _fld_meta.csv
echo "is_external,I1,"  >> _fld_meta.csv
echo "srttype,SV,is_dict_old=[true]:dict=[lkp_srttype]" >> _fld_meta.csv
echo "fldtype,SV,is_dict_old=[true]:dict=[lkp_fldtype]" >> _fld_meta.csv
echo "auxtype,SV,is_dict_old=[true]:dict=[lkp_auxtype]" >> _fld_meta.csv
echo "sumI8,I8,"        >> _fld_meta.csv
echo "minI8,I8,"        >> _fld_meta.csv
echo "maxI8,I8,"        >> _fld_meta.csv
echo "sumF8,F8,"        >> _fld_meta.csv
echo "minF8,F8,"        >> _fld_meta.csv
echo "maxF8,F8,"        >> _fld_meta.csv
echo "is_sum_nn,I1,"    >> _fld_meta.csv
echo "is_min_nn,I1,"    >> _fld_meta.csv
echo "is_max_nn,I1,"    >> _fld_meta.csv
echo "xxx,LKP,is_dict_old=[false]:dict=[lkp_flds]"         >> _fld_meta.csv
echo "fileno,I4,"       >> _fld_meta.csv

q dld Tflds _fld_meta.csv _fld.csv 'ignore_hdr=[true]'
cd -

# TODO P2 Should not need to do following cleanup. Should be done by dld
rslt=`q is_nn_fld Tflds fk_lkp_flds | cut -f 1 -d ":"`
if [ $rslt = 1 ]; then 
  n1=`q f_to_s Tflds .nn.fk_lkp_flds sum | cut -f 1 -d ":"`
  n2=`q f_to_s Tflds .nn.fk_lkp_flds sum | cut -f 2 -d ":"`
  if [ $n1 != $n2 ]; then echo FAILURE: exit 1; fi 
  q drop_nn_fld Tflds fk_lkp_flds
fi

