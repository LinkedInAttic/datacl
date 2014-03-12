#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "rng_join.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "get_type_op_fld.h"
#include "meta_globals.h"

// last review 9/22/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
rng_join(
	 char *t1,
	 char *ffrom, /* can be null */
	 char *flb,
	 char *fub,
	 char *t2,
	 char *fin,
	 char *fout
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *ffrom_X = NULL; size_t ffrom_nX = 0;
  char *flb_X = NULL; size_t flb_nX = 0;
  char *fub_X = NULL; size_t fub_nX = 0;
  char *fin_X = NULL; size_t fin_nX = 0;
  char *fout_X = NULL;    size_t n_fout_X = 0;
  char *nn_fout_X = NULL; size_t n_nn_fout_X = 0;
  FLD_REC_TYPE ffrom_rec, nn_ffrom_rec;
  FLD_REC_TYPE flb_rec, nn_flb_rec;
  FLD_REC_TYPE fub_rec, nn_fub_rec;
  FLD_REC_TYPE fin_rec, nn_fin_rec;
  FLD_REC_TYPE fout_rec, nn_fout_rec;
  long long nR1, nR2;
  TBL_REC_TYPE t1_rec, t2_rec;
  int t1_id = INT_MIN, t2_id = INT_MIN;
  int flb_id = INT_MIN, nn_flb_id = INT_MIN;
  int ffrom_id = INT_MIN, nn_ffrom_id = INT_MIN;
  int fub_id = INT_MIN, nn_fub_id = INT_MIN;
  int fin_id = INT_MIN, nn_fin_id = INT_MIN;
  int fout_id = INT_MIN, nn_fout_id = INT_MIN;
  int fldsz = INT_MAX; size_t filesz = 0;
  int ddir_id    = -1, fileno    = -1; 
  int nn_ddir_id = -1, nn_fileno = -1; 
  bool is_any_null = false;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( flb == NULL ) || ( *flb == '\0' ) ) { go_BYE(-1); }
  if ( ( fub == NULL ) || ( *fub == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( fin == NULL ) || ( *fin == '\0' ) ) { go_BYE(-1); }
  if ( ( fout == NULL ) || ( *fout == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(flb, fub) == 0 ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }

  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = t1_rec.nR;
  if ( nR1 >= INT_MAX ) { go_BYE(-1); } // not ready for this case
  //--------------------------------------------------------
  status =is_fld(NULL, t1_id, flb, &flb_id, &flb_rec, &nn_flb_id, &nn_flb_rec); 
  cBYE(status);
  chk_range(flb_id, 0, g_n_fld);
  status = get_data(flb_rec, &flb_X, &flb_nX, false); cBYE(status);
  if ( nn_flb_id >= 0 ) { go_BYE(-1); } 
  if ( flb_rec.srttype != ascending ) { go_BYE(-1); }
  if ( flb_rec.fldtype != I4 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status =is_fld(NULL, t1_id, fub, &fub_id, &fub_rec, &nn_fub_id, &nn_fub_rec); 
  cBYE(status);
  chk_range(fub_id, 0, g_n_fld);
  status = get_data(fub_rec, &fub_X, &fub_nX, false); cBYE(status);
  if ( nn_fub_id >= 0 ) { go_BYE(-1); }
  if ( fub_rec.srttype != ascending ) { go_BYE(-1); }
  if ( fub_rec.fldtype != I4 ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( ( ffrom != NULL ) && ( *ffrom != '\0' ) ) {
    status = is_fld(NULL, t1_id, ffrom, &ffrom_id, &ffrom_rec, &nn_ffrom_id, &nn_ffrom_rec); 
    cBYE(status);
    chk_range(ffrom_id, 0, g_n_fld);
    status = get_data(ffrom_rec, &ffrom_X, &ffrom_nX, false); cBYE(status);
    if ( nn_ffrom_id >= 0 ) { go_BYE(-1); } 
    if ( ffrom_rec.fldtype != I4 ) { go_BYE(-1); }
  }
  //--------------------------------------------------------
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = t2_rec.nR;
  //--------------------------------------------------------
  status =is_fld(NULL, t2_id, fin, &fin_id, &fin_rec, &nn_fin_id, &nn_fin_rec); 
  cBYE(status);
  chk_range(fin_id, 0, g_n_fld);
  status = get_data(fin_rec, &fin_X, &fin_nX, false); cBYE(status);
  if ( nn_fin_id >= 0 ) { go_BYE(-1); } 
  if ( fin_rec.srttype != ascending ) { go_BYE(-1); }
  if ( fin_rec.fldtype != I4 ) { go_BYE(-1); }
  //--------------------------------------------------------
  FLD_TYPE fout_type = I4;
  // Make space for output 
  status = get_fld_sz(fout_type, & fldsz); cBYE(status);
  filesz= fldsz * nR2; 
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &fout_X, &n_fout_X, true); cBYE(status);

  status = get_fld_sz(I1, & fldsz); cBYE(status);
  filesz= fldsz * nR2; 
  status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno); cBYE(status);
  status = q_mmap(nn_ddir_id, nn_fileno, &nn_fout_X, &n_nn_fout_X, true); 
  cBYE(status);
  
  int *foutI4 = (int *)fout_X;
  int *flbI4 = (int *)flb_X;
  int *fubI4 = (int *)fub_X;
  int *finI4 = (int *)fin_X;
  int *ffromI4 = (int *)ffrom_X;
  long long start_t1_idx = 0;
  // Candidate for cilkfor but each thread needs to keep its own
  // start_t1_idx: TODO P2
  for ( long long i = 0; i < nR2; i++ ) {
    int finval = finI4[i];
    char nn = TRUE;
    int t1idx = -1;
    for ( long long j = start_t1_idx; j < nR1; j++ ) {
      if ( finval < flbI4[j] ) {
	is_any_null = true;
	t1idx = 0;
	nn = FALSE;
	break;
      }
      else if ( ( finval >= flbI4[j] ) && ( finval <= fubI4[j] ) ) {
	t1idx = j; // index into T1
	nn = TRUE;
	break;
      }
      else {
        start_t1_idx++;
      }
    }
    if ( t1idx < 0 ) {
      nn = FALSE;
    }
    if ( ffromI4 != NULL ) { 
      foutI4[i] = ffromI4[t1idx];
    }
    else {
      foutI4[i] = t1idx;
    }
    nn_fout_X[i] = nn;
  }

  //--------------------------------------------------------
  zero_fld_rec(&fout_rec); fout_rec.fldtype = fout_type;
  status = add_fld(t2_id, fout, ddir_id, fileno, &fout_id, &fout_rec); 
  cBYE(status);
  if ( is_any_null ) { 
    zero_fld_rec(&nn_fout_rec); nn_fout_rec.fldtype = I1; 
    status = add_aux_fld(NULL, t2_id, NULL, fout_id, nn_ddir_id, 
	nn_fileno, nn, &nn_fout_id, &nn_fout_rec);
    cBYE(status);
  }
  else {
    status = q_delete(nn_ddir_id, nn_fileno);  cBYE(status);
  }
 BYE:
  rs_munmap(fout_X, n_fout_X);
  rs_munmap(nn_fout_X, n_nn_fout_X);
  rs_munmap(flb_X, flb_nX);
  rs_munmap(fub_X, fub_nX);
  rs_munmap(fin_X, fin_nX);
  return(status);
}
