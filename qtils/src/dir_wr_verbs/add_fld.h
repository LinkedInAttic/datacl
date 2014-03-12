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
get_empty_fld(
    const char *fld,
	      FLD_REC_TYPE *flds,
	      int n_fld,
	      int *ptr_fld_id
	      );
extern int
add_fld(
	int tbl_id,
	const char *fld,
	int ddir_id,
	char *filename,
	FLD_TYPE fldtype,
	int len,
	int *ptr_fld_id,
	FLD_REC_TYPE *ptr_fld_rec
	);
extern int
ext_add_fld(
	const char *tbl,
	const char *fld,
	const char *fldspec
    );
