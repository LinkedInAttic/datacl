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
determine_b_k(
	      double err,
	      int num_quantiles,
	      long long siz,
	      int *ptr_b,
	      int *ptr_k
	      )
;
//----------------------------
extern int 
approx_quantile(
		int *x, /* input data */
		long long siz, /* number of elements */
		int num_quantiles, /* number of quantiles */
		double err, /* acceptable error */
		int *y /* output */
		)
;
//----------------------------
