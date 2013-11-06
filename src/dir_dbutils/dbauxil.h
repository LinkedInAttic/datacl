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
mk_offset(
	  int *szptr,
	  long long nR,
	  long long **ptr_offset
	  )
;
extern int
mk_ifldtype(
	    char *str_fldtype,
	    int *ptr_ifldtype
	    )
;
extern int
mk_mjoin_op(
	    char *str_mjoin_op,
	    int *ptr_mjoin_op
	    )
;
extern int
mk_iop(
       char *str_op,
       int *ptr_iop
       )
;
extern int
get_spec_enum(
	      char *in,
	      int *ptr_spec_enum,
	      char **ptr_alt_name,
	      int *ptr_id
	      )
;
extern int
get_aux_fld_id(
	       int fld_id, 
	       int auxtype, 
	       int *ptr_aux_fld_id
	       )
;
extern void
pr_disp_name(
	     FILE *ofp,
	     char *disp_name
	     )
;
extern int
mk_int_auxtype(
	       char *str_auxtype, 
	       int *ptr_auxtype
	       )
;
extern int
del_ht_tbl_entry(
		 int tbl_id
		 )
;
extern int
del_ht_fld_entry(
		 int fld_id
		 )
;
extern int
del_aux_fld(
	    int primary_fld_id,
	    AUX_TYPE auxtype
	    )
;
extern int
mk_name_aux_fld(
		char *fld,
		AUX_TYPE auxtype,
		char *aux_fld
		)
;
extern   int
    chk_if_ephemeral(
	char **ptr_fld
	)
;

extern int
chk_file_size(
    char *filename,
    long long nR,
    FLD_TYPE fldtype
    );
extern int partition(
    long long num_to_divide,
    long long min_block_size,
    int max_num_chunks,
    long long *ptr_block_size,
    int *ptr_num_chunks /* this many chunks of work */
    );
extern int
get_data(
    FLD_REC_TYPE fld_rec,
    char **ptr_X,
    size_t *ptr_nX,
    bool is_write
    );
extern int 
q_mmap(
    int ddir_id,
    char *filename,
    char **ptr_X,
    size_t *ptr_nX,
    bool  is_write
    );
extern int 
q_re_mmap(
    int ddir_id,
    char *filename,
    char **ptr_X,
    size_t *ptr_nX,
    size_t new_filesize,
    bool  is_write
    );
extern int 
q_trunc(
    int ddir_id,
    char *filename,
    size_t filesz
    );
extern int
      chk_aux_info(
	  char *aux_info
	  );
extern int 
q_delete(
    int ddir_id,
    char *filename
    );
extern int
get_file_size_B(
    long long nR, 
    long long *ptr_filesz
    );
extern int
  break_into_scalars(
      char *str_scalar,
      FLD_TYPE fldtype,
      int *ptr_num_scalar_vals,
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
  double    svals_F8[MAX_NUM_SCALARS]
  );
