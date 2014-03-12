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
g_num_free_regs(
		GPU_REG_TYPE *gpu_reg,
		int n_gpu_reg
		)
;
//----------------------------
extern int
g_is_fld(
	 GPU_REG_TYPE *gpu_reg,
	 int n_gpu_reg,
	 char *d_fld,
	 int *ptr_d_fld_id,
	 GPU_REG_TYPE *ptr_gpu_reg

	 )
;
//----------------------------
extern int
g_meta_to_str(
	      char *buffer, 
	      char *d_fld, 
	      TBL_REC_TYPE tbl_rec, 
	      FLD_REC_TYPE h_fld_rec
	      )
;
//----------------------------
extern int is_fldtype_goodfor_gpu(
			   FLD_TYPE fldtype
			   )
;
//----------------------------
extern int g_call_gpu(
	       char *url, 
	       char **ptr_response
	       )
;
//----------------------------
extern int
g_info(
    )
;
//----------------------------
extern int
g_make_url(
	   char *command,
	   char *url
	   )
;
//----------------------------
extern int
g_chk_f_to_s(
	       GPU_REG_TYPE reg1,
	       const char *str_op
	       )
;
//----------------------------
extern int
g_chk_count(
	       GPU_REG_TYPE reg_1,
	       GPU_REG_TYPE reg_c,
	       GPU_REG_TYPE reg_2
	       )
;
//----------------------------
extern int
g_chk_countf(
	       GPU_REG_TYPE reg_1,
	       GPU_REG_TYPE reg_c,
	       GPU_REG_TYPE reg_2,
	       GPU_REG_TYPE reg_o
	       )
;
//----------------------------
extern int
g_chk_shift(
	       GPU_REG_TYPE reg1,
	       GPU_REG_TYPE reg2,
	       long long shift_amt,
	       long long newval
	       )
;
//----------------------------
extern int
g_chk_f1s1opf2(
	       GPU_REG_TYPE reg1,
	       GPU_REG_TYPE reg2,
	       const char *str_op
	       )
;
//----------------------------
extern int
g_chk_f1f2opf3(
	       GPU_REG_TYPE reg1,
	       GPU_REG_TYPE reg2,
	       const char *str_op,
	       GPU_REG_TYPE reg3
	       )
;
//----------------------------
extern int
g_chk_join(
	   GPU_REG_TYPE reg_ls,
	   GPU_REG_TYPE reg_ld,
	   GPU_REG_TYPE reg_vs,
	   GPU_REG_TYPE reg_vd,
	   GPU_REG_TYPE reg_vd_nn
	   )
;
//----------------------------
extern int
g_chk_f1f2_to_s(
		GPU_REG_TYPE reg1,
		GPU_REG_TYPE reg2,
		const char *str_op
		)
;
//----------------------------
extern void
zero_gpu_reg(
	     GPU_REG_TYPE *ptr_gpu_reg
	     )
;
//----------------------------
