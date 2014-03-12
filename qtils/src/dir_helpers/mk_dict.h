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
setup_ht_dict(
	      HT_REC_TYPE **ptr_ht, 
	      int *ptr_sz_ht, 
	      int *ptr_n_ht
	      );
extern int
add_to_dict(
	    char *inval,
	    HT_REC_TYPE *ht,
	    int sz_ht,
	    int *ptr_n_ht,
	    unsigned long long *ptr_ulhash
	    );
extern  int dump_ht(
      HT_REC_TYPE *ht,
      int sz_ht,
      int n_ht,
      char *data_dir,
      char *chrfile,
      char *keyfile,
      char *lenfile,
      char *offfile
      );
extern int
get_from_dict(
	    char *inval,
	    long long *keys, /* [n_keys] */
	    int n_keys,
	    char *text,
	    int *offsets, /* [n_keys] */
	    int *lens, /* [n_keys] */
	    int *ptr_idx
	    );
