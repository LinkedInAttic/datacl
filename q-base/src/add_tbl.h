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
get_empty_tbl(
	      const char *tbl,
	      TBL_REC_TYPE *tbls,
	      int n_tbl,
	      int *ptr_tbl_id
	      )
;
//----------------------------
extern int
add_tbl(
	const char *tbl,
	const char *str_nR,
	int *ptr_tbl_id,
	TBL_REC_TYPE *ptr_tbl_rec
	)
;
//----------------------------
