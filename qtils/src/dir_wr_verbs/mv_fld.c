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
/* Move a column (both data and meta data) from one table to the other. 
 * Note that it will no longer exist in source column */

#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "mv_fld.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
mv_fld(
	 char *src_tbl,
	 char *src_fld,
	 char *dst_tbl
	 )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE src_fld_rec, nn_src_fld_rec;
  FLD_REC_TYPE dst_fld_rec, nn_dst_fld_rec;
  long long nR1 = LLONG_MIN, nR2  = LLONG_MIN;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int src_fld_id = INT_MIN, dst_fld_id = INT_MIN;
  int nn_src_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  nR1 = g_tbls[src_tbl_id].nR;

  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  nR2 = g_tbls[dst_tbl_id].nR;
  if ( nR1 != nR2 ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);

  // Add field from to meta data store 
  status = add_fld(dst_tbl_id, src_fld, src_fld_rec.ddir_id, 
      src_fld_rec.filename, src_fld_rec.fldtype, -1, &dst_fld_id, &dst_fld_rec);
  cBYE(status);
  /* Sort status stays the same */
  g_flds[dst_fld_id].sort_type = g_flds[src_fld_id].sort_type;
  if ( nn_src_fld_id >= 0 ) { 
    status = add_aux_fld(NULL, dst_tbl_id ,NULL, dst_fld_id, 
	nn_src_fld_rec.ddir_id, nn_src_fld_rec.filename, "nn", 
	&nn_dst_fld_id, &nn_dst_fld_rec);
    cBYE(status);
  }
  /* Clear out src_fld meta data */
  zero_fld_rec(&(g_flds[src_fld_id]));
  if ( nn_src_fld_id >= 0 ) {
    zero_fld_rec(&(g_flds[nn_src_fld_id]));
  }
 BYE:
  return(status);
}
