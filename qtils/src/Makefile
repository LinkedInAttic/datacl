#
# /*
# © [2013] LinkedIn Corp. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License. You may obtain
# a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
#  
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an "AS IS"
# BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.
# */
#
# make with c compiler, using 4rd party .h files and .a files
#
#
CC=gcc

GCCOPTFLAG= -g
GCCOPTFLAG= -O4

# PICFLAG=-fPIC: add -fPIC when compiling q.a for shell loadable builtin
PICFLAG=
GCCFLG = ${GCCOPTFLAG} -ansi -pedantic -std=gnu99 $(PICFLAG) -Wall 
DBG = -DDEBUG
ERR = -DERR_LOG
CHECKS = -DCHECKS
VERBOSE = -DVERBOSE
VERBOSE = 
CFLAGS= ${GCCFLG} ${DBG} ${ERR} ${CHECKS} ${VERBOSE}
#
# make variable for compiles with the -I command 
#
#INCLUDES=-I/where/the/header/file/is -I/other/header/files
INCLUDES=-I./AUX -I. -I./dir_utils -I./dir_ro_verbs -I./dir_wr_verbs \
	 -I./dir_helpers -I./dir_dbutils

#
# make variable for extra libraries
# link with -L and -l
#
# LIBS=-L/where/the/lib/file/is -L/other/lib/files -lUSR1 -lUSR2 -lOTHER
LIBS=
#
#
.PHONY: all clean 

ALL = q

all: ${ALL}

