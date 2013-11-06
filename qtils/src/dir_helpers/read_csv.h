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
read_csv(
	 char *infile,
	 char fld_delim, /* double quote character */
	 char fld_sep, /* comma */
	 char rec_delim, /* new line character */
	 bool ignore_hdr, /* whether to ignore header or not */
	 char flds[MAX_NUM_FLDS][MAX_LEN_FLD_NAME+1],
	 int n_flds,
	 FLD_TYPE *fldtype,
	 FLD_PROPS_TYPE *fld_props,
	 char fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
	 char nn_fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
	 HT_REC_TYPE *hts[MAX_NUM_FLDS],
	 int sz_hts[MAX_NUM_FLDS],
	 int n_hts[MAX_NUM_FLDS],
	 int dict_tbl_id[MAX_NUM_FLDS],
	 long long *ptr_num_rows
	 );
extern int
print_cell(
	   char *X, 
	   long long lb, /* inclusive */ 
	   long long ub, /* exclusive */
	   FLD_TYPE fldtype,
	   FLD_PROPS_TYPE *ptr_fld_prop,
	   FILE *fp,
	   FILE *nn_fp, 
	   char fld_sep,
	   int n_keys,
	   char *key_X,
	   char *text_X,
	   char *off_X,
	   char *len_X,
	   HT_REC_TYPE *ht,
	   int sz_ht,
	   int *ptr_n_ht
	   );
