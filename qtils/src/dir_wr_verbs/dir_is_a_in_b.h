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
dir_is_a_in_b(
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *cfld,
	  char *t3, /* serves as directory */
	  char *lb_val,  /* lower bound of values for this range */
	  char *ub_val,  /* upper bound of values for this range */
	  char *lb_idx,  /* lower bound of indexes for this range */
	  char *ub_idx   /* upper bound of indexes for this range */
	  );
