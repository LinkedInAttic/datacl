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
patmatch(
	 char *t1,
	 char *f1, /* value to match */
	 char *brk_fld, /* session break field */
	 char *out_fld, /* condition field to be created */
	 char *tp, /* table that identifies pattern */
	 char *pat_fld, /* patfld[i] = ith member of pattern */
	 char *lb_fld, /* lb[i] = min number of times patfld[i] should occur */
	 char *ub_fld /* ub[i] = max number of times patfld[i] should occur */
	 )
;
//----------------------------
