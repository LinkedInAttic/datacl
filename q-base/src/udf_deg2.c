#ifdef IPP
#include "ipps.h"
#include "ippcore.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "is_fld.h"
#include "bin_search_I4.h"
#include "qsort_asc_I4.h"

extern int g_num_cores;

// START FUNC DECL
int
udf_deg2(
	 char *TM_tbl,  /* member table */
	 char *TM_TC_lb, /* I8 */
	 char *TM_TC_cnt, /* I4 */
	 char *TD1_tbl, /* first degree connections */
	 char *TD1_fk_TM, /* I4 */
	 char *TC_tbl, /* connections */
	 char *TC_mid, /* I4 */
	 char *TD2_tbl, /* second  degree connections */
	 char *TD2_mid  /* I4 */
	 )
// STOP FUNC DECL
{
  int status = 0;

  int *deg2 = NULL; int n_deg2 = 0;
  int *alt_deg2 = NULL, *xloc = NULL;
  unsigned int a, b;  int n_alt_deg2;
  int block_size, nT;

  char *TM_TC_lb_X = NULL;  size_t TM_TC_lb_nX = 0; 
  char *TM_TC_cnt_X = NULL; size_t TM_TC_cnt_nX = 0; 

  char *TC_mid_X = NULL;    size_t TC_mid_nX = 0; 

  char *TD2_mid_X = NULL;   size_t TD2_mid_nX = 0; 

  char *TD1_fk_TM_X = NULL; size_t TD1_fk_TM_nX = 0; 

  int TM_tbl_id = -1;  TBL_REC_TYPE TM_tbl_rec; long long nR_TM;
  int TC_tbl_id = -1;  TBL_REC_TYPE TC_tbl_rec; long long nR_TC;
  int TD1_tbl_id = -1; TBL_REC_TYPE TD1_tbl_rec; long long nR_TD1;
  int TD2_tbl_id = -1; TBL_REC_TYPE TD2_tbl_rec; long long nR_TD2;
  int nn_fld_id = -1;        FLD_REC_TYPE nn_fld_rec;

  int TM_TC_lb_fld_id = -1;  FLD_REC_TYPE TM_TC_lb_fld_rec;
  int TM_TC_cnt_fld_id = -1; FLD_REC_TYPE TM_TC_cnt_fld_rec;

  int TC_mid_fld_id = -1;    FLD_REC_TYPE TC_mid_fld_rec;

  int TD1_fk_TM_fld_id = -1;    FLD_REC_TYPE TD1_fk_TM_fld_rec;

  int TD2_mid_fld_id = -1;    FLD_REC_TYPE TD2_mid_fld_rec;

  if ( ( TM_tbl == NULL )  || ( *TM_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( TC_tbl == NULL )  || ( *TC_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( TD1_tbl == NULL ) || ( *TD1_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( TD2_tbl == NULL ) || ( *TD2_tbl == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(TD2_tbl, TM_tbl)  == 0 ) { go_BYE(-1); }
  if ( strcmp(TD2_tbl, TC_tbl)  == 0 ) { go_BYE(-1); }
  if ( strcmp(TD2_tbl, TD1_tbl) == 0 ) { go_BYE(-1); }

  status = is_tbl(TM_tbl, &TM_tbl_id, &TM_tbl_rec); cBYE(status);
  if ( TM_tbl_id < 0 ) { go_BYE(-1); }
  nR_TM = TM_tbl_rec.nR;

  status = is_fld(NULL, TM_tbl_id, TM_TC_lb, &TM_TC_lb_fld_id, 
		  &TM_TC_lb_fld_rec, &nn_fld_id, &nn_fld_rec);
  if ( TM_TC_lb_fld_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id    >= 0 ) { go_BYE(-1); }
  if ( TM_TC_lb_fld_rec.fldtype != I8 ) { go_BYE(-1); }
  status = get_data(TM_TC_lb_fld_rec, &TM_TC_lb_X, &TM_TC_lb_nX, 0); 
  long long *TM_TC_lb_I8 = (long long *)TM_TC_lb_X;

  status = is_fld(NULL, TM_tbl_id, TM_TC_cnt, &TM_TC_cnt_fld_id, 
		  &TM_TC_cnt_fld_rec, &nn_fld_id, &nn_fld_rec);
  if ( TM_TC_cnt_fld_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id    >= 0 ) { go_BYE(-1); }
  if ( TM_TC_cnt_fld_rec.fldtype != I4 ) { go_BYE(-1); }
  status = get_data(TM_TC_cnt_fld_rec, &TM_TC_cnt_X, &TM_TC_cnt_nX, 0); 
  int *TM_TC_cnt_I4 = (int *)TM_TC_cnt_X;
  /*----------------------------------------------------------------*/
  status = is_tbl(TC_tbl, &TC_tbl_id, &TC_tbl_rec); cBYE(status);
  if ( TC_tbl_id < 0 ) { go_BYE(-1); }
  nR_TC = TC_tbl_rec.nR;
  status = is_fld(NULL, TC_tbl_id, TC_mid, &TC_mid_fld_id, 
		  &TC_mid_fld_rec, &nn_fld_id, &nn_fld_rec);
  if ( TC_mid_fld_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id    >= 0 ) { go_BYE(-1); }
  if ( TC_mid_fld_rec.fldtype != I4 ) { go_BYE(-1); }
  status = get_data(TC_mid_fld_rec, &TC_mid_X, &TC_mid_nX, 0); 
  int *TC_mid_I4 = (int *)TC_mid_X;

  /*----------------------------------------------------------------*/
  status = is_tbl(TD1_tbl, &TD1_tbl_id, &TD1_tbl_rec); cBYE(status);
  if ( TD1_tbl_id < 0 ) { go_BYE(-1); }
  nR_TD1 = TD1_tbl_rec.nR;

  status = is_fld(NULL, TD1_tbl_id, TD1_fk_TM, &TD1_fk_TM_fld_id, 
		  &TD1_fk_TM_fld_rec, &nn_fld_id, &nn_fld_rec);
  if ( TD1_fk_TM_fld_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id    >= 0 ) { go_BYE(-1); }
  if ( TD1_fk_TM_fld_rec.fldtype != I4 ) { go_BYE(-1); }
  status = get_data(TD1_fk_TM_fld_rec, &TD1_fk_TM_X, &TD1_fk_TM_nX, 0); 
  int *TD1_fk_TM_I4 = (int *)TD1_fk_TM_X;

  /*----------------------------------------------------------------*/
  // Set up data structures for each thread 
#define MAX_NUM_SECOND_DEGREE 32*1048576
  deg2 = malloc(MAX_NUM_SECOND_DEGREE * sizeof(int));
  return_if_malloc_failed(deg2); 
  int *bak_deg2 = deg2;
  //--------------------------------------------------
  unsigned long long t0 = get_time_usec();
  for ( int i = 0; i < nR_TD1; i++ ) {
    int TM_idx = TD1_fk_TM_I4[i];
    long long TC_idx = TM_TC_lb_I8[TM_idx];
    int       TC_cnt = TM_TC_cnt_I4[TM_idx];
    // TODO P1 TO BE FIXED: realloc needed
    if ( n_deg2 + TC_cnt > MAX_NUM_SECOND_DEGREE ) { continue; }
    memcpy(deg2, TC_mid_I4 + TC_idx, ((sizeof(int) * TC_cnt)));
    deg2   += TC_cnt;
    n_deg2 += TC_cnt;
  }
  // fprintf(stderr, "Need to process %d people \n", cnt);
  unsigned long long t1 = get_time_usec();
  deg2   = bak_deg2;

  if ( n_deg2 > 2*1048576 ) {  // TODO P2 Document this choice
  n_alt_deg2 = max(1024, (n_deg2 * 2));
  n_alt_deg2 = prime_geq(n_alt_deg2);
  alt_deg2 = malloc(n_alt_deg2 * sizeof(int));
  return_if_malloc_failed(alt_deg2); 
  xloc = malloc(n_deg2 * sizeof(int));
  return_if_malloc_failed(xloc); 
  unsigned long long seed = get_time_usec();
  srand48(seed); 
  a = (unsigned long long)mrand48();
  a = prime_geq(a);
  b = (unsigned long long)mrand48();
  b = prime_geq(b);
  nT = g_num_cores;
//-------------------------------------
  block_size = n_deg2 / nT;
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { 
    int lb = tid * block_size;
    int ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = n_deg2; }
    for ( int i = lb; i < ub; i++ ) { 
      xloc[i] = ( ( deg2[i] * a ) + b ) % n_alt_deg2;
    }
  }
//-------------------------------------
  block_size = n_alt_deg2 / nT;
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { 
    int lb = tid * block_size;
    int ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = n_alt_deg2; }
    for ( int i = lb; i < ub; i++ ) { 
      alt_deg2[i] = -1; // indicates unused
    }
  }
//-------------------------------------
  // sequential loop
  for ( int i = 0; i < n_deg2; i++ ) { 
    int loc = xloc[i];
    int val = deg2[i];
    if ( alt_deg2[loc] == -1 ) { 
      alt_deg2[loc] = val;
    }
    else if ( alt_deg2[loc] == val ) {
      // nothing to do 
    }
    else { // sequential search for new spot 
      bool found = false;
      for ( int j = loc; j < n_alt_deg2; j++ ) { 
        if ( alt_deg2[j] == -1 ) { 
          alt_deg2[j] = val;
	  found = true;
	  break;
        }
        else if ( alt_deg2[j] == val ) {
	  found = true;
	  break;
        }
      }
      if ( found == false ) { 
	for ( int j = 0; j < loc; j++ ) { 
	  if ( alt_deg2[j] == -1 ) { 
	    alt_deg2[j] = val;
	    found = true;
	    break;
	  }
	  else if ( alt_deg2[j] == val ) {
	    found = true;
	    break;
	  }
	}
	if ( found == false ) { go_BYE(-1); } 
      }
    }
  }
  //-------------------------------------
  block_size = n_deg2 / nT;
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { 
    int lb = tid * block_size;
    int ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = n_deg2; }
    for ( int i = lb; i < ub; i++ ) { 
      deg2[i] = -1; // indicates unused
    }
  }
  //-------------------------------------
  nR_TD2 = 0; 
  for ( int i = 0; i < n_alt_deg2; i++ ) { 
    int val = alt_deg2[i];
    if ( val < 0 ) { continue; }
    deg2[nR_TD2++] = val;
  }
  }
  else {
#ifdef IPP
  ippsSortAscend_32s_I(deg2, n_deg2);
#else
  qsort_asc_I4 (deg2, n_deg2, sizeof(int), NULL);
#endif
  unsigned long long t2 = get_time_usec();
  /* Remove duplicates */
  nR_TD2 = 1; /* first guy always gets in */
  for ( int oldidx = 1; oldidx < n_deg2; oldidx++ ) {
    if ( deg2[oldidx] != deg2[oldidx-1] ) {
      deg2[nR_TD2++] = deg2[oldidx];
    }
  }
  }
  unsigned long long t3 = get_time_usec();

  /* Write output to file */
  FLD_TYPE fldtype = I4; 
  int fldsz = -1; size_t filesz = 0; int ddir_id = -1; int fileno = -1;
  status = get_fld_sz(fldtype, &fldsz); cBYE(status);
  filesz = fldsz * nR_TD2; 
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &TD2_mid_X, &TD2_mid_nX, true); cBYE(status);
  memcpy(TD2_mid_X, deg2, fldsz * nR_TD2);
  rs_munmap(TD2_mid_X, TD2_mid_nX);
  unsigned long long t4 = get_time_usec();
  /*--------------------------------------------*/
  char strbuf[32];
  sprintf(strbuf, "%lld", nR_TD2); 
  status = add_tbl(TD2_tbl, strbuf, &TD2_tbl_id, &TD2_tbl_rec); cBYE(status);
  zero_fld_rec(&TD2_mid_fld_rec); TD2_mid_fld_rec.fldtype = I4;
  status = add_fld(TD2_tbl_id, TD2_mid, ddir_id, fileno, 
		   &TD2_mid_fld_id, &TD2_mid_fld_rec);
  cBYE(status);
  /*
    fprintf(stderr, "t1 = %llu \n", t1 - t0);
    fprintf(stderr, "t2 = %llu \n", t2 - t1);
    fprintf(stderr, "t3 = %llu \n", t3 - t2);
    fprintf(stderr, "t4 = %llu \n", t4 - t3);
  */

 BYE:
  rs_munmap(TM_TC_lb_X, TM_TC_lb_nX);
  rs_munmap(TM_TC_cnt_X, TM_TC_cnt_nX);
  rs_munmap(TC_mid_X, TC_mid_nX);
  rs_munmap(TD1_fk_TM_X,  TD1_fk_TM_nX);
  rs_munmap(TD2_mid_X, TD2_mid_nX);
  free_if_non_null(deg2); 
  free_if_non_null(alt_deg2); 
  free_if_non_null(xloc); 
  return(status);
}
