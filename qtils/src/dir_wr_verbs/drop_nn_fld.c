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
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "aux_meta.h"
#include "meta_globals.h"

// START FUNC DECL
int
drop_nn_fld(
	   char *tbl,
	   char *fld
	   )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = INT_MIN, fld_id = INT_MIN, nn_fld_id = INT_MIN;
  TBL_REC_TYPE tbl_rec; FLD_REC_TYPE fld_rec, nn_fld_rec;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  chk_range(fld_id, 0, g_n_fld);
  del_aux_fld(fld_id, nn);
 BYE:
  return(status);
}
