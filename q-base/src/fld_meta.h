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
pr_min_max_sum(
	       char *mode,
	       FLD_REC_TYPE fld_rec
	       )
;
//----------------------------
extern int
fld_meta(
	 const char *tbl,
	 const char *fld,
	 const char *what_to_pr,
	 int *ptr_fld_id,
	 bool is_print
	 )
;
//----------------------------
extern int
int_get_fld_meta(
		 const char *tbl,
		 int in_tbl_id,
		 const char *fld,
		 int in_fld_id,
		 const char *attr,
		 char *value,
		 int sz
		 )
;
//----------------------------
extern int
get_fld_meta(
	     int fld_id,
	     FLD_REC_TYPE *ptr_fld_meta
	     )
;
//----------------------------
extern int
file_to_fld(
	    char *str_fileno,
	    int *ptr_fld_id,
	    char *rslt_buf,
	    int sz_rslt_buf
	    )
;
//----------------------------
