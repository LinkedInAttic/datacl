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
#include <limits.h>
#include <values.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "set_meta.h"
#include "is_fld.h"
#include "mk_file.h"
#include "ext_f_to_s.h"
#include "par_f_to_s.h"
#include "is_tbl.h"
#include "meta_globals.h"
#include "is_sorted_alldef_I1.h"
#include "is_sorted_alldef_I2.h"
#include "is_sorted_alldef_I4.h"
#include "is_sorted_alldef_I8.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
ext_f_to_s(
	   const char *tbl,
	   const char *fld,
	   const char *op,
	   char *rslt_buf,
	   int sz_rslt_buf
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  FILE  *ofp = NULL; 
#define MAX_LEN 32
  long long nR;
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE fld_rec; int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 
  char **buffers = NULL; int n_buffers = 0;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  nR = tbl_rec.nR;
  status = is_fld(tbl, -1, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  cBYE(status);
  status = get_data(fld_rec, &X, &nX, false); cBYE(status);
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, false); cBYE(status);
  }
  if ( strcmp(op, "is_sorted") == 0 ) { /* not parallelized yet */
    // If you have null values, you cannot be sorted
    if ( nn_fld_id >= 0 ) {
      g_flds[fld_id].sort_type = unsorted;
    }
    else if ( g_flds[fld_id].sort_type == unknown ) { 
      switch ( fld_rec.fldtype ) { 
	case I1 : is_sorted_alldef_I1((char *)X, nR, &(g_flds[fld_id].sort_type)); break;
	case I2 : is_sorted_alldef_I2((short *)X, nR, &(g_flds[fld_id].sort_type)); break;
	case I4 : is_sorted_alldef_I4((int *)X, nR, &(g_flds[fld_id].sort_type)); break;
	case I8 : is_sorted_alldef_I8((long long *)X, nR, &(g_flds[fld_id].sort_type)); break;
	default : go_BYE(-1); break;
      }
    }
    status = mk_str_sort_type(g_flds[fld_id].sort_type, rslt_buf);
    cBYE(status);
  }
  else {
    status = par_f_to_s(X, fld_rec.fldtype, nn_X, 0, nR, op, 
			rslt_buf, sz_rslt_buf);
    cBYE(status);
    if ( strcmp(op, "sum") == 0 ) { /* set count in fld_meta */
      char countbuf[32];
      zero_string(countbuf, 32);
      for ( int i = 0; i < 32; i++ ) { 
	if ( rslt_buf[i] == ':' ) { break; }
	if ( rslt_buf[i] == '\0' ) { go_BYE(-1); }
	countbuf[i] = rslt_buf[i];
      }
      // TODO: P2: If I1 need to make sure vals are 0 or 1.
      if ( ( fld_rec.fldtype == I1 ) || ( fld_rec.fldtype == B ) )  { 
        status = int_set_meta(tbl_id, fld_id, "cnt", countbuf); cBYE(status);
      }
    }
  }
 BYE:
  for ( int i = 0; i < n_buffers; i++ ) { 
    free_if_non_null(buffers[i]);
  }
  free_if_non_null(buffers);
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  free_if_non_null(nn_X);
  fclose_if_non_null(ofp);
  return(status);
}