clean:
	rm -f *.o ${ALL} *.so q.a ./AUX/*.o; 

INC1 = constants.h macros.h qtypes.h

COREQ = \
	is_tbl.o \
	is_fld.o \
	meta_data.o \
	list_files.o \
	get_meta.o \
	list_tbls.o \
	pr_fld.o \
	tbl_meta.o \
	fld_meta.o \
	dump.o \
	get_nR.o \
	is_nn_fld.o \
	\
	set_meta.o \
	dld.o \
  	wisifxthenyelsez.o \
	sort.o \
	add_tbl.o \
	stride.o \
	del_tbl.o \
	del_fld.o \
	ext_s_to_f.o \
	add_fld.o \
	crossprod.o \
	pack.o \
	unpack.o \
	lkpsort.o \
	rng_sort.o \
	f1f2_to_s.o \
	t1f1opt2f2.o \
 	num_in_range.o \
 	app_tbl.o \
 	set_val.o \
	count_vals.o \
	mk_bins.o \
 	f1f2opf3.o \
	is_a_in_b.o \
	is_A_in_b.o \
	dir_is_a_in_b.o \
 	copy_fld.o \
 	mv_fld.o \
 	mk_nn_fld.o \
 	copy_range.o \
 	range_to_cfld.o \
 	cfld_to_range.o \
 	f1s1opf2.o \
 	f1T1opf2.o \
 	f1opf2.o \
	f1opf2f3.o \
 	srt_join.o \
 	rng_join.o \
	count_ht.o \
	count.o \
	countT.o \
	countf.o \
 	fop.o \
 	udf_eq_B.o \
 	sortf1f2.o \
	get_t2f2_val.o \
	xfer.o \
	bindmp.o \
	binld.o \
 	drop_nn_fld.o \
 	rename_fld.o \
 	rename_tbl.o \
	set_nR.o \
	dup_fld.o \
	subsample.o \
	\
	q.o \
	\
	aux_meta.o \
	mk_file.o \
	mk_temp_file.o \
	dbauxil.o \
	logger.o \
	open_temp_file.o  \
	add_aux_fld.o \
 	get_type_op_fld.o \
	\
	auxil.o \
 	md5c.o \
	mmap.o \
	mddriver.o \
	hash_string.o \
	extract_S.o  \
	MurmurHash3.o \
	mix.o \
	find_closest_I4.o \
	find_largest_leq_I4.o \
	find_smallest_geq_I4.o \
	\
	par_s_to_f.o \
	read_meta_data.o \
	read_csv.o \
	pr_fld_clob.o \
	pr_fld_var_clob.o \
	ext_f_to_s.o \
	par_f_to_s.o \
 	app_fld.o \
 	f1opf2_shift.o \
	nn_core.o \
 	vec_f1f2opf3.o \
	mk_dict.o \
	rd_dict.o \
 	vec_f1opf2.o \
 	f1opf2_cum.o \
 	regex_match.o \
 	vec_f1s1opf2.o \
	zero_after_n.o \
	smear.o \
	\
	extract_I1.o \
	extract_I2.o \
	extract_I4.o \
	extract_I8.o \
	s_to_f_const_I1.o \
	s_to_f_const_I2.o \
	s_to_f_const_I4.o \
	s_to_f_const_I8.o \
	s_to_f_const_F4.o \
	s_to_f_seq_I1.o \
	s_to_f_seq_I2.o \
	s_to_f_seq_I4.o \
	s_to_f_seq_I8.o  \
	pr_fld_I1.o  \
	pr_fld_I2.o  \
	pr_fld_I4.o  \
	pr_fld_I8.o \
	pr_fld_F4.o  \
	pr_fld_F8.o \
	f_to_s_min_I1.o  \
	f_to_s_min_I2.o  \
	f_to_s_min_I4.o  \
	f_to_s_min_I8.o \
	f_to_s_min_F4.o \
	f_to_s_min_F8.o \
	nn_f_to_s_min_I1.o  \
	nn_f_to_s_min_I2.o  \
	nn_f_to_s_min_I4.o  \
	nn_f_to_s_min_I8.o  \
	nn_f_to_s_min_F4.o \
	nn_f_to_s_min_F8.o \
	f_to_s_max_I1.o  \
	f_to_s_max_I2.o  \
	f_to_s_max_I4.o  \
	f_to_s_max_I8.o \
	f_to_s_max_F4.o \
	f_to_s_max_F8.o \
	nn_f_to_s_max_I1.o  \
	nn_f_to_s_max_I2.o  \
	nn_f_to_s_max_I4.o  \
	nn_f_to_s_max_I8.o  \
	nn_f_to_s_max_F4.o \
	nn_f_to_s_max_F8.o \
	f_to_s_sum_B.o  \
	f_to_s_sum_I1.o  \
	f_to_s_sum_I2.o  \
	f_to_s_sum_I4.o  \
	f_to_s_sum_I8.o \
	f_to_s_sum_F4.o \
	f_to_s_sum_F8.o \
	nn_f_to_s_sum_I1.o  \
	nn_f_to_s_sum_I2.o  \
	nn_f_to_s_sum_I4.o  \
	nn_f_to_s_sum_I8.o  \
	nn_f_to_s_sum_F4.o \
	nn_f_to_s_sum_F8.o  \
	xfer_I1_I4.o \
	xfer_I2_I4.o \
	xfer_I4_I4.o \
	xfer_I8_I4.o \
	xfer_F4_I4.o \
	xfer_F8_I4.o  \
 	qsort_asc_I4.o \
 	qsort_asc_I8.o  \
 	qsort_dsc_I4.o \
 	qsort_dsc_I8.o \
 	qsort_with_idx_int.o \
 	qsort_with_idx_longlong.o  \
 	qsort_asc_val_I4_idx_I4.o \
 	qsort_asc_val_I4_idx_I8.o \
 	qsort_asc_val_I8_idx_I4.o \
 	qsort_asc_val_I8_idx_I8.o  \
 	qsort_dsc_val_I4_idx_I4.o \
 	qsort_dsc_val_I4_idx_I8.o \
 	qsort_dsc_val_I8_idx_I4.o \
 	qsort_dsc_val_I8_idx_I8.o  \
 	sort_asc_I4.o \
 	sort_dsc_I4.o \
 	sort_asc_I8.o \
 	sort_dsc_I8.o \
 	sort_asc_F4.o \
 	sort_dsc_F4.o \
 	sort_asc_F8.o \
 	sort_dsc_F8.o \
 	core_srt_join_I_I_I.o \
 	core_srt_join_I_I_L.o \
 	core_srt_join_L_I_I.o \
 	core_srt_join_L_I_L.o \
 	core_srt_join_I_L_I.o \
 	core_srt_join_I_L_L.o \
 	core_srt_join_L_L_I.o \
 	core_srt_join_L_L_L.o \
 	assign_I1.o \
 	assign_I4.o \
 	assign_I8.o \
 	is_sorted_alldef_I1.o \
 	is_sorted_alldef_I2.o \
 	is_sorted_alldef_I4.o \
 	is_sorted_alldef_I8.o \
 	cmp_lt_or_gt_I4.o \
 	cmp_le_or_ge_I4.o \
 	cmp_gt_and_lt_I4.o \
 	cmp_ge_and_le_I4.o \
 	cmp_eq_mult_scalar_I1.o \
 	cmp_eq_mult_scalar_I2.o \
 	cmp_eq_mult_scalar_I4.o \
 	cmp_eq_mult_scalar_I8.o \
 	cmp_eq_mult_scalar_simple_I1.o \
 	cmp_eq_mult_scalar_simple_I2.o \
 	cmp_eq_mult_scalar_simple_I4.o \
 	cmp_eq_mult_scalar_simple_I8.o \
 	cmp_eq_mult_scalar_log_I1.o \
 	cmp_eq_mult_scalar_log_I2.o \
 	cmp_eq_mult_scalar_log_I4.o \
 	cmp_eq_mult_scalar_log_I8.o \
 	add_scalar_I2.o \
 	sub_scalar_I2.o \
 	mul_scalar_I2.o \
 	div_scalar_I2.o \
 	rem_scalar_I2.o \
 	add_scalar_I4.o \
 	sub_scalar_I4.o \
 	mul_scalar_I4.o \
 	div_scalar_I4.o \
 	rem_scalar_I4.o \
 	add_scalar_I8.o  \
 	sub_scalar_I8.o \
 	mul_scalar_I8.o \
 	div_scalar_I8.o \
 	rem_scalar_I8.o \
 	add_scalar_F4.o  \
 	sub_scalar_F4.o \
 	mul_scalar_F4.o \
 	div_scalar_F4.o \
 	add_scalar_F8.o  \
 	sub_scalar_F8.o \
 	mul_scalar_F8.o \
 	div_scalar_F8.o \
 	and_scalar_I1.o \
 	or_scalar_I1.o \
 	bwise_a_and_not_b_I8.o \
 	bwise_and_scalar_I4.o \
 	bwise_and_scalar_I8.o \
 	bwise_and_scalar_I1.o \
 	bwise_or_scalar_I4.o \
 	bwise_or_scalar_I8.o \
 	bwise_or_scalar_I1.o \
 	bwise_xor_scalar_I4.o \
 	bwise_xor_scalar_I8.o \
 	bwise_xor_scalar_I1.o \
 	cmp_le_scalar_I4.o \
 	cmp_ge_scalar_I4.o  \
 	cmp_eq_scalar_I2.o \
 	cmp_eq_scalar_I4.o \
 	cmp_ne_scalar_I4.o \
 	cmp_gt_scalar_I4.o \
 	cmp_lt_scalar_I4.o \
 	cmp_le_scalar_I8.o \
 	cmp_ge_scalar_I8.o \
 	cmp_eq_scalar_I8.o \
 	cmp_ne_scalar_I8.o \
 	cmp_lt_scalar_I8.o \
 	cmp_gt_scalar_I8.o \
 	cmp_le_scalar_F4.o  \
 	cmp_ge_scalar_F4.o \
 	cmp_eq_scalar_F4.o \
 	cmp_ne_scalar_F4.o \
 	cmp_lt_scalar_F4.o \
 	cmp_gt_scalar_F4.o \
 	cmp_le_scalar_I1.o \
 	cmp_ge_scalar_I1.o \
 	cmp_eq_scalar_I1.o \
 	cmp_ne_scalar_I1.o \
 	cmp_lt_scalar_I1.o \
 	cmp_gt_scalar_I1.o \
 	shift_left_scalar_I4.o \
 	shift_left_scalar_I8.o \
 	shift_right_scalar_UI4.o \
 	shift_right_scalar_UI8.o \
 	conv_I1_to_B.o  \
 	conv_B_to_I1.o  \
 	conv_F8_to_I4.o  \
 	conv_F8_to_I8.o  \
 	conv_F8_to_F4.o  \
 	conv_I2_to_I4.o  \
 	conv_I2_to_I8.o  \
 	conv_I4_to_I1.o  \
 	conv_I4_to_I2.o  \
 	conv_I4_to_I8.o  \
 	conv_I4_to_F4.o  \
 	conv_I4_to_F8.o  \
 	conv_I8_to_I4.o  \
 	conv_I8_to_I1.o  \
 	conv_I8_to_F4.o  \
 	conv_I8_to_F8.o  \
 	conv_I1_to_I4.o  \
 	conv_I1_to_I8.o  \
 	conv_F4_to_I4.o  \
 	conv_F4_to_I8.o  \
 	conv_F4_to_F8.o  \
 	not_I4.o  \
 	ones_complement_I4.o  \
 	ones_complement_I8.o  \
 	ones_complement_I1.o  \
 	not_I8.o  \
 	not_I1.o  \
 	incr_I4.o  \
 	decr_I4.o  \
 	incr_I8.o  \
 	decr_I8.o \
 	cum_I4_to_I4.o \
 	cum_I4_to_I8.o \
 	cum_I8_to_I4.o \
 	cum_I8_to_I8.o \
 	cum_I1_to_I4.o \
 	cum_I1_to_I8.o \
 	bitcount_I4.o  \
 	bitcount_I8.o  \
 	vec_abs_F4.o \
 	vec_abs_F8.o \
 	vec_pval_from_zval.o \
 	vec_normal_cdf_inverse.o \
 	vec_sqrt_F4.o \
 	vec_sqrt_F8.o \
 	vec_reciprocal_F4.o \
 	vec_reciprocal_F8.o \
 	t1f1t2f2opt3f3.o \
	pos_copy_I4_I4.o \
	pos_copy_I4_I8.o \
	pos_copy_I8_I4.o \
	pos_copy_I8_I8.o \
	pos_nn_copy_I1_I4.o \
	pos_nn_copy_I1_I8.o \
	pos_nn_copy_I4_I4.o \
	pos_nn_copy_I4_I8.o \
	pos_nn_copy_I8_I4.o \
	pos_nn_copy_I8_I8.o \
	pos_sel_copy_I4_I4.o \
	pos_sel_copy_I4_I8.o \
	pos_sel_copy_I8_I4.o \
	pos_sel_copy_I8_I8.o \
	pos_sel_aux_copy_I4_I4.o \
	pos_sel_aux_copy_I4_I8.o \
	pos_sel_aux_copy_I8_I4.o \
	pos_sel_aux_copy_I8_I8.o \
	pos_sel_aux_nn_copy_I4_I4.o \
	pos_sel_aux_nn_copy_I4_I8.o \
	pos_sel_aux_nn_copy_I8_I4.o \
	pos_sel_aux_nn_copy_I8_I8.o \
	pos_count_I4_I4.o \
	pos_count_I4_I8.o \
	pos_count_I8_I4.o \
	pos_count_I8_I8.o \
	pos_count_bitvec_I4_I4.o \
	pos_count_bitvec_I4_I8.o \
	pos_count_bitvec_I8_I4.o \
	pos_count_bitvec_I8_I8.o \
	pos_mmad_I4_I4.o \
	pos_mmad_I4_I8.o \
	pos_mmad_I8_I4.o \
	pos_mmad_I8_I8.o \
	pos_mmad_I4_I4_nn.o \
	pos_mmad_I4_I8_nn.o \
	pos_mmad_I8_I4_nn.o \
	pos_mmad_I8_I8_nn.o \
	bin_search_I4.o \
	bin_search_UI4.o \
	bin_search_I8.o \
	bin_search_UI8.o \
	which_bin_I4.o \
	which_bin_I8.o \
	countf_I1.o \
	countf_I2.o \
	countf_I4.o \
	countf_I8.o \
	countf_nn_I1.o \
	countf_nn_I2.o \
	countf_nn_I4.o \
	countf_nn_I8.o \
	count_I1.o \
	count_I2.o \
	count_I4.o \
	count_I8.o \
	count_nn_I1.o \
	count_nn_I2.o \
	count_nn_I4.o \
	count_nn_I8.o \
 	concat_I4_I4_to_I8.o \
 	add2_I4.o \
 	sub2_I4.o \
 	mul2_I4.o \
 	div2_I4.o \
 	rem2_I4.o \
 	shiftleft2_I4.o \
 	shiftright2_UI4.o \
 	add2_I8.o \
 	sub2_I8.o \
 	mul2_I8.o \
 	div2_I8.o \
 	rem2_I8.o \
 	shiftleft2_I8.o \
 	shiftright2_UI8.o \
 	add2_F4.o \
 	sub2_F4.o \
 	mul2_F4.o \
 	div2_F4.o \
 	add2_F8.o \
 	sub2_F8.o \
 	mul2_F8.o \
 	div2_F8.o \
 	and2_I1.o \
 	or2_I1.o \
 	a_and_not_b_I1.o \
 	bwise_and2_I4.o \
 	bwise_and2_I8.o \
 	bwise_and2_I1.o \
 	bwise_or2_I4.o \
 	bwise_or2_I8.o \
 	bwise_or2_I1.o \
 	bwise_xor2_I4.o \
 	bwise_xor2_I8.o \
 	bwise_xor2_I1.o \
 	cmp_ge_I1.o \
 	cmp_le_I1.o \
 	cmp_eq_I1.o \
 	cmp_ne_I1.o \
 	cmp_gt_I1.o \
 	cmp_lt_I1.o \
 	cmp_ge_I4.o \
 	cmp_le_I4.o \
 	cmp_eq_I4.o \
 	cmp_ne_I4.o \
 	cmp_gt_I4.o \
 	cmp_lt_I4.o \
 	cmp_ge_F4.o \
 	cmp_le_F4.o \
 	cmp_eq_F4.o \
 	cmp_ne_F4.o \
 	cmp_gt_F4.o \
 	cmp_lt_F4.o \
 	cmp_ge_I8.o \
 	cmp_le_I8.o \
 	cmp_eq_I8.o \
 	cmp_ne_I8.o \
 	cmp_gt_I8.o \
 	cmp_lt_I8.o \
 	uniq_I4.o  \
 	uniq_I8.o  \
 	uniq_cnt_I4.o  \
 	uniq_cnt_I8.o  \
 	uniq_alldef_I4.o  \
 	uniq_alldef_I8.o \
 	uniq_cnt_alldef_I4.o  \
 	uniq_cnt_alldef_I8.o \
 	core_wisifxthenyelsez_I1.o \
 	core_wisifxthenyelsez_I4.o \
 	core_wisifxthenyelsez_I8.o \
 	core_wisifxthenyelsez_I1.o \
 	core_wisifxthenyelsez_F4.o \
 	union_I4.o  \
 	union_I8.o  \

# To make q.a, you need to say: make -e "PICFLAG=-fPIC" -e "OPTFLAG=-O4" q.a

q: qwrap.o $(COREQ) 
	$(CC) $(CFLAGS) -o $@ $^ -ldl -lm 


orig_meta_globals.c: orig_meta_globals.h
	cp orig_meta_globals.h orig_meta_globals.c

#
# DO NOT include 3rd party headers for your source
# DO     include your own headers
#
qwrap.o: qwrap.c $(INC1) 
	$(CC) $(CFLAGS) $(INCLUDES) -c qwrap.c

q.o: q.c $(INC1)
	$(CC) $(CCFLAGS) $(INCLUDES) -c q.c 

parsort1.o: parsort1.c parsort1.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c parsort1.c

#---------------------------------------------------------
xfer.o:	./dir_wr_verbs/xfer.c ./dir_wr_verbs/xfer.h $(INC1) \
	./dir_wr_verbs/incl_xfer_I1.c  \
	./dir_wr_verbs/incl_xfer_I2.c  \
	./dir_wr_verbs/incl_xfer_I4.c  \
	./dir_wr_verbs/incl_xfer_I8.c  \
	./dir_wr_verbs/incl_xfer_F4.c  \
	./dir_wr_verbs/incl_xfer_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/xfer.c

#---------------------------------------------------------
par_f_to_s.o: ./dir_helpers/par_f_to_s.c ./dir_helpers/par_f_to_s.h $(INC1) \
	./dir_helpers/incl_f_to_s_max.c ./dir_helpers/incl_f_to_s_min.c \
	./dir_helpers/incl_f_to_s_sum.c
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/par_f_to_s.c

#--------------------------------------------------------
xfer_I1_I4.o :	./AUX/xfer_I1_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/xfer_I1_I4.c 
xfer_I2_I4.o :	./AUX/xfer_I2_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/xfer_I2_I4.c 
xfer_I4_I4.o :	./AUX/xfer_I4_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/xfer_I4_I4.c 
xfer_I8_I4.o :	./AUX/xfer_I8_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/xfer_I8_I4.c 
xfer_F4_I4.o :	./AUX/xfer_F4_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/xfer_F4_I4.c 
xfer_F8_I4.o :	./AUX/xfer_F8_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/xfer_F8_I4.c 
#--------------------------------------------------------
f_to_s_min_I1.o :	./AUX/f_to_s_min_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_min_I1.c 
f_to_s_min_I2.o :	./AUX/f_to_s_min_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_min_I2.c 
f_to_s_min_I4.o :	./AUX/f_to_s_min_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_min_I4.c 
f_to_s_min_I8.o :	./AUX/f_to_s_min_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_min_I8.c 
f_to_s_min_F4.o :	./AUX/f_to_s_min_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_min_F4.c 
f_to_s_min_F8.o :	./AUX/f_to_s_min_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_min_F8.c 
nn_f_to_s_min_I1.o :	./AUX/nn_f_to_s_min_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_min_I1.c 
nn_f_to_s_min_I2.o :	./AUX/nn_f_to_s_min_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_min_I2.c 
nn_f_to_s_min_I4.o :	./AUX/nn_f_to_s_min_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_min_I4.c 
nn_f_to_s_min_I8.o :	./AUX/nn_f_to_s_min_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_min_I8.c 
nn_f_to_s_min_F4.o :	./AUX/nn_f_to_s_min_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_min_F4.c 
nn_f_to_s_min_F8.o :	./AUX/nn_f_to_s_min_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_min_F8.c 
#--------------------------------------------------------
f_to_s_max_I1.o :	./AUX/f_to_s_max_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_max_I1.c 
f_to_s_max_I2.o :	./AUX/f_to_s_max_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_max_I2.c 
f_to_s_max_I4.o :	./AUX/f_to_s_max_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_max_I4.c 
f_to_s_max_I8.o :	./AUX/f_to_s_max_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_max_I8.c 
f_to_s_max_F4.o :	./AUX/f_to_s_max_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_max_F4.c 
f_to_s_max_F8.o :	./AUX/f_to_s_max_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_max_F8.c 
nn_f_to_s_max_I1.o :	./AUX/nn_f_to_s_max_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_max_I1.c 
nn_f_to_s_max_I2.o :	./AUX/nn_f_to_s_max_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_max_I2.c 
nn_f_to_s_max_I4.o :	./AUX/nn_f_to_s_max_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_max_I4.c 
nn_f_to_s_max_I8.o :	./AUX/nn_f_to_s_max_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_max_I8.c 
nn_f_to_s_max_F4.o :	./AUX/nn_f_to_s_max_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_max_F4.c 
nn_f_to_s_max_F8.o :	./AUX/nn_f_to_s_max_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_max_F8.c 
#--------------------------------------------------------
f_to_s_sum_B.o :	./AUX/f_to_s_sum_B.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_sum_B.c 
f_to_s_sum_I1.o :	./AUX/f_to_s_sum_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_sum_I1.c 
f_to_s_sum_I2.o :	./AUX/f_to_s_sum_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_sum_I2.c 
f_to_s_sum_I4.o :	./AUX/f_to_s_sum_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_sum_I4.c 
f_to_s_sum_I8.o :	./AUX/f_to_s_sum_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_sum_I8.c 
f_to_s_sum_F4.o :	./AUX/f_to_s_sum_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_sum_F4.c 
f_to_s_sum_F8.o :	./AUX/f_to_s_sum_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/f_to_s_sum_F8.c 
nn_f_to_s_sum_I1.o :	./AUX/nn_f_to_s_sum_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_sum_I1.c 
nn_f_to_s_sum_I2.o :	./AUX/nn_f_to_s_sum_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_sum_I2.c 
nn_f_to_s_sum_I4.o :	./AUX/nn_f_to_s_sum_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_sum_I4.c 
nn_f_to_s_sum_I8.o :	./AUX/nn_f_to_s_sum_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_sum_I8.c 
nn_f_to_s_sum_F4.o :	./AUX/nn_f_to_s_sum_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_sum_F4.c 
nn_f_to_s_sum_F8.o :	./AUX/nn_f_to_s_sum_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/nn_f_to_s_sum_F8.c 
#--------------------------------------------------------
pr_fld_I1.o :	./AUX/pr_fld_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pr_fld_I1.c 
pr_fld_I2.o :	./AUX/pr_fld_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pr_fld_I2.c 
pr_fld_I4.o :	./AUX/pr_fld_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pr_fld_I4.c 
pr_fld_I8.o :	./AUX/pr_fld_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pr_fld_I8.c 
pr_fld_F4.o :	./AUX/pr_fld_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pr_fld_F4.c 
pr_fld_F8.o :	./AUX/pr_fld_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pr_fld_F8.c 
#--------------------------------------------------------
extract_I1.o :	./AUX/extract_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/extract_I1.c 
extract_I2.o :	./AUX/extract_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/extract_I2.c 
extract_I4.o :	./AUX/extract_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/extract_I4.c 
extract_I8.o :	./AUX/extract_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/extract_I8.c 
#--------------------------------------------------------
s_to_f_const_I1.o :	./AUX/s_to_f_const_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_const_I1.c 
s_to_f_const_I2.o :	./AUX/s_to_f_const_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_const_I2.c 
s_to_f_const_I4.o :	./AUX/s_to_f_const_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_const_I4.c 
s_to_f_const_I8.o :	./AUX/s_to_f_const_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_const_I8.c 
s_to_f_const_F4.o :	./AUX/s_to_f_const_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_const_F4.c 
#--------------------------------------------------------
s_to_f_seq_I1.o :	./AUX/s_to_f_seq_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_seq_I1.c 
s_to_f_seq_I2.o :	./AUX/s_to_f_seq_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_seq_I2.c 
s_to_f_seq_I4.o :	./AUX/s_to_f_seq_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_seq_I4.c 
s_to_f_seq_I8.o :	./AUX/s_to_f_seq_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/s_to_f_seq_I8.c 
#--------------------------------------------------------
assign_I1.o :	./AUX/assign_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/assign_I1.c 
assign_I4.o :	./AUX/assign_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/assign_I4.c 
assign_I8.o :	./AUX/assign_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/assign_I8.c 

add2_I4.o : ./AUX/add2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add2_I4.c 
sub2_I4.o : ./AUX/sub2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub2_I4.c 
mul2_I4.o : ./AUX/mul2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul2_I4.c 
div2_I4.o : ./AUX/div2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div2_I4.c 
rem2_I4.o : ./AUX/rem2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/rem2_I4.c 
shiftleft2_I4.o : ./AUX/shiftleft2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shiftleft2_I4.c 
shiftright2_UI4.o : ./AUX/shiftright2_UI4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shiftright2_UI4.c 
add2_I8.o : ./AUX/add2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add2_I8.c 
sub2_I8.o : ./AUX/sub2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub2_I8.c 
mul2_I8.o : ./AUX/mul2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul2_I8.c 
div2_I8.o : ./AUX/div2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div2_I8.c 
rem2_I8.o : ./AUX/rem2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/rem2_I8.c 
shiftleft2_I8.o : ./AUX/shiftleft2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shiftleft2_I8.c 
shiftright2_UI8.o : ./AUX/shiftright2_UI8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shiftright2_UI8.c 
add2_F8.o : ./AUX/add2_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add2_F8.c 
sub2_F8.o : ./AUX/sub2_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub2_F8.c 
mul2_F8.o : ./AUX/mul2_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul2_F8.c 
div2_F8.o : ./AUX/div2_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div2_F8.c 
add2_F4.o : ./AUX/add2_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add2_F4.c 
sub2_F4.o : ./AUX/sub2_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub2_F4.c 
mul2_F4.o : ./AUX/mul2_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul2_F4.c 
div2_F4.o : ./AUX/div2_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div2_F4.c 
and2_I1.o : ./AUX/and2_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/and2_I1.c 
or2_I1.o : ./AUX/or2_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/or2_I1.c 
bwise_and2_I1.o : ./AUX/bwise_and2_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_and2_I1.c 
bwise_and2_I8.o : ./AUX/bwise_and2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_and2_I8.c 
bwise_and2_I4.o : ./AUX/bwise_and2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_and2_I4.c 
bwise_or2_I1.o : ./AUX/bwise_or2_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_or2_I1.c 
bwise_or2_I8.o : ./AUX/bwise_or2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_or2_I8.c 
bwise_or2_I4.o : ./AUX/bwise_or2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_or2_I4.c 
bwise_xor2_I1.o : ./AUX/bwise_xor2_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_xor2_I1.c 
bwise_xor2_I8.o : ./AUX/bwise_xor2_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_xor2_I8.c 
bwise_xor2_I4.o : ./AUX/bwise_xor2_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_xor2_I4.c 
#---------------------------------------------
cmp_ge_I1.o : ./AUX/cmp_ge_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_I1.c 
cmp_le_I1.o : ./AUX/cmp_le_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_I1.c 
cmp_eq_I1.o : ./AUX/cmp_eq_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_I1.c 
cmp_ne_I1.o : ./AUX/cmp_ne_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_I1.c 
cmp_gt_I1.o : ./AUX/cmp_gt_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_I1.c 
cmp_lt_I1.o : ./AUX/cmp_lt_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_I1.c 
#---------------------------------------------
cmp_ge_I4.o : ./AUX/cmp_ge_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_I4.c 
cmp_le_I4.o : ./AUX/cmp_le_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_I4.c 
cmp_eq_I4.o : ./AUX/cmp_eq_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_I4.c 
cmp_ne_I4.o : ./AUX/cmp_ne_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_I4.c 
cmp_gt_I4.o : ./AUX/cmp_gt_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_I4.c 
cmp_lt_I4.o : ./AUX/cmp_lt_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_I4.c 
#---------------------------------------------
cmp_ge_F4.o : ./AUX/cmp_ge_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_F4.c 
cmp_le_F4.o : ./AUX/cmp_le_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_F4.c 
cmp_eq_F4.o : ./AUX/cmp_eq_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_F4.c 
cmp_ne_F4.o : ./AUX/cmp_ne_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_F4.c 
cmp_gt_F4.o : ./AUX/cmp_gt_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_F4.c 
cmp_lt_F4.o : ./AUX/cmp_lt_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_F4.c 
#---------------------------------------------
cmp_ge_I8.o : ./AUX/cmp_ge_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_I8.c 
cmp_le_I8.o : ./AUX/cmp_le_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_I8.c 
cmp_eq_I8.o : ./AUX/cmp_eq_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_I8.c 
cmp_ne_I8.o : ./AUX/cmp_ne_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_I8.c 
cmp_gt_I8.o : ./AUX/cmp_gt_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_I8.c 
cmp_lt_I8.o : ./AUX/cmp_lt_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_I8.c 
#---------------------------------------------
cmp_lt_or_gt_I4.o : ./AUX/cmp_lt_or_gt_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_or_gt_I4.c 
cmp_le_or_ge_I4.o : ./AUX/cmp_le_or_ge_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_or_ge_I4.c 
cmp_ge_and_le_I4.o : ./AUX/cmp_ge_and_le_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_and_le_I4.c
cmp_gt_and_lt_I4.o : ./AUX/cmp_gt_and_lt_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_and_lt_I4.c
#---------------------------------------------
cmp_eq_mult_scalar_I1.o : ./AUX/cmp_eq_mult_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_I1.c 
cmp_eq_mult_scalar_I2.o : ./AUX/cmp_eq_mult_scalar_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_I2.c 
cmp_eq_mult_scalar_I4.o : ./AUX/cmp_eq_mult_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_I4.c 
cmp_eq_mult_scalar_I8.o : ./AUX/cmp_eq_mult_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_I8.c 
cmp_eq_mult_scalar_simple_I1.o : ./AUX/cmp_eq_mult_scalar_simple_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_simple_I1.c 
cmp_eq_mult_scalar_simple_I2.o : ./AUX/cmp_eq_mult_scalar_simple_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_simple_I2.c 
cmp_eq_mult_scalar_simple_I4.o : ./AUX/cmp_eq_mult_scalar_simple_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_simple_I4.c 
cmp_eq_mult_scalar_simple_I8.o : ./AUX/cmp_eq_mult_scalar_simple_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_simple_I8.c 
cmp_eq_mult_scalar_log_I1.o : ./AUX/cmp_eq_mult_scalar_log_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_log_I1.c 
cmp_eq_mult_scalar_log_I2.o : ./AUX/cmp_eq_mult_scalar_log_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_log_I2.c 
cmp_eq_mult_scalar_log_I4.o : ./AUX/cmp_eq_mult_scalar_log_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_log_I4.c 
cmp_eq_mult_scalar_log_I8.o : ./AUX/cmp_eq_mult_scalar_log_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_mult_scalar_log_I8.c 
add_scalar_I2.o : ./AUX/add_scalar_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add_scalar_I2.c 
sub_scalar_I2.o : ./AUX/sub_scalar_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub_scalar_I2.c 
mul_scalar_I2.o : ./AUX/mul_scalar_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul_scalar_I2.c 
div_scalar_I2.o : ./AUX/div_scalar_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div_scalar_I2.c 
rem_scalar_I2.o : ./AUX/rem_scalar_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/rem_scalar_I2.c 
add_scalar_I4.o : ./AUX/add_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add_scalar_I4.c 
sub_scalar_I4.o : ./AUX/sub_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub_scalar_I4.c 
mul_scalar_I4.o : ./AUX/mul_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul_scalar_I4.c 
div_scalar_I4.o : ./AUX/div_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div_scalar_I4.c 
rem_scalar_I4.o : ./AUX/rem_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/rem_scalar_I4.c 
add_scalar_I8.o : ./AUX/add_scalar_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add_scalar_I8.c  
sub_scalar_I8.o : ./AUX/sub_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub_scalar_I8.c 
mul_scalar_I8.o : ./AUX/mul_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul_scalar_I8.c 
div_scalar_I8.o : ./AUX/div_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div_scalar_I8.c 
rem_scalar_I8.o : ./AUX/rem_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/rem_scalar_I8.c 
add_scalar_F8.o : ./AUX/add_scalar_F8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add_scalar_F8.c  
sub_scalar_F8.o : ./AUX/sub_scalar_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub_scalar_F8.c 
mul_scalar_F8.o : ./AUX/mul_scalar_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul_scalar_F8.c 
div_scalar_F8.o : ./AUX/div_scalar_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div_scalar_F8.c 
add_scalar_F4.o : ./AUX/add_scalar_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/add_scalar_F4.c  
sub_scalar_F4.o : ./AUX/sub_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sub_scalar_F4.c 
mul_scalar_F4.o : ./AUX/mul_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/mul_scalar_F4.c 
div_scalar_F4.o : ./AUX/div_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/div_scalar_F4.c 
and_scalar_I1.o : ./AUX/and_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/and_scalar_I1.c 
or_scalar_I1.o : ./AUX/or_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/or_scalar_I1.c 
a_and_not_b_I1.o : ./AUX/a_and_not_b_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/a_and_not_b_I1.c 
bwise_a_and_not_b.o : bwise_a_and_not_b.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c bwise_a_and_not_b.c 
bwise_and_scalar_I4.o : ./AUX/bwise_and_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_and_scalar_I4.c 
bwise_and_scalar_I8.o : ./AUX/bwise_and_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_and_scalar_I8.c 
bwise_and_scalar_I1.o : ./AUX/bwise_and_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_and_scalar_I1.c 
shift_left_scalar_I4.o : ./AUX/shift_left_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shift_left_scalar_I4.c 
shift_left_scalar_I8.o : ./AUX/shift_left_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shift_left_scalar_I8.c 
shift_right_scalar_UI4.o : ./AUX/shift_right_scalar_UI4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shift_right_scalar_UI4.c 
shift_right_scalar_UI8.o : ./AUX/shift_right_scalar_UI8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/shift_right_scalar_UI8.c 
bwise_or_scalar_I4.o : ./AUX/bwise_or_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_or_scalar_I4.c 
bwise_or_scalar_I8.o : ./AUX/bwise_or_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_or_scalar_I8.c 
bwise_or_scalar_I1.o : ./AUX/bwise_or_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_or_scalar_I1.c 
bwise_xor_scalar_I4.o : ./AUX/bwise_xor_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_xor_scalar_I4.c 
bwise_xor_scalar_I8.o : ./AUX/bwise_xor_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_xor_scalar_I8.c 
bwise_xor_scalar_I1.o : ./AUX/bwise_xor_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bwise_xor_scalar_I1.c 
cmp_le_scalar_I4.o : ./AUX/cmp_le_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_scalar_I4.c 
cmp_ge_scalar_I4.o : ./AUX/cmp_ge_scalar_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_scalar_I4.c  
cmp_eq_scalar_I2.o : ./AUX/cmp_eq_scalar_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_scalar_I2.c 
cmp_eq_scalar_I4.o : ./AUX/cmp_eq_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_scalar_I4.c 
cmp_ne_scalar_I4.o : ./AUX/cmp_ne_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_scalar_I4.c 
cmp_gt_scalar_I4.o : ./AUX/cmp_gt_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_scalar_I4.c 
cmp_lt_scalar_I4.o : ./AUX/cmp_lt_scalar_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_scalar_I4.c 
cmp_le_scalar_I8.o : ./AUX/cmp_le_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_scalar_I8.c 
cmp_ge_scalar_I8.o : ./AUX/cmp_ge_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_scalar_I8.c 
cmp_eq_scalar_I8.o : ./AUX/cmp_eq_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_scalar_I8.c 
cmp_ne_scalar_I8.o : ./AUX/cmp_ne_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_scalar_I8.c 
cmp_lt_scalar_I8.o : ./AUX/cmp_lt_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_scalar_I8.c 
cmp_gt_scalar_I8.o : ./AUX/cmp_gt_scalar_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_scalar_I8.c 
cmp_le_scalar_F4.o : ./AUX/cmp_le_scalar_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_scalar_F4.c  
cmp_ge_scalar_F4.o : ./AUX/cmp_ge_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_scalar_F4.c 
cmp_eq_scalar_F4.o : ./AUX/cmp_eq_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_scalar_F4.c 
cmp_ne_scalar_F4.o : ./AUX/cmp_ne_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_scalar_F4.c 
cmp_lt_scalar_F4.o : ./AUX/cmp_lt_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_scalar_F4.c 
cmp_gt_scalar_F4.o : ./AUX/cmp_gt_scalar_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_scalar_F4.c 
cmp_le_scalar_I1.o : ./AUX/cmp_le_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_le_scalar_I1.c 
cmp_ge_scalar_I1.o : ./AUX/cmp_ge_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ge_scalar_I1.c 
cmp_eq_scalar_I1.o : ./AUX/cmp_eq_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_eq_scalar_I1.c 
cmp_ne_scalar_I1.o : ./AUX/cmp_ne_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_ne_scalar_I1.c 
cmp_lt_scalar_I1.o : ./AUX/cmp_lt_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_lt_scalar_I1.c 
cmp_gt_scalar_I1.o : ./AUX/cmp_gt_scalar_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cmp_gt_scalar_I1.c 
conv_I1_to_B.o : ./AUX/conv_I1_to_B.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I1_to_B.c 
conv_B_to_I1.o : ./AUX/conv_B_to_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_B_to_I1.c 
conv_F8_to_I4.o : ./AUX/conv_F8_to_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_F8_to_I4.c 
conv_F8_to_I8.o : ./AUX/conv_F8_to_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_F8_to_I8.c  
conv_F8_to_F4.o : ./AUX/conv_F8_to_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_F8_to_F4.c  
conv_I2_to_I4.o : ./AUX/conv_I2_to_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I2_to_I4.c  
conv_I2_to_I8.o : ./AUX/conv_I2_to_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I2_to_I8.c  
conv_I4_to_I1.o : ./AUX/conv_I4_to_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I4_to_I1.c  
conv_I4_to_I2.o : ./AUX/conv_I4_to_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I4_to_I2.c  
conv_I4_to_I8.o : ./AUX/conv_I4_to_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I4_to_I8.c  
conv_I4_to_F4.o : ./AUX/conv_I4_to_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I4_to_F4.c  
conv_I4_to_F8.o : ./AUX/conv_I4_to_F8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I4_to_F8.c  
conv_I8_to_I4.o : ./AUX/conv_I8_to_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I8_to_I4.c  
conv_I8_to_I1.o : ./AUX/conv_I8_to_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I8_to_I1.c  
conv_I8_to_F4.o : ./AUX/conv_I8_to_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I8_to_F4.c  
conv_I8_to_F8.o : ./AUX/conv_I8_to_F8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I8_to_F8.c  
conv_I1_to_I4.o : ./AUX/conv_I1_to_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I1_to_I4.c  
conv_I1_to_I8.o : ./AUX/conv_I1_to_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_I1_to_I8.c  
conv_F4_to_I4.o : ./AUX/conv_F4_to_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_F4_to_I4.c  
conv_F4_to_I8.o : ./AUX/conv_F4_to_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_F4_to_I8.c  
conv_F4_to_F8.o : ./AUX/conv_F4_to_F8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/conv_F4_to_F8.c  
ones_complement_I4.o : ./AUX/ones_complement_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/ones_complement_I4.c  
not_I4.o : ./AUX/not_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/not_I4.c  
ones_complement_I8.o : ./AUX/ones_complement_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/ones_complement_I8.c  
not_I8.o : ./AUX/not_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/not_I8.c  
ones_complement_I1.o : ./AUX/ones_complement_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/ones_complement_I1.c  
not_I1.o : ./AUX/not_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/not_I1.c  
incr_I4.o : ./AUX/incr_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/incr_I4.c  
decr_I4.o : ./AUX/decr_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/decr_I4.c  
incr_I8.o : ./AUX/incr_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/incr_I8.c  
decr_I8.o : ./AUX/decr_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/decr_I8.c 
cum_I4_to_I4.o : ./AUX/cum_I4_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_I4_to_I4.c 
cum_I4_to_I8.o : ./AUX/cum_I4_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_I4_to_I8.c 
cum_I8_to_I4.o : ./AUX/cum_I8_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_I8_to_I4.c 
cum_I8_to_I8.o : ./AUX/cum_I8_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_I8_to_I8.c 
cum_I1_to_I4.o : ./AUX/cum_I1_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_I1_to_I4.c 
cum_I1_to_I8.o : ./AUX/cum_I1_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_I1_to_I8.c 
cum_reset_int_to_I4.o : ./AUX/cum_reset_int_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_reset_int_to_I4.c 
cum_reset_int_to_I8.o : ./AUX/cum_reset_int_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_reset_int_to_I8.c 
cum_reset_longlong_to_I4.o : ./AUX/cum_reset_longlong_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_reset_longlong_to_I4.c 
cum_reset_longlong_to_I8.o : ./AUX/cum_reset_longlong_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_reset_longlong_to_I8.c 
cum_reset_bool_to_I4.o : ./AUX/cum_reset_bool_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_reset_bool_to_I4.c 
cum_reset_bool_to_I8.o : ./AUX/cum_reset_bool_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/cum_reset_bool_to_I8.c 
sort_asc_I4.o : ./AUX/sort_asc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_asc_I4.c 
sort_dsc_I4.o : ./AUX/sort_dsc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_dsc_I4.c 
sort_asc_I8.o : ./AUX/sort_asc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_asc_I8.c 
sort_dsc_I8.o : ./AUX/sort_dsc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_dsc_I8.c 
sort_asc_F4.o : ./AUX/sort_asc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_asc_F4.c 
sort_dsc_F4.o : ./AUX/sort_dsc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_dsc_F4.c 
sort_asc_F8.o : ./AUX/sort_asc_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_asc_F8.c 
sort_dsc_F8.o : ./AUX/sort_dsc_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort_dsc_F8.c 
sort2_asc_I4.o : ./AUX/sort2_asc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort2_asc_I4.c 
sort2_dsc_I4.o : ./AUX/sort2_dsc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort2_dsc_I4.c 
sort2_asc_I8.o : ./AUX/sort2_asc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort2_asc_I8.c 
sort2_dsc_I8.o : ./AUX/sort2_dsc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort2_dsc_I8.c 
sort2_asc_float.o : ./AUX/sort2_asc_float.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort2_asc_float.c 
sort2_dsc_float.o : ./AUX/sort2_dsc_float.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort2_dsc_float.c 
sort3_asc_I4.o : ./AUX/sort3_asc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort3_asc_I4.c 
sort3_dsc_I4.o : ./AUX/sort3_dsc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort3_dsc_I4.c 
sort3_asc_I8.o : ./AUX/sort3_asc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort3_asc_I8.c 
sort3_dsc_I8.o : ./AUX/sort3_dsc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort3_dsc_I8.c 
sort3_asc_F4.o : ./AUX/sort3_asc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort3_asc_F4.c 
sort3_dsc_F4.o : ./AUX/sort3_dsc_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/sort3_dsc_F4.c  
uniq_I4.o : ./AUX/uniq_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_I4.c  
uniq_I8.o : ./AUX/uniq_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_I8.c  
uniq_cnt_I4.o : ./AUX/uniq_cnt_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_cnt_I4.c  
uniq_cnt_I8.o : ./AUX/uniq_cnt_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_cnt_I8.c  
uniq_alldef_I4.o : ./AUX/uniq_alldef_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_alldef_I4.c  
uniq_alldef_I8.o : ./AUX/uniq_alldef_I8.c
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_alldef_I8.c
uniq_cnt_alldef_I4.o : ./AUX/uniq_cnt_alldef_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_cnt_alldef_I4.c 
uniq_cnt_alldef_I8.o : ./AUX/uniq_cnt_alldef_I8.c
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/uniq_cnt_alldef_I8.c
bitcount_I4.o : ./AUX/bitcount_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bitcount_I4.c  
bitcount_I8.o : ./AUX/bitcount_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bitcount_I8.c  

qsort_asc_I4.o : ./AUX/qsort_asc_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_asc_I4.c  
qsort_asc_I8.o : ./AUX/qsort_asc_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_asc_I8.c  
qsort_dsc_I4.o : ./AUX/qsort_dsc_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_dsc_I4.c  
qsort_dsc_I8.o : ./AUX/qsort_dsc_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_dsc_I8.c  

qsort_with_idx_int.o : ./AUX/qsort_with_idx_int.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_with_idx_int.c  
qsort_with_idx_longlong.o : ./AUX/qsort_with_idx_longlong.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_with_idx_longlong.c  
#------------------------------------------------------------
qsort_asc_val_I4_idx_I4.o : ./AUX/qsort_asc_val_I4_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_asc_val_I4_idx_I4.c  
qsort_asc_val_I4_idx_I8.o : ./AUX/qsort_asc_val_I4_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_asc_val_I4_idx_I8.c  
qsort_asc_val_I8_idx_I4.o : ./AUX/qsort_asc_val_I8_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_asc_val_I8_idx_I4.c  
qsort_asc_val_I8_idx_I8.o : ./AUX/qsort_asc_val_I8_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_asc_val_I8_idx_I8.c  
#--------------------------------------------------------------------------
qsort_dsc_val_I4_idx_I4.o : ./AUX/qsort_dsc_val_I4_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_dsc_val_I4_idx_I4.c  
qsort_dsc_val_I4_idx_I8.o : ./AUX/qsort_dsc_val_I4_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_dsc_val_I4_idx_I8.c  
qsort_dsc_val_I8_idx_I4.o : ./AUX/qsort_dsc_val_I8_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_dsc_val_I8_idx_I4.c  
qsort_dsc_val_I8_idx_I8.o : ./AUX/qsort_dsc_val_I8_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/qsort_dsc_val_I8_idx_I8.c  
#--------------------------------------------------------------------------
core_srt_join_I_I_I.o : ./AUX/core_srt_join_I_I_I.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_I_I_I.c  
core_srt_join_I_I_L.o : ./AUX/core_srt_join_I_I_L.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_I_I_L.c  
core_srt_join_L_I_I.o : ./AUX/core_srt_join_L_I_I.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_L_I_I.c  
core_srt_join_L_I_L.o : ./AUX/core_srt_join_L_I_L.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_L_I_L.c  
core_srt_join_I_L_I.o : ./AUX/core_srt_join_I_L_I.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_I_L_I.c  
core_srt_join_I_L_L.o : ./AUX/core_srt_join_I_L_L.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_I_L_L.c  
core_srt_join_L_L_I.o : ./AUX/core_srt_join_L_L_I.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_L_L_I.c  
core_srt_join_L_L_L.o : ./AUX/core_srt_join_L_L_L.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_srt_join_L_L_L.c  
#--------------------------------------------------------------------------
vec_abs_F4.o : ./AUX/vec_abs_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/vec_abs_F4.c  
vec_abs_F8.o : ./AUX/vec_abs_F8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/vec_abs_F8.c  
vec_sqrt_F4.o : ./AUX/vec_sqrt_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/vec_sqrt_F4.c  
vec_sqrt_F8.o : ./AUX/vec_sqrt_F8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/vec_sqrt_F8.c  
vec_reciprocal_F4.o : ./AUX/vec_reciprocal_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/vec_reciprocal_F4.c  
vec_reciprocal_F8.o : ./AUX/vec_reciprocal_F8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/vec_reciprocal_F8.c  
#--------------------------------------------------------------------------
core_crossprod_I_I.o : ./AUX/core_crossprod_I_I.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_crossprod_I_I.c  
core_crossprod_I_L.o : ./AUX/core_crossprod_I_L.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_crossprod_I_L.c  
core_crossprod_L_I.o : ./AUX/core_crossprod_L_I.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_crossprod_L_I.c  
core_crossprod_L_L.o : ./AUX/core_crossprod_L_L.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_crossprod_L_L.c  
#--------------------------------------------------------------------------
core_wisifxthenyelsez_I4.o : ./AUX/core_wisifxthenyelsez_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_wisifxthenyelsez_I4.c  
core_wisifxthenyelsez_I8.o : ./AUX/core_wisifxthenyelsez_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_wisifxthenyelsez_I8.c  
core_wisifxthenyelsez_I1.o : ./AUX/core_wisifxthenyelsez_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_wisifxthenyelsez_I1.c  
core_wisifxthenyelsez_F4.o : ./AUX/core_wisifxthenyelsez_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/core_wisifxthenyelsez_F4.c  
#--------------------------------------------------------------------------
is_sorted_alldef_I1.o : ./AUX/is_sorted_alldef_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/is_sorted_alldef_I1.c  
is_sorted_alldef_I2.o : ./AUX/is_sorted_alldef_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/is_sorted_alldef_I2.c  
is_sorted_alldef_I4.o : ./AUX/is_sorted_alldef_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/is_sorted_alldef_I4.c  
is_sorted_alldef_I8.o : ./AUX/is_sorted_alldef_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/is_sorted_alldef_I8.c  
#--------------------------------------------------------------------------
pos_copy_I4_I4.o : ./AUX/pos_copy_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_copy_I4_I4.c  
pos_copy_I4_I8.o : ./AUX/pos_copy_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_copy_I4_I8.c  
pos_copy_I8_I4.o : ./AUX/pos_copy_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_copy_I8_I4.c  
pos_copy_I8_I8.o : ./AUX/pos_copy_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_copy_I8_I8.c  
#--------------------------------------------------------------------------
pos_nn_copy_I1_I4.o : ./AUX/pos_nn_copy_I1_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_nn_copy_I1_I4.c  
pos_nn_copy_I1_I8.o : ./AUX/pos_nn_copy_I1_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_nn_copy_I1_I8.c  
pos_nn_copy_I4_I4.o : ./AUX/pos_nn_copy_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_nn_copy_I4_I4.c  
pos_nn_copy_I4_I8.o : ./AUX/pos_nn_copy_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_nn_copy_I4_I8.c  
pos_nn_copy_I8_I4.o : ./AUX/pos_nn_copy_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_nn_copy_I8_I4.c  
pos_nn_copy_I8_I8.o : ./AUX/pos_nn_copy_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_nn_copy_I8_I8.c  
#--------------------------------------------------------------------------
pos_sel_copy_I4_I4.o : ./AUX/pos_sel_copy_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_copy_I4_I4.c  
pos_sel_copy_I4_I8.o : ./AUX/pos_sel_copy_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_copy_I4_I8.c  
pos_sel_copy_I8_I4.o : ./AUX/pos_sel_copy_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_copy_I8_I4.c  
pos_sel_copy_I8_I8.o : ./AUX/pos_sel_copy_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_copy_I8_I8.c  
#--------------------------------------------------------------------------
pos_sel_aux_copy_I4_I4.o : ./AUX/pos_sel_aux_copy_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_copy_I4_I4.c  
pos_sel_aux_copy_I4_I8.o : ./AUX/pos_sel_aux_copy_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_copy_I4_I8.c  
pos_sel_aux_copy_I8_I4.o : ./AUX/pos_sel_aux_copy_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_copy_I8_I4.c  
pos_sel_aux_copy_I8_I8.o : ./AUX/pos_sel_aux_copy_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_copy_I8_I8.c  
#--------------------------------------------------------------------------
pos_sel_aux_nn_copy_I4_I4.o : ./AUX/pos_sel_aux_nn_copy_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_nn_copy_I4_I4.c  
pos_sel_aux_nn_copy_I4_I8.o : ./AUX/pos_sel_aux_nn_copy_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_nn_copy_I4_I8.c  
pos_sel_aux_nn_copy_I8_I4.o : ./AUX/pos_sel_aux_nn_copy_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_nn_copy_I8_I4.c  
pos_sel_aux_nn_copy_I8_I8.o : ./AUX/pos_sel_aux_nn_copy_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_sel_aux_nn_copy_I8_I8.c  
#--------------------------------------------------------------------------
pos_count_I4_I4.o : ./AUX/pos_count_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_I4_I4.c  
pos_count_I4_I8.o : ./AUX/pos_count_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_I4_I8.c  
pos_count_I8_I4.o : ./AUX/pos_count_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_I8_I4.c  
pos_count_I8_I8.o : ./AUX/pos_count_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_I8_I8.c  
#-----------------------------------------------------------------------
pos_count_bitvec_I4_I4.o : ./AUX/pos_count_bitvec_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_bitvec_I4_I4.c  
pos_count_bitvec_I4_I8.o : ./AUX/pos_count_bitvec_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_bitvec_I4_I8.c  
pos_count_bitvec_I8_I4.o : ./AUX/pos_count_bitvec_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_bitvec_I8_I4.c  
pos_count_bitvec_I8_I8.o : ./AUX/pos_count_bitvec_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_count_bitvec_I8_I8.c  
#-----------------------------------------------------------------------
pos_mmad_I4_I4.o : ./AUX/pos_mmad_I4_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I4_I4.c  
pos_mmad_I4_I8.o : ./AUX/pos_mmad_I4_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I4_I8.c  
pos_mmad_I8_I4.o : ./AUX/pos_mmad_I8_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I8_I4.c  
pos_mmad_I8_I8.o : ./AUX/pos_mmad_I8_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I8_I8.c  
#--------------------------------------------------------------------------
pos_mmad_I4_I4_nn.o : ./AUX/pos_mmad_I4_I4_nn.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I4_I4_nn.c  
pos_mmad_I4_I8_nn.o : ./AUX/pos_mmad_I4_I8_nn.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I4_I8_nn.c  
pos_mmad_I8_I4_nn.o : ./AUX/pos_mmad_I8_I4_nn.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I8_I4_nn.c  
pos_mmad_I8_I8_nn.o : ./AUX/pos_mmad_I8_I8_nn.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/pos_mmad_I8_I8_nn.c  
#--------------------------------------------------------------------------
bin_search_I4.o : ./AUX/bin_search_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bin_search_I4.c  
bin_search_I8.o : ./AUX/bin_search_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bin_search_I8.c  
bin_search_UI4.o : ./AUX/bin_search_UI4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bin_search_UI4.c  
bin_search_UI8.o : ./AUX/bin_search_UI8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/bin_search_UI8.c  
#--------------------------------------------------------------------------
countf_nn_I1.o : ./AUX/countf_nn_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_nn_I1.c  
countf_nn_I2.o : ./AUX/countf_nn_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_nn_I2.c  
countf_nn_I4.o : ./AUX/countf_nn_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_nn_I4.c  
countf_nn_I8.o : ./AUX/countf_nn_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_nn_I8.c  
#--------------------------------------------------------------------------
countf_I1.o : ./AUX/countf_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_I1.c  
countf_I2.o : ./AUX/countf_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_I2.c  
countf_I4.o : ./AUX/countf_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_I4.c  
countf_I8.o : ./AUX/countf_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/countf_I8.c  
#--------------------------------------------------------------------------
count_nn_I1.o : ./AUX/count_nn_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_nn_I1.c  
count_nn_I2.o : ./AUX/count_nn_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_nn_I2.c  
count_nn_I4.o : ./AUX/count_nn_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_nn_I4.c  
count_nn_I8.o : ./AUX/count_nn_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_nn_I8.c  
#--------------------------------------------------------------------------
count_I1.o : ./AUX/count_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_I1.c  
count_I2.o : ./AUX/count_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_I2.c  
count_I4.o : ./AUX/count_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_I4.c  
count_I8.o : ./AUX/count_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/count_I8.c  
#--------------------------------------------------------------------------
which_bin_I4.o : ./AUX/which_bin_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/which_bin_I4.c  
which_bin_I8.o : ./AUX/which_bin_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/which_bin_I8.c  
#--------------------------------------------------------------------------
union_I4.o : ./AUX/union_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/union_I4.c  
union_I8.o : ./AUX/union_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUX/union_I8.c  

#--------------------------------------------------------------------------
# dir_helpers
vec_normal_cdf_inverse.o: ./dir_helpers/vec_normal_cdf_inverse.c ./dir_helpers/vec_normal_cdf_inverse.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/vec_normal_cdf_inverse.c
vec_pval_from_zval.o: ./dir_helpers/vec_pval_from_zval.c ./dir_helpers/vec_pval_from_zval.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/vec_pval_from_zval.c
concat_I4_I4_to_I8.o: ./dir_helpers/concat_I4_I4_to_I8.c ./dir_helpers/concat_I4_I4_to_I8.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/concat_I4_I4_to_I8.c
bwise_a_and_not_b_I8.o: ./dir_helpers/bwise_a_and_not_b_I8.c ./dir_helpers/bwise_a_and_not_b_I8.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/bwise_a_and_not_b_I8.c
par_s_to_f.o: ./dir_helpers/par_s_to_f.c ./dir_helpers/par_s_to_f.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/par_s_to_f.c
read_meta_data.o: ./dir_helpers/read_meta_data.c ./dir_helpers/read_meta_data.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/read_meta_data.c
read_csv.o: ./dir_helpers/read_csv.c ./dir_helpers/read_csv.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/read_csv.c
pr_fld_clob.o: ./dir_helpers/pr_fld_clob.c ./dir_helpers/pr_fld_clob.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/pr_fld_clob.c
pr_fld_var_clob.o: ./dir_helpers/pr_fld_var_clob.c ./dir_helpers/pr_fld_var_clob.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/pr_fld_var_clob.c
ext_f_to_s.o: ./dir_helpers/ext_f_to_s.c ./dir_helpers/ext_f_to_s.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/ext_f_to_s.c
app_fld.o: ./dir_helpers/app_fld.c ./dir_helpers/app_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/app_fld.c
f1opf2_shift.o: ./dir_helpers/f1opf2_shift.c ./dir_helpers/f1opf2_shift.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/f1opf2_shift.c
nn_core.o: ./dir_helpers/nn_core.c ./dir_helpers/nn_core.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/nn_core.c
vec_f1f2opf3.o: ./dir_helpers/vec_f1f2opf3.c ./dir_helpers/vec_f1f2opf3.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/vec_f1f2opf3.c
mk_dict.o: ./dir_helpers/mk_dict.c ./dir_helpers/mk_dict.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/mk_dict.c
rd_dict.o: ./dir_helpers/rd_dict.c ./dir_helpers/rd_dict.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/rd_dict.c
vec_f1opf2.o: ./dir_helpers/vec_f1opf2.c ./dir_helpers/vec_f1opf2.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/vec_f1opf2.c
f1opf2_cum.o: ./dir_helpers/f1opf2_cum.c ./dir_helpers/f1opf2_cum.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/f1opf2_cum.c
regex_match.o: ./dir_helpers/regex_match.c ./dir_helpers/regex_match.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/regex_match.c
vec_f1s1opf2.o: ./dir_helpers/vec_f1s1opf2.c ./dir_helpers/vec_f1s1opf2.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/vec_f1s1opf2.c
zero_after_n.o: ./dir_helpers/zero_after_n.c ./dir_helpers/zero_after_n.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/zero_after_n.c
smear.o: ./dir_helpers/smear.c ./dir_helpers/smear.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_helpers/smear.c
#--------------------------------------------------------------------------
# dir_ro_verbs
is_tbl.o: ./dir_ro_verbs/is_tbl.c ./dir_ro_verbs/is_tbl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/is_tbl.c
is_fld.o: ./dir_ro_verbs/is_fld.c ./dir_ro_verbs/is_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/is_fld.c
meta_data.o: ./dir_ro_verbs/meta_data.c ./dir_ro_verbs/meta_data.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/meta_data.c
list_files.o: ./dir_ro_verbs/list_files.c ./dir_ro_verbs/list_files.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/list_files.c
get_meta.o: ./dir_ro_verbs/get_meta.c ./dir_ro_verbs/get_meta.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/get_meta.c
list_tbls.o: ./dir_ro_verbs/list_tbls.c ./dir_ro_verbs/list_tbls.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/list_tbls.c
pr_fld.o: ./dir_ro_verbs/pr_fld.c ./dir_ro_verbs/pr_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/pr_fld.c
tbl_meta.o: ./dir_ro_verbs/tbl_meta.c ./dir_ro_verbs/tbl_meta.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/tbl_meta.c
fld_meta.o: ./dir_ro_verbs/fld_meta.c ./dir_ro_verbs/fld_meta.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/fld_meta.c
dump.o: ./dir_ro_verbs/dump.c ./dir_ro_verbs/dump.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/dump.c
get_nR.o: ./dir_ro_verbs/get_nR.c ./dir_ro_verbs/get_nR.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/get_nR.c
is_nn_fld.o: ./dir_ro_verbs/is_nn_fld.c ./dir_ro_verbs/is_nn_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_ro_verbs/is_nn_fld.c
#--------------------------------------------------------------------------
# dir_wr_verbs
t1f1t2f2opt3f3.o: ./dir_wr_verbs/t1f1t2f2opt3f3.c ./dir_wr_verbs/t1f1t2f2opt3f3.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/t1f1t2f2opt3f3.c

set_meta.o: ./dir_wr_verbs/set_meta.c ./dir_wr_verbs/set_meta.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/set_meta.c
dld.o: ./dir_wr_verbs/dld.c ./dir_wr_verbs/dld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/dld.c
wisifxthenyelsez.o: ./dir_wr_verbs/wisifxthenyelsez.c ./dir_wr_verbs/wisifxthenyelsez.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/wisifxthenyelsez.c
sort.o: ./dir_wr_verbs/sort.c ./dir_wr_verbs/sort.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/sort.c
add_tbl.o: ./dir_wr_verbs/add_tbl.c ./dir_wr_verbs/add_tbl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/add_tbl.c
stride.o: ./dir_wr_verbs/stride.c ./dir_wr_verbs/stride.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/stride.c
del_tbl.o: ./dir_wr_verbs/del_tbl.c ./dir_wr_verbs/del_tbl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/del_tbl.c
del_fld.o: ./dir_wr_verbs/del_fld.c ./dir_wr_verbs/del_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/del_fld.c
ext_s_to_f.o: ./dir_wr_verbs/ext_s_to_f.c ./dir_wr_verbs/ext_s_to_f.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/ext_s_to_f.c
add_fld.o: ./dir_wr_verbs/add_fld.c ./dir_wr_verbs/add_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/add_fld.c
crossprod.o: ./dir_wr_verbs/crossprod.c ./dir_wr_verbs/crossprod.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/crossprod.c
pack.o: ./dir_wr_verbs/pack.c ./dir_wr_verbs/pack.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/pack.c
unpack.o: ./dir_wr_verbs/unpack.c ./dir_wr_verbs/unpack.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/unpack.c
lkpsort.o: ./dir_wr_verbs/lkpsort.c ./dir_wr_verbs/lkpsort.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/lkpsort.c
rng_sort.o: ./dir_wr_verbs/rng_sort.c ./dir_wr_verbs/rng_sort.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/rng_sort.c
f1f2_to_s.o: ./dir_wr_verbs/f1f2_to_s.c ./dir_wr_verbs/f1f2_to_s.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/f1f2_to_s.c
t1f1opt2f2.o: ./dir_wr_verbs/t1f1opt2f2.c ./dir_wr_verbs/t1f1opt2f2.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/t1f1opt2f2.c
num_in_range.o: ./dir_wr_verbs/num_in_range.c ./dir_wr_verbs/num_in_range.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/num_in_range.c
app_tbl.o: ./dir_wr_verbs/app_tbl.c ./dir_wr_verbs/app_tbl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/app_tbl.c
set_val.o: ./dir_wr_verbs/set_val.c ./dir_wr_verbs/set_val.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/set_val.c
count_vals.o: ./dir_wr_verbs/count_vals.c ./dir_wr_verbs/count_vals.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/count_vals.c
mk_bins.o: ./dir_wr_verbs/mk_bins.c ./dir_wr_verbs/mk_bins.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/mk_bins.c
f1f2opf3.o: ./dir_wr_verbs/f1f2opf3.c ./dir_wr_verbs/f1f2opf3.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/f1f2opf3.c
is_a_in_b.o: ./dir_wr_verbs/is_a_in_b.c ./dir_wr_verbs/is_a_in_b.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/is_a_in_b.c
is_A_in_b.o: ./dir_wr_verbs/is_A_in_b.c ./dir_wr_verbs/is_A_in_b.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/is_A_in_b.c
dir_is_a_in_b.o: ./dir_wr_verbs/dir_is_a_in_b.c ./dir_wr_verbs/dir_is_a_in_b.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/dir_is_a_in_b.c
copy_fld.o: ./dir_wr_verbs/copy_fld.c ./dir_wr_verbs/copy_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/copy_fld.c
mv_fld.o: ./dir_wr_verbs/mv_fld.c ./dir_wr_verbs/mv_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/mv_fld.c
mk_nn_fld.o: ./dir_wr_verbs/mk_nn_fld.c ./dir_wr_verbs/mk_nn_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/mk_nn_fld.c
copy_range.o: ./dir_wr_verbs/copy_range.c ./dir_wr_verbs/copy_range.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/copy_range.c
range_to_cfld.o: ./dir_wr_verbs/range_to_cfld.c ./dir_wr_verbs/range_to_cfld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/range_to_cfld.c
cfld_to_range.o: ./dir_wr_verbs/cfld_to_range.c ./dir_wr_verbs/cfld_to_range.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/cfld_to_range.c
f1s1opf2.o: ./dir_wr_verbs/f1s1opf2.c ./dir_wr_verbs/f1s1opf2.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/f1s1opf2.c
f1T1opf2.o: ./dir_wr_verbs/f1T1opf2.c ./dir_wr_verbs/f1T1opf2.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/f1T1opf2.c
f1opf2.o: ./dir_wr_verbs/f1opf2.c ./dir_wr_verbs/f1opf2.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/f1opf2.c
f1opf2f3.o: ./dir_wr_verbs/f1opf2f3.c ./dir_wr_verbs/f1opf2f3.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/f1opf2f3.c
srt_join.o: ./dir_wr_verbs/srt_join.c ./dir_wr_verbs/srt_join.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/srt_join.c
rng_join.o: ./dir_wr_verbs/rng_join.c ./dir_wr_verbs/rng_join.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/rng_join.c
count_ht.o: ./dir_wr_verbs/count_ht.c ./dir_wr_verbs/count_ht.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/count_ht.c
count.o: ./dir_wr_verbs/count.c ./dir_wr_verbs/count.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/count.c
countT.o: ./dir_wr_verbs/countT.c ./dir_wr_verbs/countT.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/countT.c
countf.o: ./dir_wr_verbs/countf.c ./dir_wr_verbs/countf.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/countf.c
fop.o: ./dir_wr_verbs/fop.c ./dir_wr_verbs/fop.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/fop.c
udf_eq_B.o: ./dir_wr_verbs/udf_eq_B.c ./dir_wr_verbs/udf_eq_B.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/udf_eq_B.c
sortf1f2.o: ./dir_wr_verbs/sortf1f2.c ./dir_wr_verbs/sortf1f2.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/sortf1f2.c
get_t2f2_val.o: ./dir_wr_verbs/get_t2f2_val.c ./dir_wr_verbs/get_t2f2_val.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/get_t2f2_val.c
bindmp.o: ./dir_wr_verbs/bindmp.c ./dir_wr_verbs/bindmp.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/bindmp.c
binld.o: ./dir_wr_verbs/binld.c ./dir_wr_verbs/binld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/binld.c
drop_nn_fld.o: ./dir_wr_verbs/drop_nn_fld.c ./dir_wr_verbs/drop_nn_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/drop_nn_fld.c
rename_fld.o: ./dir_wr_verbs/rename_fld.c ./dir_wr_verbs/rename_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/rename_fld.c
rename_tbl.o: ./dir_wr_verbs/rename_tbl.c ./dir_wr_verbs/rename_tbl.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/rename_tbl.c
set_nR.o: ./dir_wr_verbs/set_nR.c ./dir_wr_verbs/set_nR.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/set_nR.c
dup_fld.o: ./dir_wr_verbs/dup_fld.c ./dir_wr_verbs/dup_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/dup_fld.c
subsample.o: ./dir_wr_verbs/subsample.c ./dir_wr_verbs/subsample.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_wr_verbs/subsample.c
#--------------------------------------------------------------------------
# dir_dbutils
aux_meta.o: ./dir_dbutils/aux_meta.c ./dir_dbutils/aux_meta.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/aux_meta.c
mk_file.o: ./dir_dbutils/mk_file.c ./dir_dbutils/mk_file.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/mk_file.c
mk_temp_file.o: ./dir_dbutils/mk_temp_file.c ./dir_dbutils/mk_temp_file.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/mk_temp_file.c
dbauxil.o: ./dir_dbutils/dbauxil.c ./dir_dbutils/dbauxil.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/dbauxil.c
logger.o: ./dir_dbutils/logger.c ./dir_dbutils/logger.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/logger.c
open_temp_file.o: ./dir_dbutils/open_temp_file.c ./dir_dbutils/open_temp_file.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/open_temp_file.c
add_aux_fld.o: ./dir_dbutils/add_aux_fld.c ./dir_dbutils/add_aux_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/add_aux_fld.c
get_type_op_fld.o: ./dir_dbutils/get_type_op_fld.c ./dir_dbutils/get_type_op_fld.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_dbutils/get_type_op_fld.c
#--------------------------------------------------------------------------
# dir_utils
md5c.o: ./dir_utils/md5c.c ./dir_utils/md5c.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/md5c.c
auxil.o: ./dir_utils/auxil.c ./dir_utils/auxil.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/auxil.c
mmap.o: ./dir_utils/mmap.c ./dir_utils/mmap.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/mmap.c
mddriver.o: ./dir_utils/mddriver.c ./dir_utils/mddriver.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/mddriver.c
hash_string.o: ./dir_utils/hash_string.c ./dir_utils/hash_string.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/hash_string.c
extract_S.o: ./dir_utils/extract_S.c ./dir_utils/extract_S.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/extract_S.c
MurmurHash3.o: ./dir_utils/MurmurHash3.c ./dir_utils/MurmurHash3.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/MurmurHash3.c
mix.o: ./dir_utils/mix.c ./dir_utils/mix.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/mix.c
find_closest_I4.o: ./dir_utils/find_closest_I4.c ./dir_utils/find_closest_I4.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/find_closest_I4.c
find_largest_leq_I4.o: ./dir_utils/find_largest_leq_I4.c ./dir_utils/find_largest_leq_I4.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/find_largest_leq_I4.c
find_smallest_geq_I4.o: ./dir_utils/find_smallest_geq_I4.c ./dir_utils/find_smallest_geq_I4.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./dir_utils/find_smallest_geq_I4.c
#--------------------------------------------------------------------------
