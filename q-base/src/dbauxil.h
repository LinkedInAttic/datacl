/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
extern int
mk_join_op(
	   char *str_join_op,
	   int *ptr_join_op
	   )
;
//----------------------------
extern int
del_ht_tbl_entry(
		 int tbl_id
		 )
;
//----------------------------
extern int
del_ht_fld_entry(
		 int fld_id
		 )
;
//----------------------------
extern int
del_aux_fld(
	    int primary_fld_id,
	    AUX_TYPE auxtype
	    )
;
//----------------------------
extern int
mk_name_aux_fld(
		char *fld,
		AUX_TYPE auxtype,
		char *aux_fld
		)
;
//----------------------------
extern int
chk_file_size(
	      long long nR,
	      FLD_REC_TYPE fld_rec,
	      size_t *ptr_filesz
	      )
;
//----------------------------
extern int partition(
	      long long num_to_divide,
	      long long min_block_size,
	      int max_num_chunks,
	      long long *ptr_block_size,
	      int *ptr_num_chunks /* this many chunks of work */
	      )
;
//----------------------------
extern int
get_data(
	 FLD_REC_TYPE fld_rec,
	 char **ptr_X,
	 size_t *ptr_nX,
	 bool is_write
	 )
;
//----------------------------
extern int 
q_mmap(
       int ddir_id,
       int fileno,
       char **ptr_X,
       size_t *ptr_nX,
       bool  is_write
       )
;
//----------------------------
extern int 
q_re_mmap(
	  int ddir_id,
	  int fileno,
	  char **ptr_X,
	  size_t *ptr_nX,
	  size_t new_filesize,
	  bool  is_write
	  )
;
//----------------------------
extern int 
q_delete(
	 int ddir_id,
	 int fileno
	 )
;
//----------------------------
extern int 
q_trunc(
	int ddir_id, 
	int fileno,
	size_t filesz
	)
;
//----------------------------
extern int
chk_aux_info(
	     const char *aux_info
	     )
;
//----------------------------
extern int
get_file_size_B(
		long long nR, 
		size_t *ptr_filesz
		)
;
//----------------------------
extern int
break_into_scalars(
		   const char *str_scalar,
		   FLD_TYPE fldtype,
		   int *ptr_num_scalars,
		   long long *ptr_sval_I8,
		   int       *ptr_sval_I4,
		   short     *ptr_sval_I2,
		   char      *ptr_sval_I1,
		   float     *ptr_sval_F4,
		   double    *ptr_sval_F8,
		   long long svals_I8[MAX_NUM_SCALARS], 
		   int       svals_I4[MAX_NUM_SCALARS], 
		   short     svals_I2[MAX_NUM_SCALARS], 
		   char      svals_I1[MAX_NUM_SCALARS], 
		   float     svals_F4[MAX_NUM_SCALARS], 
		   double    svals_F8[MAX_NUM_SCALARS],
		   char      **ptr_hashes_I1,
		   short     **ptr_hashes_I2,
		   int       **ptr_hashes_I4,
		   long long **ptr_hashes_I8,
		   unsigned int *ptr_n,
		   unsigned int *ptr_a,
		   unsigned int *ptr_b
		   )
;
//----------------------------
extern int
switch_dirs(
	    int ddir_id
	    )
;
//----------------------------
extern int
chk_env_vars(
	     char *in_docroot, 
	     char *in_gpu_server,
	     int *ptr_gpu_port
	     )
;
//----------------------------
extern int
get_max_fileno()
;
//----------------------------
extern int
is_ddir( /* ddir = data directory */
	const char *in_ddir,
	int *ptr_ddir_id
	 )
;
//----------------------------
extern int
set_ddir( /* ddir = data directory */
	 const char *in_ddir,
	 const char *str_ddir_id
	  )
;
//----------------------------
extern int
get_ddir( /* ddir = data directory */
	 const char *str_ddir_id,
	 char *buf,
	 int buflen
	  )
;
//----------------------------
extern int
mk_full_filename(
		 int ddir_id,
		 int fileno,
		 char *filename,
		 int bufsize
		 )
;
//----------------------------
extern int
add_to_fld_info(
		int fld_id,
		int mode,
		int aux_id
		)
;
//----------------------------
extern int
get_fld_info(
	     FLD_INFO_MODE_TYPE mode,
	     int fld_id,
	     int *ptr_out_id
	     )
;
//----------------------------
extern int
del_from_fld_info(
		  int fld_id
		  )
;
//----------------------------
extern int
del_from_fld_info_1(
		    int fld_id,
		    FLD_INFO_MODE_TYPE mode
		    )
;
//----------------------------
extern bool
chk_dsk_ddir_id(
		DDIR_REC_TYPE *ddir
		)
;
//----------------------------
extern int
start_compound_expr(
	  )
;
//----------------------------
extern int
stop_compound_expr(
	  )
;
//----------------------------
extern int
is_in_compound(
    int argc,
    char **argv,
    bool *ptr_b_is_in_compound
      )
;
//----------------------------
extern int
compute_nT_for_count(
    long long src_nR, 
    long long dst_nR, 
    int *ptr_nT
    )
;
//----------------------------
