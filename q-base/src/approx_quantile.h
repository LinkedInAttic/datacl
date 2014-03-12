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
ext_approx_quantile(
   char *tbl,
   char *fld,
   char *cfld,
   char *outtbl,
   char *str_num_q,
   char *str_eps,
   int *ptr_is_good
   )
;
//----------------------------
extern int 
approx_quantile(
		int * x,
		char * cfld,
		long long siz,
		int num_quantiles,
		double eps,
		int *y,
		long long y_siz,
		int *ptr_estimate_is_good
		)
;
//----------------------------
