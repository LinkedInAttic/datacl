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
print_cell(
	   long long nR, /* current row */
	   char *fld, /* current field */
	   char *X,  
	   long long lb, /* inclusive */ 
	   long long ub, /* exclusive */
	   FLD_TYPE fldtype,
	   FLD_PROPS_TYPE *ptr_fld_prop,
	   FILE *fp,
	   FILE *nn_fp, 
	   FILE *len_fp, 
	   int n_keys,
	   char *key_X,
	   char *txt_X,
	   char *off_X,
	   char *len_X,
	   HT_REC_TYPE *ht,
	   int sz_ht,
	   int *ptr_n_ht
	   )
;
//----------------------------
