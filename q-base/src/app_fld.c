#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "mk_temp_file.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "add_fld.h"
#include "app_fld.h"
#include "get_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "drop_aux_fld.h"
#include "assign_I1.h"
#include "meta_globals.h"

extern char g_cwd[MAX_LEN_DIR_NAME+1];

//---------------------------------------------------------------
// Appends data of field f2 in table T2 to field f1 in Table t1
// START FUNC DECL
int 
app_fld(
	int t1_id,
	int f1_id,
	int t2_id,
	int f2_id
	)
// STOP FUNC DECL
{
  int status = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *Y = NULL; size_t nY = 0;
  char *X2 = NULL; size_t nX2 = 0;
  FLD_REC_TYPE f1_rec, nn_f1_rec, f2_rec, nn_f2_rec;
  char nn_val;
  long long nR1, nR2; 
  int nn_f1_id;
  int nn_f2_id;
  FILE *ofp = NULL;
  int f1_ddir_id = -1, f1_fileno = -1, nn_f1_ddir_id = -1, nn_f1_fileno = -1;
  int f2_ddir_id = -1, f2_fileno = -1;
  char f1_filename[MAX_LEN_FILE_NAME+1];
  char f2_filename[MAX_LEN_FILE_NAME+1];
  char nn_f1_filename[MAX_LEN_FILE_NAME+1];
  char nn_f2_filename[MAX_LEN_FILE_NAME+1];
  size_t filesz = 0; int fldsz = 0, nn_fldsz = 0;
  //----------------------------------------------------------------
  if ( t1_id < 0 ) { go_BYE(-1); }
  if ( t2_id < 0 ) { go_BYE(-1); }
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( f2_id < 0 ) { go_BYE(-1); }
  zero_string(f1_filename, MAX_LEN_FILE_NAME+1);
  zero_string(f2_filename, MAX_LEN_FILE_NAME+1);
  zero_string(nn_f1_filename, MAX_LEN_FILE_NAME+1);
  zero_string(nn_f2_filename, MAX_LEN_FILE_NAME+1);
  //--------------------------------------------------------
  nR1        = g_tbls[t1_id].nR; nR2        = g_tbls[t2_id].nR;
  f1_rec     = g_flds[f1_id];    f2_rec     = g_flds[f2_id];
  f1_ddir_id = f1_rec.ddir_id;   f2_ddir_id = f2_rec.ddir_id;
  f1_fileno  = f1_rec.fileno;    f2_fileno  = f2_rec.fileno;
  nn_f1_id   = f1_rec.nn_fld_id; nn_f2_id   = f2_rec.nn_fld_id; 

  status = switch_dirs(f1_ddir_id); cBYE(status);
  mk_file_name(f1_filename, f1_rec.fileno);
  ofp = fopen(f1_filename, "ab");
  return_if_fopen_failed(ofp, f1_filename, "ab");

  status = q_mmap(f2_ddir_id, f2_fileno, &X2, &nX2, 0); cBYE(status);
  fwrite(X2, nX2, 1, ofp);
  rs_munmap(X2, nX2);
  fclose_if_non_null(ofp);
  status = chdir(g_cwd); cBYE(status);
  //--------------------------------------------------------
  // See if there is an auxilary field (nn) to copy
  if ( ( nn_f1_id < 0 ) && ( nn_f2_id < 0 ) ) { 
    /* Nothing to do */
  }
  else if ( ( nn_f1_id < 0 ) && ( nn_f2_id >= 0 ) ) { 
    /* f1 does not have a nn field but f2 does */
    nn_f2_id = g_flds[f2_id].nn_fld_id;
    nn_f2_rec = g_flds[nn_f2_id];
    /* Create a file for newly created nn field for f1 */
    status = get_fld_sz(I1, &fldsz); cBYE(status);
    filesz = fldsz * (nR1+nR2);
    status = mk_temp_file(filesz, &nn_f1_ddir_id, &nn_f1_fileno); cBYE(status);
    status = q_mmap(nn_f1_ddir_id, nn_f1_fileno, &nn_X, &nn_nX, true); cBYE(status);
    /* Set first nR1 values to true */
#ifdef IPP
    ippsSet_8u(1, nn_X, nR1);
#else
    assign_const_I1(nn_X, nR1, 1);
#endif
    /* Set next nR2 values based on f2 */
    status = q_mmap(nn_f2_rec.ddir_id, nn_f2_rec.fileno, &Y, &nY, 0); 
    cBYE(status);
#ifdef IPP
    ippsCopy_8u(nn_X+nR1, Y, nY);
#else
    memcpy(nn_X+nR1, Y, nY);
#endif
    rs_munmap(Y, nY);
    rs_munmap(nn_X, nn_nX);
    /* Add to meta data */
    zero_fld_rec(&nn_f1_rec); nn_f1_rec.fldtype = I1; 
    status = add_aux_fld(NULL, t1_id, NULL, f1_id, nn_f1_ddir_id, 
	nn_f1_fileno, nn, &nn_f1_id, &nn_f1_rec);
    cBYE(status);
    status = get_fld_sz(I1, &nn_fldsz); cBYE(status);
    g_flds[nn_f1_id].filesz = (nR1+nR2)*nn_fldsz;
  }
  else if ( ( nn_f1_id >= 0 ) && ( nn_f2_id < 0 ) ) {
    nn_f1_id  = g_flds[f1_id].nn_fld_id;
    nn_f1_rec = g_flds[nn_f1_id];
    /* f1 has a nn field but f2 does not */
    status = switch_dirs(nn_f1_ddir_id); cBYE(status);
    mk_file_name(nn_f1_filename, nn_f1_rec.fileno);
    ofp = fopen(nn_f1_filename, "ab"); 
    /* Set next nR2 values to true */
    nn_val = TRUE;
    for ( long long i = 0; i < nR2; i++ ) {  // TODO: SPEED THIS UP
      fwrite(&nn_val, sizeof(char), 1, ofp);
    }
    fclose_if_non_null(ofp);
    status = chdir(g_cwd); cBYE(status);
    /* Add to meta data */
    status = drop_aux_fld(NULL, NULL, "nn", f1_id); cBYE(status); 

    zero_fld_rec(&nn_f1_rec); nn_f1_rec.fldtype = I1; 
    status = add_aux_fld(NULL, t1_id, NULL, f1_id, f1_rec.ddir_id, 
	nn_f1_fileno, nn, &nn_f1_id, &nn_f1_rec);
    cBYE(status);
    // done in add_aux_fld g_fld[f1_id].nn_fld_id = nn_f1_id;
    status = get_fld_sz(I1, &nn_fldsz); cBYE(status);
    g_flds[nn_f1_id].filesz = (nR1+nR2)*nn_fldsz;
  }
  else { /* Both f1 and f2 have nn fields */
    nn_f1_id  = g_flds[f1_id].nn_fld_id;
    nn_f1_rec = g_flds[nn_f1_id];
    nn_f2_id = g_flds[f2_id].nn_fld_id;
    nn_f2_rec = g_flds[nn_f2_id];

    status = switch_dirs(nn_f1_ddir_id); cBYE(status);
    mk_file_name(nn_f1_filename, nn_f1_rec.fileno);
    ofp = fopen(nn_f1_filename, "ab");
    return_if_fopen_failed(ofp, nn_f1_filename, "ab");
    status = q_mmap(nn_f2_rec.ddir_id, nn_f2_rec.fileno, &Y, &nY, 0); 
    cBYE(status);
    fwrite(Y, nY, 1, ofp);
    fclose_if_non_null(ofp);
    status = chdir(g_cwd); cBYE(status);
    g_flds[nn_f1_id].filesz = (nR1+nR2)*nn_fldsz;
  }
  // meta data updates
  g_flds[f1_id].srttype = unknown;
  g_flds[f1_id].sumval.sumI8 = g_flds[f1_id].minval.minI8 = g_flds[f1_id].maxval.maxI8 = 0;
  g_flds[f1_id].sumval.sumF8 = g_flds[f1_id].minval.minF8 = g_flds[f1_id].maxval.maxF8 = 0;
  g_flds[f1_id].is_sum_nn= g_flds[f1_id].is_min_nn= g_flds[f1_id].is_max_nn=0;
  status = get_fld_sz(g_flds[f1_id].fldtype, &fldsz); cBYE(status);
  filesz = fldsz * (nR1+nR2);
  g_flds[f1_id].filesz = filesz;

  //--------------------------------------------------------
 BYE:
  fclose_if_non_null(ofp);
  return status ;
}
