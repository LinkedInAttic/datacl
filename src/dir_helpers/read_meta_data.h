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
read_meta_data(
	       char *infile,
	       char flds[MAX_NUM_FLDS][MAX_LEN_FLD_NAME+1],
	       FLD_TYPE fldtype[MAX_NUM_FLDS],
	       FLD_PROPS_TYPE fld_props[MAX_NUM_FLDS],
	       char dicts[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
	       int *ptr_n_flds
	       );
