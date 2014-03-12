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
lkp_sort(
	char *t1,
	char *f1,
	char *t2,
	char *cnt, /* t2[i] is the number of times t1.f1 has value i */
	/* Hence, t1.f1 < num_rows(t2) */
	char *idx_f1,
	char *srt_f1
	)
;
//----------------------------
