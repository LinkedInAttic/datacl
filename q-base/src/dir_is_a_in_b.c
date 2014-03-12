#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "aux_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "./AUTOGEN/GENFILES/bin_search_I4.h"
#include "./AUTOGEN/GENFILES/bin_search_I8.h"
#include "get_nR.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "f_to_s.h"

#define BUFLEN 32

#define chk_is_srt(tbl, fld) { \
  status = f_to_s(tbl, fld, "is_sorted", strbuf, BUFLEN); cBYE(status); \
  if ( strcmp(strbuf, "ascending") != 0 ) { go_BYE(-1); } \
}
/* f1 is sorted ascending, f2 is sorted ascending Tdir provides
 * directory */

//---------------------------------------------------------------
// START FUNC DECL
int 
dir_is_a_in_b(
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *cfld,
	  char *t3, /* serves as directory */
	  char *lb_val,  /* lower bound of values for this range */
	  char *ub_val,  /* upper bound of values for this range */
	  char *lb_idx,  /* lower bound of indexes for this range */
	  char *ub_idx   /* upper bound of indexes for this range */
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *lb_val_X = NULL; size_t lb_val_nX = 0;
  char *ub_val_X = NULL; size_t ub_val_nX = 0;
  char *lb_idx_X = NULL; size_t lb_idx_nX = 0;
  char *ub_idx_X = NULL; size_t ub_idx_nX = 0;
  char *cfX = NULL; size_t cf_nX = 0;
  FLD_REC_TYPE f1_rec, nn_f1_rec; int f1_id, nn_f1_id;
  FLD_REC_TYPE f2_rec, nn_f2_rec; int f2_id, nn_f2_id;
  FLD_REC_TYPE lb_val_rec, nn_lb_val_rec; int lb_val_id, nn_lb_val_id;
  FLD_REC_TYPE ub_val_rec, nn_ub_val_rec; int ub_val_id, nn_ub_val_id;
  FLD_REC_TYPE lb_idx_rec, nn_lb_idx_rec; int lb_idx_id, nn_lb_idx_id;
  FLD_REC_TYPE ub_idx_rec, nn_ub_idx_rec; int ub_idx_id, nn_ub_idx_id;

  TBL_REC_TYPE t1_rec, t2_rec, t3_rec;
  long long nR1, nR2, nR3;
  int t1_id, t2_id, t3_id;
  FLD_REC_TYPE cfld_rec; int cfld_id;
  int fldsz, ddir_id =1, fileno = -1; size_t filesz;
  char strbuf[BUFLEN];
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( cfld == NULL ) || ( *cfld == '\0' ) ) {  go_BYE(-1); }
  if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
  if ( ( t3 == NULL ) || ( *t3 == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_val == NULL ) || ( *lb_val == '\0' ) ) { go_BYE(-1); }
  if ( ( ub_val == NULL ) || ( *ub_val == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_idx == NULL ) || ( *lb_idx == '\0' ) ) { go_BYE(-1); }
  if ( ( ub_idx == NULL ) || ( *ub_idx == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  if ( strcmp(t2, t3) == 0 ) { go_BYE(-1); }
  if ( strcmp(t3, t1) == 0 ) { go_BYE(-1); }
  if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb_val, ub_val) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb_idx, ub_idx) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR; if ( nR1 <= 0 ) { go_BYE(-1); }
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  if ( f1_rec.fldtype != I8 ) { go_BYE(-1); }
  chk_is_srt(t1, f1);
  //--------------------------------------------------------
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbls[t2_id].nR; if ( nR2 <= 0 ) { go_BYE(-1); }
  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec);
  chk_range(f2_id, 0, g_n_fld);
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  if ( nn_f2_id >= 0 ) { go_BYE(-1); }
  if ( f2_rec.fldtype != I8 ) { go_BYE(-1); }
  //--------------------------------------------------------
  if (f1_rec.fldtype != f2_rec.fldtype ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t3, &t3_id, &t3_rec); cBYE(status);
  chk_range(t3_id, 0, g_n_tbl);
  nR3 = g_tbls[t3_id].nR; if ( nR3 <= 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_fld(NULL, t3_id, lb_val, &lb_val_id, &lb_val_rec, &nn_lb_val_id, &nn_lb_val_rec);
  chk_range(lb_val_id, 0, g_n_fld);
  status = get_data(lb_val_rec, &lb_val_X, &lb_val_nX, false); cBYE(status);
  if ( nn_lb_val_id >= 0 ) { go_BYE(-1); }
  if ( lb_val_rec.fldtype != I8 ) { go_BYE(-1); }
  chk_is_srt(t3, lb_val);
  //--------------------------------------------------------
  status = is_fld(NULL, t3_id, ub_val, &ub_val_id, &ub_val_rec, &nn_ub_val_id, &nn_ub_val_rec);
  chk_range(ub_val_id, 0, g_n_fld);
  status = get_data(ub_val_rec, &ub_val_X, &ub_val_nX, false); cBYE(status);
  if ( nn_ub_val_id >= 0 ) { go_BYE(-1); }
  if ( ub_val_rec.fldtype != I8 ) { go_BYE(-1); }
  chk_is_srt(t3, ub_val);
  //--------------------------------------------------------
  status = is_fld(NULL, t3_id, lb_idx, &lb_idx_id, &lb_idx_rec, &nn_lb_idx_id, &nn_lb_idx_rec);
  chk_range(lb_idx_id, 0, g_n_fld);
  status = get_data(lb_idx_rec, &lb_idx_X, &lb_idx_nX, false); cBYE(status);
  if ( nn_lb_idx_id >= 0 ) { go_BYE(-1); }
  if ( lb_idx_rec.fldtype != I8 ) { go_BYE(-1); }
  chk_is_srt(t3, lb_idx);
  //--------------------------------------------------------
  status = is_fld(NULL, t3_id, ub_idx, &ub_idx_id, &ub_idx_rec, &nn_ub_idx_id, &nn_ub_idx_rec);
  chk_range(ub_idx_id, 0, g_n_fld);
  status = get_data(ub_idx_rec, &ub_idx_X, &ub_idx_nX, false); cBYE(status);
  if ( nn_ub_idx_id >= 0 ) { go_BYE(-1); }
  if ( ub_idx_rec.fldtype != I8 ) { go_BYE(-1); }
  chk_is_srt(t3, ub_idx);
  //--------------------------------------------------------
  /* Make space for output condition field */
  status = get_fld_sz(I1, &fldsz); cBYE(status);
  filesz = fldsz * nR1;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &cfX, &cf_nX, 1); cBYE(status);
  //--------------------------------------------------------
  long long *lbidx = (long long *)lb_idx_X;
  long long *ubidx = (long long *)ub_idx_X;

  long long *lbvalI8 = (long long *)lb_val_X;
  long long *ubvalI8 = (long long *)ub_val_X;
  long long *f1I8ptr = (long long *)f1_X;
  long long *f2I8ptr = (long long *)f2_X;

  long long minval = lbvalI8[0];
  long long maxval = ubvalI8[nR3-1];
  long long t3_start = 0;
  for ( long long i = 0; i < nR1; i++ ) { 
    long long f1I8val = f1I8ptr[i];
    /* find range [t2lb, t2ub], for this guy */
    bool found_range = false; long long t2lb, t2ub;
//	f1I8val, t3_start, nR3);
    for ( long long j = t3_start; j < nR3; j++ ) {
      if ( f1I8val < minval ) { found_range = false; break; }
      if ( f1I8val > maxval ) { found_range = false; break; }
      t3_start = j;
      if ( ( f1I8val >= lbvalI8[j] ) &&  ( f1I8val <= ubvalI8[j] ) ) {
	t2lb = lbidx[j];
	t2ub = ubidx[j];
	if ( t2ub > nR2 ) { t2ub = nR2; }
	found_range = true;
	break;
      }
    }
    if ( found_range == false ) {
      cfX[i] = FALSE;
    }
    else {
      // Do binary search of t2 between t2lb and t2ub
      long long pos = -1;
      status = bin_search_I8(f2I8ptr+t2lb, (t2ub-t2lb), f1I8val, &pos, "");
      cBYE(status);
      if ( pos <  0 ) {
        cfX[i] = FALSE;
      }
      else {
        cfX[i] = TRUE;
      }
    }
  }
  zero_fld_rec(&cfld_rec); cfld_rec.fldtype = I4;
  status = add_fld(t1_id, cfld, ddir_id, fileno, &cfld_id, &cfld_rec);
  cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(lb_val_X, lb_val_nX);
  rs_munmap(ub_val_X, ub_val_nX);
  rs_munmap(lb_idx_X, lb_idx_nX);
  rs_munmap(ub_idx_X, ub_idx_nX);
  rs_munmap(cfX, cf_nX);
  return status ;
}
