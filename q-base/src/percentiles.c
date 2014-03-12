#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "f_to_s.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

#include "uniq_I4.h"
#include "uniq_I8.h"
#include "uniq_alldef_I4.h"
#include "uniq_alldef_I8.h"
#include "uniq_cnt_alldef_I4.h"
#include "uniq_cnt_alldef_I8.h"

/* Assumption is that src_fld is sorted. TODO: Put in a check for this */
//---------------------------------------------------------------
// START FUNC DECL
int 
percentiles(
	    char *src_tbl,
	    char *src_fld,
	    char *dst_tbl,
	    char *str_n_out
	    )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  TBL_REC_TYPE    src_tbl_rec; int    src_tbl_id = -1; long long src_nR = -1; 
  TBL_REC_TYPE    dst_tbl_rec; int    dst_tbl_id = -1; long long dst_nR = -1;
  FLD_REC_TYPE    src_fld_rec; int    src_fld_id = -1;
  FLD_REC_TYPE nn_src_fld_rec; int nn_src_fld_id = -1;

  FLD_REC_TYPE    min_rec; int    min_id = -1;
  FLD_REC_TYPE    max_rec; int    max_id = -1;
  FLD_REC_TYPE    avg_rec; int    avg_id = -1;
  FLD_REC_TYPE    cnt_rec; int    cnt_id = -1;

  char *minX = NULL; size_t min_nX = 0; int min_ddir_id = -1, min_fileno = 0; 
  char *maxX = NULL; size_t max_nX = 0; int max_ddir_id = -1, max_fileno = 0; 
  char *cntX = NULL; size_t cnt_nX = 0; int cnt_ddir_id = -1, cnt_fileno = 0; 
  char *avgX = NULL; size_t avg_nX = 0; int avg_ddir_id = -1, avg_fileno = 0; 

  int fldsz = 0; long long filesz = 0;
  int bin_size;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }

  //--------------------------------------------------------
  status = stoI8(str_n_out, &dst_nR); cBYE(status);
  if ( ( dst_nR >= MAX_BINS_FOR_PERCENTILE ) || ( dst_nR <= 1 ) ) {
    fprintf(stderr, "num_bins = %s not in valid range [2, %d] \n",
	    str_n_out, MAX_BINS_FOR_PERCENTILE);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  if (src_tbl_id < 0 ) { go_BYE(-1); }
  src_nR = src_tbl_rec.nR;
  if ( dst_nR >= src_nR ) {
    fprintf(stderr, "Source Table [%s] has insufficient rows [%lld]\n",
	    src_tbl, src_nR);
    go_BYE(-1);
  }
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec);
  if ( src_fld_id < 0 ) { go_BYE(-1); }
  switch ( src_fld_rec.fldtype ) { 
  case I4 : case I8 : case F4 : case F8 : break;
  default : go_BYE(-1); break;
  }
  if ( nn_src_fld_id >= 0 ) { go_BYE(-1); }
  /* Make sure src_fld field is sorted ascending */
  char srttype[32]; zero_string(srttype, 32);
  status = f_to_s(src_tbl, src_fld, "is_sorted", srttype, 32); cBYE(status);
  if ( strcmp(srttype, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_fld, src_tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  status = get_data(src_fld_rec, &src_fld_X, &src_fld_nX, 0); cBYE(status);
  //--------------------------------------------------------
  // Create 4 files for the 4 fields to be created
  // min, max, cnt, avg

  int n1, n2;
  status = get_fld_sz(I8, &n1); cBYE(status);
  status = get_fld_sz(F8, &n2); cBYE(status);
  if ( n1 != n2 ) { go_BYE(-1); }
  
  status = get_fld_sz(I8, &fldsz); cBYE(status); 
  filesz = fldsz * dst_nR; 
  status = mk_temp_file(filesz, &min_ddir_id, &min_fileno); cBYE(status);
  status = mk_temp_file(filesz, &max_ddir_id, &max_fileno); cBYE(status);
  status = mk_temp_file(filesz, &cnt_ddir_id, &cnt_fileno); cBYE(status);

  status = get_fld_sz(F8, &fldsz); cBYE(status); 
  filesz = fldsz * dst_nR; 
  status = mk_temp_file(filesz, &avg_ddir_id, &avg_fileno); cBYE(status);

  status = q_mmap(min_ddir_id, min_fileno, &minX, &min_nX, 1); cBYE(status);
  status = q_mmap(max_ddir_id, max_fileno, &maxX, &max_nX, 1); cBYE(status);
  status = q_mmap(cnt_ddir_id, cnt_fileno, &cntX, &cnt_nX, 1); cBYE(status);
  status = q_mmap(avg_ddir_id, avg_fileno, &avgX, &avg_nX, 1); cBYE(status);

  //------------------------------------------------------
  /* Delete table if it exists. Create brand new table */
  status = is_tbl(dst_tbl, &dst_tbl_id , &dst_tbl_rec); cBYE(status);
  if ( dst_tbl_id >= 0 ) { 
    status = del_tbl(NULL, dst_tbl_id); cBYE(status);
  }
  zero_tbl_rec(&dst_tbl_rec);
  status = add_tbl(dst_tbl, str_n_out, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  //-----------------------------------------------------------
  // START: Here starts the processing
  /* b is the bin number */
  long long *minI8  = (long long *)minX;
  long long *maxI8  = (long long *)maxX;
  double    *minF8  = (double    *)minX;
  double    *maxF8  = (double    *)maxX;
  long long *cntI8  = (long long *)cntX;
  double    *avgF8  = (double    *)avgX;
  bin_size = src_nR / dst_nR;

  cilkfor ( int b = 0; b < dst_nR; b++ ) { 
    long long lb = b * bin_size;
    long long ub = lb + bin_size;
    if ( b == ( dst_nR -1 ) ) { ub = src_nR; }
    int *valsI4       = (int       *) src_fld_X; valsI4 += lb;
    float *valsF4     = (float     *) src_fld_X; valsF4 += lb;
    long long *valsI8 = (long long *) src_fld_X; valsI8 += lb;
    double *valsF8    = (double    *) src_fld_X; valsF8 += lb;
    //-----------------------------------------------
    double lvalF8 = 0; long long lvalI8 = 0;
    double lsumF8 = 0; long long lsumI8 = 0;
    long long lminI8 = LLONG_MAX, lmaxI8 = LLONG_MIN; 
    double    lminF8 = DBL_MIN,   lmaxF8 = DBL_MIN; 
    //-----------------------------------------------
    for ( long long i = 0; i < (ub - lb); i++ ) { 
      switch ( src_fld_rec.fldtype ) { 
      case I4 : lvalI8 = valsI4[i]; break;
      case I8 : lvalI8 = valsI8[i]; break;
      case F4 : lvalF8 = valsI4[i]; break;
      case F8 : lvalF8 = valsI8[i]; break;
      default : status = -1; continue; break;
      }
      switch ( src_fld_rec.fldtype ) { 
      case I4 : case I8 : 
	lminI8  = min(lminI8 , lvalI8); 
	lmaxI8  = max(lmaxI8 , lvalI8); 
	lsumI8 += lvalI8; 
	break;
      case F4 : case F8 : 
	lminF8  = min(lminF8 , lvalF8); 
	lmaxF8  = max(lmaxF8 , lvalF8); 
	lsumF8 += lvalF8; 
	break;
      default : status = -1; continue; break;
      }
    }
    // Write out the values 
    cntI8[b]  = (ub - lb);
    double n = ub - lb;
    switch ( src_fld_rec.fldtype ) {
    case I4 : case I8 : 
      minI8[b]  = lminI8;
      maxI8[b]  = lmaxI8;
      avgF8[b]  = (double)lsumI8 / n; 
      break; 
    case F4 : case F8 : 
      minF8[b]  = lminF8;
      maxF8[b]  = lmaxF8;
      avgF8[b]  = (double)lsumF8 / n; 
      break; 
    default : status = -1; continue; break;
    }
  }

  //-----------------------------------------------------------
  status = add_tbl(dst_tbl, str_n_out, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  // Add output field(s) to meta data 
  zero_fld_rec(&min_rec); zero_fld_rec(&max_rec); 
  switch ( src_fld_rec.fldtype ) { 
  case I4 : case I8 : min_rec.fldtype = I8; max_rec.fldtype = I8; break;
  case F4 : case F8 : min_rec.fldtype = F8; max_rec.fldtype = F8; break;
  default : go_BYE(-1); break;
  }
  status = add_fld(dst_tbl_id, "min", min_ddir_id, min_fileno, 
		   &min_id, &min_rec); cBYE(status);

  status = add_fld(dst_tbl_id, "max", max_ddir_id, max_fileno, 
		   &max_id, &max_rec); cBYE(status);

  zero_fld_rec(&avg_rec); avg_rec.fldtype = F8;
  status = add_fld(dst_tbl_id, "avg", avg_ddir_id, avg_fileno, 
		   &avg_id, &avg_rec); cBYE(status);
  zero_fld_rec(&cnt_rec); cnt_rec.fldtype = I8;
  status = add_fld(dst_tbl_id, "cnt", cnt_ddir_id, cnt_fileno, 
		   &cnt_id, &cnt_rec); cBYE(status);
  //-----------------------------------------------------------
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(minX, min_nX);
  rs_munmap(maxX, max_nX);
  rs_munmap(cntX, cnt_nX);
  rs_munmap(avgX, avg_nX);
  return(status);
}
