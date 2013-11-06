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
#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "fop.h"
#include "meta_globals.h"
#include "set_meta.h"

#include "extract_I8.h"
#include "extract_S.h"
#include "zero_after_n.h"

#include "sort_asc_F4.h"
#include "sort_dsc_F4.h"

#include "qsort_asc_I4.h"
#include "qsort_dsc_I4.h"

#include "qsort_asc_I8.h"
#include "qsort_dsc_I8.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
fop(
       char *tbl,
       char *fld,
       char *str_op_spec
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  TBL_REC_TYPE tbl_rec;    int tbl_id; 
  FLD_REC_TYPE fld_rec;    int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 
  long long nR; 
#define MAX_LEN 32
  char op[MAX_LEN];

  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  zero_string(op, MAX_LEN);
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbls[tbl_id].nR;
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  if ( fld_rec.is_external ) { go_BYE(-1); }

  status = get_data(fld_rec, &X, &nX, true); cBYE(status);
  if ( nX == 0 ) { go_BYE(-1); }
  if ( strcmp(str_op_spec, "sortA") == 0 ) {
    if ( fld_rec.fldtype == I4 ) {
      // OLD qsort(X, nR, sizeof(int), sort_asc_I4);
      if ( nR < INT_MAX ) { 
#ifdef IPP
        ippsSortAscend_32s_I((int *)X, nR);
#else
        qsort_asc_I4(X, nR, sizeof(int), NULL);
#endif
      }
      else {
        qsort_asc_I4(X, nR, sizeof(int), NULL);
      }
    }
    else if ( fld_rec.fldtype == F4 ) { 
      if ( nR < INT_MAX ) { 
#ifdef IPP
        ippsSortAscend_32f_I((float *)X, nR);
#else
        qsort(X, nR, sizeof(int), sort_asc_F4);
#endif
      }
      else {
        qsort(X, nR, sizeof(int), sort_asc_F4);
      }
    }
    else if ( fld_rec.fldtype == I8 ) { 
      // OLD qsort(X, nR, sizeof(long long), sort_asc_I8);
      qsort_asc_I8(X, nR, sizeof(long long), NULL);
    }
    else { 
      fprintf(stderr, "Not implemented\n"); go_BYE(-1);
    }
    status = int_set_meta(tbl_id, fld_id, "sort_type", "ascending");
    cBYE(status);
  }
  else if ( strcmp(str_op_spec, "sortD") == 0 ) { 
    if ( fld_rec.fldtype == I4 ) { 
      // OLD qsort(X, nR, sizeof(int), sort_dsc_I4);
      if ( nR < INT_MAX ) { 
#ifdef IPP
        ippsSortDescend_32s_I((int *)X, nR);
#else
        qsort_dsc_I4(X, nR, sizeof(int), NULL);
#endif
      }
      else {
        qsort_dsc_I4(X, nR, sizeof(int), NULL);
      }
    }
    else if ( fld_rec.fldtype == F4 ) { 
      if ( nR < INT_MAX ) { 
#ifdef IPP
        ippsSortDescend_32f_I((float *)X, nR);
#else
        qsort(X, nR, sizeof(float), sort_dsc_F4);
#endif
      }
      else {
        qsort(X, nR, sizeof(float), sort_dsc_F4);
      }
    }
    else if ( fld_rec.fldtype == I8 ) { 
      // OLD qsort(X, nR, sizeof(long long), sort_dsc_I8);
      qsort_dsc_I8(X, nR, sizeof(long long), NULL);
    }
    else { 
      fprintf(stderr, "Not implemented\n");
      go_BYE(-1);
    }
    status = int_set_meta(tbl_id, fld_id, "sort_type", "descending");
    cBYE(status);
  }
  else {
    bool is_null;
    long long one_cnt;
#define MAX_LEN_OP 32
    char op[MAX_LEN_OP];
    status = extract_S(str_op_spec, "op=[", "]", op, MAX_LEN_OP, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( strcmp(op, "zero_after_n") == 0 ) {
      if ( fld_rec.fldtype != B ) { go_BYE(-1); }
      status = extract_I8(str_op_spec, "limit=[", "]", &one_cnt, &is_null);
      cBYE(status);
      if ( is_null ) { go_BYE(-1); }
      if ( one_cnt <= 0 ) { go_BYE(-1); }
      status = zero_after_n(X, nR, one_cnt); cBYE(status);
      rs_munmap(X, nX);
    }
    else {
      go_BYE(-1);
    }
  }
  //-------------------------------------------------------
BYE:
  rs_munmap(X, nX);
  return(status);
}
