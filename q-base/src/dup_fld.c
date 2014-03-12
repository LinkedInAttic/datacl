#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "add_fld.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "meta_globals.h"

// TODO: What about duplicating strings? Does it work?
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
  int tbl_id, f1_id, f2_id, nn_f1_id, nn_f2_id;
  int ddir_id    = -1, fileno    = -1;
  int nn_ddir_id = -1, nn_fileno = -1; 
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec, f2_rec, nn_f2_rec;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); } 
  //-------------------------------------------------------- 
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status); 
  chk_range(tbl_id, 0, g_n_tbl); 
  status = is_fld(NULL, tbl_id, f2, &f2_id, &f2_rec, 
      &nn_f2_id, &nn_f2_rec); cBYE(status); 
  if ( f2_id >= 0 ) { 
    status = del_fld(NULL, tbl_id, NULL, f2_id); cBYE(status);
  }
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  cBYE(status); 
  chk_range(f1_id, 0, g_n_fld); 
  // Open input file 
  status = get_data(g_flds[f1_id], &X, &nX, false); cBYE(status); 
  // Create a copy of the data 
  status = mk_temp_file(nX, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &Y, &nY, 1); cBYE(status);

  memcpy(Y, X, nX);
  // Get empty spot for f2
  status = get_empty_fld(f2, g_flds, g_n_fld, &f2_id); cBYE(status);
  // f2 is same as f1 except for data and "is_external"
  g_flds[f2_id] = g_flds[f1_id];
  g_flds[f2_id].is_external = false;
  g_flds[f2_id].nn_fld_id = -1;
  status = chk_fld_name(f2);
  zero_string(g_flds[f2_id].name, MAX_LEN_FLD_NAME+1);
  g_flds[f2_id].fileno = -1;
  strcpy(g_flds[f2_id].name, f2);
  g_flds[f2_id].fileno  = fileno;
  g_flds[f2_id].ddir_id = ddir_id;
  // Add to hash table 
  //--------------------------------------------------------
  // Add auxiliary field nn if if it exists
  nn_f1_id = g_flds[f1_id].nn_fld_id;
  if ( nn_f1_id >= 0 ) { 
    // open input file 
    status = get_data(g_flds[nn_f1_id], &nn_X, &nn_nX, false); cBYE(status); 
    // create copy of data 
    status = mk_temp_file(nn_nX, &nn_ddir_id, &nn_fileno); cBYE(status);
    status = q_mmap(nn_ddir_id, nn_fileno, &Y, &nY, true);
    memcpy(Y, nn_X, nn_nX);
    rs_munmap(Y, nY);
    rs_munmap(nn_X, nn_nX);
    nn_f1_id = INT_MIN;
    zero_fld_rec(&nn_f2_rec); nn_f2_rec.fldtype = nn_f1_rec.fldtype;
    status = add_aux_fld(NULL, tbl_id, NULL, f2_id, nn_ddir_id, 
	nn_fileno, nn, &nn_f2_id, &nn_f2_rec);
    cBYE(status);
  }
  //--------------------------------------------------------
BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(Y, nY);
  return status ;
}
