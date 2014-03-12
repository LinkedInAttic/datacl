#include "qtypes.h"
#include "mmap.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

#include "core_wisifxthenyelsez_I4.h"
#include "core_wisifxthenyelsez_I8.h"
#include "core_wisifxthenyelsez_I1.h"
#include "core_wisifxthenyelsez_I2.h"
#include "core_wisifxthenyelsez_F4.h"

// last review 9/8/2013
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
  int fileno = -1, ddir_id = INT_MAX; 
  int nn_fileno = -1, nn_ddir_id = INT_MAX;
  int tbl_id = INT_MIN; 
  int x_id = INT_MIN, nn_x_id = INT_MIN;
  int y_id = INT_MIN, nn_y_id = INT_MIN;
  int z_id = INT_MIN, nn_z_id = INT_MIN;
  int w_id = INT_MIN, nn_w_id = INT_MIN;
  long long nR;
  bool is_some_null = false;  
  FLD_TYPE w_fldtype;
  int       yvalI4 = INT_MAX; 
  long long yvalI8 = LLONG_MAX; 
  char      yvalI1 = SCHAR_MAX; 
  short     yvalI2 = SHRT_MAX; 
  float     yvalF4 = FLT_MAX;
  int       zvalI4 = INT_MAX; 
  long long zvalI8 = LLONG_MAX; 
  char      zvalI1 = SCHAR_MAX; 
  short     zvalI2 = SHRT_MAX; 
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
      case I1 : status = stoI1(y, &yvalI1); cBYE(status);  break;
      case I2 : status = stoI2(y, &yvalI2); cBYE(status);  break;
      case I4 : status = stoI4(y, &yvalI4); cBYE(status);  break;
      case I8 : status = stoI8(y, &yvalI8); cBYE(status);  break;
      case F4 : status = stoF4(y, &yvalF4); cBYE(status);  break;
      default : go_BYE(-1); break;
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
      case I1 : status = stoI1(z, &zvalI1); cBYE(status);  break;
      case I2 : status = stoI2(z, &zvalI2); cBYE(status);  break;
      case I4 : status = stoI4(z, &zvalI4); cBYE(status);  break;
      case I8 : status = stoI8(z, &zvalI8); cBYE(status);  break;
      case F4 : status = stoF4(z, &zvalF4); cBYE(status);  break;
      default : go_BYE(-1); break;
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
  w_fldtype = undef_fldtype;
  if ( y_id >= 0 ) {
    w_fldtype = y_rec.fldtype;
  }
  if ( z_id >= 0 ) {
    w_fldtype = z_rec.fldtype;
  }
  if ( ( y_id < 0 ) && ( z_id < 0 ) )  { go_BYE(-1); }
  //--------------------------------------------------------
  int fldsz; size_t filesz; 

  status = get_fld_sz(w_fldtype, &fldsz);
  filesz = nR * fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); 
  status = q_mmap(ddir_id, fileno, &W, &nW, 1); cBYE(status);

  status = get_fld_sz(I1, &fldsz); cBYE(status);
  filesz = nR * fldsz;
  status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno);
  status = q_mmap(nn_ddir_id, nn_fileno, &nn_W, &nn_nW, 1); cBYE(status);


  switch ( w_fldtype ) { 
    case I1 : 
    status = core_wisifxthenyelsez_I1(X, nn_X, nR, yvalI1, Y, nn_Y, 
	zvalI1, Z, nn_Z, W, nn_W, &is_some_null);
    break; 
    case I2 : 
    status = core_wisifxthenyelsez_I2(X, nn_X, nR, yvalI2, Y, nn_Y, 
	zvalI2, Z, nn_Z, W, nn_W, &is_some_null);
    break; 
    case I4 : 
    status = core_wisifxthenyelsez_I4(X, nn_X, nR, yvalI4, Y, nn_Y, 
	zvalI4, Z, nn_Z, W, nn_W, &is_some_null);
    cBYE(status);
    break;
    case I8 : 
    status = core_wisifxthenyelsez_I8(X, nn_X, nR, yvalI8, Y, nn_Y, 
	zvalI8, Z, nn_Z, W, nn_W, &is_some_null);
    break;
    case F4 : 
    status = core_wisifxthenyelsez_F4(X, nn_X, nR, yvalF4, Y, nn_Y, 
	zvalF4, Z, nn_Z, W, nn_W, &is_some_null);
    break;
    default : 
    go_BYE(-1);
    break;
  }
  cBYE(status);

  zero_fld_rec(&w_rec); w_rec.fldtype = w_fldtype;
  status = add_fld(tbl_id, w, ddir_id, fileno, &w_id, &w_rec); 
  cBYE(status);
  if ( is_some_null ) { 
    status = add_aux_fld(NULL, tbl_id, NULL, w_id, nn_ddir_id, nn_fileno, 
	nn, &nn_w_id, &nn_w_rec); 
    cBYE(status);
  }
  else {
    status = q_delete(nn_ddir_id, nn_fileno); cBYE(status);
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(Y, nY);
  rs_munmap(Z, nZ);
  rs_munmap(W, nW);
  rs_munmap(nn_Y, nn_nY);
  rs_munmap(nn_Z, nn_nZ);
  rs_munmap(nn_W, nn_nW);
  return status ;
}
