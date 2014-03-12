#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "meta_globals.h"

#include "core_wisifxthenyelsez_I.h"
#include "core_wisifxthenyelsez_L.h"
#include "core_wisifxthenyelsez_B.h"
#include "core_wisifxthenyelsez_F.h"

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
  char *nn_W = NULL; size_t nn_nW = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *nn_Y = NULL; size_t nn_nY = 0;
  char *nn_Z = NULL; size_t nn_nZ = 0;
  FLD_TYPE *x_fld_meta = NULL, *nn_x_fld_meta = NULL;
  FLD_TYPE *y_fld_meta = NULL, *nn_y_fld_meta = NULL;
  FLD_TYPE *z_fld_meta = NULL, *nn_z_fld_meta = NULL;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  char str_meta_data[256];
  int tbl_id = INT_MIN; 
  int x_fld_id = INT_MIN, nn_x_fld_id = INT_MIN;
  int y_fld_id = INT_MIN, nn_y_fld_id = INT_MIN;
  int z_fld_id = INT_MIN, nn_z_fld_id = INT_MIN;
  int w_fld_id = INT_MIN, nn_w_fld_id = INT_MIN;
  long long nR;
  bool is_some_null = false;
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
  zero_string(str_meta_data, 256);

  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  //--------------------------------------------------------
  // Get info about field x
  status = is_fld(NULL, tbl_id, x, &x_fld_id); cBYE(status);
  chk_range(x_fld_id, 0, g_n_fld);
  x_fld_meta = &(g_fld[x_fld_id]);
  if ( strcmp(x_fld_meta->fldtype, "bool") != 0 ) { go_BYE(-1); }
  status = rs_mmap(x_fld_meta->filename, &X, &nX, 0); cBYE(status);
  nn_x_fld_id = x_fld_meta->nn_fld_id;
  if ( nn_x_fld_id >= 0 ) { 
    nn_x_fld_meta = &(g_fld[nn_x_fld_id]);
    status = rs_mmap(nn_x_fld_meta->filename, &nn_X, &nn_nX, 0); cBYE(status);
  }
  //-=======================================================
  // Get info about field y
  status = is_fld(NULL, tbl_id, y, &y_fld_id); cBYE(status);
  chk_range(y_fld_id, 0, g_n_fld);
  y_fld_meta = &(g_fld[y_fld_id]);
  status = rs_mmap(y_fld_meta->filename, &Y, &nY, 0); cBYE(status);
  nn_y_fld_id = y_fld_meta->nn_fld_id;
  if ( nn_y_fld_id >= 0 ) { 
    nn_y_fld_meta = &(g_fld[nn_y_fld_id]);
    status = rs_mmap(nn_y_fld_meta->filename, &nn_Y, &nn_nY, 0); cBYE(status);
  }
  //-=======================================================
  // Get info about field z
  status = is_fld(NULL, tbl_id, z, &z_fld_id); cBYE(status);
  chk_range(z_fld_id, 0, g_n_fld);
  z_fld_meta = &(g_fld[z_fld_id]);
  status = rs_mmap(z_fld_meta->filename, &Z, &nZ, 0); cBYE(status);
  nn_z_fld_id = z_fld_meta->nn_fld_id;
  if ( nn_z_fld_id >= 0 ) { 
    nn_z_fld_meta = &(g_fld[nn_z_fld_id]);
    status = rs_mmap(nn_z_fld_meta->filename, &nn_Z, &nn_nZ, 0); cBYE(status);
  }
  //-=======================================================
  //--------------------------------------------------------
  if ( strcmp(y_fld_meta->fldtype, z_fld_meta->fldtype) != 0 ) { go_BYE(-1); } 
  if ( y_fld_meta->n_sizeof != z_fld_meta->n_sizeof ) { go_BYE(-1); }

  //--------------------------------------------------------
  long long filesz = y_fld_meta->n_sizeof * nR;
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &W, &nW, 1); cBYE(status);

  long long nnfilesz = sizeof(char) * nR;
  status = open_temp_file(&nn_ofp, &nn_opfile, nnfilesz); cBYE(status);
  fclose_if_non_null(nn_ofp);
  status = mk_file(nn_opfile, nnfilesz); cBYE(status);
  status = rs_mmap(nn_opfile, &nn_W, &nn_nW, 1); cBYE(status);

  if ( strcmp(y_fld_meta->fldtype, "int") == 0 ) {
    core_wisifxthenyelsez_I(X, nn_X, nR, Y, nn_Y, Z, nn_Z, W, nn_W,
	&is_some_null);
  }
  else if ( strcmp(y_fld_meta->fldtype, "long long") == 0 ) {
    core_wisifxthenyelsez_L(X, nn_X, nR, Y, nn_Y, Z, nn_Z, W, nn_W,
	&is_some_null);
  }
  else if ( strcmp(y_fld_meta->fldtype, "bool") == 0 ) {
    core_wisifxthenyelsez_B(X, nn_X, nR, Y, nn_Y, Z, nn_Z, W, nn_W,
	&is_some_null);
  }
  else if ( strcmp(y_fld_meta->fldtype, "float") == 0 ) {
    core_wisifxthenyelsez_F(X, nn_X, nR, Y, nn_Y, Z, nn_Z, W, nn_W,
	&is_some_null);
  }

  sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d",
	  opfile, y_fld_meta->fldtype, z_fld_meta->n_sizeof);
  status = add_fld(tbl, w, str_meta_data, &w_fld_id); cBYE(status);
  if ( is_some_null ) { 
    status = add_aux_fld(tbl, w, nn_opfile, "nn", &nn_w_fld_id); cBYE(status);
  }
  else {
    unlink(nn_opfile);
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(Y, nY);
  rs_munmap(Z, nZ);
  rs_munmap(nn_Y, nn_nY);
  rs_munmap(nn_Z, nn_nZ);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  return(status);
}
