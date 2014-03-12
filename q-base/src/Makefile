#
# make with c compiler, using 4rd party .h files and .a files
#
#
CC=gcc
ICC=icc

GCCOPTFLAG= -g
ICCOPTFLAG= -g

GCCOPTFLAG= -O4
ICCOPTFLAG= -O3

LINKER=icc


# PICFLAG=-fPIC: add -fPIC when compiling q.a for shell loadable builtin
PICFLAG=
GCCFLG = ${GCCOPTFLAG} -ansi -pedantic -std=gnu99 $(PICFLAG) -Wall -fopenmp
ICCFLG = ${ICCOPTFLAG}       -pedantic -std=c99  $(PICFLAG) -Wall 
DBG = -DDEBUG 
CFLAGS= ${GCCFLG} ${DBG} 
ICCFLAGS= ${ICCFLG} ${DBG} -ipp -DIPP -DCILK 

## FOLLOWING FOR MAC_OSX
# GCCFLG = ${GCCOPTFLAG} -ansi -pedantic -std=c99 $(PICFLAG) -Wall -DMAC_OSX
# ICCFLG = ${ICCOPTFLAG}       -pedantic -std=c99  $(PICFLAG) -Wall -DMAC_OSX
# ICCFLAGS= ${ICCFLG} ${DBG} 
# ICCOPTFLAG= -O4
# LDB_FOR_MAC=../leveldb-1.12.0/libleveldb.a
# ICC=gcc
# LINKER=g++
## ABOVE FOR MAC_OSX

# >>>>>>> Uncomment following for Valgrind
# ICC=gcc
# ICCFLAGS= ${GCCFLG} ${DBG} ${ERR} ${CHECKS} ${VERBOSE} -fopenmp
# LINKER=g++
# <<<<<<< Uncomment above     for Valgrind
#
# I use IPP in ifdef statements to preserve independence from Intel 
# In other words, every function that I use from IPP has my own
# implementation as well
#
# make variable for compiles with the -I command 
#
#INCLUDES=-I/where/the/header/file/is -I/other/header/files
INCLUDES=-I./AUTOGEN/GENFILES -I. -I/home/rsubramo/WORK/Q/src/ 
# FOR ICC INCLUDES=-I./AUTOGEN/GENFILES -I. -I/home/rsubramo/WORK/Q/src/ -I/home/rsubramo/intel/composer_xe_2013.0.079/compiler/include/


#
# make variable for extra libraries
# link with -L and -l
#
# LIBS=-L/where/the/lib/file/is -L/other/lib/files -lUSR1 -lUSR2 -lOTHER
LIBS=
#
#
.PHONY: all clean 

ALL = \
	q \
	ut_webhdfs \
	qhttpd \
	qtils \
	test_driver_qsort  \
	qldb \

all: ${ALL}

