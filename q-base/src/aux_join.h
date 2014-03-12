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
join_prep(
	  char *src_tbl,
	  char *src_lnk, 
	  char *src_val,
	  char *dst_tbl,
	  char *dst_lnk,
	  char *dst_val,
	  FLD_TYPE *ptr_src_val_meta,
	  FLD_TYPE *ptr_src_lnk_meta,
	  FLD_TYPE *ptr_nn_src_val_meta,
	  FLD_TYPE *ptr_nn_src_lnk_meta,
	  FLD_TYPE *ptr_dst_lnk_meta,
	  FLD_TYPE *ptr_nn_dst_lnk_meta,
	  long long *ptr_src_nR,
	  long long *ptr_dst_nR,
	  char **ptr_src_val_X,
	  size_t *ptr_src_val_nX,
	  char **ptr_src_lnk_X,
	  size_t *ptr_src_lnk_nX,
	  char **ptr_nn_src_val_X,
	  size_t *ptr_nn_src_val_nX,
	  char **ptr_nn_src_lnk_X,
	  size_t *ptr_nn_src_lnk_nX,
	  char **ptr_dst_lnk_X,
	  size_t *ptr_dst_lnk_nX,
	  char **ptr_nn_dst_lnk_X,
	  size_t *ptr_nn_dst_lnk_nX
	  )
;
//----------------------------
