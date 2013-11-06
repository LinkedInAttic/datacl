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
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "assign_I1.h"
#include "assign_I4.h"
#include "assign_I8.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
set_val(
       char *tbl,
       char *fld,
       char *str_range,
       char *str_val
       )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0; 
  char **Y = NULL; int nY = 0;
  char *I1ptr = NULL; int *I4ptr = NULL; long long *I8ptr = NULL;

  int tbl_id, fld_id, nn_fld_id;
  long long nR;
  char *endptr = NULL; 
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_range == NULL ) || ( *str_range == '\0' ) ) { go_BYE(-1); }
  if ( ( str_val == NULL ) || ( *str_val == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
  if ( fld_id < 0 ) { 
    fprintf(stderr, "Field [%s] not in Table [%s] \n", fld, tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  status = get_data(fld_rec, &X, &nX, true); cBYE(status);
  //--------------------------------------------------------
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, true); cBYE(status);
  }
  //--------------------------------------------------------
  status = explode(str_range, ':', &Y, &nY); cBYE(status);
  if ( nY != 2 ) { go_BYE(-1); }

  long long lb = strtoll(Y[0], &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  chk_range(lb, 0, nR); 

  long long ub = strtoll(Y[1], &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  chk_range(ub, 0, nR+1); 
  if ( lb >= ub ) { go_BYE(-1); }

  long long I8val = strtoll(str_val, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  //--------------------------------------------------------
  switch ( fld_rec.fldtype ) { 
    case I1 :
      I1ptr = (char *)X;
      if ( ( I8val < SCHAR_MIN ) || (I8val > SCHAR_MAX ) ) { go_BYE(-1); }
      assign_const_I1(I1ptr + lb, (ub-lb), (int)I8val);
      break;
    case I4 :
      I4ptr = (int*)X;
      if ( ( I8val < INT_MIN ) || (I8val > INT_MAX ) ) { go_BYE(-1); }
      assign_const_I4(I4ptr + lb, (ub-lb), (int)I8val);
      break;
    case I8 :
      I8ptr = (long long*)X;
      assign_const_I8(I8ptr + lb, (ub-lb), I8val);
      break;
    default : 
      go_BYE(-1);
      break;
  }
  if ( nn_X != NULL ) { 
    assign_const_I1(nn_X + lb, (ub-lb), 1);
  }
  // P2 TODO: Need to make sure that if nn field is no longer needed, then
  // we delete the field 
 BYE:
  if ( Y != NULL ) { 
    for ( int i = 0; i < nY; i++ ) {
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  return(status);
}
