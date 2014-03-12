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
core_pr_flds(
	     int tbl_id,
	     const char *str_flds,
	     char *cfld_X,
	     long long lb,
	     long long ub,
	    long long nR,
	     FILE *ofp
	     );
extern int
core_pr_fld(
	    const FLD_REC_TYPE fld_rec,
	    const FLD_REC_TYPE nn_fld_rec,
	    int nn_fld_id,
	    char *cfld_X,
	    long long lb,
	    long long ub,
	    long long nR,
	    FILE *ofp
	    );
extern int 
pr_fld(
       const char *tbl,
       const char *fld,
       const char *filter,
       FILE *ofp
       );
