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
chk_fld_meta(
    FLD_REC_TYPE M,
    bool is_partial, 
    long long nR
    )
;
extern void
zero_fld_rec(
    FLD_REC_TYPE *ptr_X
    )
;
extern int
pr_fld_meta(
    FLD_REC_TYPE X
    )
;
extern int 
fld_meta_to_str(
    FLD_REC_TYPE fld_meta,
    char *buffer
    )
;
extern int
mk_str_sort_type(
    int sort_type,
    char *str_sort_type
    );
extern int
mk_str_fldtype(
    int fldtype,
    char *str_fldtype
    )
;
extern int
unstr_fldtype(
    char *str_fldtype,
    FLD_TYPE *ptr_fldtype
    );
extern void
zero_tbl_rec(
    TBL_REC_TYPE *ptr_X
    )
;
extern int
chk_tbl_name(
    const char *X
    )
;
extern void
copy_fld_meta(
  FLD_REC_TYPE *ptr_dst_fld_meta,
  FLD_REC_TYPE src_fld_meta
  )
;
extern int
get_fld_sz(
    FLD_TYPE fldtype,
    int *ptr_fld_sz
    );
extern int
unstr_auxtype(
    const char *str_auxtype,
    AUX_TYPE *ptr_auxtype
    );
extern void
zero_ddir_rec(
    DDIR_REC_TYPE *ptr_X
    );
extern int
get_ddir_id(
    char *data_dir,
    DDIR_REC_TYPE *ddirs,
    int n_dddirs,
    bool add_if_missing,
    int *ptr_ddir_id
    );
extern int
get_fld_meta(
    int fld_id,
    FLD_REC_TYPE *ptr_fld_meta
    );
extern int
chk_fld_name(
    const char *in_X,
    int is_aux /* 0 => false, 1 => true, -1 => dont care */
    );
extern void
copy_tbl_meta(
	      TBL_REC_TYPE *ptr_dst_tbl_meta,
	      TBL_REC_TYPE src_tbl_meta
	      );
