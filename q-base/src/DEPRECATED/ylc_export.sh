#!/usr/local/bin/bash
set -e
if [ $# != 3 ]; then echo FAILURE; $LINENO; exit 1; fi 
list=$1
OP_SPEC_FILE=$2
optbl=$3

mtbl=TB # Master Table 
mfld=member # PK of Master Table

test -f $OP_SPEC_FILE
# Get ready for output
q delete $optbl
q delete T_ylc_op
cat $OP_SPEC_FILE | grep -v column | cut -f 1 -d "," | sort | uniq > _ylc_col.csv
echo "descr_hash,str,'is_load=[true]:is_all_def=[true]:is_dict_old=[false]:dict=[lkp_op_col]'" > _meta.csv
q_alt_data_dir=$Q_ALT_DATA_DIR
unset Q_ALT_DATA_DIR
q dld tempt _meta.csv _ylc_col.csv ''
q delete tempt
q pr_fld lkp_op_col idx:text

cat $OP_SPEC_FILE | grep -v column | cut -f 3 -d "," | sort | uniq > _ylc_lbl.csv
echo "descr_hash,str,'is_load=[true]:is_all_def=[true]:is_dict_old=[false]:dict=[lkp_op_lbl]'" > _meta.csv
q dld tempt _meta.csv _ylc_lbl.csv ''
q delete tempt
q pr_fld lkp_op_lbl idx:text
lkp_op_lbl_id=`q is_tbl lkp_op_lbl | cut -f 2 -d ","`


# Find number of segments in the list
num_in_list=`ylc describe $list | wc -l | cut -f 1 -d " "`
# Load output specification
q dld T_ylc_op ylc_op.meta.csv $OP_SPEC_FILE 'ignore_hdr=[true]'

set -e
tbl=T_ylc_op
q dup_fld  $tbl fk_lkp_col ilbl; q set_meta $tbl ilbl dict_tbl_id -1
q dup_fld  $tbl fk_lkp_col icol; q set_meta $tbl icol dict_tbl_id -1
q f1s1opf2 $tbl icol    20 '<<' tmp1
q f1s1opf2 $tbl seg_pos 10 '<<' tmp2
q f1f2opf3 $tbl tmp1 tmp2 '|' tmp3
q f1f2opf3 $tbl tmp3 icol '|' compfld
q delete   $tbl ilbl:icol:tmp1:tmp2:tmp3
# Gather all unique values of compfld (sorted) into tempt1
q dup_fld $tbl compfld tmp1
q fop     $tbl tmp1 sortA
q count_vals $tbl tmp1 '' tempt1 compfld cnt
q delete tempt1 cnt
q delete $tbl tmp1
# Sort compfld/percentage on compfld
q bindmp $tbl compfld:percentage '' _tempf
qtils sortbindmp _tempf II AA 
q binld tempt2 compfld:percentage I4:I4 _tempf
rm -f _tempf
# Add up percentages 
q srt_join tempt2 compfld percentage tempt1 compfld sum_perc sum
q f1s1opf2 tempt1 sum_perc 100 '!=' x
nx=`q f_to_s tempt1 x sum | cut -f 1 -d ":"`
if [ $nx != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
q delete tempt1:tempt2

q dup_fld $tbl seg_pos tmp1
q fop     $tbl tmp1 sortA
q count_vals $tbl tmp1 '' tempt1 val cnt
n=`q get_nR tempt1`
if [ $n != $num_in_list ]; then echo FAILURE: $LINENO; exit 1; fi 
q s_to_f tempt1 idx 'op=[seq]:start=[1]:incr=[1]:fldtype=[I4]'
q f1f2opf3 tempt1 val idx '!=' x 
nx=`q f_to_s tempt1 x sum | cut -f 1 -d ":"`
if [ $nx != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

q delete tempt1 
q delete $tbl tmp1

# Now let's start creating the output table
# First let us get the number of rows in it 
q delete $optbl
nop=`ylc num_in_list $list lmt_ | cut -f 2 -d ":"`
q add_tbl $optbl $nop
#-------------------------------------------------
# Create primary key 
num_segs=`ylc describe $list | wc -l | cut -f 1 -d " "`
segidx=1
q delete tempt
while [ $segidx -le $num_segs ]; do 
  echo "Processing Segment $segidx of Primary Key Column"
  bfld=`ylc describe $list $segidx lmt_ name`
  cfld=temp_cfld
  currtype=`q describe $mtbl $bfld fldtype`
  # TODO Why is currtype = I1 and not B as I would have expected?
  if [ "$currtype" != "I1" ]; then 
    q f1opf2 $mtbl $bfld "op=[conv]:newtype=[I1]" $cfld
  else 
    cfld=$bfld
  fi
  if [ $segidx = 1 ]; then 
    q delete tempt
    q copy_fld $mtbl $mfld $cfld tempt
  else 
    q delete tempt2
    q copy_fld $mtbl $mfld $cfld tempt2
    q app_tbl tempt tempt2
    q delete tempt2
  fi
  q delete $mtbl $cfld 
  segidx=`expr $segidx + 1`
done
q mv_fld tempt $mfld $optbl
q delete tempt

#-------------------------------------------------
num_cols=`q get_nR lkp_op_col`
num_segs=`ylc describe $list | wc -l | cut -f 1 -d " "`
colidx=0
while [ $colidx -lt $num_cols ]; do 
  segidx=1
  q delete tempt
  while [ $segidx -le $num_segs ]; do 
    echo "Processing Segment $segidx of Output Column $colidx"
    n=`ylc describe $list $segidx lmt_ cnt`
    echo "Creating column with $n rows";
    #----------------------------------------------
    q dup_fld $tbl fk_lkp_lbl ilbl
    q dup_fld $tbl fk_lkp_col icol
    q set_meta $tbl ilbl dict_tbl_id -1
    q set_meta $tbl icol dict_tbl_id -1
    q f1s1opf2 $tbl icol    $colidx '==' x1
    q f1s1opf2 $tbl seg_pos $segidx '==' x2
    q f1f2opf3 $tbl x1 x2 '&&' x
    tempf=_tempf
    q pr_fld $tbl ilbl:percentage 'cond=[x]' > $tempf
    qtils replace_char $tempf bslashn ":"
    nbytes=`stat $tempf --printf=%s`
    nbytes=`expr $nbytes - 1 `
    truncate -s $nbytes $tempf
    x=`cat $tempf`
    if [ $segidx = 1 ]; then 
      q add_tbl tempt $n
      echo "x = [$x]"
      q s_to_f tempt  lbl "op=[proportional]:proportions=[$x]:fldtype=[I4]"
    else 
      q add_tbl tempt2 $n
      q s_to_f tempt2 lbl "op=[proportional]:proportions=[$x]:fldtype=[I4]"
      q app_tbl tempt tempt2
      q delete tempt2
    fi
    rm -f $tempf
    #----------------------------------------------
    segidx=`expr $segidx + 1`
  done
  nR=`q get_nR tempt`
  echo "Created tempt with $nR rows "
  q rename tempt lbl lbl_$colidx
  q mv_fld tempt lbl_$colidx $optbl 
  q set_meta $optbl lbl_$colidx dict_tbl_id $lkp_op_lbl_id
  q delete tempt

  colidx=`expr $colidx + 1`
done
echo "ALL DONE"
