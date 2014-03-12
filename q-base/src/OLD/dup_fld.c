#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_aux_fld.h"
#include "add_fld.h"
#include "mk_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
dup_fld(
       char *tbl,
       char *f1,
       char *f2
       )
// STOP FUNC DECL
{
  int status = 0;
  char *Y = NULL; size_t nY = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *sz_X = NULL; size_t sz_nX = 0;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  FILE *sz_ofp = NULL; char *sz_opfile = NULL;
  int tbl_id, f1_fld_id, f2_fld_id, nn_fld_id, sz_fld_id;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); } 
  //-------------------------------------------------------- 
  status = is_tbl(tbl, &tbl_id); cBYE(status); 
  chk_range(tbl_id, 0, g_n_tbl); 
  status = is_fld(NULL, tbl_id, f1, &f1_fld_id); cBYE(status); 
  chk_range(f1_fld_id, 0, g_n_fld); 
  status = is_fld(NULL, tbl_id, f2, &f2_fld_id); cBYE(status); 
  if ( f2_fld_id >= 0 ) { 
    status = del_fld(NULL, tbl_id, NULL, f2_fld_id); cBYE(status);
  }
  // Open input file 
  status = rs_mmap(g_fld[f1_fld_id].filename, &X, &nX, 0); cBYE(status); 
  // Create a copy of the data 
  status = open_temp_file(&ofp, &opfile, nX); cBYE(status); 
  fclose_if_non_null(ofp); 
  status = mk_file(opfile, nX); cBYE(status);
  status = rs_mmap(opfile, &Y, &nY, 1);
  memcpy(Y, X, nX);
  rs_munmap(Y, nY);
  // Get empty spot for f2
  status = get_empty_fld(&f2_fld_id); cBYE(status);
  // f2 is same as f1 except for data and "is_external"
  g_fld[f2_fld_id] = g_fld[f1_fld_id];
  g_fld[f2_fld_id].is_external = false;
  g_fld[f2_fld_id].nn_fld_id = -1;
  g_fld[f2_fld_id].sz_fld_id = -1;
  status = chk_fld_name(f2, 0);
  zero_string(g_fld[f2_fld_id].name, MAX_LEN_FLD_NAME+1);
  zero_string(g_fld[f2_fld_id].filename, MAX_LEN_FILE_NAME+1);
  strcpy(g_fld[f2_fld_id].name, f2);
  strcpy(g_fld[f2_fld_id].filename, opfile);
  // Add to hash table 
  //--------------------------------------------------------
  // Add auxiliary field nn if if it exists
  nn_fld_id = g_fld[f1_fld_id].nn_fld_id;
  if ( nn_fld_id >= 0 ) { 
    // open input file 
    status = rs_mmap(g_fld[nn_fld_id].filename, &nn_X, &nn_nX, 0); cBYE(status); 
    // create copy of data 
    status = open_temp_file(&nn_ofp, &nn_opfile, nn_nX); cBYE(status);
    fclose_if_non_null(nn_ofp);
    status = mk_file(nn_opfile, nn_nX); cBYE(status);
    status = rs_mmap(nn_opfile, &Y, &nY, 1);
    memcpy(Y, nn_X, nn_nX);
    rs_munmap(Y, nY);
    rs_munmap(nn_X, nn_nX);
    nn_fld_id = INT_MIN;
    status = add_aux_fld(tbl, f2, nn_opfile, "nn", &nn_fld_id);
    cBYE(status);
  }
  //--------------------------------------------------------
  // Add auxiliary field sz if if it exists
  sz_fld_id = g_fld[f1_fld_id].sz_fld_id;
  if ( sz_fld_id >= 0 ) { 
    status = open_temp_file(&sz_ofp, &sz_opfile, sz_nX); cBYE(status);
    fclose_if_non_null(sz_ofp);
    status = mk_file(sz_opfile, sz_nX); cBYE(status);
    status = rs_mmap(sz_opfile, &Y, &nY, 1);
    status = rs_mmap(g_fld[sz_fld_id].filename, &sz_X, &sz_nX, 0);
    memcpy(Y, sz_X, sz_nX);
    rs_munmap(Y, nY);
    rs_munmap(sz_X, sz_nX);
    status = add_aux_fld(tbl, f2, sz_opfile, "sz", &sz_fld_id);
    cBYE(status);
  }
  //--------------------------------------------------------
BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(sz_X, sz_nX);
  rs_munmap(Y, nY);
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  fclose_if_non_null(sz_ofp);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  free_if_non_null(sz_opfile);
  return(status);
}
