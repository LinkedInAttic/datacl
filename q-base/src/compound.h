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
extern int proc_op_spec(
		 char *op_spec, 
		 int lno, 
		 COMP_EXPR_TYPE *comp_expr,
		 char *pure_op
		 )
;
//----------------------------
extern int proc_qfn(
	     const char *op, 
	     int lno, 
	     COMP_EXPR_TYPE *comp_expr
	     )
;
//----------------------------
extern int
is_created_prior(
		 char *fld,
		 COMP_EXPR_TYPE *comp_expr,
		 int n_comp_expr,
		 int is_temp,
		 int *ptr_expr_idx,
		 int *ptr_fld_idx
		 )
;
//----------------------------
extern int proc_scalar(
		char *scalar, 
		int lno, 
		COMP_EXPR_TYPE *comp_expr
		)
;
//----------------------------
extern int proc_env_var(
		 char *env_var, 
		 int lno, 
		 COMP_EXPR_TYPE *comp_expr
		 )
;
//----------------------------
extern int
proc_fld(
	 int tbl_id,
	 long long nR,
	 char *fld, 
	 FLD_TYPE dst_fldtype,
	 bool dst_has_null,
	 int expr_idx,
	 int fld_idx,
	 int is_write, 
	 COMP_EXPR_TYPE *comp_expr,
	 FLD_TYPE *ptr_fldtype,
	 bool *ptr_has_null
	 )
;
//----------------------------
extern int 
chk_comp_expr(
	      char *filename,
	      COMP_EXPR_TYPE **ptr_comp_expr,
	      int *ptr_n_comp_expr,
	      int *ptr_tbl_id,
	      TBL_REC_TYPE *ptr_tbl_rec

	      )
;
//----------------------------
extern void 
zero_comp_expr(
	       COMP_EXPR_TYPE *comp_expr, 
	       int n_comp_expr
	       )
;
//----------------------------
extern int
release_comp_expr(
		  COMP_EXPR_TYPE **ptr_comp_expr, 
		  int *ptr_n_comp_expr
		  )
;
//----------------------------
extern int
exec_comp_expr(
	       int tbl_id,
	       TBL_REC_TYPE tbl_rec,
	       COMP_EXPR_TYPE *comp_expr, 
	       int n_comp_expr
	       )
;
//----------------------------
