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
extern int chk_fld_info(
		 FLD_INFO_TYPE X
		 )
;
//----------------------------
extern int
chk_tbl_meta(
	     int tbl_id
	     )
;
//----------------------------
extern int
chk_fld_meta(
	     int fld_id,
	     long long nR
	     )
;
//----------------------------
extern void
zero_fld_info_rec(
		  FLD_INFO_TYPE *ptr_X
		  )
;
//----------------------------
extern bool
is_zero_fld_info_rec(
		     FLD_INFO_TYPE X
		     )
;
//----------------------------
extern void
zero_ddir_rec(
	      DDIR_REC_TYPE *ptr_X
	      )
;
//----------------------------
extern void
zero_fld_rec(
	     FLD_REC_TYPE *ptr_X
	     )
;
//----------------------------
extern bool
is_zero_fld_rec(
		FLD_REC_TYPE X
		)
;
//----------------------------
extern int
pr_fld_meta(
	    int fld_id,
	    FLD_REC_TYPE X
	    )
;
//----------------------------
extern int
mk_str_srttype(
	       int int_srttype,
	       char *str_srttype
	       )
;
//----------------------------
extern int
mk_str_tbltype(

	       TBL_TYPE tbltype,
	       char *str_tbltype
	       )
;
//----------------------------
extern int
chk_tbltype(
	    int tbltype
	    )
;
//----------------------------
extern int
chk_fldtype(
	    int fldtype
	    )
;
//----------------------------
extern int
chk_auxtype(
	    int auxtype
	    )
;
//----------------------------
extern int
chk_srttype(
	    int srttype
	    )
;
//----------------------------
extern int
mk_str_fldtype(
	       int fldtype,
	       char *str_fldtype
	       )
;
//----------------------------
extern int
mk_str_auxtype(
	       int auxtype,
	       char *str_auxtype
	       )
;
//----------------------------
extern int
get_fld_sz(
	   FLD_TYPE fldtype,
	   int *ptr_fld_sz
	   )
;
//----------------------------
extern int
unstr_tbltype(
	      const char *str_tbltype,
	      TBL_TYPE *ptr_tbltype
	      )
;
//----------------------------
extern int
unstr_fldtype(
	      const char *str_fldtype,
	      FLD_TYPE *ptr_fldtype
	      )
;
//----------------------------
extern int
unstr_auxtype(
	      const char *str_auxtype,
	      AUX_TYPE *ptr_auxtype
	      )
;
//----------------------------
extern bool
is_zero_tbl_rec(
		TBL_REC_TYPE X
		)
;
//----------------------------
extern void
zero_tbl_rec(
	     TBL_REC_TYPE *ptr_X
	     )
;
//----------------------------
extern int
chk_tbl_name(
	     const char *X
	     )
;
//----------------------------
extern void
copy_fld_meta(
	      FLD_REC_TYPE *ptr_dst_fld_meta,
	      FLD_REC_TYPE src_fld_meta
	      )
;
//----------------------------
extern int
get_ddir_id(
	    char *in_data_dir,
	    DDIR_REC_TYPE *ddirs,
	    int n_ddirs,
	    bool add_if_missing,
	    int *ptr_ddir_id
	    )
;
//----------------------------
extern int
chk_fld_name(
	     const char *in_X /* not to be used for aux fields */
	     )
;
//----------------------------
extern void
copy_tbl_meta(
	      TBL_REC_TYPE *ptr_dst_tbl_meta,
	      TBL_REC_TYPE src_tbl_meta
	      )
;
//----------------------------
extern int all_chk_fld_meta(
		     int num_to_check
		     )
;
//----------------------------
extern int
mk_str_fld_info_mode(
		     FLD_INFO_MODE_TYPE mode,
		     char *strbuf
		     )
;
//----------------------------
extern int
mk_str_exttype(
	       bool is_external,
	       char *strbuf
	       )
;
//----------------------------
extern int
copy_meta(
	  int dst_fld_id,
	  int src_fld_id,
	  int mode
	  )
;
//----------------------------
