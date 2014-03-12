#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "aux_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "get_nR.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "f_to_s.h"
#include "is_A_in_b.h"
#include "assign_I1.h"
#include "find_largest_leq_I4.h"
#include "find_smallest_geq_I4.h"
#include "find_largest_leq_I8.h"
#include "find_smallest_geq_I8.h"

/* This is used when f1 and f2 are non-null, sorted ascending */
/* Usually used when T1 is large and T2 is small */

//---------------------------------------------------------------
// START FUNC DECL
int 
is_A_in_b(
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *cfld
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *cfX = NULL;  size_t cf_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  FLD_REC_TYPE f1_rec, f2_rec, cfld_rec;
  FLD_REC_TYPE nn_f1_rec, nn_f2_rec;
  TBL_REC_TYPE t1_rec, t2_rec;
  long long nR1, nR2;
  int t1_id, t2_id; 
  int f1_id, nn_f1_id; 
  int f2_id, nn_f2_id; 
  int cfld_id = INT_MIN;
  int fldsz = 0; size_t filesz = 0;
  int ddir_id = -1, fileno = -1;
#define BUFSZ 16
  char buf[BUFSZ];
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR; if ( nR1 <= 0 ) { go_BYE(-1); }
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  status = f_to_s(t1, f1, "is_sorted", buf, BUFSZ); cBYE(status);
  if ( strcmp(buf, "ascending") != 0 ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbls[t2_id].nR;
  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec);
  chk_range(f2_id, 0, g_n_fld);
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  status = f_to_s(t2, f2, "is_sorted", buf, BUFSZ); cBYE(status);
  if ( strcmp(buf, "ascending") != 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  if (f1_rec.fldtype != f2_rec.fldtype ) { go_BYE(-1); }
  //--------------------------------------------------------
  // There can be no null values in f1, f2 (because sorted=> all def 
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  if ( nn_f2_id >= 0 ) { go_BYE(-1); }
  /*------------------------------------------------------------*/
  /* Make space for output condition field */
  status = get_fld_sz(I1, &fldsz); cBYE(status);
  filesz = nR1 * fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &cfX, &cf_nX, 1); cBYE(status);
  /*------------------------------------------------------------------*/

  int       *f2_I4 = (int       *)f2_X;
  long long *f2_I8 = (long long *)f2_X;
  int       max_val_f2_I4 = f2_I4[nR2-1];
  long long max_val_f2_I8 = f2_I8[nR2-1];
  int       *max_f2_I4  = f2_I4 + nR2;
  long long *max_f2_I8  = f2_I8 + nR2;

  long long min_block_size = 8192, block_size; int nT;
  status = partition(nR1, min_block_size, -1, &block_size, &nT); cBYE(status);
  cilkfor ( int tid = 0; tid < nT; tid++ ) {
    if ( status < 0 ) { continue; }
    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR1; }
    long long nX = ub - lb;
    // These are kept local to avoid sharing issues
    int       *f1_I4 = (int       *)f1_X; f1_I4 += lb;
    long long *f1_I8 = (long long *)f1_X; f1_I8 += lb;
    char      *l_cfX = cfX; 
    //------------------------------------
    long long f2_idx = -1;
    bool      all_zero;
    long long min_val_f1_I8 = *f1_I4;
    int       min_val_f1_I4 = *f1_I8;
    switch ( f1_rec.fldtype ) { 
    case I4 : 
      if ( min_val_f1_I4 > max_val_f2_I4 ) { 
	all_zero = true;
      }
      else {
	all_zero = false;
	status = find_largest_leq_I4 (f2_I4, nR2, min_val_f1_I4, 
				      &f2_idx);
      }
      break;
    case I8 : 
      if ( min_val_f1_I8 > max_val_f2_I8 ) { 
	all_zero = true;
      }
      else {
	all_zero = false;
	status = find_largest_leq_I8 (f2_I8, nR2, min_val_f1_I8, 
				      &f2_idx);
      }
      break;
    default : 
      if ( status == 0 ) { WHEREAMI; } status = -1;
      break;
    }
    if ( all_zero == true ) { 
      // Do optimization if all values in this block are null 
      assign_const_I1(cfX, nX, 0); 
      continue;
    }
    // Cannot happen 
    if ( f2_idx < 0 ) {  if ( status == 0 ) { WHEREAMI; } status = -1; }
    int       *l_f2_I4      = f2_I4 + f2_idx;
    long long *l_f2_I8      = f2_I8 + f2_idx;
    //-----------------------------------
    switch ( f1_rec.fldtype ) {
    case I4 : 
#include "incl_is_A_in_b_I4.c"
      break;
    case I8 : 
#include "incl_is_A_in_b_I8.c"
      break;
    default  :
      if ( status == 0 ) { WHEREAMI; } status = -1;
      break;
    }
  }
  cBYE(status);

  // Add output field to meta data 
  zero_fld_rec(&cfld_rec); cfld_rec.fldtype = I1;
  status = add_fld(t1_id, cfld, ddir_id, fileno, &cfld_id, &cfld_rec);
  cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(cfX, cf_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  return(status);
}
