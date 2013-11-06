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
core_srt_join_I_I_I(
				      int *src_lnk,
				      int *src_val,
				      long long src_nR,
				      int *dst_lnk,
				      int *dst_val,
				      char *nn_dst_val,
				      long long dst_lb,
				      long long dst_ub,
				      int ijoin_op,
				      bool *ptr_is_any_null
				      )
;

