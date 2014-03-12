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
    char *data_dir,
	 char *infile,
	 char fld_delim, /* double quote character */
	 char fld_sep, /* comma */
	 char rec_delim, /* new line character */
	 bool ignore_hdr, /* whether to ignore header or not */
	 char flds[MAX_NUM_FLDS_TO_LOAD][MAX_LEN_FLD_NAME+1],
	 int n_flds,
	 FLD_TYPE fldtype[MAX_NUM_FLDS_TO_LOAD],
	 FLD_PROPS_TYPE fld_props[MAX_NUM_FLDS_TO_LOAD], /* properties of fields */
	 int filenos[MAX_NUM_FLDS_TO_LOAD],
	 int nn_filenos[MAX_NUM_FLDS_TO_LOAD],
	 int len_filenos[MAX_NUM_FLDS_TO_LOAD],
	 HT_REC_TYPE *hts[MAX_NUM_FLDS_TO_LOAD],
	 int sz_hts[MAX_NUM_FLDS_TO_LOAD],
	 int n_hts[MAX_NUM_FLDS_TO_LOAD],
	 int dict_tbl_id[MAX_NUM_FLDS_TO_LOAD],
	 long long *ptr_num_rows
	 )
;
//----------------------------
