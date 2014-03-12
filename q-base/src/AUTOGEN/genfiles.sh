#!/bin/sh
set -e 
if [ $# != 2 ] && [ $# != 1 ]; then echo "Usage is $0: <OUTDIR> "; exit 1; fi
OUTDIR="$1"
if [ $# = 1 ]; then 
  COMPILER=gcc
else 
  COMPILER=$2
fi
if [ "$OUTDIR" != "" ]; then
  rm -r -f $OUTDIR;
  mkdir $OUTDIR;
  test -d $OUTDIR
fi
COMMONFLAGS=" -c -Wall -pedantic "
if [ "$COMPILER" = "icc" ]; then 
#  COMPILER="icc -O3 -vec-report=6 -DICC -DCILK"
  COMPILER="icc -O3 -DICC -DCILK -ipp -std=c99"
fi
if [ "$COMPILER" = "gcc" ]; then 
  COMPILER="gcc -fopenmp -std=gnu99"
fi

foo()
{
  label=$1;
  srcfile=$2;
  replfile=$3;
  outdir=$4
  outfile=$5

  set +e
  test -f $srcfile
  if [ $? != 0 ]; then echo "source file $srcfile not found "; exit 1; fi 
  test -f $replfile
  if [ $? != 0 ]; then echo "replacement file $replfile not found "; exit 1; fi 
   
  php subs.php $srcfile $label $replfile "$outdir" "$outfile"
  if [ "$outfile" == "" ]; then 
    opfile=`echo $srcfile | sed s"/tmpl/$label/"g `;
  else
    opfile=$outfile
  fi
  $COMPILER $COMMONFLAGS $outdir/$outfile -I..  -I./GENFILES/
  if [ $? != 0 ]; then echo FAILURE on $outfile; exit 1; fi

  php mk_hdrs.php "$outdir" $opfile;
}
#---------------------------------------------------
replfile=repl3.csv
srcfile=tmpl_assign.c
foo assign_I1 $srcfile $replfile "$OUTDIR" assign_I1.c
foo assign_I2 $srcfile $replfile "$OUTDIR" assign_I2.c
foo assign_I4 $srcfile $replfile "$OUTDIR" assign_I4.c
foo assign_I8 $srcfile $replfile "$OUTDIR" assign_I8.c
foo assign_F4 $srcfile $replfile "$OUTDIR" assign_F4.c
foo assign_F8 $srcfile $replfile "$OUTDIR" assign_F8.c

replfile=repl.csv 
srcfile=tmpl_find_smallest_geq.c
foo find_smallest_geq_I4 $srcfile $replfile $OUTDIR find_smallest_geq_I4.c
foo find_smallest_geq_I8 $srcfile $replfile $OUTDIR find_smallest_geq_I8.c

replfile=repl7.csv 
srcfile=tmpl_qsort.c
foo qsort_asc_I1 $srcfile $replfile "$OUTDIR" qsort_asc_I1.c
foo F0000 $srcfile $replfile "$OUTDIR" qsort_asc_I4.c
foo F0001 $srcfile $replfile "$OUTDIR" qsort_dsc_I4.c
foo F0002 $srcfile $replfile "$OUTDIR" qsort_asc_I8.c
foo F0003 $srcfile $replfile "$OUTDIR" qsort_dsc_I8.c


replfile=repl.csv 
srcfile=tmpl_par_sort_with_bins.c
foo par_sort_with_bins_I4 $srcfile $replfile $OUTDIR par_sort_with_bins_I4.c
foo par_sort_with_bins_I8 $srcfile $replfile $OUTDIR par_sort_with_bins_I8.c

replfile=repl.csv 
srcfile=tmpl_core_patmatch.c
foo patmatch_I1 $srcfile $replfile $OUTDIR patmatch_I1.c
foo patmatch_I2 $srcfile $replfile $OUTDIR patmatch_I2.c
foo patmatch_I4 $srcfile $replfile $OUTDIR patmatch_I4.c

replfile=repl.csv 
srcfile=tmpl_is_s_in_f_seq.c
foo is_s_in_f_seq_I1 $srcfile $replfile $OUTDIR is_s_in_f_seq_I1.c
foo is_s_in_f_seq_I2 $srcfile $replfile $OUTDIR is_s_in_f_seq_I2.c
foo is_s_in_f_seq_I4 $srcfile $replfile $OUTDIR is_s_in_f_seq_I4.c
foo is_s_in_f_seq_I8 $srcfile $replfile $OUTDIR is_s_in_f_seq_I8.c

replfile=repl.csv 
srcfile=tmpl_union.c
foo f_to_s_sum_I4  $srcfile $replfile $OUTDIR union_I4.c
foo f_to_s_sum_I8  $srcfile $replfile $OUTDIR union_I8.c

replfile=repl.csv 
srcfile=tmpl_a_minus_b.c
foo f_to_s_sum_I4  $srcfile $replfile $OUTDIR a_minus_b_I4.c
foo f_to_s_sum_I8  $srcfile $replfile $OUTDIR a_minus_b_I8.c

replfile=repl.csv 
srcfile=tmpl_intersection.c
foo f_to_s_sum_I4  $srcfile $replfile $OUTDIR intersection_I4.c
foo f_to_s_sum_I8  $srcfile $replfile $OUTDIR intersection_I8.c

replfile=repl_f1s1s2opf2.csv
srcfile=tmpl_f1s1s2opf2_cond.c
foo LTORGT  $srcfile $replfile "$OUTDIR" cmp_lt_or_gt_I4.c
foo LEORGE  $srcfile $replfile "$OUTDIR" cmp_le_or_ge_I4.c
foo GTANDLT $srcfile $replfile "$OUTDIR" cmp_gt_and_lt_I4.c
foo GEANDLE $srcfile $replfile "$OUTDIR" cmp_ge_and_le_I4.c

foo cmp_lt_or_gt_I2  $srcfile $replfile "$OUTDIR" cmp_lt_or_gt_I2.c
foo cmp_le_or_ge_I2  $srcfile $replfile "$OUTDIR" cmp_le_or_ge_I2.c
foo cmp_gt_and_lt_I2 $srcfile $replfile "$OUTDIR" cmp_gt_and_lt_I2.c
foo cmp_ge_and_le_I2 $srcfile $replfile "$OUTDIR" cmp_ge_and_le_I2.c

foo cmp_lt_or_gt_I1  $srcfile $replfile "$OUTDIR" cmp_lt_or_gt_I1.c
foo cmp_le_or_ge_I1  $srcfile $replfile "$OUTDIR" cmp_le_or_ge_I1.c
foo cmp_gt_and_lt_I1 $srcfile $replfile "$OUTDIR" cmp_gt_and_lt_I1.c
foo cmp_ge_and_le_I1 $srcfile $replfile "$OUTDIR" cmp_ge_and_le_I1.c

replfile=repl.csv 
srcfile=tmpl_s_to_f_const.c
foo s_to_f_const_I1 $srcfile $replfile "$OUTDIR" s_to_f_const_I1.c
foo s_to_f_const_I2 $srcfile $replfile "$OUTDIR" s_to_f_const_I2.c
foo s_to_f_const_I4 $srcfile $replfile "$OUTDIR" s_to_f_const_I4.c
foo s_to_f_const_I8 $srcfile $replfile "$OUTDIR" s_to_f_const_I8.c
foo s_to_f_const_F4 $srcfile $replfile "$OUTDIR" s_to_f_const_F4.c

replfile=repl.csv 
srcfile=tmpl_s_to_f_seq.c
foo s_to_f_seq_I1 $srcfile $replfile "$OUTDIR" s_to_f_seq_I1.c
foo s_to_f_seq_I2 $srcfile $replfile "$OUTDIR" s_to_f_seq_I2.c
foo s_to_f_seq_I4 $srcfile $replfile "$OUTDIR" s_to_f_seq_I4.c
foo s_to_f_seq_I8 $srcfile $replfile "$OUTDIR" s_to_f_seq_I8.c

replfile=repl.csv 
srcfile=tmpl_extract_I.c
foo extract_I1  $srcfile $replfile $OUTDIR extract_I1.c
foo extract_I2  $srcfile $replfile $OUTDIR extract_I2.c
foo extract_I4  $srcfile $replfile $OUTDIR extract_I4.c
foo extract_I8  $srcfile $replfile $OUTDIR extract_I8.c
foo extract_UI8  $srcfile $replfile $OUTDIR extract_UI8.c

replfile=repl.csv 
srcfile=tmpl_pr_fld.c
foo pr_fld_I1  $srcfile $replfile $OUTDIR pr_fld_I1.c
foo pr_fld_I2  $srcfile $replfile $OUTDIR pr_fld_I2.c
foo pr_fld_I4  $srcfile $replfile $OUTDIR pr_fld_I4.c
foo pr_fld_I8  $srcfile $replfile $OUTDIR pr_fld_I8.c
foo pr_fld_F4  $srcfile $replfile $OUTDIR pr_fld_F4.c
foo pr_fld_F8  $srcfile $replfile $OUTDIR pr_fld_F8.c

replfile=repl.csv 
srcfile=tmpl_f_to_s.c
foo f_to_s_min_I1  $srcfile $replfile $OUTDIR f_to_s_min_I1.c
foo f_to_s_min_I2  $srcfile $replfile $OUTDIR f_to_s_min_I2.c
foo f_to_s_min_I4  $srcfile $replfile $OUTDIR f_to_s_min_I4.c
foo f_to_s_min_I8  $srcfile $replfile $OUTDIR f_to_s_min_I8.c
foo f_to_s_min_F4  $srcfile $replfile $OUTDIR f_to_s_min_F4.c
foo f_to_s_min_F8  $srcfile $replfile $OUTDIR f_to_s_min_F8.c

replfile=repl.csv 
srcfile=tmpl_nn_f_to_s.c
foo f_to_s_min_I1  $srcfile $replfile $OUTDIR nn_f_to_s_min_I1.c
foo f_to_s_min_I2  $srcfile $replfile $OUTDIR nn_f_to_s_min_I2.c
foo f_to_s_min_I4  $srcfile $replfile $OUTDIR nn_f_to_s_min_I4.c
foo f_to_s_min_I8  $srcfile $replfile $OUTDIR nn_f_to_s_min_I8.c
foo f_to_s_min_F4  $srcfile $replfile $OUTDIR nn_f_to_s_min_F4.c
foo f_to_s_min_F8  $srcfile $replfile $OUTDIR nn_f_to_s_min_F8.c

replfile=repl.csv 
srcfile=tmpl_f_to_s.c
foo f_to_s_max_I1  $srcfile $replfile $OUTDIR f_to_s_max_I1.c
foo f_to_s_max_I2  $srcfile $replfile $OUTDIR f_to_s_max_I2.c
foo f_to_s_max_I4  $srcfile $replfile $OUTDIR f_to_s_max_I4.c
foo f_to_s_max_I8  $srcfile $replfile $OUTDIR f_to_s_max_I8.c
foo f_to_s_max_F4  $srcfile $replfile $OUTDIR f_to_s_max_F4.c
foo f_to_s_max_F8  $srcfile $replfile $OUTDIR f_to_s_max_F8.c

replfile=repl.csv 
srcfile=tmpl_nn_f_to_s.c
foo f_to_s_max_I1  $srcfile $replfile $OUTDIR nn_f_to_s_max_I1.c
foo f_to_s_max_I2  $srcfile $replfile $OUTDIR nn_f_to_s_max_I2.c
foo f_to_s_max_I4  $srcfile $replfile $OUTDIR nn_f_to_s_max_I4.c
foo f_to_s_max_I8  $srcfile $replfile $OUTDIR nn_f_to_s_max_I8.c
foo f_to_s_max_F4  $srcfile $replfile $OUTDIR nn_f_to_s_max_F4.c
foo f_to_s_max_F8  $srcfile $replfile $OUTDIR nn_f_to_s_max_F8.c

replfile=repl.csv 
srcfile=tmpl_f_to_s_sum.c
foo f_to_s_sum_I1  $srcfile $replfile $OUTDIR f_to_s_sum_I1.c
foo f_to_s_sum_I2  $srcfile $replfile $OUTDIR f_to_s_sum_I2.c
foo f_to_s_sum_I4  $srcfile $replfile $OUTDIR f_to_s_sum_I4.c
foo f_to_s_sum_I8  $srcfile $replfile $OUTDIR f_to_s_sum_I8.c
foo f_to_s_sum_F4  $srcfile $replfile $OUTDIR f_to_s_sum_F4.c
foo f_to_s_sum_F8  $srcfile $replfile $OUTDIR f_to_s_sum_F8.c

replfile=repl.csv 
srcfile=tmpl_nn_f_to_s_sum.c
foo f_to_s_sum_I1  $srcfile $replfile $OUTDIR nn_f_to_s_sum_I1.c
foo f_to_s_sum_I2  $srcfile $replfile $OUTDIR nn_f_to_s_sum_I2.c
foo f_to_s_sum_I4  $srcfile $replfile $OUTDIR nn_f_to_s_sum_I4.c
foo f_to_s_sum_I8  $srcfile $replfile $OUTDIR nn_f_to_s_sum_I8.c
foo f_to_s_sum_F4  $srcfile $replfile $OUTDIR nn_f_to_s_sum_F4.c
foo f_to_s_sum_F8  $srcfile $replfile $OUTDIR nn_f_to_s_sum_F8.c

replfile=repl.csv 
srcfile=tmpl_xfer.c
foo xfer_I1_I4  $srcfile $replfile $OUTDIR xfer_I1_I4.c
foo xfer_I2_I4  $srcfile $replfile $OUTDIR xfer_I2_I4.c
foo xfer_I4_I4  $srcfile $replfile $OUTDIR xfer_I4_I4.c
foo xfer_I8_I4  $srcfile $replfile $OUTDIR xfer_I8_I4.c
foo xfer_F4_I4  $srcfile $replfile $OUTDIR xfer_F4_I4.c
foo xfer_F8_I4  $srcfile $replfile $OUTDIR xfer_F8_I4.c

replfile=repl6.csv 
srcfile=tmpl_sort_1.c
foo sort_asc_I4 $srcfile $replfile "$OUTDIR" sort_asc_I4.c
foo sort_dsc_I4 $srcfile $replfile "$OUTDIR" sort_dsc_I4.c
foo sort_asc_I8 $srcfile $replfile "$OUTDIR" sort_asc_I8.c
foo sort_dsc_I8 $srcfile $replfile "$OUTDIR" sort_dsc_I8.c
foo sort_asc_F4 $srcfile $replfile "$OUTDIR" sort_asc_F4.c
foo sort_dsc_F4 $srcfile $replfile "$OUTDIR" sort_dsc_F4.c
foo sort_asc_F8 $srcfile $replfile "$OUTDIR" sort_asc_F8.c
foo sort_dsc_F8 $srcfile $replfile "$OUTDIR" sort_dsc_F8.c

replfile=repl6.csv 
srcfile=tmpl_sort_2.c
foo sort_asc_I4 $srcfile $replfile "$OUTDIR" sort2_asc_I4.c
foo sort_dsc_I4 $srcfile $replfile "$OUTDIR" sort2_dsc_I4.c
foo sort_asc_I8 $srcfile $replfile "$OUTDIR" sort2_asc_I8.c
foo sort_dsc_I8 $srcfile $replfile "$OUTDIR" sort2_dsc_I8.c
foo sort_asc_F4 $srcfile $replfile "$OUTDIR" sort2_asc_F4.c
foo sort_dsc_F4 $srcfile $replfile "$OUTDIR" sort2_dsc_F4.c

replfile=repl6.csv 
srcfile=tmpl_sort_3.c
foo sort_asc_I4 $srcfile $replfile "$OUTDIR" sort3_asc_I4.c
foo sort_dsc_I4 $srcfile $replfile "$OUTDIR" sort3_dsc_I4.c
foo sort_asc_I8 $srcfile $replfile "$OUTDIR" sort3_asc_I8.c
foo sort_dsc_I8 $srcfile $replfile "$OUTDIR" sort3_dsc_I8.c
foo sort_asc_F4 $srcfile $replfile "$OUTDIR" sort3_asc_F4.c
foo sort_dsc_F4 $srcfile $replfile "$OUTDIR" sort3_dsc_F4.c

replfile=repl6.csv 
srcfile=tmpl_sort_4.c
foo sort_asc_I4 $srcfile $replfile "$OUTDIR" sort4_asc_I4.c
foo sort_dsc_I4 $srcfile $replfile "$OUTDIR" sort4_dsc_I4.c
foo sort_asc_I8 $srcfile $replfile "$OUTDIR" sort4_asc_I8.c
foo sort_dsc_I8 $srcfile $replfile "$OUTDIR" sort4_dsc_I8.c
foo sort_asc_F4 $srcfile $replfile "$OUTDIR" sort4_asc_F4.c
foo sort_dsc_F4 $srcfile $replfile "$OUTDIR" sort4_dsc_F4.c

replfile=repl7.csv 
srcfile=tmpl_qsort_with_idx.c
foo E0000 $srcfile $replfile "$OUTDIR" qsort_with_idx_int.c
foo E0002 $srcfile $replfile "$OUTDIR" qsort_with_idx_longlong.c

replfile=repl7.csv 
srcfile=tmpl_qsort_nofn_yesidx.c
foo G0000 $srcfile $replfile "$OUTDIR" qsort_asc_val_I4_idx_I4.c
foo G0001 $srcfile $replfile "$OUTDIR" qsort_asc_val_I4_idx_I8.c
foo G0002 $srcfile $replfile "$OUTDIR" qsort_asc_val_I8_idx_I4.c
foo G0003 $srcfile $replfile "$OUTDIR" qsort_asc_val_I8_idx_I8.c

foo G0004 $srcfile $replfile "$OUTDIR" qsort_dsc_val_I4_idx_I4.c
foo G0005 $srcfile $replfile "$OUTDIR" qsort_dsc_val_I4_idx_I8.c
foo G0006 $srcfile $replfile "$OUTDIR" qsort_dsc_val_I8_idx_I4.c
foo G0007 $srcfile $replfile "$OUTDIR" qsort_dsc_val_I8_idx_I8.c

#---------------------------------------------------
replfile=repl8.csv
srcfile=tmpl_core_srt_join_X_X_X.c
foo I4_I4_I4 $srcfile $replfile "$OUTDIR" core_srt_join_I4_I4_I4.c
foo I4_I4_I8 $srcfile $replfile "$OUTDIR" core_srt_join_I4_I4_I8.c
foo I4_I8_I4 $srcfile $replfile "$OUTDIR" core_srt_join_I4_I8_I4.c
foo I4_I8_I8 $srcfile $replfile "$OUTDIR" core_srt_join_I4_I8_I8.c
foo I8_I4_I4 $srcfile $replfile "$OUTDIR" core_srt_join_I8_I4_I4.c
foo I8_I4_I8 $srcfile $replfile "$OUTDIR" core_srt_join_I8_I4_I8.c
foo I8_I8_I4 $srcfile $replfile "$OUTDIR" core_srt_join_I8_I8_I4.c
foo I8_I8_I8 $srcfile $replfile "$OUTDIR" core_srt_join_I8_I8_I8.c
#------------------------------------------------
replfile=repl10.csv
srcfile=tmpl_is_sorted.c
foo issrtI1 $srcfile $replfile "$OUTDIR" is_sorted_alldef_I1.c
foo issrtI2 $srcfile $replfile "$OUTDIR" is_sorted_alldef_I2.c
foo issrtI4 $srcfile $replfile "$OUTDIR" is_sorted_alldef_I4.c
foo issrtI8 $srcfile $replfile "$OUTDIR" is_sorted_alldef_I8.c

replfile=repl2.csv
srcfile=tmpl_f1f2opf3_cond.c
foo BI100 $srcfile $replfile "$OUTDIR"  cmp_ge_I1.c
foo BI101 $srcfile $replfile "$OUTDIR"  cmp_le_I1.c
foo BI102 $srcfile $replfile "$OUTDIR"  cmp_eq_I1.c
foo BI103 $srcfile $replfile "$OUTDIR"  cmp_ne_I1.c
foo BI104 $srcfile $replfile "$OUTDIR"  cmp_gt_I1.c
foo BI105 $srcfile $replfile "$OUTDIR"  cmp_lt_I1.c
#--------
foo B0000 $srcfile $replfile "$OUTDIR"  cmp_ge_I4.c
foo B0001 $srcfile $replfile "$OUTDIR"  cmp_le_I4.c
foo B0002 $srcfile $replfile "$OUTDIR"  cmp_eq_I4.c
foo B0003 $srcfile $replfile "$OUTDIR"  cmp_ne_I4.c
foo B0004 $srcfile $replfile "$OUTDIR"  cmp_gt_I4.c
foo B0005 $srcfile $replfile "$OUTDIR"  cmp_lt_I4.c
#--------
foo B0040 $srcfile $replfile "$OUTDIR"  cmp_ge_F4.c
foo B0041 $srcfile $replfile "$OUTDIR"  cmp_le_F4.c
foo B0042 $srcfile $replfile "$OUTDIR"  cmp_eq_F4.c
foo B0043 $srcfile $replfile "$OUTDIR"  cmp_ne_F4.c
foo B0044 $srcfile $replfile "$OUTDIR"  cmp_gt_F4.c
foo B0045 $srcfile $replfile "$OUTDIR"  cmp_lt_F4.c
#--------
foo cmp_ge_F8 $srcfile $replfile "$OUTDIR"  cmp_ge_F8.c
foo cmp_le_F8 $srcfile $replfile "$OUTDIR"  cmp_le_F8.c
foo cmp_eq_F8 $srcfile $replfile "$OUTDIR"  cmp_eq_F8.c
foo cmp_ne_F8 $srcfile $replfile "$OUTDIR"  cmp_ne_F8.c
foo cmp_gt_F8 $srcfile $replfile "$OUTDIR"  cmp_gt_F8.c
foo cmp_lt_F8 $srcfile $replfile "$OUTDIR"  cmp_lt_F8.c
#--------
foo B0010 $srcfile $replfile "$OUTDIR"  cmp_ge_I8.c
foo B0011 $srcfile $replfile "$OUTDIR"  cmp_le_I8.c
foo B0012 $srcfile $replfile "$OUTDIR"  cmp_eq_I8.c
foo B0013 $srcfile $replfile "$OUTDIR"  cmp_ne_I8.c
foo B0014 $srcfile $replfile "$OUTDIR"  cmp_gt_I8.c
foo B0015 $srcfile $replfile "$OUTDIR"  cmp_lt_I8.c

replfile=repl2s.csv
srcfile=tmpl_f1s1opf2_cond.c
foo BS0000 $srcfile $replfile "$OUTDIR" cmp_ge_scalar_I4.c
foo BS0001 $srcfile $replfile "$OUTDIR" cmp_le_scalar_I4.c
foo BS0002 $srcfile $replfile "$OUTDIR" cmp_eq_scalar_I4.c
foo BS0003 $srcfile $replfile "$OUTDIR" cmp_ne_scalar_I4.c
foo BS0004 $srcfile $replfile "$OUTDIR" cmp_gt_scalar_I4.c
foo BS0005 $srcfile $replfile "$OUTDIR" cmp_lt_scalar_I4.c
#--------
foo BS0010 $srcfile $replfile "$OUTDIR" cmp_ge_scalar_I8.c
foo BS0011 $srcfile $replfile "$OUTDIR" cmp_le_scalar_I8.c
foo BS0012 $srcfile $replfile "$OUTDIR" cmp_eq_scalar_I8.c
foo BS0013 $srcfile $replfile "$OUTDIR" cmp_ne_scalar_I8.c
foo BS0014 $srcfile $replfile "$OUTDIR" cmp_gt_scalar_I8.c
foo BS0015 $srcfile $replfile "$OUTDIR" cmp_lt_scalar_I8.c
#--------
foo BS0020 $srcfile $replfile "$OUTDIR" cmp_ge_scalar_F4.c
foo BS0021 $srcfile $replfile "$OUTDIR" cmp_le_scalar_F4.c
foo BS0022 $srcfile $replfile "$OUTDIR" cmp_eq_scalar_F4.c
foo BS0023 $srcfile $replfile "$OUTDIR" cmp_ne_scalar_F4.c
foo BS0024 $srcfile $replfile "$OUTDIR" cmp_gt_scalar_F4.c
foo BS0025 $srcfile $replfile "$OUTDIR" cmp_lt_scalar_F4.c
#--------
foo cmp_ge_scalar_F8 $srcfile $replfile "$OUTDIR" cmp_ge_scalar_F8.c
foo cmp_le_scalar_F8 $srcfile $replfile "$OUTDIR" cmp_le_scalar_F8.c
foo cmp_eq_scalar_F8 $srcfile $replfile "$OUTDIR" cmp_eq_scalar_F8.c
foo cmp_ne_scalar_F8 $srcfile $replfile "$OUTDIR" cmp_ne_scalar_F8.c
foo cmp_gt_scalar_F8 $srcfile $replfile "$OUTDIR" cmp_gt_scalar_F8.c
foo cmp_lt_scalar_F8 $srcfile $replfile "$OUTDIR" cmp_lt_scalar_F8.c
#--------
foo BS0030 $srcfile $replfile "$OUTDIR" cmp_ge_scalar_I1.c
foo BS0031 $srcfile $replfile "$OUTDIR" cmp_le_scalar_I1.c
foo BS0032 $srcfile $replfile "$OUTDIR" cmp_eq_scalar_I1.c
foo BS0033 $srcfile $replfile "$OUTDIR" cmp_ne_scalar_I1.c
foo BS0034 $srcfile $replfile "$OUTDIR" cmp_gt_scalar_I1.c
foo BS0035 $srcfile $replfile "$OUTDIR" cmp_lt_scalar_I1.c
#--------
foo cmp_eq_scalar_I2 $srcfile $replfile "$OUTDIR" cmp_eq_scalar_I2.c
foo cmp_ne_scalar_I2 $srcfile $replfile "$OUTDIR" cmp_ne_scalar_I2.c
foo cmp_gt_scalar_I2 $srcfile $replfile "$OUTDIR" cmp_gt_scalar_I2.c
foo cmp_lt_scalar_I2 $srcfile $replfile "$OUTDIR" cmp_lt_scalar_I2.c
foo cmp_ge_scalar_I2 $srcfile $replfile "$OUTDIR" cmp_ge_scalar_I2.c
foo cmp_le_scalar_I2 $srcfile $replfile "$OUTDIR" cmp_le_scalar_I2.c

replfile=repl1s.csv
srcfile=tmpl_f1s1opf2_arith.c
foo add_scalar_I1 $srcfile $replfile "$OUTDIR" add_scalar_I1.c
foo sub_scalar_I1 $srcfile $replfile "$OUTDIR" sub_scalar_I1.c
foo mul_scalar_I1 $srcfile $replfile "$OUTDIR" mul_scalar_I1.c
foo div_scalar_I1 $srcfile $replfile "$OUTDIR" div_scalar_I1.c
foo rem_scalar_I1 $srcfile $replfile "$OUTDIR" rem_scalar_I1.c
#--------
foo AS0000 $srcfile $replfile "$OUTDIR" add_scalar_I4.c
foo add_scalar_I2 $srcfile $replfile "$OUTDIR" add_scalar_I2.c
foo sub_scalar_I2 $srcfile $replfile "$OUTDIR" sub_scalar_I2.c
foo mul_scalar_I2 $srcfile $replfile "$OUTDIR" mul_scalar_I2.c
foo div_scalar_I2 $srcfile $replfile "$OUTDIR" div_scalar_I2.c
foo rem_scalar_I2 $srcfile $replfile "$OUTDIR" rem_scalar_I2.c
#--------
foo AS0000 $srcfile $replfile "$OUTDIR" add_scalar_I4.c
foo AS0001 $srcfile $replfile "$OUTDIR" sub_scalar_I4.c
foo AS0002 $srcfile $replfile "$OUTDIR" mul_scalar_I4.c
foo AS0003 $srcfile $replfile "$OUTDIR" div_scalar_I4.c
foo AS0004 $srcfile $replfile "$OUTDIR" rem_scalar_I4.c
#--------
foo AS0005 $srcfile $replfile "$OUTDIR" add_scalar_I8.c
foo AS0006 $srcfile $replfile "$OUTDIR" sub_scalar_I8.c
foo AS0007 $srcfile $replfile "$OUTDIR" mul_scalar_I8.c
foo AS0008 $srcfile $replfile "$OUTDIR" div_scalar_I8.c
foo AS0009 $srcfile $replfile "$OUTDIR" rem_scalar_I8.c
#-----------
foo AS0010 $srcfile $replfile "$OUTDIR" add_scalar_F4.c
foo AS0011 $srcfile $replfile "$OUTDIR" sub_scalar_F4.c
foo AS0012 $srcfile $replfile "$OUTDIR" mul_scalar_F4.c
foo AS0013 $srcfile $replfile "$OUTDIR" div_scalar_F4.c
#--------
foo add_scalar_F8 $srcfile $replfile "$OUTDIR" add_scalar_F8.c
foo sub_scalar_F8 $srcfile $replfile "$OUTDIR" sub_scalar_F8.c
foo mul_scalar_F8 $srcfile $replfile "$OUTDIR" mul_scalar_F8.c
foo div_scalar_F8 $srcfile $replfile "$OUTDIR" div_scalar_F8.c
#--------
foo AS0015 $srcfile $replfile "$OUTDIR" and_scalar_I1.c
foo AS0016 $srcfile $replfile "$OUTDIR" or_scalar_I1.c

#------------------------------
foo AS0050 $srcfile $replfile "$OUTDIR" shift_left_scalar_I4.c
foo AS0051 $srcfile $replfile "$OUTDIR" shift_left_scalar_I8.c
foo AS0060 $srcfile $replfile "$OUTDIR" shift_right_scalar_UI4.c
foo AS0061 $srcfile $replfile "$OUTDIR" shift_right_scalar_UI8.c

#------------------------------
replfile=repl_bwise.csv
srcfile=tmpl_f1s1opf2_arith.c
foo bwise_and_I4 $srcfile $replfile "$OUTDIR" bwise_and_scalar_I4.c
foo bwise_and_I8 $srcfile $replfile "$OUTDIR" bwise_and_scalar_I8.c
foo bwise_and_I1 $srcfile $replfile "$OUTDIR" bwise_and_scalar_I1.c
#------------------------------
foo bwise_or_I4  $srcfile $replfile "$OUTDIR" bwise_or_scalar_I4.c
foo bwise_or_I8  $srcfile $replfile "$OUTDIR" bwise_or_scalar_I8.c
foo bwise_or_I1  $srcfile $replfile "$OUTDIR" bwise_or_scalar_I1.c
#------------------------------
foo bwise_xor_I4 $srcfile $replfile "$OUTDIR" bwise_xor_scalar_I4.c
foo bwise_xor_I8 $srcfile $replfile "$OUTDIR" bwise_xor_scalar_I8.c
foo bwise_xor_I1 $srcfile $replfile "$OUTDIR" bwise_xor_scalar_I1.c
#------------------------------
#--------
replfile=repl2s.csv
srcfile=tmpl_opt_cmp_eq_mult_scalar.c
foo opt_cmp_eq_mult_scalar_I1 $srcfile $replfile "$OUTDIR" opt_cmp_eq_mult_scalar_I1.c
foo opt_cmp_eq_mult_scalar_I2 $srcfile $replfile "$OUTDIR" opt_cmp_eq_mult_scalar_I2.c
foo opt_cmp_eq_mult_scalar_I4 $srcfile $replfile "$OUTDIR" opt_cmp_eq_mult_scalar_I4.c
foo opt_cmp_eq_mult_scalar_I8 $srcfile $replfile "$OUTDIR" opt_cmp_eq_mult_scalar_I8.c
#--------
replfile=repl2s.csv
srcfile=tmpl_dbg_cmp_neq_mult_scalar.c
foo dbg_cmp_neq_mult_scalar_I1 $srcfile $replfile "$OUTDIR" dbg_cmp_neq_mult_scalar_I1.c
foo dbg_cmp_neq_mult_scalar_I2 $srcfile $replfile "$OUTDIR" dbg_cmp_neq_mult_scalar_I2.c
foo dbg_cmp_neq_mult_scalar_I4 $srcfile $replfile "$OUTDIR" dbg_cmp_neq_mult_scalar_I4.c
foo dbg_cmp_neq_mult_scalar_I8 $srcfile $replfile "$OUTDIR" dbg_cmp_neq_mult_scalar_I8.c
#--------
replfile=repl2s.csv
srcfile=tmpl_dbg_cmp_eq_mult_scalar.c
foo dbg_cmp_eq_mult_scalar_I1 $srcfile $replfile "$OUTDIR" dbg_cmp_eq_mult_scalar_I1.c
foo dbg_cmp_eq_mult_scalar_I2 $srcfile $replfile "$OUTDIR" dbg_cmp_eq_mult_scalar_I2.c
foo dbg_cmp_eq_mult_scalar_I4 $srcfile $replfile "$OUTDIR" dbg_cmp_eq_mult_scalar_I4.c
foo dbg_cmp_eq_mult_scalar_I8 $srcfile $replfile "$OUTDIR" dbg_cmp_eq_mult_scalar_I8.c
#--------
replfile=repl2s.csv
srcfile=tmpl_cmp_eq_mult_scalar.c
foo cmp_eq_mult_scalar_I1 $srcfile $replfile "$OUTDIR" cmp_eq_mult_scalar_I1.c
foo cmp_eq_mult_scalar_I2 $srcfile $replfile "$OUTDIR" cmp_eq_mult_scalar_I2.c
foo cmp_eq_mult_scalar_I4 $srcfile $replfile "$OUTDIR" cmp_eq_mult_scalar_I4.c
foo cmp_eq_mult_scalar_I8 $srcfile $replfile "$OUTDIR" cmp_eq_mult_scalar_I8.c
#--------
replfile=repl5.csv
srcfile=tmpl_saturate.c

foo saturate_I1 $srcfile $replfile "$OUTDIR" saturate_I1.c
foo saturate_I2 $srcfile $replfile "$OUTDIR" saturate_I2.c
foo saturate_I4 $srcfile $replfile "$OUTDIR" saturate_I4.c
foo saturate_I8 $srcfile $replfile "$OUTDIR" saturate_I8.c

#--------------------------------------------------------------
replfile=repl5.csv
srcfile=tmpl_conv.c

foo conv_F8_to_I4 $srcfile $replfile "$OUTDIR" conv_F8_to_I4.c
foo conv_F8_to_I8 $srcfile $replfile "$OUTDIR" conv_F8_to_I8.c
foo conv_F8_to_F4 $srcfile $replfile "$OUTDIR" conv_F8_to_F4.c

foo conv_I4_to_I8 $srcfile $replfile "$OUTDIR" conv_I4_to_I8.c
foo conv_I4_to_I1 $srcfile $replfile "$OUTDIR" conv_I4_to_I1.c
foo conv_I4_to_I2 $srcfile $replfile "$OUTDIR" conv_I4_to_I2.c
foo conv_I4_to_F4 $srcfile $replfile "$OUTDIR" conv_I4_to_F4.c
foo conv_I4_to_F8 $srcfile $replfile "$OUTDIR" conv_I4_to_F8.c

foo conv_I8_to_I4 $srcfile $replfile "$OUTDIR" conv_I8_to_I4.c
foo conv_I8_to_I1 $srcfile $replfile "$OUTDIR" conv_I8_to_I1.c
foo conv_I8_to_F4 $srcfile $replfile "$OUTDIR" conv_I8_to_F4.c
foo conv_I8_to_F8 $srcfile $replfile "$OUTDIR" conv_I8_to_F8.c

foo conv_I1_to_I8 $srcfile $replfile "$OUTDIR" conv_I1_to_I8.c
foo conv_I1_to_I4 $srcfile $replfile "$OUTDIR" conv_I1_to_I4.c

foo conv_I2_to_I4 $srcfile $replfile "$OUTDIR" conv_I2_to_I8.c
foo conv_I2_to_I8 $srcfile $replfile "$OUTDIR" conv_I2_to_I4.c

foo conv_F4_to_I4 $srcfile $replfile "$OUTDIR" conv_F4_to_I4.c
foo conv_F4_to_I8 $srcfile $replfile "$OUTDIR" conv_F4_to_I8.c
foo conv_F4_to_F8 $srcfile $replfile "$OUTDIR" conv_F4_to_F8.c
#--------------------------------------------------------------
replfile=repl5.csv
srcfile=tmpl_nn_conv.c

foo conv_F8_to_I4 $srcfile $replfile "$OUTDIR" nn_conv_F8_to_I4.c
foo conv_F8_to_I8 $srcfile $replfile "$OUTDIR" nn_conv_F8_to_I8.c
foo conv_F8_to_F4 $srcfile $replfile "$OUTDIR" nn_conv_F8_to_F4.c

foo conv_I4_to_I8 $srcfile $replfile "$OUTDIR" nn_conv_I4_to_I8.c
foo conv_I4_to_I1 $srcfile $replfile "$OUTDIR" nn_conv_I4_to_I1.c
foo conv_I4_to_I2 $srcfile $replfile "$OUTDIR" nn_conv_I4_to_I2.c
foo conv_I4_to_F4 $srcfile $replfile "$OUTDIR" nn_conv_I4_to_F4.c
foo conv_I4_to_F8 $srcfile $replfile "$OUTDIR" nn_conv_I4_to_F8.c

foo conv_I8_to_I4 $srcfile $replfile "$OUTDIR" nn_conv_I8_to_I4.c
foo conv_I8_to_I1 $srcfile $replfile "$OUTDIR" nn_conv_I8_to_I1.c
foo conv_I8_to_F4 $srcfile $replfile "$OUTDIR" nn_conv_I8_to_F4.c
foo conv_I8_to_F8 $srcfile $replfile "$OUTDIR" nn_conv_I8_to_F8.c

foo conv_I1_to_I8 $srcfile $replfile "$OUTDIR" nn_conv_I1_to_I8.c
foo conv_I1_to_I4 $srcfile $replfile "$OUTDIR" nn_conv_I1_to_I4.c

foo conv_I2_to_I4 $srcfile $replfile "$OUTDIR" nn_conv_I2_to_I8.c
foo conv_I2_to_I8 $srcfile $replfile "$OUTDIR" nn_conv_I2_to_I4.c

foo conv_F4_to_I4 $srcfile $replfile "$OUTDIR" nn_conv_F4_to_I4.c
foo conv_F4_to_I8 $srcfile $replfile "$OUTDIR" nn_conv_F4_to_I8.c
foo conv_F4_to_F8 $srcfile $replfile "$OUTDIR" nn_conv_F4_to_F8.c
#--------------------------------------------------------------


replfile=repl5.csv
srcfile=tmpl_unary_op.c
foo B0 $srcfile $replfile "$OUTDIR" not_I4.c
foo B1 $srcfile $replfile "$OUTDIR" ones_complement_I4.c

foo B2 $srcfile $replfile "$OUTDIR" not_I8.c
foo B3 $srcfile $replfile "$OUTDIR" ones_complement_I8.c

foo B4 $srcfile $replfile "$OUTDIR" not_I1.c
foo B5 $srcfile $replfile "$OUTDIR" ones_complement_I1.c

replfile=repl5.csv
srcfile=tmpl_incr.c
foo C0 $srcfile $replfile "$OUTDIR" incr_I4.c
foo C1 $srcfile $replfile "$OUTDIR" decr_I4.c

foo C2 $srcfile $replfile "$OUTDIR" incr_I8.c
foo C3 $srcfile $replfile "$OUTDIR" decr_I8.c

replfile=repl5.csv
srcfile=tmpl_cum.c
foo F0 $srcfile $replfile "$OUTDIR" cum_I4_to_I4.c
foo F1 $srcfile $replfile "$OUTDIR" cum_I4_to_I8.c
foo F2 $srcfile $replfile "$OUTDIR" cum_I8_to_I4.c
foo F3 $srcfile $replfile "$OUTDIR" cum_I8_to_I8.c
foo F4 $srcfile $replfile "$OUTDIR" cum_I1_to_I4.c
foo F5 $srcfile $replfile "$OUTDIR" cum_I1_to_I8.c
foo cum_I2_to_I4 $srcfile $replfile "$OUTDIR" cum_I2_to_I4.c
foo cum_I2_to_I8 $srcfile $replfile "$OUTDIR" cum_I2_to_I8.c



replfile=repl7.csv
srcfile=tmpl_bitcount.c
foo bitcount_I4 $srcfile $replfile "$OUTDIR" bitcount_I4.c
foo bitcount_I8 $srcfile $replfile "$OUTDIR" bitcount_I8.c

replfile=repl.csv 
srcfile=tmpl_pos_copy.c
foo pos_copy_I4_I4 $srcfile $replfile "$OUTDIR" pos_copy_I4_I4.c
foo pos_copy_I4_I8 $srcfile $replfile "$OUTDIR" pos_copy_I4_I8.c
foo pos_copy_I8_I4 $srcfile $replfile "$OUTDIR" pos_copy_I8_I4.c
foo pos_copy_I8_I8 $srcfile $replfile "$OUTDIR" pos_copy_I8_I8.c

replfile=repl.csv 
srcfile=tmpl_pos_nn_copy.c
foo pos_copy_I1_I4 $srcfile $replfile "$OUTDIR" pos_nn_copy_I1_I4.c
foo pos_copy_I1_I8 $srcfile $replfile "$OUTDIR" pos_nn_copy_I1_I8.c
foo pos_copy_I4_I4 $srcfile $replfile "$OUTDIR" pos_nn_copy_I4_I4.c
foo pos_copy_I4_I8 $srcfile $replfile "$OUTDIR" pos_nn_copy_I4_I8.c
foo pos_copy_I8_I4 $srcfile $replfile "$OUTDIR" pos_nn_copy_I8_I4.c
foo pos_copy_I8_I8 $srcfile $replfile "$OUTDIR" pos_nn_copy_I8_I8.c

replfile=repl.csv 
srcfile=tmpl_bin_search.c
foo bin_search_I1 $srcfile $replfile "$OUTDIR" bin_search_I1.c
foo bin_search_I2 $srcfile $replfile "$OUTDIR" bin_search_I2.c
foo bin_search_I4 $srcfile $replfile "$OUTDIR" bin_search_I4.c
foo bin_search_UI4 $srcfile $replfile "$OUTDIR" bin_search_UI4.c
foo bin_search_I8 $srcfile $replfile "$OUTDIR" bin_search_I8.c
foo bin_search_UI8 $srcfile $replfile "$OUTDIR" bin_search_UI8.c

replfile=repl.csv 

# srcfile=tmpl_mark_pos.c
# foo mark_pos_I1 $srcfile $replfile "$OUTDIR" mark_pos_I1.c
# foo mark_pos_I2 $srcfile $replfile "$OUTDIR" mark_pos_I2.c
# foo mark_pos_I4 $srcfile $replfile "$OUTDIR" mark_pos_I4.c
# foo mark_pos_I8 $srcfile $replfile "$OUTDIR" mark_pos_I8.c
# 
# srcfile=tmpl_mark_pos_nn.c
# foo mark_pos_I1 $srcfile $replfile "$OUTDIR" mark_pos_nn_I1.c
# foo mark_pos_I2 $srcfile $replfile "$OUTDIR" mark_pos_nn_I2.c
# foo mark_pos_I4 $srcfile $replfile "$OUTDIR" mark_pos_nn_I4.c
# foo mark_pos_I8 $srcfile $replfile "$OUTDIR" mark_pos_nn_I8.c

srcfile=tmpl_par_countf_cfld.c
foo par_countf_cfld_I1 $srcfile $replfile "$OUTDIR" par_countf_cfld_I1.c
foo par_countf_cfld_I2 $srcfile $replfile "$OUTDIR" par_countf_cfld_I2.c

srcfile=tmpl_par_count_cfld.c
foo par_count_cfld_I1 $srcfile $replfile "$OUTDIR" par_count_cfld_I1.c
foo par_count_cfld_I2 $srcfile $replfile "$OUTDIR" par_count_cfld_I2.c

srcfile=tmpl_par_count.c
foo par_count_I1 $srcfile $replfile "$OUTDIR" par_count_I1.c
foo par_count_I2 $srcfile $replfile "$OUTDIR" par_count_I2.c

srcfile=tmpl_par_countf.c
foo par_countf_I1 $srcfile $replfile "$OUTDIR" par_countf_I1.c
foo par_countf_I2 $srcfile $replfile "$OUTDIR" par_countf_I2.c

srcfile=tmpl_count.c
foo count_I1 $srcfile $replfile "$OUTDIR" count_I1.c
foo count_I2 $srcfile $replfile "$OUTDIR" count_I2.c
foo count_I4 $srcfile $replfile "$OUTDIR" count_I4.c
foo count_I8 $srcfile $replfile "$OUTDIR" count_I8.c

srcfile=tmpl_countf.c
foo count_I1 $srcfile $replfile "$OUTDIR" countf_I1.c
foo count_I2 $srcfile $replfile "$OUTDIR" countf_I2.c
foo count_I4 $srcfile $replfile "$OUTDIR" countf_I4.c
foo count_I8 $srcfile $replfile "$OUTDIR" countf_I8.c

srcfile=tmpl_count_nn.c
foo count_I1 $srcfile $replfile "$OUTDIR" count_nn_I1.c
foo count_I2 $srcfile $replfile "$OUTDIR" count_nn_I2.c
foo count_I4 $srcfile $replfile "$OUTDIR" count_nn_I4.c
foo count_I8 $srcfile $replfile "$OUTDIR" count_nn_I8.c

srcfile=tmpl_countf_nn.c
foo count_I1 $srcfile $replfile "$OUTDIR" countf_nn_I1.c
foo count_I2 $srcfile $replfile "$OUTDIR" countf_nn_I2.c
foo count_I4 $srcfile $replfile "$OUTDIR" countf_nn_I4.c
foo count_I8 $srcfile $replfile "$OUTDIR" countf_nn_I8.c

replfile=repl1.csv 
srcfile=tmpl_f1f2opf3_arith.c
foo add2_I2 $srcfile $replfile "$OUTDIR" add2_I2.c
foo sub2_I2 $srcfile $replfile "$OUTDIR" sub2_I2.c

foo A0000 $srcfile $replfile "$OUTDIR" add2_I4.c
foo A0001 $srcfile $replfile "$OUTDIR" sub2_I4.c
foo A0002 $srcfile $replfile "$OUTDIR" mul2_I4.c
foo A0003 $srcfile $replfile "$OUTDIR" div2_I4.c
foo A0004 $srcfile $replfile "$OUTDIR" rem2_I4.c
foo A0050 $srcfile $replfile "$OUTDIR" shiftleft2_I4.c
foo A0051 $srcfile $replfile "$OUTDIR" shiftright2_UI4.c
#--------
foo A0005 $srcfile $replfile "$OUTDIR" add2_I8.c
foo A0006 $srcfile $replfile "$OUTDIR" sub2_I8.c
foo A0007 $srcfile $replfile "$OUTDIR" mul2_I8.c
foo A0008 $srcfile $replfile "$OUTDIR" div2_I8.c
foo A0009 $srcfile $replfile "$OUTDIR" rem2_I8.c
foo A0055 $srcfile $replfile "$OUTDIR" shiftleft2_I8.c
foo A0056 $srcfile $replfile "$OUTDIR" shiftright2_UI8.c
#-----------
foo add2_I2 $srcfile $replfile "$OUTDIR" add2_I2.c
foo sub2_I2 $srcfile $replfile "$OUTDIR" sub2_I2.c
foo mul2_I2 $srcfile $replfile "$OUTDIR" mul2_I2.c
foo div2_I2 $srcfile $replfile "$OUTDIR" div2_I2.c
foo rem2_I2 $srcfile $replfile "$OUTDIR" rem2_I2.c
#-----------
foo A0010 $srcfile $replfile "$OUTDIR" add2_F4.c
foo A0011 $srcfile $replfile "$OUTDIR" sub2_F4.c
foo A0012 $srcfile $replfile "$OUTDIR" mul2_F4.c
foo A0013 $srcfile $replfile "$OUTDIR" div2_F4.c
#--------
foo add2_F8 $srcfile $replfile "$OUTDIR" add2_F8.c
foo sub2_F8 $srcfile $replfile "$OUTDIR" sub2_F8.c
foo mul2_F8 $srcfile $replfile "$OUTDIR" mul2_F8.c
foo div2_F8 $srcfile $replfile "$OUTDIR" div2_F8.c
#--------
foo A0015 $srcfile $replfile "$OUTDIR" and2_I1.c
foo A0016 $srcfile $replfile "$OUTDIR" or2_I1.c

#------------------------------
replfile=repl.csv 
srcfile=tmpl_bin_search.c
foo bin_search_I4 $srcfile $replfile "$OUTDIR" bin_search_I4.c
foo bin_search_UI4 $srcfile $replfile "$OUTDIR" bin_search_UI4.c
foo bin_search_I8 $srcfile $replfile "$OUTDIR" bin_search_I8.c
foo bin_search_UI8 $srcfile $replfile "$OUTDIR" bin_search_UI8.c

replfile=repl.csv 
srcfile=tmpl_count.c
foo count_I1 $srcfile $replfile "$OUTDIR" count_I1.c
foo count_I2 $srcfile $replfile "$OUTDIR" count_I2.c
foo count_I4 $srcfile $replfile "$OUTDIR" count_I4.c
foo count_I8 $srcfile $replfile "$OUTDIR" count_I8.c

replfile=repl1.csv 
srcfile=tmpl_f1f2opf3_arith.c
foo A0000 $srcfile $replfile "$OUTDIR" add2_I4.c
foo A0001 $srcfile $replfile "$OUTDIR" sub2_I4.c
foo A0002 $srcfile $replfile "$OUTDIR" mul2_I4.c
foo A0003 $srcfile $replfile "$OUTDIR" div2_I4.c
foo A0004 $srcfile $replfile "$OUTDIR" rem2_I4.c
foo A0050 $srcfile $replfile "$OUTDIR" shiftleft2_I4.c
foo A0051 $srcfile $replfile "$OUTDIR" shiftright2_UI4.c
#--------
foo A0005 $srcfile $replfile "$OUTDIR" add2_I8.c
foo A0006 $srcfile $replfile "$OUTDIR" sub2_I8.c
foo A0007 $srcfile $replfile "$OUTDIR" mul2_I8.c
foo A0008 $srcfile $replfile "$OUTDIR" div2_I8.c
foo A0009 $srcfile $replfile "$OUTDIR" rem2_I8.c
foo A0055 $srcfile $replfile "$OUTDIR" shiftleft2_I8.c
foo A0056 $srcfile $replfile "$OUTDIR" shiftright2_UI8.c
#-----------
foo A0010 $srcfile $replfile "$OUTDIR" add2_F4.c
foo A0011 $srcfile $replfile "$OUTDIR" sub2_F4.c
foo A0012 $srcfile $replfile "$OUTDIR" mul2_F4.c
foo A0013 $srcfile $replfile "$OUTDIR" div2_F4.c
#--------
foo A0015 $srcfile $replfile "$OUTDIR" and2_I1.c
foo A0016 $srcfile $replfile "$OUTDIR" or2_I1.c

#------------------------------
replfile=repl_bwise.csv 
srcfile=tmpl_f1f2opf3_arith.c
foo bwise_and_I4 $srcfile $replfile "$OUTDIR" bwise_and2_I4.c
foo bwise_and_I8 $srcfile $replfile "$OUTDIR" bwise_and2_I8.c
foo bwise_and_I1 $srcfile $replfile "$OUTDIR" bwise_and2_I1.c
#------------------------------
foo bwise_or_I4  $srcfile $replfile "$OUTDIR" bwise_or2_I4.c
foo bwise_or_I8  $srcfile $replfile "$OUTDIR" bwise_or2_I8.c
foo bwise_or_I1  $srcfile $replfile "$OUTDIR" bwise_or2_I1.c
#------------------------------
foo bwise_xor_I4 $srcfile $replfile "$OUTDIR" bwise_xor2_I4.c
foo bwise_xor_I8 $srcfile $replfile "$OUTDIR" bwise_xor2_I8.c
foo bwise_xor_I1 $srcfile $replfile "$OUTDIR" bwise_xor2_I1.c
#------------------------------
replfile=repl2.csv 
srcfile=tmpl_f1f2opf3_cond.c

foo cmp_ge_I2 $srcfile $replfile "$OUTDIR"  cmp_ge_I2.c
foo cmp_le_I2 $srcfile $replfile "$OUTDIR"  cmp_le_I2.c
foo cmp_eq_I2 $srcfile $replfile "$OUTDIR"  cmp_eq_I2.c
foo cmp_ne_I2 $srcfile $replfile "$OUTDIR"  cmp_ne_I2.c
foo cmp_gt_I2 $srcfile $replfile "$OUTDIR"  cmp_gt_I2.c
foo cmp_lt_I2 $srcfile $replfile "$OUTDIR"  cmp_lt_I2.c
#--------

replfile=repl7.csv
srcfile=tmpl_uniq.c
foo uniq_I4 $srcfile $replfile "$OUTDIR" uniq_I4.c
foo uniq_I8 $srcfile $replfile "$OUTDIR" uniq_I8.c

replfile=repl7.csv
srcfile=tmpl_uniq_cnt.c
foo uniq_I4 $srcfile $replfile "$OUTDIR" uniq_cnt_I4.c
foo uniq_I8 $srcfile $replfile "$OUTDIR" uniq_cnt_I8.c

replfile=repl7.csv
srcfile=tmpl_uniq_alldef.c
foo uniq_I4 $srcfile $replfile "$OUTDIR" uniq_alldef_I4.c
foo uniq_I8 $srcfile $replfile "$OUTDIR" uniq_alldef_I8.c

replfile=repl7.csv
srcfile=tmpl_uniq_cnt_alldef.c
foo uniq_I4 $srcfile $replfile "$OUTDIR" uniq_cnt_alldef_I4.c
foo uniq_I8 $srcfile $replfile "$OUTDIR" uniq_cnt_alldef_I8.c

replfile=repl10.csv
srcfile=tmpl_core_wisifxthenyelsez.c
foo wisifxthenyelsez_I4 $srcfile $replfile "$OUTDIR" core_wisifxthenyelsez_I4.c
foo wisifxthenyelsez_I8 $srcfile $replfile "$OUTDIR" core_wisifxthenyelsez_I8.c
foo wisifxthenyelsez_I1 $srcfile $replfile "$OUTDIR" core_wisifxthenyelsez_I1.c
foo wisifxthenyelsez_I2 $srcfile $replfile "$OUTDIR" core_wisifxthenyelsez_I2.c
foo wisifxthenyelsez_F4 $srcfile $replfile "$OUTDIR" core_wisifxthenyelsez_F4.c

replfile=repl.csv 
srcfile=tmpl_which_bin.c
foo f_to_s_sum_I4  $srcfile $replfile $OUTDIR which_bin_I4.c
foo f_to_s_sum_I8  $srcfile $replfile $OUTDIR which_bin_I8.c

replfile=repl.csv 
srcfile=tmpl_core_is_a_in_b.c
foo core_is_a_in_b_I1 $srcfile $replfile $OUTDIR core_is_a_in_b_I1.c
foo core_is_a_in_b_I2 $srcfile $replfile $OUTDIR core_is_a_in_b_I2.c
foo core_is_a_in_b_I4 $srcfile $replfile $OUTDIR core_is_a_in_b_I4.c
foo core_is_a_in_b_I8 $srcfile $replfile $OUTDIR core_is_a_in_b_I8.c
#---------------------------------------------------
replfile=repl.csv 
srcfile=tmpl_permute.c
foo permute_I4 $srcfile $replfile $OUTDIR permute_I4.c
foo permute_I8 $srcfile $replfile $OUTDIR permute_I8.c
#---------------------------------------------------
replfile=repl.csv 
srcfile=tmpl_mk_hashes.c
foo mk_hashes_I1 $srcfile $replfile $OUTDIR mk_hashes_I1.c
foo mk_hashes_I2 $srcfile $replfile $OUTDIR mk_hashes_I2.c
foo mk_hashes_I4 $srcfile $replfile $OUTDIR mk_hashes_I4.c
foo mk_hashes_I8 $srcfile $replfile $OUTDIR mk_hashes_I8.c

replfile=repl.csv 
srcfile=tmpl_hash.c
foo hash_I4 $srcfile $replfile $OUTDIR hash_I4.c
foo hash_I8 $srcfile $replfile $OUTDIR hash_I8.c

replfile=repl.csv 
srcfile=tmpl_find_largest_leq.c
foo find_largest_leq_I4 $srcfile $replfile $OUTDIR find_largest_leq_I4.c
foo find_largest_leq_I8 $srcfile $replfile $OUTDIR find_largest_leq_I8.c

replfile=repl.csv 
srcfile=tmpl_srt_uniform.c
foo srt_uniform_I4 $srcfile $replfile $OUTDIR srt_uniform_I4.c
foo srt_uniform_I8 $srcfile $replfile $OUTDIR srt_uniform_I8.c

#---------------------------------------------------
test -f tmpl_pack_incl.c 
sed s'/__ITYPE__/int/'g       tmpl_pack_incl.c > ../incl_pack_I4.c
sed s'/__ITYPE__/long long/'g tmpl_pack_incl.c > ../incl_pack_I8.c

#---------------------------------------------------
test -f tmpl_set_val.c
sed s'/__ITYPE__/short/'g tmpl_set_val.c | \
	sed s'/__XTYPE__/I2/'g | sed s'/__NBITS__/16/'g > ../incl_set_val_I2.c
sed s'/__ITYPE__/int/'g tmpl_set_val.c | \
	sed s'/__XTYPE__/I4/'g | sed s'/__NBITS__/32/'g > ../incl_set_val_I4.c
sed s'/__ITYPE__/long long/'g tmpl_set_val.c | \
	sed s'/__XTYPE__/I8/'g | sed s'/__NBITS__/64/'g > ../incl_set_val_I8.c
#---------------------------------------------------
