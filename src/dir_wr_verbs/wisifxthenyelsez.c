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
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

#include "core_wisifxthenyelsez_I4.h"
#include "core_wisifxthenyelsez_I8.h"
#include "core_wisifxthenyelsez_I1.h"
#include "core_wisifxthenyelsez_F4.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
wisifxthenyelsez(
		 char *tbl,
		 char *w,
		 char *x,
		 char *y,
		 char *z
		 )
// STOP FUNC DECL
{
  int status = 0;
  char *W = NULL; size_t nW = 0;
  char *X = NULL; size_t nX = 0;
  char *Y = NULL; size_t nY = 0;
  char *Z = NULL; size_t nZ = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *nn_W = NULL; size_t nn_nW = 0;
  char *nn_Y = NULL; size_t nn_nY = 0;
  char *nn_Z = NULL; size_t nn_nZ = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE x_rec, nn_x_rec;
  FLD_REC_TYPE y_rec, nn_y_rec;
  FLD_REC_TYPE z_rec, nn_z_rec;
  FLD_REC_TYPE w_rec, nn_w_rec;
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  int tbl_id = INT_MIN; 
  int x_id = INT_MIN, nn_x_id = INT_MIN;
  int y_id = INT_MIN, nn_y_id = INT_MIN;
  int z_id = INT_MIN, nn_z_id = INT_MIN;
  int w_id = INT_MIN, nn_w_id = INT_MIN;
  long long nR;
  bool is_some_null = false;  char *endptr; 
  FLD_TYPE w_fldtype;
  int       yvalI4 = INT_MAX; 
  long long yvalI8 = LLONG_MAX; 
  char      yvalI1 = SCHAR_MAX; 
  float     yvalF4 = FLT_MAX;
  int       zvalI4 = INT_MAX; 
  long long zvalI8 = LLONG_MAX; 
  char      zvalI1 = SCHAR_MAX; 
  float     zvalF4 = FLT_MAX;

  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( w == NULL ) || ( *w == '\0' ) ) { go_BYE(-1); }
  if ( ( x == NULL ) || ( *x == '\0' ) ) { go_BYE(-1); }
  if ( ( y == NULL ) || ( *y == '\0' ) ) { go_BYE(-1); }
  if ( ( z == NULL ) || ( *z == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(w, x) == 0 ) { go_BYE(-1); }
  if ( strcmp(x, y) == 0 ) { go_BYE(-1); }
  if ( strcmp(y, z) == 0 ) { go_BYE(-1); }
  if ( strcmp(z, w) == 0 ) { go_BYE(-1); }
  if ( strcmp(y, w) == 0 ) { go_BYE(-1); }
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(nn_opfile, (MAX_LEN_FILE_NAME+1));
  zero_fld_rec(&y_rec); zero_fld_rec(&nn_y_rec);
  zero_fld_rec(&z_rec); zero_fld_rec(&nn_z_rec);
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  // Get info about field x
  status = is_fld(NULL, tbl_id, x, &x_id, &x_rec, &nn_x_id, &nn_x_rec); 
  cBYE(status);
  chk_range(x_id, 0, g_n_fld);
  if ( x_rec.fldtype != I1 ) { go_BYE(-1); }
  status = get_data(x_rec, &X, &nX, 0); cBYE(status);
  if ( nn_x_id >= 0 ) { 
    status = get_data(nn_x_rec, &nn_X, &nn_nX, 0); cBYE(status);
  }
  //-=======================================================
  // Get info about field y
  status = is_fld(NULL, tbl_id, y, &y_id, &y_rec, &nn_y_id, &nn_y_rec); 
  cBYE(status);
  // Get info about field z
  status = is_fld(NULL, tbl_id, z, &z_id, &z_rec, &nn_z_id, &nn_z_rec); 
  cBYE(status);

  if ( y_id < 0 ) { /* Then y is a scalar */
    switch ( z_rec.fldtype ) {
      case I1 : 
	yvalI8 = strtoll(y, &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	if ( ( yvalI8 < SCHAR_MIN ) || ( yvalI8 > SCHAR_MAX ) ) { go_BYE(-1); }
	yvalI1 = yvalI8;
	break;
      case I4 : 
	yvalI8 = strtoll(y, &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	if ( ( yvalI8 < INT_MIN ) || ( yvalI8 > INT_MAX ) ) { go_BYE(-1); }
	yvalI4 = yvalI8;
	break;
      case I8 : 
	yvalI8 = strtoll(y, &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	break;
      case F4 : 
	yvalI4 = strtod(y, &endptr);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	break;
      case I2 : 
      case clob : 
      case F8 : 
      default : 
	go_BYE(-1);
	break;
    }
  }
  else {
    chk_range(y_id, 0, g_n_fld);
    status = get_data(y_rec, &Y, &nY, 0); cBYE(status);
    if ( nn_y_id >= 0 ) { 
      status = get_data(nn_y_rec, &nn_Y, &nn_nY, 0); cBYE(status);
    }
  }
  //-=======================================================
  // Get info about field z
  status = is_fld(NULL, tbl_id, z, &z_id, &z_rec, &nn_z_id, &nn_z_rec); 
  cBYE(status);
  if ( z_id < 0 ) { /* Then z is a scalar */
    switch ( y_rec.fldtype ) { 
      case I1 : 
	zvalI8 = strtoll(z, &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	if ( ( zvalI8 < SCHAR_MIN ) || ( zvalI8 > SCHAR_MAX ) ) { go_BYE(-1); }
	zvalI1 = zvalI8;
	break;
      case I4 : 
	zvalI8 = strtoll(z, &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	if ( ( zvalI8 < INT_MIN ) || ( zvalI8 > INT_MAX ) ) { go_BYE(-1); }
	zvalI4 = zvalI8;
	break;
      case I8 : 
	zvalI8 = strtoll(z, &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	break;
      case F4 : 
	zvalI4 = strtod(z, &endptr);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	break;
      case I2 : 
      case clob : 
      case F8 : 
      default : 
	go_BYE(-1);
	break;
    }
  }
  else {
    chk_range(z_id, 0, g_n_fld);
    status = get_data(z_rec, &Z, &nZ, 0); cBYE(status);
    if ( nn_z_id >= 0 ) { 
      status = get_data(nn_z_rec, &nn_Z, &nn_nZ, 0); cBYE(status);
    }
  }
  if ( ( y_id >= 0 ) && ( z_id >= 0 ) )  {
    if ( y_rec.fldtype != z_rec.fldtype ) { go_BYE(-1); }
  }
  w_fldtype = xunknown;
  if ( y_id >= 0 ) {
    w_fldtype = y_rec.fldtype;
  }
  if ( z_id >= 0 ) {
    w_fldtype = z_rec.fldtype;
  }
  if ( ( y_id < 0 ) && ( z_id < 0 ) )  { go_BYE(-1); }
  //--------------------------------------------------------
  int fldsz, ddir_id = INT_MAX, nn_ddir_id = INT_MAX;
  status = get_fld_sz(w_fldtype, &fldsz);

  status = mk_temp_file(opfile, (nR * fldsz), &ddir_id); 
  status = q_mmap(ddir_id, opfile, &W, &nW, 1); cBYE(status);

  status = mk_temp_file(nn_opfile, (nR * sizeof(char)), &nn_ddir_id); 
  status = q_mmap(nn_ddir_id, nn_opfile, &nn_W, &nn_nW, 1); cBYE(status);

  switch ( w_fldtype ) { 
    case I4 : 
    core_wisifxthenyelsez_I4(X, nn_X, nR, yvalI4, Y, nn_Y, zvalI4, Z, nn_Z, W, nn_W,
	&is_some_null);
    break;
    case I8 : 
    core_wisifxthenyelsez_I8(X, nn_X, nR, yvalI8, Y, nn_Y, zvalI8, Z, nn_Z, W, nn_W,
	&is_some_null);
    break;
    case I1 : 
    core_wisifxthenyelsez_I1(X, nn_X, nR, yvalI1, Y, nn_Y, zvalI1, Z, nn_Z, W, nn_W,
	&is_some_null);
    break; 
    case F4 : 
    core_wisifxthenyelsez_F4(X, nn_X, nR, yvalF4, Y, nn_Y, zvalF4, Z, nn_Z, W, nn_W,
	&is_some_null);
    break;
    case I2 : 
    case F8 : 
    default : 
    go_BYE(-1);
    break;
  }

  status = add_fld(tbl_id, w, ddir_id, opfile, w_fldtype, -1, &w_id, &w_rec); 
  cBYE(status);
  if ( is_some_null ) { 
    status = add_aux_fld(NULL, tbl_id, NULL, w_id, nn_ddir_id, nn_opfile, "nn", &nn_w_id, &nn_w_rec); 
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(Y, nY);
  rs_munmap(Z, nZ);
  rs_munmap(W, nW);
  rs_munmap(nn_Y, nn_nY);
  rs_munmap(nn_Z, nn_nZ);
  rs_munmap(nn_W, nn_nW);
  return(status);
}