clean:
	rm -f *.o ${ALL} *.so q.a ;
	rm -f incl_f_to_s_*.c ; 
	rm -f ./AUTOGEN/*.o; rm -f orig_meta_globals.c
	make -f ./httpd_Makefile clean

#	is_dict.o \
#	del_dict.o \
#	n_dict.o \
#	list_dicts.o \
#	pr_dict.o \
#
INC1 = constants.h macros.h qtypes.h

RD_META_Q = \
	dbauxil.o \
	auxil.o \
	open_file_in_dir.o  \
	mmap.o \
	mk_file.o \
	logger.o \
 	md5c.o \
	mddriver.o \
	hash_string.o \
	MurmurHash3.o \
	mix.o \
	extract_S.o \
	aux_meta.o \
	url.o \
	\
 	get_type_op_fld.o \
	list_files.o \
	dump.o \
	list_tbls.o \
	is_tbl.o \
	is_fld.o \
	is_aux_fld.o \
	tbl_meta.o \
	fld_meta.o \
	get_meta.o \
	get_nR.o \
	meta_data.o  \
 	assign_I1.o \
 	assign_I2.o \
 	sort_asc_I8.o  \
 	sort_asc_F8.o  \


CPLUSPLUSFILES =  \
	count_vals_maps.cc.o  \
	LevelDict.cc.o \
	DictFunctions.cc.o \
	DBManager.cc.o 


count_vals_maps.cc.o : count_vals_maps.cc
	g++ -ggdb -Wall -fPIC -g -c -O4 count_vals_maps.cc -o count_vals_maps.cc.o

#g++ -ggdb -fPIC $(GCCFLG) count_vals_maps.cc -o count_vals_maps.cc.o

LevelDict.cc.o : LevelDict.cc
	g++ -I../leveldb-1.12.0/include/ -ggdb -Wall -fPIC -g -c LevelDict.cc -o LevelDict.cc.o

DictFunctions.cc.o : DictFunctions.cc
	g++ -I../leveldb-1.12.0/include/ -ggdb -Wall -fPIC -g -c DictFunctions.cc -o DictFunctions.cc.o

DBManager.cc.o : DBManager.cc
	g++ -I../leveldb-1.12.0/include/ -ggdb -Wall -fPIC -g -c DBManager.cc -o DBManager.cc.o

COREQ = \
	q.o \
	q_core.o \
	q_init.o \
	q_batch.o \
	top_n.o \
	count_vals_maps.o \
	count_vals_ldb.o \
	QLevelDB.o \
	$(CPLUSPLUSFILES) \
	compound.o \
	mk_temp_file.o \
	add_tbl.o \
	add_fld.o \
	add_aux_fld.o \
	del_tbl.o \
	del_fld.o \
	set_meta.o \
	unset_meta.o \
 	drop_aux_fld.o \
 	rename_fld.o \
 	rename_tbl.o \
	f_to_s.o \
	vec_f_to_s.o \
	f1f2_to_s.o \
	pr_fld.o \
	pr_fld_SC.o \
	pr_fld_SV.o \
	bindmp.o \
	mk_hashes_I1.o \
	mk_hashes_I2.o \
	mk_hashes_I4.o \
	mk_hashes_I8.o \
	permute_I4.o \
	permute_I8.o \
	saturate_I1.o \
	saturate_I2.o \
	saturate_I4.o \
	saturate_I8.o \
	g_describe.o \
	g_print_reg_data.o \
	g_print_reg_data.o \
	g_f1f2opf3.o \
	g_count.o \
	g_countf.o \
	g_f_to_s.o \
	g_f1f2_to_s.o \
	g_shift.o \
	g_load.o \
	g_aux.o \
	g_meta_data.o \
	g_store.o \
	g_f1s1opf2.o \
	g_del_fld.o \
	g_add_fld.o \
 	set_val.o \
 	get_val.o \
	read_meta_data.o \
	read_csv.o \
	print_cell.o \
	aux_dld.o \
	dld.o \
	binld.o \
  	wisifxthenyelsez.o \
	stride.o \
	s_to_f.o \
	vec_s_to_f.o \
	pack.o \
	unpack.o \
	find_largest_leq_I4.o \
	find_smallest_geq_I4.o \
	find_largest_leq_I8.o \
	find_smallest_geq_I8.o \
	lkp_sort.o \
	rng_sort.o \
 	num_in_range.o \
	crossprod.o \
  	percentiles.o \
 	app_tbl.o \
 	f1opf2_shift.o \
	count_vals.o \
	mk_bins.o \
 	f1f2opf3.o \
 	vec_f1f2opf3.o \
	is_a_in_b.o \
	dir_is_a_in_b.o \
	mk_dict.o \
	rd_dict.o \
 	vec_f1opf2.o \
	conv.o \
 	f1opf2_cum.o \
 	f1opf2_idx_with_reset.o \
 	copy_fld.o \
 	copy_tbl.o \
 	copy_fld_ranges.o \
 	mv_fld.o \
 	mk_nn_fld.o \
 	break_nn_fld.o \
	spooky_hash.o \
	approx_unique.o \
	approx_quantile.o \
	approx_frequent.o \
	sorted_array_to_id_freq.o \
	update_counter.o \
	Collapse.o \
	New.o \
	Output.o \
	determine_b_k.o \
	determine_rho_loc.o \
 	regex_match.o \
 	f1s1opf2.o \
 	f1opf2.o \
	f1opf2f3.o \
 	vec_f1s1opf2.o \
 	srt_join.o \
 	rng_join.o \
	count_ht.o \
	count.o \
	countf.o \
	zero_after_n.o \
	smear.o \
 	is_s_in_f.o \
 	patmatch.o \
 	par_sort_with_bins.o \
 	udf_deg2.o \
 	fop.o \
 	sortf1f2.o \
	xfer.o \
	dup_fld.o \
 	app_fld.o \
	subsample.o \
 	patmatch_I1.o \
 	patmatch_I2.o \
 	patmatch_I4.o \
	extract_I1.o \
	extract_I2.o \
	extract_I4.o \
	extract_I8.o \
	extract_UI8.o \
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
	xfer_SC_I4.o \
	xfer_I1_I4.o \
	xfer_I2_I4.o \
	xfer_I4_I4.o \
	xfer_I8_I4.o \
	xfer_F4_I4.o \
	xfer_F8_I4.o  \
 	qsort_asc_I1.o \
 	qsort_asc_I4.o \
 	qsort_asc_I8.o \
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
 	sort_dsc_F8.o \
 	core_srt_join_I4_I4_I4.o \
 	core_srt_join_I4_I4_I8.o \
 	core_srt_join_I8_I4_I4.o \
 	core_srt_join_I8_I4_I8.o \
 	core_srt_join_I4_I8_I4.o \
 	core_srt_join_I4_I8_I8.o \
 	core_srt_join_I8_I8_I4.o \
 	core_srt_join_I8_I8_I8.o \
 	assign_I4.o \
 	assign_I8.o \
 	is_sorted_alldef_I1.o \
 	is_sorted_alldef_I2.o \
 	is_sorted_alldef_I4.o \
 	is_sorted_alldef_I8.o \
 	cmp_lt_or_gt_I1.o \
 	cmp_le_or_ge_I1.o \
 	cmp_gt_and_lt_I1.o \
 	cmp_ge_and_le_I1.o \
 	cmp_lt_or_gt_I2.o \
 	cmp_le_or_ge_I2.o \
 	cmp_gt_and_lt_I2.o \
 	cmp_ge_and_le_I2.o \
 	cmp_lt_or_gt_I4.o \
 	cmp_le_or_ge_I4.o \
 	cmp_gt_and_lt_I4.o \
 	cmp_ge_and_le_I4.o \
 	cmp_eq_mult_scalar_I1.o \
 	cmp_eq_mult_scalar_I2.o \
 	cmp_eq_mult_scalar_I4.o \
 	cmp_eq_mult_scalar_I8.o \
 	dbg_cmp_neq_mult_scalar_I1.o \
 	dbg_cmp_neq_mult_scalar_I2.o \
 	dbg_cmp_neq_mult_scalar_I4.o \
 	dbg_cmp_neq_mult_scalar_I8.o \
 	dbg_cmp_eq_mult_scalar_I1.o \
 	dbg_cmp_eq_mult_scalar_I2.o \
 	dbg_cmp_eq_mult_scalar_I4.o \
 	dbg_cmp_eq_mult_scalar_I8.o \
 	opt_cmp_eq_mult_scalar_I1.o \
 	opt_cmp_eq_mult_scalar_I2.o \
 	opt_cmp_eq_mult_scalar_I4.o \
 	opt_cmp_eq_mult_scalar_I8.o \
 	add_scalar_I1.o \
 	sub_scalar_I1.o \
 	mul_scalar_I1.o \
 	div_scalar_I1.o \
 	rem_scalar_I1.o \
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
 	bwise_and_scalar_I4.o \
 	bwise_not_and_scalar_I8.o \
 	bwise_not_or_scalar_I8.o \
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
 	cmp_le_scalar_F8.o  \
 	cmp_ge_scalar_F8.o \
 	cmp_eq_scalar_F8.o \
 	cmp_ne_scalar_F8.o \
 	cmp_lt_scalar_F8.o \
 	cmp_gt_scalar_F8.o \
 	cmp_le_scalar_I1.o \
 	cmp_ge_scalar_I1.o \
 	cmp_eq_scalar_I1.o \
 	cmp_ne_scalar_I1.o \
 	cmp_lt_scalar_I1.o \
 	cmp_gt_scalar_I1.o \
 	cmp_le_scalar_I2.o \
 	cmp_ge_scalar_I2.o \
 	cmp_eq_scalar_I2.o \
 	cmp_ne_scalar_I2.o \
 	cmp_lt_scalar_I2.o \
 	cmp_gt_scalar_I2.o \
 	shift_left_scalar_I4.o \
 	shift_left_scalar_I8.o \
 	shift_right_scalar_UI4.o \
 	shift_right_scalar_UI8.o \
 	conv_I1_to_B.o  \
 	conv_B_to_I1.o  \
 	conv_F8_to_I4.o  \
 	conv_F8_to_I8.o  \
 	conv_F8_to_F4.o  \
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
 	nn_conv_F8_to_I4.o  \
 	nn_conv_F8_to_I8.o  \
 	nn_conv_F8_to_F4.o  \
 	nn_conv_I4_to_I1.o  \
 	nn_conv_I4_to_I2.o  \
 	nn_conv_I4_to_I8.o  \
 	nn_conv_I4_to_F4.o  \
 	nn_conv_I4_to_F8.o  \
 	nn_conv_I8_to_I4.o  \
 	nn_conv_I8_to_I1.o  \
 	nn_conv_I8_to_F4.o  \
 	nn_conv_I8_to_F8.o  \
 	nn_conv_I1_to_I4.o  \
 	nn_conv_I1_to_I8.o  \
 	nn_conv_F4_to_I4.o  \
 	nn_conv_F4_to_I8.o  \
 	nn_conv_F4_to_F8.o  \
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
 	cum_I2_to_I4.o \
 	cum_I2_to_I8.o \
	is_s_in_f_seq_I1.o \
	is_s_in_f_seq_I2.o \
	is_s_in_f_seq_I4.o \
	is_s_in_f_seq_I8.o \
 	srt_uniform_I4.o  \
 	srt_uniform_I8.o  \
 	hash_I4.o  \
 	hash_I8.o  \
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
	bin_search_I1.o \
	bin_search_I2.o \
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
	par_count_I1.o \
	par_count_I2.o \
	par_count_cfld_I1.o \
	par_count_cfld_I2.o \
	par_countf_I1.o \
	par_countf_I2.o \
	par_countf_cfld_I1.o \
	par_countf_cfld_I2.o \
	count_nn_I1.o \
	count_nn_I2.o \
	count_nn_I4.o \
	count_nn_I8.o \
 	concat_I4_I4_to_I8.o \
 	add2_I2.o \
 	sub2_I2.o \
 	mul2_I2.o \
 	div2_I2.o \
 	rem2_I2.o \
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
 	bwise_a_and_not_b_I8.o \
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
 	cmp_ge_I2.o \
 	cmp_le_I2.o \
 	cmp_eq_I2.o \
 	cmp_ne_I2.o \
 	cmp_gt_I2.o \
 	cmp_lt_I2.o \
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
 	cmp_ge_F8.o \
 	cmp_le_F8.o \
 	cmp_eq_F8.o \
 	cmp_ne_F8.o \
 	cmp_gt_F8.o \
 	cmp_lt_F8.o \
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
 	core_wisifxthenyelsez_I4.o \
 	core_wisifxthenyelsez_I8.o \
 	core_wisifxthenyelsez_I1.o \
 	core_wisifxthenyelsez_I2.o \
 	core_wisifxthenyelsez_F4.o \
 	union_I4.o  \
 	union_I8.o  \
 	a_minus_b_I4.o  \
 	a_minus_b_I8.o  \
 	par_sort_with_bins_I4.o  \
 	par_sort_with_bins_I8.o  \
 	intersection_I4.o  \
 	intersection_I8.o  \


# 	aux_join.o \
#  	count_range.o \
# 	dateconv.o \
# 	mv_range.o \

# To make q.a, you need to say: make -e "PICFLAG=-fPIC" -e "OPTFLAG=-O4" q.a

# IPP_LIBS=libippi_l.a libipps_l.a libippcore_l.a libippm_l.a 

qldb: LevelDict.cc LevelDict.h DictFunctions.cc DictFunctions.h DBManager.cc DBManager.h qldb.o assign_I1.o auxil.o mmap.o
	g++ -I../leveldb-1.12.0/include/ -ggdb -Wall -fPIC -g -c LevelDict.cc     -o LevelDict.cc.o -DMAC_OSX
	g++ -I../leveldb-1.12.0/include/ -ggdb -Wall -fPIC -g -c DictFunctions.cc -o DictFunctions.cc.o -DMAC_OSX
	g++ -I../leveldb-1.12.0/include/ -ggdb -Wall -fPIC -g -c DBManager.cc     -o DBManager.cc.o -DMAC_OSX 
	g++ -fPIC LevelDict.cc.o DictFunctions.cc.o DBManager.cc.o -lpthread -O2 -shared -o leveldbcxx.a $(LDB_FOR_MAC)
	$(LINKER)  -fPIC -Wall -DMAC_OSX \
	LevelDict.cc.o DictFunctions.cc.o DBManager.cc.o QLevelDB.o \
	qldb.o auxil.o mmap.o assign_I1.o \
	-lm -lpthread leveldbcxx.a  ../leveldb-1.12.0/libleveldb.a -o qldb


# PUT THIS BACK FOR LINUX	cp leveldbcxx.a ${HOME}/local/lib/

q.a: 	$(COREQ) orig_meta_globals.c
	make $(COREQ) 
	ar r q.a $(COREQ) $(IPP_LIBS)
	rm -f $(COREQ)

# USED TO WORK	$(ICC) $(ICCFLAGS) -o $@ $^ -ldl -lm -lpthread $(LDFLAGS) $(IPP_LIBS)

ut_webhdfs : ut_webhdfs.o 
	$(CC) $(CCFLAFS) -o $@ $^ -lcurl

qhttpd: qhttpd.o $(COREQ) $(RD_META_Q)
	$(LINKER) $(ICCFLAGS) -o $@ $^ -ldl -lm -L$(IPPROOT)/lib/intel64 -ipp \
	-lcurl -lpthread ../leveldb-1.12.0/libleveldb.a 

metaq: metaqwrap.o $(RD_META_Q)
	$(CC) $(CFLAGS) -o $@ $^ -ldl -lm 

# USED TO WORK	$(ICC) $(ICCFLAGS) -o $@ $^ -ldl -lm -lpthread $(LDFLAGS) $(IPP_LIBS)
q: qwrap.o $(COREQ) $(RD_META_Q)
# Use g++ instead of ICC below if you want to eliminate Valgrind errors
	$(LINKER) $(ICCFLAGS) -o $@ $^ -ldl -lm \
	-L$(IPPROOT)/lib/intel64 -ipp \
	-fopenmp -lgomp -lcurl -lpthread ../leveldb-1.12.0/libleveldb.a 

# USED TO WORK	$(ICC) $(ICCFLAGS) -o $@ $^ -ldl -lm -lpthread $(LDFLAGS) $(IPP_LIBS)
#	$(ICC) $(ICCFLAGS) -o $@ $^ -ldl -lm -L$(IPPROOT)/lib/intel64 -lippi -lipps -lippcore -lippm -ipp 

ut_mk_dict: ut_mk_dict.o mk_dict.o MurmurHash3.o \
	auxil.o mmap.o mix.o
	$(CC) $(CFLAGS) -o $@ $^ -ldl -lm -lpthread $(LDFLAGS)  

ut_rd_dict: ut_rd_dict.o rd_dict.o MurmurHash3.o \
	auxil.o mmap.o mix.o bin_search_UI8.o 
	$(CC) $(CFLAGS) -o $@ $^ -ldl -lm -lpthread $(LDFLAGS)  

mk_dict.o: mk_dict.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c mk_dict.c

rd_dict.o: rd_dict.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c rd_dict.c



qtils: qtilswrap.o qtils.o time1.o replace_char.o \
	extract_S.o \
	date_to_I4.o \
	tab_split.o \
	aux_meta.o \
	is_tbl.o \
	is_fld.o \
	dbauxil.o \
	auxil.o  mmap.o qhead.o chk_ascii.o \
	esc_for_latex.o hash_string.o md5c.o mddriver.o \
	hdfs_get.o \
	aux_hdfs.o \
	mk_file.o \
	url.o \
	assign_I1.o \
	asc2bin.o bin2asc.o txt_sub.o sortbindmp.o \
	mix.o \
	extract_I4.o \
	mk_ldb_dict.o \
	QLevelDB.o \
	$(CPLUSPLUSFILES) \
 	qsort_dsc_I8.o \
 	qsort_asc_I8.o \
	mk_hashes_I1.o \
	assign_I1.o \
	assign_I2.o \
	assign_I4.o \
	assign_I8.o \
	mk_hashes_I2.o \
	mk_hashes_I4.o \
	mk_hashes_I8.o \
	qsort_asc_I4.o \
	qsort_dsc_I4.o \
	sort_asc_I4.o \
	sort_asc_I8.o \
	sort_asc_F8.o \
	sort_dsc_I4.o \
	sort2_asc_I4.o \
	sort2_dsc_I4.o \
	sort2_asc_I8.o \
	sort2_dsc_I8.o \
	sort2_asc_F4.o \
	sort2_dsc_F4.o \
	sort3_asc_I4.o \
	sort3_dsc_I4.o \
	sort3_asc_I8.o \
	sort3_dsc_I8.o \
	sort4_asc_I8.o \
	sort4_dsc_I8.o \
	sort3_asc_F4.o \
	sort3_dsc_F4.o 
	$(LINKER) -o $@ $^ -ldl -lcurl -lm leveldbcxx.a  ../leveldb-1.12.0/libleveldb.a -L$(IPPROOT) -ipp 

# -L$(IPPROOT)/lib/intel64 -lippi -lipps -lippcore -lippm

orig_meta_globals.c: orig_meta_globals.h
	cp orig_meta_globals.h orig_meta_globals.c

verbatim_include: verbatim_include.o  mmap.o open_file_in_dir.o
	$(CC) $(CFLAGS) -o $@ $^ 

test_driver_qsort: test_driver_qsort.o \
	qsort_with_idx.o \
	qsort_with_idx_int.o \
	qsort_with_idx_longlong.o \
	qsort_dsc_I4.o  \
	qsort_asc_I4.o  \
	qsort_dsc_I8.o  \
	qsort_asc_I8.o  \
	sort_asc_I4.o sort_dsc_I4.o  \
	sort_asc_I8.o sort_dsc_I8.o  \
	qsort_asc_val_I4_idx_I4.o \
	qsort_asc_val_I4_idx_I8.o \
	qsort_asc_val_I8_idx_I4.o \
	qsort_asc_val_I8_idx_I8.o \
	qsort_dsc_val_I4_idx_I4.o \
	qsort_dsc_val_I4_idx_I8.o \
	qsort_dsc_val_I8_idx_I4.o \
	qsort_dsc_val_I8_idx_I8.o 
	$(CC) $(CFLAGS) -o $@ $^ 

qsort_with_idx.o: qsort_with_idx.c $(INC1) 
	$(CC) $(CFLAGS) $(INCLUDES) -c qsort_with_idx.c

test_driver_qsort.o: test_driver_qsort.c $(INC1) 
	$(CC) $(CFLAGS) $(INCLUDES) -c test_driver_qsort.c

gen_data: gen_data.o 
	$(CC) $(CFLAGS) -o $@ $^ 


#
#
# DO NOT include 3rd party headers for your source
# DO     include your own headers
#
qwrap.o: qwrap.c $(INC1) 
	$(CC) $(CFLAGS) $(INCLUDES) -c qwrap.c

qhttpd.o: qhttpd.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c qhttpd.c 

q_init.o: q_init.c $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c q_init.c 

q_core.o: q_core.c $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c q_core.c 

q_batch.o: q_batch.c $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c q_batch.c 

q.o: q.c $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c q.c 

srt_join.o: srt_join.c srt_join.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c srt_join.c

mark_pos.o: mark_pos.c mark_pos.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c mark_pos.c

count.o: count.c count.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c count.c

countf.o: countf.c countf.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c countf.c

f1opf2.o: f1opf2.c f1opf2.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c f1opf2.c

f1opf2f3.o: f1opf2f3.c f1opf2f3.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c f1opf2f3.c

f1T1opf2.o: f1T1opf2.c f1T1opf2.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c f1T1opf2.c

mmap.o: mmap.c mmap.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c mmap.c

is_s_in_f.o: is_s_in_f.c is_s_in_f.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c is_s_in_f.c

zero_after_n.o: zero_after_n.c zero_after_n.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c zero_after_n.c

percentiles.o: percentiles.c percentiles.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c percentiles.c

read_meta_data.o: read_meta_data.c read_meta_data.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c read_meta_data.c

s_to_f.o: s_to_f.c s_to_f.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c s_to_f.c

mk_ldb_dict.o: mk_ldb_dict.c mk_ldb_dict.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c mk_ldb_dict.c

del_fld.o: del_fld.c del_fld.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c del_fld.c

dump.o: dump.c dump.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c dump.c

patmatch.o: patmatch.c patmatch.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c patmatch.c

compound.o: compound.c compound.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c compound.c

par_sort_with_bins.o: par_sort_with_bins.c par_sort_with_bins.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c par_sort_with_bins.c

udf_deg2.o: udf_deg2.c udf_deg2.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c udf_deg2.c

fop.o: fop.c fop.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c fop.c

auxil.o: auxil.c auxil.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c auxil.c

add_fld.o: add_fld.c add_fld.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c add_fld.c

approx_unique.o: approx_unique.c approx_unique.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c approx_unique.c

approx_quantile.o: approx_quantile.c approx_quantile.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c approx_quantile.c

approx_frequent.o: approx_frequent.c approx_frequent.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c approx_frequent.c

pack.o: pack.c pack.h $(INC1) incl_pack_I4.c incl_pack_I8.c
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c pack.c

unpack.o: unpack.c unpack.h $(INC1) 
	$(CC) $(CFLAGS) $(INCLUDES) -c unpack.c

subsample.o: subsample.c subsample.h $(INC1) 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c subsample.c

smear.o: smear.c smear.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c smear.c

rng_sort.o: rng_sort.c rng_sort.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c rng_sort.c

udf_eq_B.o: udf_eq_B.c udf_eq_B.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c udf_eq_B.c

f1f2_to_s.o: f1f2_to_s.c f1f2_to_s.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c f1f2_to_s.c

dbauxil.o: dbauxil.c dbauxil.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c dbauxil.c

parsort1.o: parsort1.c parsort1.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c parsort1.c

wisifxthenyelsez.o: wisifxthenyelsez.c wisifxthenyelsez.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c wisifxthenyelsez.c

num_in_range.o: num_in_range.c num_in_range.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c num_in_range.c

find_conns.o: find_conns.c find_conns.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c find_conns.c

set_val.o: set_val.c set_val.h $(INC1) \
    	incl_set_val_I2.c incl_set_val_I4.c incl_set_val_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c set_val.c

mk_bins.o: mk_bins.c mk_bins.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c mk_bins.c

app_fld.o: app_fld.c app_fld.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c app_fld.c 

f1opf2_shift.o: f1opf2_shift.c f1opf2_shift.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c f1opf2_shift.c 

count_vals.o: count_vals.c count_vals.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c count_vals.c

vec_f1f2opf3.o: vec_f1f2opf3.c vec_f1f2opf3.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c vec_f1f2opf3.c

is_a_in_b.o: is_a_in_b.c is_a_in_b.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c is_a_in_b.c

is_A_in_b.o: is_A_in_b.c is_A_in_b.h $(INC1) \
	incl_is_A_in_b_I4.c  incl_is_A_in_b_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c is_A_in_b.c

vec_f1opf2.o: vec_f1opf2.c vec_f1opf2.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c vec_f1opf2.c

conv.o: conv.c conv.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c conv.c

vec_f1s1opf2.o: vec_f1s1opf2.c vec_f1s1opf2.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c vec_f1s1opf2.c 

sortf1f2.o: sortf1f2.c sortf1f2.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c sortf1f2.c

t1f1t2f2opt3f3.o: t1f1t2f2opt3f3.c t1f1t2f2opt3f3.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c t1f1t2f2opt3f3.c

f1opf2_cum.o: f1opf2_cum.c f1opf2_cum.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c f1opf2_cum.c

f1opf2_idx_with_reset.o: f1opf2_idx_with_reset.c f1opf2_idx_with_reset.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c f1opf2_idx_with_reset.c

count_ht.o: count_ht.c count_ht.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c count_ht.c

sortbindmp.o: sortbindmp.c sortbindmp.h $(INC1)
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c sortbindmp.c

aux_join_elim_null_vals.o: aux_join_elim_null_vals.c aux_join_elim_null_vals.h $(INC1)
	$(CC) $(CFLAGS) $(INCLUDES) -c aux_join_elim_null_vals.c

f_to_s.o: f_to_s.c f_to_s.h $(INC1) 
	$(CC) $(CFLAGS) $(INCLUDES) -c f_to_s.c

flush.o: flush.c flush.h $(INC1) 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c flush.c

#---------------------------------------------------------
xfer.o:	xfer.c xfer.h $(INC1) \
	incl_xfer_I1.c  \
	incl_xfer_I2.c  \
	incl_xfer_I4.c  \
	incl_xfer_I8.c  \
	incl_xfer_F4.c  \
	incl_xfer_F8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c xfer.c

incl_xfer_I1.c : tmpl_xfer_incl_1.c
	sed s'/SRC/I1/'g tmpl_xfer_incl_1.c > incl_xfer_I1.c
incl_xfer_I2.c : tmpl_xfer_incl_1.c
	sed s'/SRC/I2/'g tmpl_xfer_incl_1.c > incl_xfer_I2.c
incl_xfer_I4.c : tmpl_xfer_incl_1.c
	sed s'/SRC/I4/'g tmpl_xfer_incl_1.c > incl_xfer_I4.c
incl_xfer_I8.c : tmpl_xfer_incl_1.c
	sed s'/SRC/I8/'g tmpl_xfer_incl_1.c > incl_xfer_I8.c
incl_xfer_F4.c : tmpl_xfer_incl_1.c
	sed s'/SRC/F4/'g tmpl_xfer_incl_1.c > incl_xfer_F4.c
incl_xfer_F8.c : tmpl_xfer_incl_1.c
	sed s'/SRC/F8/'g tmpl_xfer_incl_1.c > incl_xfer_F8.c
#---------------------------------------------------------
vec_s_to_f.o: vec_s_to_f.c vec_s_to_f.h $(INC1) 
	$(CC) $(CFLAGS) $(INCLUDES) -c vec_s_to_f.c

vec_f_to_s.o: vec_f_to_s.c vec_f_to_s.h $(INC1) \
	incl_f_to_s_max.c incl_f_to_s_min.c incl_f_to_s_sum.c
	$(CC) $(CFLAGS) $(INCLUDES) -c vec_f_to_s.c

incl_f_to_s_max.c : tmpl_incl_f_to_s.c
	sed s'/XXX/max/'g tmpl_incl_f_to_s.c > incl_f_to_s_max.c
incl_f_to_s_min.c : tmpl_incl_f_to_s.c
	sed s'/XXX/min/'g tmpl_incl_f_to_s.c > incl_f_to_s_min.c
incl_f_to_s_sum.c : tmpl_incl_f_to_s.c
	sed s'/XXX/sum/'g tmpl_incl_f_to_s.c > incl_f_to_s_sum.c

incl_is_A_in_b_I4.c : tmpl_incl_is_A_in_b.c
	sed s'/__XTYPE__/I4/'g tmpl_incl_is_A_in_b.c | \
	sed s'/__ITYPE__/int/'g > incl_is_A_in_b_I4.c 
incl_is_A_in_b_I8.c : tmpl_incl_is_A_in_b.c
	sed s'/__XTYPE__/I8/'g tmpl_incl_is_A_in_b.c | \
	sed s'/__ITYPE__/int/'g > incl_is_A_in_b_I8.c 

#--------------------------------------------------------
xfer_I1_I4.o :	./AUTOGEN/GENFILES/xfer_I1_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/xfer_I1_I4.c 
xfer_I2_I4.o :	./AUTOGEN/GENFILES/xfer_I2_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/xfer_I2_I4.c 
xfer_I4_I4.o :	./AUTOGEN/GENFILES/xfer_I4_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/xfer_I4_I4.c 
xfer_I8_I4.o :	./AUTOGEN/GENFILES/xfer_I8_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/xfer_I8_I4.c 
xfer_F4_I4.o :	./AUTOGEN/GENFILES/xfer_F4_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/xfer_F4_I4.c 
xfer_F8_I4.o :	./AUTOGEN/GENFILES/xfer_F8_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/xfer_F8_I4.c 
#--------------------------------------------------------
#--------------------------------------------------------
f_to_s_min_I1.o :	./AUTOGEN/GENFILES/f_to_s_min_I1.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_min_I1.c 
f_to_s_min_I2.o :	./AUTOGEN/GENFILES/f_to_s_min_I2.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_min_I2.c 
f_to_s_min_I4.o :	./AUTOGEN/GENFILES/f_to_s_min_I4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_min_I4.c 
f_to_s_min_I8.o :	./AUTOGEN/GENFILES/f_to_s_min_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_min_I8.c 
f_to_s_min_F4.o :	./AUTOGEN/GENFILES/f_to_s_min_F4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_min_F4.c 
f_to_s_min_F8.o :	./AUTOGEN/GENFILES/f_to_s_min_F8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_min_F8.c 
nn_f_to_s_min_I1.o :	./AUTOGEN/GENFILES/nn_f_to_s_min_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_min_I1.c 
nn_f_to_s_min_I2.o :	./AUTOGEN/GENFILES/nn_f_to_s_min_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_min_I2.c 
nn_f_to_s_min_I4.o :	./AUTOGEN/GENFILES/nn_f_to_s_min_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_min_I4.c 
nn_f_to_s_min_I8.o :	./AUTOGEN/GENFILES/nn_f_to_s_min_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_min_I8.c 
nn_f_to_s_min_F4.o :	./AUTOGEN/GENFILES/nn_f_to_s_min_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_min_F4.c 
nn_f_to_s_min_F8.o :	./AUTOGEN/GENFILES/nn_f_to_s_min_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_min_F8.c 
#--------------------------------------------------------
f_to_s_max_I1.o :	./AUTOGEN/GENFILES/f_to_s_max_I1.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_max_I1.c 
f_to_s_max_I2.o :	./AUTOGEN/GENFILES/f_to_s_max_I2.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_max_I2.c 
f_to_s_max_I4.o :	./AUTOGEN/GENFILES/f_to_s_max_I4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_max_I4.c 
f_to_s_max_I8.o :	./AUTOGEN/GENFILES/f_to_s_max_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_max_I8.c 
f_to_s_max_F4.o :	./AUTOGEN/GENFILES/f_to_s_max_F4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_max_F4.c 
f_to_s_max_F8.o :	./AUTOGEN/GENFILES/f_to_s_max_F8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_max_F8.c 
nn_f_to_s_max_I1.o :	./AUTOGEN/GENFILES/nn_f_to_s_max_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_max_I1.c 
nn_f_to_s_max_I2.o :	./AUTOGEN/GENFILES/nn_f_to_s_max_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_max_I2.c 
nn_f_to_s_max_I4.o :	./AUTOGEN/GENFILES/nn_f_to_s_max_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_max_I4.c 
nn_f_to_s_max_I8.o :	./AUTOGEN/GENFILES/nn_f_to_s_max_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_max_I8.c 
nn_f_to_s_max_F4.o :	./AUTOGEN/GENFILES/nn_f_to_s_max_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_max_F4.c 
nn_f_to_s_max_F8.o :	./AUTOGEN/GENFILES/nn_f_to_s_max_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_max_F8.c 
#--------------------------------------------------------
f_to_s_sum_B.o :	f_to_s_sum_B.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c f_to_s_sum_B.c 
f_to_s_sum_I1.o :	./AUTOGEN/GENFILES/f_to_s_sum_I1.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_sum_I1.c 
f_to_s_sum_I2.o :	./AUTOGEN/GENFILES/f_to_s_sum_I2.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_sum_I2.c 
f_to_s_sum_I4.o :	./AUTOGEN/GENFILES/f_to_s_sum_I4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_sum_I4.c 
f_to_s_sum_I8.o :	./AUTOGEN/GENFILES/f_to_s_sum_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_sum_I8.c 
f_to_s_sum_F4.o :	./AUTOGEN/GENFILES/f_to_s_sum_F4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_sum_F4.c 
f_to_s_sum_F8.o :	./AUTOGEN/GENFILES/f_to_s_sum_F8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/f_to_s_sum_F8.c 
nn_f_to_s_sum_I1.o :	./AUTOGEN/GENFILES/nn_f_to_s_sum_I1.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_sum_I1.c 
nn_f_to_s_sum_I2.o :	./AUTOGEN/GENFILES/nn_f_to_s_sum_I2.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_sum_I2.c 
nn_f_to_s_sum_I4.o :	./AUTOGEN/GENFILES/nn_f_to_s_sum_I4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_sum_I4.c 
nn_f_to_s_sum_I8.o :	./AUTOGEN/GENFILES/nn_f_to_s_sum_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_sum_I8.c 
nn_f_to_s_sum_F4.o :	./AUTOGEN/GENFILES/nn_f_to_s_sum_F4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_sum_F4.c 
nn_f_to_s_sum_F8.o :	./AUTOGEN/GENFILES/nn_f_to_s_sum_F8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_f_to_s_sum_F8.c 
#--------------------------------------------------------
pr_fld_I1.o :	./AUTOGEN/GENFILES/pr_fld_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/pr_fld_I1.c 
pr_fld_I2.o :	./AUTOGEN/GENFILES/pr_fld_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/pr_fld_I2.c 
pr_fld_I4.o :	./AUTOGEN/GENFILES/pr_fld_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/pr_fld_I4.c 
pr_fld_I8.o :	./AUTOGEN/GENFILES/pr_fld_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/pr_fld_I8.c 
pr_fld_F4.o :	./AUTOGEN/GENFILES/pr_fld_F4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/pr_fld_F4.c 
pr_fld_F8.o :	./AUTOGEN/GENFILES/pr_fld_F8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/pr_fld_F8.c 
#--------------------------------------------------------
extract_I1.o :	./AUTOGEN/GENFILES/extract_I1.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/extract_I1.c 
extract_I2.o :	./AUTOGEN/GENFILES/extract_I2.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/extract_I2.c 
extract_I4.o :	./AUTOGEN/GENFILES/extract_I4.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/extract_I4.c 
extract_I8.o :	./AUTOGEN/GENFILES/extract_I8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/extract_I8.c 
extract_UI8.o :	./AUTOGEN/GENFILES/extract_UI8.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/extract_UI8.c 
#--------------------------------------------------------
s_to_f_const_I1.o :	./AUTOGEN/GENFILES/s_to_f_const_I1.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_const_I1.c 
s_to_f_const_I2.o :	./AUTOGEN/GENFILES/s_to_f_const_I2.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_const_I2.c 
s_to_f_const_I4.o :	./AUTOGEN/GENFILES/s_to_f_const_I4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_const_I4.c 
s_to_f_const_I8.o :	./AUTOGEN/GENFILES/s_to_f_const_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_const_I8.c 
s_to_f_const_F4.o :	./AUTOGEN/GENFILES/s_to_f_const_F4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_const_F4.c 
#--------------------------------------------------------
s_to_f_seq_I1.o :	./AUTOGEN/GENFILES/s_to_f_seq_I1.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_seq_I1.c 
s_to_f_seq_I2.o :	./AUTOGEN/GENFILES/s_to_f_seq_I2.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_seq_I2.c 
s_to_f_seq_I4.o :	./AUTOGEN/GENFILES/s_to_f_seq_I4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_seq_I4.c 
s_to_f_seq_I8.o :	./AUTOGEN/GENFILES/s_to_f_seq_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/s_to_f_seq_I8.c 
#--------------------------------------------------------
mk_hashes_I1.o : ./AUTOGEN/GENFILES/mk_hashes_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mk_hashes_I1.c 
mk_hashes_I2.o : ./AUTOGEN/GENFILES/mk_hashes_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mk_hashes_I2.c 
mk_hashes_I4.o : ./AUTOGEN/GENFILES/mk_hashes_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mk_hashes_I4.c 
mk_hashes_I8.o : ./AUTOGEN/GENFILES/mk_hashes_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mk_hashes_I8.c 
#--------------------------------------------------------
permute_I4.o : ./AUTOGEN/GENFILES/permute_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/permute_I4.c 
permute_I8.o : ./AUTOGEN/GENFILES/permute_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/permute_I8.c 
#--------------------------------------------------------
saturate_I1.o : ./AUTOGEN/GENFILES/saturate_I1.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/saturate_I1.c 
saturate_I2.o : ./AUTOGEN/GENFILES/saturate_I2.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/saturate_I2.c 
saturate_I4.o : ./AUTOGEN/GENFILES/saturate_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/saturate_I4.c 
saturate_I8.o : ./AUTOGEN/GENFILES/saturate_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/saturate_I8.c 
#--------------------------------------------------------
assign_I1.o :	./AUTOGEN/GENFILES/assign_I1.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/assign_I1.c 
assign_I2.o :	./AUTOGEN/GENFILES/assign_I2.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/assign_I2.c 
assign_I4.o :	./AUTOGEN/GENFILES/assign_I4.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/assign_I4.c 
assign_I8.o :	./AUTOGEN/GENFILES/assign_I8.c 
	$(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/assign_I8.c 

add2_I2.o : ./AUTOGEN/GENFILES/add2_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add2_I2.c 
sub2_I2.o : ./AUTOGEN/GENFILES/sub2_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub2_I2.c 
mul2_I2.o : ./AUTOGEN/GENFILES/mul2_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul2_I2.c 
div2_I2.o : ./AUTOGEN/GENFILES/div2_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div2_I2.c 
rem2_I2.o : ./AUTOGEN/GENFILES/rem2_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/rem2_I2.c 
add2_I4.o : ./AUTOGEN/GENFILES/add2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add2_I4.c 
sub2_I4.o : ./AUTOGEN/GENFILES/sub2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub2_I4.c 
mul2_I4.o : ./AUTOGEN/GENFILES/mul2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul2_I4.c 
div2_I4.o : ./AUTOGEN/GENFILES/div2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div2_I4.c 
rem2_I4.o : ./AUTOGEN/GENFILES/rem2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/rem2_I4.c 
shiftleft2_I4.o : ./AUTOGEN/GENFILES/shiftleft2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shiftleft2_I4.c 
shiftright2_UI4.o : ./AUTOGEN/GENFILES/shiftright2_UI4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shiftright2_UI4.c 
add2_I8.o : ./AUTOGEN/GENFILES/add2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add2_I8.c 
sub2_I8.o : ./AUTOGEN/GENFILES/sub2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub2_I8.c 
mul2_I8.o : ./AUTOGEN/GENFILES/mul2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul2_I8.c 
div2_I8.o : ./AUTOGEN/GENFILES/div2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div2_I8.c 
rem2_I8.o : ./AUTOGEN/GENFILES/rem2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/rem2_I8.c 
shiftleft2_I8.o : ./AUTOGEN/GENFILES/shiftleft2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shiftleft2_I8.c 
shiftright2_UI8.o : ./AUTOGEN/GENFILES/shiftright2_UI8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shiftright2_UI8.c 
add2_F8.o : ./AUTOGEN/GENFILES/add2_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add2_F8.c 
sub2_F8.o : ./AUTOGEN/GENFILES/sub2_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub2_F8.c 
mul2_F8.o : ./AUTOGEN/GENFILES/mul2_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul2_F8.c 
div2_F8.o : ./AUTOGEN/GENFILES/div2_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div2_F8.c 
add2_F4.o : ./AUTOGEN/GENFILES/add2_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add2_F4.c 
sub2_F4.o : ./AUTOGEN/GENFILES/sub2_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub2_F4.c 
mul2_F4.o : ./AUTOGEN/GENFILES/mul2_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul2_F4.c 
div2_F4.o : ./AUTOGEN/GENFILES/div2_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div2_F4.c 
and2_I1.o : ./AUTOGEN/GENFILES/and2_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/and2_I1.c 
or2_I1.o : ./AUTOGEN/GENFILES/or2_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/or2_I1.c 
bwise_and2_I1.o : ./AUTOGEN/GENFILES/bwise_and2_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_and2_I1.c 
bwise_and2_I8.o : ./AUTOGEN/GENFILES/bwise_and2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_and2_I8.c 
bwise_and2_I4.o : ./AUTOGEN/GENFILES/bwise_and2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_and2_I4.c 
bwise_or2_I1.o : ./AUTOGEN/GENFILES/bwise_or2_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_or2_I1.c 
bwise_or2_I8.o : ./AUTOGEN/GENFILES/bwise_or2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_or2_I8.c 
bwise_or2_I4.o : ./AUTOGEN/GENFILES/bwise_or2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_or2_I4.c 
bwise_xor2_I1.o : ./AUTOGEN/GENFILES/bwise_xor2_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_xor2_I1.c 
bwise_xor2_I8.o : ./AUTOGEN/GENFILES/bwise_xor2_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_xor2_I8.c 
bwise_xor2_I4.o : ./AUTOGEN/GENFILES/bwise_xor2_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_xor2_I4.c 
#---------------------------------------------
cmp_ge_I1.o : ./AUTOGEN/GENFILES/cmp_ge_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_I1.c 
cmp_le_I1.o : ./AUTOGEN/GENFILES/cmp_le_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_I1.c 
cmp_eq_I1.o : ./AUTOGEN/GENFILES/cmp_eq_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_I1.c 
cmp_ne_I1.o : ./AUTOGEN/GENFILES/cmp_ne_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_I1.c 
cmp_gt_I1.o : ./AUTOGEN/GENFILES/cmp_gt_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_I1.c 
cmp_lt_I1.o : ./AUTOGEN/GENFILES/cmp_lt_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_I1.c 
#---------------------------------------------
cmp_ge_I2.o : ./AUTOGEN/GENFILES/cmp_ge_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_I2.c 
cmp_le_I2.o : ./AUTOGEN/GENFILES/cmp_le_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_I2.c 
cmp_eq_I2.o : ./AUTOGEN/GENFILES/cmp_eq_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_I2.c 
cmp_ne_I2.o : ./AUTOGEN/GENFILES/cmp_ne_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_I2.c 
cmp_gt_I2.o : ./AUTOGEN/GENFILES/cmp_gt_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_I2.c 
cmp_lt_I2.o : ./AUTOGEN/GENFILES/cmp_lt_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_I2.c 
#---------------------------------------------
cmp_ge_I4.o : ./AUTOGEN/GENFILES/cmp_ge_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_I4.c 
cmp_le_I4.o : ./AUTOGEN/GENFILES/cmp_le_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_I4.c 
cmp_eq_I4.o : ./AUTOGEN/GENFILES/cmp_eq_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_I4.c 
cmp_ne_I4.o : ./AUTOGEN/GENFILES/cmp_ne_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_I4.c 
cmp_gt_I4.o : ./AUTOGEN/GENFILES/cmp_gt_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_I4.c 
cmp_lt_I4.o : ./AUTOGEN/GENFILES/cmp_lt_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_I4.c 
#---------------------------------------------
cmp_ge_F4.o : ./AUTOGEN/GENFILES/cmp_ge_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_F4.c 
cmp_le_F4.o : ./AUTOGEN/GENFILES/cmp_le_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_F4.c 
cmp_eq_F4.o : ./AUTOGEN/GENFILES/cmp_eq_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_F4.c 
cmp_ne_F4.o : ./AUTOGEN/GENFILES/cmp_ne_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_F4.c 
cmp_gt_F4.o : ./AUTOGEN/GENFILES/cmp_gt_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_F4.c 
cmp_lt_F4.o : ./AUTOGEN/GENFILES/cmp_lt_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_F4.c 
#---------------------------------------------
cmp_ge_F8.o : ./AUTOGEN/GENFILES/cmp_ge_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_F8.c 
cmp_le_F8.o : ./AUTOGEN/GENFILES/cmp_le_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_F8.c 
cmp_eq_F8.o : ./AUTOGEN/GENFILES/cmp_eq_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_F8.c 
cmp_ne_F8.o : ./AUTOGEN/GENFILES/cmp_ne_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_F8.c 
cmp_gt_F8.o : ./AUTOGEN/GENFILES/cmp_gt_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_F8.c 
cmp_lt_F8.o : ./AUTOGEN/GENFILES/cmp_lt_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_F8.c 
#---------------------------------------------
cmp_ge_I8.o : ./AUTOGEN/GENFILES/cmp_ge_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_I8.c 
cmp_le_I8.o : ./AUTOGEN/GENFILES/cmp_le_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_I8.c 
cmp_eq_I8.o : ./AUTOGEN/GENFILES/cmp_eq_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_I8.c 
cmp_ne_I8.o : ./AUTOGEN/GENFILES/cmp_ne_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_I8.c 
cmp_gt_I8.o : ./AUTOGEN/GENFILES/cmp_gt_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_I8.c 
cmp_lt_I8.o : ./AUTOGEN/GENFILES/cmp_lt_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_I8.c 
#---------------------------------------------
cmp_lt_or_gt_I1.o : ./AUTOGEN/GENFILES/cmp_lt_or_gt_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_or_gt_I1.c 
cmp_le_or_ge_I1.o : ./AUTOGEN/GENFILES/cmp_le_or_ge_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_or_ge_I1.c 
cmp_ge_and_le_I1.o : ./AUTOGEN/GENFILES/cmp_ge_and_le_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_and_le_I1.c
cmp_gt_and_lt_I1.o : ./AUTOGEN/GENFILES/cmp_gt_and_lt_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_and_lt_I1.c
cmp_lt_or_gt_I2.o : ./AUTOGEN/GENFILES/cmp_lt_or_gt_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_or_gt_I2.c 
cmp_le_or_ge_I2.o : ./AUTOGEN/GENFILES/cmp_le_or_ge_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_or_ge_I2.c 
cmp_ge_and_le_I2.o : ./AUTOGEN/GENFILES/cmp_ge_and_le_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_and_le_I2.c
cmp_gt_and_lt_I2.o : ./AUTOGEN/GENFILES/cmp_gt_and_lt_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_and_lt_I2.c
cmp_lt_or_gt_I4.o : ./AUTOGEN/GENFILES/cmp_lt_or_gt_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_or_gt_I4.c 
cmp_le_or_ge_I4.o : ./AUTOGEN/GENFILES/cmp_le_or_ge_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_or_ge_I4.c 
cmp_ge_and_le_I4.o : ./AUTOGEN/GENFILES/cmp_ge_and_le_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_and_le_I4.c
cmp_gt_and_lt_I4.o : ./AUTOGEN/GENFILES/cmp_gt_and_lt_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_and_lt_I4.c
#---------------------------------------------
dbg_cmp_neq_mult_scalar_I1.o : ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I1.c 
dbg_cmp_neq_mult_scalar_I2.o : ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I2.c 
dbg_cmp_neq_mult_scalar_I4.o : ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I4.c 
dbg_cmp_neq_mult_scalar_I8.o : ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_neq_mult_scalar_I8.c 
dbg_cmp_eq_mult_scalar_I1.o : ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I1.c 
dbg_cmp_eq_mult_scalar_I2.o : ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I2.c 
dbg_cmp_eq_mult_scalar_I4.o : ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I4.c 
dbg_cmp_eq_mult_scalar_I8.o : ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/dbg_cmp_eq_mult_scalar_I8.c 
opt_cmp_eq_mult_scalar_I1.o : ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I1.c 
opt_cmp_eq_mult_scalar_I2.o : ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I2.c 
opt_cmp_eq_mult_scalar_I4.o : ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I4.c 
opt_cmp_eq_mult_scalar_I8.o : ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/opt_cmp_eq_mult_scalar_I8.c 
cmp_eq_mult_scalar_I1.o : ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I1.c 
cmp_eq_mult_scalar_I2.o : ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I2.c 
cmp_eq_mult_scalar_I4.o : ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I4.c 
cmp_eq_mult_scalar_I8.o : ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_mult_scalar_I8.c 
add_scalar_I1.o : ./AUTOGEN/GENFILES/add_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add_scalar_I1.c 
sub_scalar_I1.o : ./AUTOGEN/GENFILES/sub_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub_scalar_I1.c 
mul_scalar_I1.o : ./AUTOGEN/GENFILES/mul_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul_scalar_I1.c 
div_scalar_I1.o : ./AUTOGEN/GENFILES/div_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div_scalar_I1.c 
rem_scalar_I1.o : ./AUTOGEN/GENFILES/rem_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/rem_scalar_I1.c 
add_scalar_I2.o : ./AUTOGEN/GENFILES/add_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add_scalar_I2.c 
sub_scalar_I2.o : ./AUTOGEN/GENFILES/sub_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub_scalar_I2.c 
mul_scalar_I2.o : ./AUTOGEN/GENFILES/mul_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul_scalar_I2.c 
div_scalar_I2.o : ./AUTOGEN/GENFILES/div_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div_scalar_I2.c 
rem_scalar_I2.o : ./AUTOGEN/GENFILES/rem_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/rem_scalar_I2.c 
add_scalar_I4.o : ./AUTOGEN/GENFILES/add_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add_scalar_I4.c 
sub_scalar_I4.o : ./AUTOGEN/GENFILES/sub_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub_scalar_I4.c 
mul_scalar_I4.o : ./AUTOGEN/GENFILES/mul_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul_scalar_I4.c 
div_scalar_I4.o : ./AUTOGEN/GENFILES/div_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div_scalar_I4.c 
rem_scalar_I4.o : ./AUTOGEN/GENFILES/rem_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/rem_scalar_I4.c 
add_scalar_I8.o : ./AUTOGEN/GENFILES/add_scalar_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add_scalar_I8.c  
sub_scalar_I8.o : ./AUTOGEN/GENFILES/sub_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub_scalar_I8.c 
mul_scalar_I8.o : ./AUTOGEN/GENFILES/mul_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul_scalar_I8.c 
div_scalar_I8.o : ./AUTOGEN/GENFILES/div_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div_scalar_I8.c 
rem_scalar_I8.o : ./AUTOGEN/GENFILES/rem_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/rem_scalar_I8.c 
add_scalar_F8.o : ./AUTOGEN/GENFILES/add_scalar_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add_scalar_F8.c  
sub_scalar_F8.o : ./AUTOGEN/GENFILES/sub_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub_scalar_F8.c 
mul_scalar_F8.o : ./AUTOGEN/GENFILES/mul_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul_scalar_F8.c 
div_scalar_F8.o : ./AUTOGEN/GENFILES/div_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div_scalar_F8.c 
add_scalar_F4.o : ./AUTOGEN/GENFILES/add_scalar_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/add_scalar_F4.c  
sub_scalar_F4.o : ./AUTOGEN/GENFILES/sub_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sub_scalar_F4.c 
mul_scalar_F4.o : ./AUTOGEN/GENFILES/mul_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/mul_scalar_F4.c 
div_scalar_F4.o : ./AUTOGEN/GENFILES/div_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/div_scalar_F4.c 
and_scalar_I1.o : ./AUTOGEN/GENFILES/and_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/and_scalar_I1.c 
or_scalar_I1.o : ./AUTOGEN/GENFILES/or_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/or_scalar_I1.c 
a_and_not_b_I1.o : a_and_not_b_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c a_and_not_b_I1.c 
bwise_a_and_not_b.o : bwise_a_and_not_b.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c bwise_a_and_not_b.c 
bwise_and_scalar_I4.o : ./AUTOGEN/GENFILES/bwise_and_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_and_scalar_I4.c 
bwise_not_or_scalar_I8.o : bwise_not_or_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c bwise_not_or_scalar_I8.c 
bwise_not_and_scalar_I8.o : bwise_not_and_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c bwise_not_and_scalar_I8.c 
bwise_and_scalar_I8.o : ./AUTOGEN/GENFILES/bwise_and_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_and_scalar_I8.c 
bwise_and_scalar_I1.o : ./AUTOGEN/GENFILES/bwise_and_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_and_scalar_I1.c 
shift_left_scalar_I4.o : ./AUTOGEN/GENFILES/shift_left_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shift_left_scalar_I4.c 
shift_left_scalar_I8.o : ./AUTOGEN/GENFILES/shift_left_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shift_left_scalar_I8.c 
shift_right_scalar_UI4.o : ./AUTOGEN/GENFILES/shift_right_scalar_UI4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shift_right_scalar_UI4.c 
shift_right_scalar_UI8.o : ./AUTOGEN/GENFILES/shift_right_scalar_UI8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/shift_right_scalar_UI8.c 
bwise_or_scalar_I4.o : ./AUTOGEN/GENFILES/bwise_or_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_or_scalar_I4.c 
bwise_or_scalar_I8.o : ./AUTOGEN/GENFILES/bwise_or_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_or_scalar_I8.c 
bwise_or_scalar_I1.o : ./AUTOGEN/GENFILES/bwise_or_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_or_scalar_I1.c 
bwise_xor_scalar_I4.o : ./AUTOGEN/GENFILES/bwise_xor_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_xor_scalar_I4.c 
bwise_xor_scalar_I8.o : ./AUTOGEN/GENFILES/bwise_xor_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_xor_scalar_I8.c 
bwise_xor_scalar_I1.o : ./AUTOGEN/GENFILES/bwise_xor_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bwise_xor_scalar_I1.c 
cmp_le_scalar_I4.o : ./AUTOGEN/GENFILES/cmp_le_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_scalar_I4.c 
cmp_ge_scalar_I4.o : ./AUTOGEN/GENFILES/cmp_ge_scalar_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_scalar_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_scalar_I4.c 
cmp_ne_scalar_I4.o : ./AUTOGEN/GENFILES/cmp_ne_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_scalar_I4.c 
cmp_gt_scalar_I4.o : ./AUTOGEN/GENFILES/cmp_gt_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_scalar_I4.c 
cmp_lt_scalar_I4.o : ./AUTOGEN/GENFILES/cmp_lt_scalar_I4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_scalar_I4.c 
cmp_le_scalar_I8.o : ./AUTOGEN/GENFILES/cmp_le_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_scalar_I8.c 
cmp_ge_scalar_I8.o : ./AUTOGEN/GENFILES/cmp_ge_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_scalar_I8.c 
cmp_eq_scalar_I8.o : ./AUTOGEN/GENFILES/cmp_eq_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_scalar_I8.c 
cmp_ne_scalar_I8.o : ./AUTOGEN/GENFILES/cmp_ne_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_scalar_I8.c 
cmp_lt_scalar_I8.o : ./AUTOGEN/GENFILES/cmp_lt_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_scalar_I8.c 
cmp_gt_scalar_I8.o : ./AUTOGEN/GENFILES/cmp_gt_scalar_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_scalar_I8.c 
cmp_le_scalar_F4.o : ./AUTOGEN/GENFILES/cmp_le_scalar_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_scalar_F4.c  
cmp_ge_scalar_F4.o : ./AUTOGEN/GENFILES/cmp_ge_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_scalar_F4.c 
cmp_eq_scalar_F4.o : ./AUTOGEN/GENFILES/cmp_eq_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_scalar_F4.c 
cmp_ne_scalar_F4.o : ./AUTOGEN/GENFILES/cmp_ne_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_scalar_F4.c 
cmp_lt_scalar_F4.o : ./AUTOGEN/GENFILES/cmp_lt_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_scalar_F4.c 
cmp_gt_scalar_F4.o : ./AUTOGEN/GENFILES/cmp_gt_scalar_F4.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_scalar_F4.c 
cmp_le_scalar_F8.o : ./AUTOGEN/GENFILES/cmp_le_scalar_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_scalar_F8.c  
cmp_ge_scalar_F8.o : ./AUTOGEN/GENFILES/cmp_ge_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_scalar_F8.c 
cmp_eq_scalar_F8.o : ./AUTOGEN/GENFILES/cmp_eq_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_scalar_F8.c 
cmp_ne_scalar_F8.o : ./AUTOGEN/GENFILES/cmp_ne_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_scalar_F8.c 
cmp_lt_scalar_F8.o : ./AUTOGEN/GENFILES/cmp_lt_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_scalar_F8.c 
cmp_gt_scalar_F8.o : ./AUTOGEN/GENFILES/cmp_gt_scalar_F8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_scalar_F8.c 
cmp_le_scalar_I1.o : ./AUTOGEN/GENFILES/cmp_le_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_scalar_I1.c 
cmp_ge_scalar_I1.o : ./AUTOGEN/GENFILES/cmp_ge_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_scalar_I1.c 
cmp_eq_scalar_I1.o : ./AUTOGEN/GENFILES/cmp_eq_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_scalar_I1.c 
cmp_ne_scalar_I1.o : ./AUTOGEN/GENFILES/cmp_ne_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_scalar_I1.c 
cmp_lt_scalar_I1.o : ./AUTOGEN/GENFILES/cmp_lt_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_scalar_I1.c 
cmp_gt_scalar_I1.o : ./AUTOGEN/GENFILES/cmp_gt_scalar_I1.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_scalar_I1.c 
cmp_le_scalar_I2.o : ./AUTOGEN/GENFILES/cmp_le_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_le_scalar_I2.c 
cmp_ge_scalar_I2.o : ./AUTOGEN/GENFILES/cmp_ge_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ge_scalar_I2.c 
cmp_eq_scalar_I2.o : ./AUTOGEN/GENFILES/cmp_eq_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_eq_scalar_I2.c 
cmp_ne_scalar_I2.o : ./AUTOGEN/GENFILES/cmp_ne_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_ne_scalar_I2.c 
cmp_lt_scalar_I2.o : ./AUTOGEN/GENFILES/cmp_lt_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_lt_scalar_I2.c 
cmp_gt_scalar_I2.o : ./AUTOGEN/GENFILES/cmp_gt_scalar_I2.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cmp_gt_scalar_I2.c 
conv_I1_to_B.o : conv_I1_to_B.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c conv_I1_to_B.c 

conv_F8_to_I4.o : ./AUTOGEN/GENFILES/conv_F8_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_F8_to_I4.c 
conv_F8_to_I8.o : ./AUTOGEN/GENFILES/conv_F8_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_F8_to_I8.c  
conv_F8_to_F4.o : ./AUTOGEN/GENFILES/conv_F8_to_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_F8_to_F4.c  
conv_I4_to_I1.o : ./AUTOGEN/GENFILES/conv_I4_to_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I4_to_I1.c  
conv_I4_to_I2.o : ./AUTOGEN/GENFILES/conv_I4_to_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I4_to_I2.c  
conv_I4_to_I8.o : ./AUTOGEN/GENFILES/conv_I4_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I4_to_I8.c  
conv_I4_to_F4.o : ./AUTOGEN/GENFILES/conv_I4_to_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I4_to_F4.c  
conv_I4_to_F8.o : ./AUTOGEN/GENFILES/conv_I4_to_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I4_to_F8.c  
conv_I8_to_I4.o : ./AUTOGEN/GENFILES/conv_I8_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I8_to_I4.c  
conv_I8_to_I1.o : ./AUTOGEN/GENFILES/conv_I8_to_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I8_to_I1.c  
conv_I8_to_F4.o : ./AUTOGEN/GENFILES/conv_I8_to_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I8_to_F4.c  
conv_I8_to_F8.o : ./AUTOGEN/GENFILES/conv_I8_to_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I8_to_F8.c  
conv_I1_to_I4.o : ./AUTOGEN/GENFILES/conv_I1_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I1_to_I4.c  
conv_I1_to_I8.o : ./AUTOGEN/GENFILES/conv_I1_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_I1_to_I8.c  
conv_F4_to_I4.o : ./AUTOGEN/GENFILES/conv_F4_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_F4_to_I4.c  
conv_F4_to_I8.o : ./AUTOGEN/GENFILES/conv_F4_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_F4_to_I8.c  
conv_F4_to_F8.o : ./AUTOGEN/GENFILES/conv_F4_to_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/conv_F4_to_F8.c  
#--------------------------------------------------------------------
nn_conv_F8_to_I4.o : ./AUTOGEN/GENFILES/nn_conv_F8_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_F8_to_I4.c 
nn_conv_F8_to_I8.o : ./AUTOGEN/GENFILES/nn_conv_F8_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_F8_to_I8.c  
nn_conv_F8_to_F4.o : ./AUTOGEN/GENFILES/nn_conv_F8_to_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_F8_to_F4.c  
nn_conv_I4_to_I1.o : ./AUTOGEN/GENFILES/nn_conv_I4_to_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I4_to_I1.c  
nn_conv_I4_to_I2.o : ./AUTOGEN/GENFILES/nn_conv_I4_to_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I4_to_I2.c  
nn_conv_I4_to_I8.o : ./AUTOGEN/GENFILES/nn_conv_I4_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I4_to_I8.c  
nn_conv_I4_to_F4.o : ./AUTOGEN/GENFILES/nn_conv_I4_to_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I4_to_F4.c  
nn_conv_I4_to_F8.o : ./AUTOGEN/GENFILES/nn_conv_I4_to_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I4_to_F8.c  
nn_conv_I8_to_I4.o : ./AUTOGEN/GENFILES/nn_conv_I8_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I8_to_I4.c  
nn_conv_I8_to_I1.o : ./AUTOGEN/GENFILES/nn_conv_I8_to_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I8_to_I1.c  
nn_conv_I8_to_F4.o : ./AUTOGEN/GENFILES/nn_conv_I8_to_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I8_to_F4.c  
nn_conv_I8_to_F8.o : ./AUTOGEN/GENFILES/nn_conv_I8_to_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I8_to_F8.c  
nn_conv_I1_to_I4.o : ./AUTOGEN/GENFILES/nn_conv_I1_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I1_to_I4.c  
nn_conv_I1_to_I8.o : ./AUTOGEN/GENFILES/nn_conv_I1_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_I1_to_I8.c  
nn_conv_F4_to_I4.o : ./AUTOGEN/GENFILES/nn_conv_F4_to_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_F4_to_I4.c  
nn_conv_F4_to_I8.o : ./AUTOGEN/GENFILES/nn_conv_F4_to_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_F4_to_I8.c  
nn_conv_F4_to_F8.o : ./AUTOGEN/GENFILES/nn_conv_F4_to_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/nn_conv_F4_to_F8.c  
#--------------------------------------------------------------------
ones_complement_I4.o : ./AUTOGEN/GENFILES/ones_complement_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/ones_complement_I4.c  
not_I4.o : ./AUTOGEN/GENFILES/not_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/not_I4.c  
ones_complement_I8.o : ./AUTOGEN/GENFILES/ones_complement_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/ones_complement_I8.c  
not_I8.o : ./AUTOGEN/GENFILES/not_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/not_I8.c  
ones_complement_I1.o : ./AUTOGEN/GENFILES/ones_complement_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/ones_complement_I1.c  
not_I1.o : ./AUTOGEN/GENFILES/not_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/not_I1.c  
incr_I4.o : ./AUTOGEN/GENFILES/incr_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/incr_I4.c  
decr_I4.o : ./AUTOGEN/GENFILES/decr_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/decr_I4.c  
incr_I8.o : ./AUTOGEN/GENFILES/incr_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/incr_I8.c  
decr_I8.o : ./AUTOGEN/GENFILES/decr_I8.c 
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/decr_I8.c 
cum_I4_to_I4.o : ./AUTOGEN/GENFILES/cum_I4_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I4_to_I4.c 
cum_I4_to_I8.o : ./AUTOGEN/GENFILES/cum_I4_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I4_to_I8.c 
cum_I8_to_I4.o : ./AUTOGEN/GENFILES/cum_I8_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I8_to_I4.c 
cum_I8_to_I8.o : ./AUTOGEN/GENFILES/cum_I8_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I8_to_I8.c 
cum_I2_to_I4.o : ./AUTOGEN/GENFILES/cum_I2_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I2_to_I4.c 
cum_I2_to_I8.o : ./AUTOGEN/GENFILES/cum_I2_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I2_to_I8.c 
cum_I1_to_I4.o : ./AUTOGEN/GENFILES/cum_I1_to_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I1_to_I4.c 
cum_I1_to_I8.o : ./AUTOGEN/GENFILES/cum_I1_to_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/cum_I1_to_I8.c 
sort_asc_I4.o : ./AUTOGEN/GENFILES/sort_asc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_asc_I4.c 
sort_dsc_I4.o : ./AUTOGEN/GENFILES/sort_dsc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_dsc_I4.c 
sort_asc_I8.o : ./AUTOGEN/GENFILES/sort_asc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_asc_I8.c 
sort_dsc_I8.o : ./AUTOGEN/GENFILES/sort_dsc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_dsc_I8.c 
sort2_dsc_F4.o : ./AUTOGEN/GENFILES/sort2_dsc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_dsc_F4.c 
sort2_asc_F4.o : ./AUTOGEN/GENFILES/sort2_asc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_asc_F4.c 
sort_asc_F4.o : ./AUTOGEN/GENFILES/sort_asc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_asc_F4.c 
sort_dsc_F4.o : ./AUTOGEN/GENFILES/sort_dsc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_dsc_F4.c 
sort_asc_F8.o : ./AUTOGEN/GENFILES/sort_asc_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_asc_F8.c 
sort_dsc_F8.o : ./AUTOGEN/GENFILES/sort_dsc_F8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort_dsc_F8.c 
sort2_asc_I4.o : ./AUTOGEN/GENFILES/sort2_asc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_asc_I4.c 
sort2_dsc_I4.o : ./AUTOGEN/GENFILES/sort2_dsc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_dsc_I4.c 
sort2_asc_I8.o : ./AUTOGEN/GENFILES/sort2_asc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_asc_I8.c 
sort2_dsc_I8.o : ./AUTOGEN/GENFILES/sort2_dsc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_dsc_I8.c 
sort2_asc_float.o : ./AUTOGEN/GENFILES/sort2_asc_float.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_asc_float.c 
sort2_dsc_float.o : ./AUTOGEN/GENFILES/sort2_dsc_float.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort2_dsc_float.c 
sort3_asc_I4.o : ./AUTOGEN/GENFILES/sort3_asc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort3_asc_I4.c 
sort3_dsc_I4.o : ./AUTOGEN/GENFILES/sort3_dsc_I4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort3_dsc_I4.c 
sort3_asc_I8.o : ./AUTOGEN/GENFILES/sort3_asc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort3_asc_I8.c 
sort3_dsc_I8.o : ./AUTOGEN/GENFILES/sort3_dsc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort3_dsc_I8.c 
sort4_asc_I8.o : ./AUTOGEN/GENFILES/sort4_asc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort4_asc_I8.c 
sort4_dsc_I8.o : ./AUTOGEN/GENFILES/sort4_dsc_I8.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort4_dsc_I8.c 
sort3_asc_F4.o : ./AUTOGEN/GENFILES/sort3_asc_F4.c 
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort3_asc_F4.c 
sort3_dsc_F4.o : ./AUTOGEN/GENFILES/sort3_dsc_F4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/sort3_dsc_F4.c  
uniq_I4.o : ./AUTOGEN/GENFILES/uniq_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_I4.c  
uniq_I8.o : ./AUTOGEN/GENFILES/uniq_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_I8.c  
uniq_cnt_I4.o : ./AUTOGEN/GENFILES/uniq_cnt_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_cnt_I4.c  
uniq_cnt_I8.o : ./AUTOGEN/GENFILES/uniq_cnt_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_cnt_I8.c  
uniq_alldef_I4.o : ./AUTOGEN/GENFILES/uniq_alldef_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_alldef_I4.c  
uniq_alldef_I8.o : ./AUTOGEN/GENFILES/uniq_alldef_I8.c
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_alldef_I8.c
uniq_cnt_alldef_I4.o : ./AUTOGEN/GENFILES/uniq_cnt_alldef_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_cnt_alldef_I4.c 
uniq_cnt_alldef_I8.o : ./AUTOGEN/GENFILES/uniq_cnt_alldef_I8.c
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/uniq_cnt_alldef_I8.c
bitcount_I4.o : ./AUTOGEN/GENFILES/bitcount_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bitcount_I4.c  
bitcount_I8.o : ./AUTOGEN/GENFILES/bitcount_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bitcount_I8.c  
is_s_in_f_seq_I1.o : ./AUTOGEN/GENFILES/is_s_in_f_seq_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_s_in_f_seq_I1.c  
is_s_in_f_seq_I2.o : ./AUTOGEN/GENFILES/is_s_in_f_seq_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_s_in_f_seq_I2.c  
is_s_in_f_seq_I4.o : ./AUTOGEN/GENFILES/is_s_in_f_seq_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_s_in_f_seq_I4.c  
is_s_in_f_seq_I8.o : ./AUTOGEN/GENFILES/is_s_in_f_seq_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_s_in_f_seq_I8.c  
srt_uniform_I4.o : ./AUTOGEN/GENFILES/srt_uniform_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/srt_uniform_I4.c  
srt_uniform_I8.o : ./AUTOGEN/GENFILES/srt_uniform_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/srt_uniform_I8.c  
hash_I4.o : ./AUTOGEN/GENFILES/hash_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/hash_I4.c  
hash_I8.o : ./AUTOGEN/GENFILES/hash_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/hash_I8.c  

find_largest_leq_I4.o : ./AUTOGEN/GENFILES/find_largest_leq_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/find_largest_leq_I4.c  
find_largest_leq_I8.o : ./AUTOGEN/GENFILES/find_largest_leq_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/find_largest_leq_I8.c  

find_smallest_geq_I4.o : ./AUTOGEN/GENFILES/find_smallest_geq_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/find_smallest_geq_I4.c  
find_smallest_geq_I8.o : ./AUTOGEN/GENFILES/find_smallest_geq_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/find_smallest_geq_I8.c  

qsort_asc_I1.o : ./AUTOGEN/GENFILES/qsort_asc_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_asc_I1.c  
qsort_asc_I4.o : ./AUTOGEN/GENFILES/qsort_asc_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_asc_I4.c  
qsort_asc_I8.o : ./AUTOGEN/GENFILES/qsort_asc_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_asc_I8.c  
qsort_dsc_I4.o : ./AUTOGEN/GENFILES/qsort_dsc_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_dsc_I4.c  
qsort_dsc_I8.o : ./AUTOGEN/GENFILES/qsort_dsc_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_dsc_I8.c  

qsort_with_idx_int.o : ./AUTOGEN/GENFILES/qsort_with_idx_int.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_with_idx_int.c  
qsort_with_idx_longlong.o : ./AUTOGEN/GENFILES/qsort_with_idx_longlong.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_with_idx_longlong.c  
#------------------------------------------------------------
qsort_asc_val_I4_idx_I4.o : ./AUTOGEN/GENFILES/qsort_asc_val_I4_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_asc_val_I4_idx_I4.c  
qsort_asc_val_I4_idx_I8.o : ./AUTOGEN/GENFILES/qsort_asc_val_I4_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_asc_val_I4_idx_I8.c  
qsort_asc_val_I8_idx_I4.o : ./AUTOGEN/GENFILES/qsort_asc_val_I8_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_asc_val_I8_idx_I4.c  
qsort_asc_val_I8_idx_I8.o : ./AUTOGEN/GENFILES/qsort_asc_val_I8_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_asc_val_I8_idx_I8.c  
#--------------------------------------------------------------------------
qsort_dsc_val_I4_idx_I4.o : ./AUTOGEN/GENFILES/qsort_dsc_val_I4_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_dsc_val_I4_idx_I4.c  
qsort_dsc_val_I4_idx_I8.o : ./AUTOGEN/GENFILES/qsort_dsc_val_I4_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_dsc_val_I4_idx_I8.c  
qsort_dsc_val_I8_idx_I4.o : ./AUTOGEN/GENFILES/qsort_dsc_val_I8_idx_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_dsc_val_I8_idx_I4.c  
qsort_dsc_val_I8_idx_I8.o : ./AUTOGEN/GENFILES/qsort_dsc_val_I8_idx_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/qsort_dsc_val_I8_idx_I8.c  
#--------------------------------------------------------------------------
core_srt_join_I4_I4_I4.o : ./AUTOGEN/GENFILES/core_srt_join_I4_I4_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I4_I4_I4.c  
core_srt_join_I4_I4_I8.o : ./AUTOGEN/GENFILES/core_srt_join_I4_I4_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I4_I4_I8.c  
core_srt_join_I8_I4_I4.o : ./AUTOGEN/GENFILES/core_srt_join_I8_I4_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I8_I4_I4.c  
core_srt_join_I8_I4_I8.o : ./AUTOGEN/GENFILES/core_srt_join_I8_I4_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I8_I4_I8.c  
core_srt_join_I4_I8_I4.o : ./AUTOGEN/GENFILES/core_srt_join_I4_I8_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I4_I8_I4.c  
core_srt_join_I4_I8_I8.o : ./AUTOGEN/GENFILES/core_srt_join_I4_I8_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I4_I8_I8.c  
core_srt_join_I8_I8_I4.o : ./AUTOGEN/GENFILES/core_srt_join_I8_I8_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I8_I8_I4.c  
core_srt_join_I8_I8_I8.o : ./AUTOGEN/GENFILES/core_srt_join_I8_I8_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_srt_join_I8_I8_I8.c  
#--------------------------------------------------------------------------
vec_abs_F4.o : vec_abs_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c vec_abs_F4.c  
vec_abs_F8.o : vec_abs_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c vec_abs_F8.c  
vec_sqrt_F4.o : vec_sqrt_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c vec_sqrt_F4.c  
vec_sqrt_F8.o : vec_sqrt_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c vec_sqrt_F8.c  
vec_reciprocal_F4.o : vec_reciprocal_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c vec_reciprocal_F4.c  
vec_reciprocal_F8.o : vec_reciprocal_F8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c vec_reciprocal_F8.c  
#--------------------------------------------------------------------------
core_wisifxthenyelsez_I4.o : ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I4.c  
core_wisifxthenyelsez_I8.o : ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I8.c  
core_wisifxthenyelsez_I1.o : ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I1.c  
core_wisifxthenyelsez_I2.o : ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_wisifxthenyelsez_I2.c  
core_wisifxthenyelsez_F4.o : ./AUTOGEN/GENFILES/core_wisifxthenyelsez_F4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/core_wisifxthenyelsez_F4.c  
#--------------------------------------------------------------------------
is_sorted_alldef_I1.o : ./AUTOGEN/GENFILES/is_sorted_alldef_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_sorted_alldef_I1.c  
is_sorted_alldef_I2.o : ./AUTOGEN/GENFILES/is_sorted_alldef_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_sorted_alldef_I2.c  
is_sorted_alldef_I4.o : ./AUTOGEN/GENFILES/is_sorted_alldef_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_sorted_alldef_I4.c  
is_sorted_alldef_I8.o : ./AUTOGEN/GENFILES/is_sorted_alldef_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/is_sorted_alldef_I8.c  
#--------------------------------------------------------------------------
bin_search_I1.o : ./AUTOGEN/GENFILES/bin_search_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bin_search_I1.c  
bin_search_I2.o : ./AUTOGEN/GENFILES/bin_search_I2.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bin_search_I2.c  
bin_search_I4.o : ./AUTOGEN/GENFILES/bin_search_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bin_search_I4.c  
bin_search_I8.o : ./AUTOGEN/GENFILES/bin_search_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bin_search_I8.c  
bin_search_UI4.o : ./AUTOGEN/GENFILES/bin_search_UI4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bin_search_UI4.c  
bin_search_UI8.o : ./AUTOGEN/GENFILES/bin_search_UI8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/bin_search_UI8.c  
#--------------------------------------------------------------------------
countf_nn_I1.o : ./AUTOGEN/GENFILES/countf_nn_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_nn_I1.c  
countf_nn_I2.o : ./AUTOGEN/GENFILES/countf_nn_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_nn_I2.c  
countf_nn_I4.o : ./AUTOGEN/GENFILES/countf_nn_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_nn_I4.c  
countf_nn_I8.o : ./AUTOGEN/GENFILES/countf_nn_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_nn_I8.c  
#--------------------------------------------------------------------------
countf_I1.o : ./AUTOGEN/GENFILES/countf_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_I1.c  
countf_I2.o : ./AUTOGEN/GENFILES/countf_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_I2.c  
countf_I4.o : ./AUTOGEN/GENFILES/countf_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_I4.c  
countf_I8.o : ./AUTOGEN/GENFILES/countf_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/countf_I8.c  
#--------------------------------------------------------------------------
par_count_I1.o : ./AUTOGEN/GENFILES/par_count_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_count_I1.c  
par_count_I2.o : ./AUTOGEN/GENFILES/par_count_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_count_I2.c  
#--------------------------------------------------------------------------
par_count_cfld_I1.o : ./AUTOGEN/GENFILES/par_count_cfld_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_count_cfld_I1.c  
par_count_cfld_I2.o : ./AUTOGEN/GENFILES/par_count_cfld_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_count_cfld_I2.c  
#--------------------------------------------------------------------------
par_countf_I1.o : ./AUTOGEN/GENFILES/par_countf_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_countf_I1.c  
par_countf_I2.o : ./AUTOGEN/GENFILES/par_countf_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_countf_I2.c  
#--------------------------------------------------------------------------
par_countf_cfld_I1.o : ./AUTOGEN/GENFILES/par_countf_cfld_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_countf_cfld_I1.c  
par_countf_cfld_I2.o : ./AUTOGEN/GENFILES/par_countf_cfld_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_countf_cfld_I2.c  
#--------------------------------------------------------------------------
count_nn_I1.o : ./AUTOGEN/GENFILES/count_nn_I1.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_nn_I1.c  
count_nn_I2.o : ./AUTOGEN/GENFILES/count_nn_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_nn_I2.c  
count_nn_I4.o : ./AUTOGEN/GENFILES/count_nn_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_nn_I4.c  
count_nn_I8.o : ./AUTOGEN/GENFILES/count_nn_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_nn_I8.c  
#--------------------------------------------------------------------------
count_I1.o : ./AUTOGEN/GENFILES/count_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_I1.c  
count_I2.o : ./AUTOGEN/GENFILES/count_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_I2.c  
count_I4.o : ./AUTOGEN/GENFILES/count_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_I4.c  
count_I8.o : ./AUTOGEN/GENFILES/count_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/count_I8.c  
#--------------------------------------------------------------------------
which_bin_I4.o : ./AUTOGEN/GENFILES/which_bin_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/which_bin_I4.c  
which_bin_I8.o : ./AUTOGEN/GENFILES/which_bin_I8.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/which_bin_I8.c  
#--------------------------------------------------------------------------
patmatch_I1.o : ./AUTOGEN/GENFILES/patmatch_I1.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/patmatch_I1.c  
patmatch_I2.o : ./AUTOGEN/GENFILES/patmatch_I2.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/patmatch_I2.c  
patmatch_I4.o : ./AUTOGEN/GENFILES/patmatch_I4.c  
	 $(ICC) $(ICCFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/patmatch_I4.c  
#--------------------------------------------------------------------------
union_I4.o : ./AUTOGEN/GENFILES/union_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/union_I4.c  
union_I8.o : ./AUTOGEN/GENFILES/union_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/union_I8.c  
intersection_I4.o : ./AUTOGEN/GENFILES/intersection_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/intersection_I4.c  
intersection_I8.o : ./AUTOGEN/GENFILES/intersection_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/intersection_I8.c  
a_minus_b_I4.o : ./AUTOGEN/GENFILES/a_minus_b_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/a_minus_b_I4.c  
a_minus_b_I8.o : ./AUTOGEN/GENFILES/a_minus_b_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/a_minus_b_I8.c  
#--------------------------------------------------------------------------
par_sort_with_bins_I4.o : ./AUTOGEN/GENFILES/par_sort_with_bins_I4.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_sort_with_bins_I4.c  
par_sort_with_bins_I8.o : ./AUTOGEN/GENFILES/par_sort_with_bins_I8.c  
	 $(CC) $(CFLAGS) $(INCLUDES) -c ./AUTOGEN/GENFILES/par_sort_with_bins_I8.c  
