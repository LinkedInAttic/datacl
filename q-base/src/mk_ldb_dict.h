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
mk_ldb_dict(
	    char *infile,
	    char *str_fld_sep,
	    char *ldb_dict,
	    char *str_mode, /* read, write, append */
	    char *joinfile, /* to store join key if provided */
	    char *outfile, /* to store outputs as I4 */
	    char *options, 
	    char *in_q_data_dir
	    )
;
//----------------------------
