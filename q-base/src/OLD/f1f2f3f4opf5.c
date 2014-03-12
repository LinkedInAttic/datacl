#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "aux_fld_meta.h"
#include "open_temp_file.h"
#include "latlong_distance.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
f1f2f3f4opf5(
	     char *tbl,
	     char *f1,
	     char *f2,
	     char *f3,
	     char *f4,
	     char *str_op_spec,
	     char *f5
	     )
// STOP FUNC DECL
{
  int status = 0;

  char *f1_X = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
  char *f3_X = NULL; size_t f3_nX = 0;
  char *nn_f3_X = NULL; size_t nn_f3_nX = 0;
  char *f4_X = NULL; size_t f4_nX = 0;
  char *nn_f4_X = NULL; size_t nn_f4_nX = 0;

  char *out_X = NULL; size_t out_nX = 0;
  char *nn_out_X = NULL; size_t nn_out_nX = 0;

  char *opfile = NULL, *nn_opfile = NULL; FILE *ofp = NULL;

  FLD_TYPE *f1_meta = NULL, *nn_f1_meta = NULL;
  FLD_TYPE *f2_meta = NULL, *nn_f2_meta = NULL;
  FLD_TYPE *f3_meta = NULL, *nn_f3_meta = NULL;
  FLD_TYPE *f4_meta = NULL, *nn_f4_meta = NULL;
  long long nR; 
  int tbl_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN, f3_id = INT_MIN, 
      f4_id = INT_MIN, f5_id = INT_MIN;
  int nn_f1_id = INT_MIN, nn_f2_id = INT_MIN, nn_f3_id = INT_MIN, 
      nn_f4_id = INT_MIN, nn_f5_id = INT_MIN;
  char str_meta_data[1024];
  bool is_any_null = false;
  float *f1ptr = NULL, *f2ptr = NULL, *f3ptr = NULL, *f4ptr = NULL; 
  float *foutptr = NULL; char *nn_out = NULL;
  char *op = NULL;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }
  if ( ( f4 == NULL ) || ( *f4 == '\0' ) ) { go_BYE(-1); }
  if ( ( f5 == NULL ) || ( *f5 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }


  zero_string(str_meta_data, 1024);
  //--------------------------------------------------------
  status = extract_name_value(str_op_spec, "op=", ":", &op);
  cBYE(status);
  if ( op == NULL ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  //--------------------------------------------------------
  /* Get information for field f1 */
  status = is_fld(NULL, tbl_id, f1, &f1_id);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); cBYE(status);
  nn_f1_id = f1_meta->nn_fld_id;
  if ( nn_f1_id >= 0 ) {
    nn_f1_meta = &(g_fld[nn_f1_id]);
    status = rs_mmap(nn_f1_meta->filename, &nn_f1_X, &nn_f1_nX, 0); 
    cBYE(status);
  }
  //--------------------------------------------------------
  /* Get information for field f2 */
  status = is_fld(NULL, tbl_id, f2, &f2_id);
  f2_meta = &(g_fld[f2_id]);
  status = rs_mmap(f2_meta->filename, &f2_X, &f2_nX, 0); cBYE(status);
  nn_f2_id = f2_meta->nn_fld_id;
  if ( nn_f2_id >= 0 ) {
    nn_f2_meta = &(g_fld[nn_f2_id]);
    status = rs_mmap(nn_f2_meta->filename, &nn_f2_X, &nn_f2_nX, 0); 
    cBYE(status);
  }
  //--------------------------------------------------------
  /* Get information for field f3 */
  status = is_fld(NULL, tbl_id, f3, &f3_id);
  f3_meta = &(g_fld[f3_id]);
  status = rs_mmap(f3_meta->filename, &f3_X, &f3_nX, 0); cBYE(status);
  nn_f3_id = f3_meta->nn_fld_id;
  if ( nn_f3_id >= 0 ) {
    nn_f3_meta = &(g_fld[nn_f3_id]);
    status = rs_mmap(nn_f3_meta->filename, &nn_f3_X, &nn_f3_nX, 0); 
    cBYE(status);
  }
  //--------------------------------------------------------
  /* Get information for field f4 */
  status = is_fld(NULL, tbl_id, f4, &f4_id);
  f4_meta = &(g_fld[f4_id]);
  status = rs_mmap(f4_meta->filename, &f4_X, &f4_nX, 0); cBYE(status);
  nn_f4_id = f4_meta->nn_fld_id;
  if ( nn_f4_id >= 0 ) {
    nn_f4_meta = &(g_fld[nn_f4_id]);
    status = rs_mmap(nn_f4_meta->filename, &nn_f4_X, &nn_f4_nX, 0); 
    cBYE(status);
  }
  //--------------------------------------------------------
  if ( strcmp(f1_meta->fldtype, "float") != 0 ) { go_BYE(-1); }
  if ( strcmp(f2_meta->fldtype, "float") != 0 ) { go_BYE(-1); }
  if ( strcmp(f3_meta->fldtype, "float") != 0 ) { go_BYE(-1); }
  if ( strcmp(f4_meta->fldtype, "float") != 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  f1ptr = (float *)f1_X;
  f2ptr = (float *)f2_X;
  f3ptr = (float *)f3_X;
  f4ptr = (float *)f4_X;
  //--------------------------------------------------------
  // Create output file 
  long long filesz = sizeof(float) * nR;
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status); fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &out_X, &out_nX, 1); cBYE(status);
  foutptr = (float*)out_X;
  //--------------------------------------------------------
  // Create nn field for output file 
  long long nnfilesz = sizeof(char) * nR;
  status = open_temp_file(&ofp, &nn_opfile, nnfilesz); cBYE(status); 
  fclose_if_non_null(ofp);
  status = mk_file(nn_opfile, filesz); cBYE(status);
  status = rs_mmap(nn_opfile, &nn_out_X, &nn_out_nX, 1); cBYE(status);
  nn_out = (char*)nn_out_X;
  //--------------------------------------------------------
  if ( strcmp(op, "lat_long_dist") == 0 ) {
    for ( long long i = 0; i < nR; i++ ) { 
      if ( ( nn_f1_X != NULL ) && ( nn_f1_X[i] == 0 ) ) {
	is_any_null = true; foutptr[i] = 0; nn_out[i] = 0; continue;
      }
      if ( ( nn_f2_X != NULL ) && ( nn_f2_X[i] == 0 ) ) {
	is_any_null = true; foutptr[i] = 0; nn_out[i] = 0; continue;
      }
      if ( ( nn_f3_X != NULL ) && ( nn_f3_X[i] == 0 ) ) {
	is_any_null = true; foutptr[i] = 0; nn_out[i] = 0; continue;
      }
      if ( ( nn_f4_X != NULL ) && ( nn_f4_X[i] == 0 ) ) {
	is_any_null = true; foutptr[i] = 0; nn_out[i] = 0; continue;
      }
      if ( ( f1ptr[i] < -90  ) || ( f1ptr[i] > 90  ) ) { go_BYE(-1); }
      if ( ( f2ptr[i] < -180 ) || ( f2ptr[i] > 180 ) ) { go_BYE(-1); }
      if ( ( f3ptr[i] < -90  ) || ( f3ptr[i] > 90  ) ) { go_BYE(-1); }
      if ( ( f4ptr[i] < -180 ) || ( f4ptr[i] > 180 ) ) { go_BYE(-1); }
      status = latlong_distance(f1ptr[i], f2ptr[i], f3ptr[i], f4ptr[i],
				&(foutptr[i]));
      cBYE(status);
      nn_out[i] = TRUE;
    }
  }
  sprintf(str_meta_data, "filename=%s:fldtype=float:n_sizeof=4", opfile);
  status = add_fld(tbl, f5, str_meta_data, &f5_id);
  cBYE(status);
  if ( is_any_null ) {
    status = add_aux_fld(tbl, f5, nn_opfile, "nn", &nn_f5_id);
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
    free_if_non_null(nn_opfile);
  }
BYE:
  rs_munmap(f1_X, f1_nX); rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(f2_X, f2_nX); rs_munmap(nn_f2_X, nn_f2_nX);
  rs_munmap(f3_X, f3_nX); rs_munmap(nn_f3_X, nn_f3_nX);
  rs_munmap(f4_X, f4_nX); rs_munmap(nn_f4_X, nn_f4_nX);
  rs_munmap(out_X, out_nX); rs_munmap(nn_out_X, nn_out_nX);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  free_if_non_null(op);
  return(status);
}
