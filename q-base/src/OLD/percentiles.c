#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "f_to_s.h"
#include "meta_globals.h"

#include "uniq_int.h"
#include "uniq_longlong.h"
#include "uniq_alldef_int.h"
#include "uniq_alldef_longlong.h"
#include "uniq_cnt_alldef_i_i.h"
#include "uniq_cnt_alldef_ll_ll.h"
#include "uniq_cnt_alldef_ll_dbl.h"

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
  FLD_TYPE *src_fld_meta; 
  long long src_nR; 
  int src_tbl_id, src_fld_id, dst_tbl_id, dst_fld_id;
  int n_out, isrc_fldtype;
  char str_meta_data[256]; char *endptr = NULL;

  char *min_X = NULL; size_t n_min_X = 0; char *minfile = NULL;
  char *max_X = NULL; size_t n_max_X = 0; char *maxfile = NULL;
  char *cnt_X = NULL; size_t n_cnt_X = 0; char *cntfile = NULL;
  char *avg_X = NULL; size_t n_avg_X = 0; char *avgfile = NULL;
  char *sd_X = NULL;  size_t n_sd_X = 0;  char *sdfile = NULL;

  int *iminptr = NULL, *imaxptr = NULL;
  float *fminptr = NULL, *fmaxptr = NULL;
  long long *cntptr = NULL;
  double *avgptr = NULL;
  double *sdptr = NULL;

  int b, bin_size;
  FILE *ofp = 0;
  char str_rslt[32];

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }

  zero_string(str_meta_data, 256);
  //--------------------------------------------------------
  n_out = strtol(str_n_out, &endptr, 10);
  if ( ( n_out >= MAX_BINS_FOR_PERCENTILE ) || ( n_out <= 1 ) ) {
    fprintf(stderr, "num_bins = %s not in valid range [2, %d] \n",
	    str_n_out, MAX_BINS_FOR_PERCENTILE);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;
  if ( n_out >= src_nR ) {
    fprintf(stderr, "Source Table [%s] has insufficient rows [%lld]\n",
	    src_tbl, src_nR);
    go_BYE(-1);
  }
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id);
  chk_range(src_fld_id, 0, g_n_fld);
  src_fld_meta = &(g_fld[src_fld_id]);
  if ( ( strcmp(src_fld_meta->fldtype, "int") == 0 ) || 
       ( strcmp(src_fld_meta->fldtype, "int") == 0 ) )  {
    /* all is well */
  }
  else { go_BYE(-1); }
  /* Make sure src_fld field is sorted ascending */
  zero_string(str_rslt, 32);
  status = f_to_s(src_tbl, src_fld, "is_sorted", str_rslt);
  cBYE(status);
  if ( strcmp(str_rslt, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_fld, src_tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0);
  cBYE(status);
  status = mk_ifldtype(src_fld_meta->fldtype, &isrc_fldtype);
  // Check that no null values
  if ( src_fld_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  // Create 5 files for the 5 fields to be created
  // min, max, cnt, avg, sd
  int n_sizeof = src_fld_meta->n_sizeof;
  long long ifilesz = n_sizeof * n_out;
  long long lfilesz = sizeof(long long) * n_out;
  long long dfilesz = sizeof(double) * n_out;

  status=open_temp_file(&ofp, &minfile, ifilesz); cBYE(status); fclose_if_non_null(ofp);
  status=open_temp_file(&ofp, &maxfile, ifilesz); cBYE(status); fclose_if_non_null(ofp);
  status=open_temp_file(&ofp, &cntfile, lfilesz); cBYE(status); fclose_if_non_null(ofp);
  status=open_temp_file(&ofp, &avgfile, dfilesz); cBYE(status); fclose_if_non_null(ofp);
  status=open_temp_file(&ofp, &sdfile, dfilesz);  cBYE(status); fclose_if_non_null(ofp);

  status = mk_file(minfile, ifilesz); cBYE(status);
  rs_mmap(minfile, &min_X, &n_min_X, 1); cBYE(status);

  status = mk_file(maxfile, ifilesz); cBYE(status);
  rs_mmap(maxfile, &max_X, &n_max_X, 1); cBYE(status);

  status = mk_file(cntfile, lfilesz); cBYE(status);
  rs_mmap(cntfile, &cnt_X, &n_cnt_X, 1); cBYE(status);

  status = mk_file(avgfile, dfilesz); cBYE(status);
  rs_mmap(avgfile, &avg_X, &n_avg_X, 1); cBYE(status);

  status = mk_file(sdfile, dfilesz); cBYE(status);
  rs_mmap(sdfile,  &sd_X, &n_sd_X, 1); cBYE(status);

  //------------------------------------------------------
  /* Delete table if it exists. Create brand new table */
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  if ( dst_tbl_id >= 0 ) { 
    status = del_tbl(NULL, dst_tbl_id); cBYE(status);
  }
  status = add_tbl(dst_tbl, str_n_out, &dst_tbl_id); cBYE(status);
  //-----------------------------------------------------------
  // START: Here starts the processing
  /* b is the bin number */
  iminptr = (int *)min_X;
  imaxptr = (int *)max_X;
  fminptr = (float *)min_X;
  fmaxptr = (float *)max_X;
  cntptr  = (long long *)cnt_X;
  avgptr  = (double *)avg_X;
  sdptr   = (double *)sd_X;
  bin_size = src_nR / n_out;
  for ( b = 0; b < n_out; b++ ) { 
    long long lb, ub;
    double dsum;
    int   ival, iminval, imaxval; int *iptr = NULL;
    float fval, fminval, fmaxval; float *fptr = NULL;
    lb = b * bin_size;
    ub = lb + bin_size;
    if ( b == ( n_out -1 ) ) { ub = src_nR; }
    //-----------------------------------------------
    dsum = 0;
    iminval = INT_MAX; imaxval = INT_MIN; 
    fminval = FLT_MAX; fmaxval = FLT_MIN; 
    iptr = ((int *)src_fld_X)   + lb;
    fptr = ((float *)src_fld_X) + lb;
    //-----------------------------------------------
    for ( int i = 0; i < (ub - lb); i++ ) { 
      switch ( isrc_fldtype ) {
      case FLDTYPE_INT : 
	ival = iptr[i];
	iminval = min(iminval, ival);
	imaxval = max(imaxval, ival);
	dsum += ival;
	break;
      case FLDTYPE_FLOAT : 
	fval = fptr[i];
	fminval = min(fminval, fval);
	fmaxval = max(fmaxval, fval);
	dsum += fval;
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    // Write out the values 
    cntptr[b]  = (ub - lb);
    avgptr[b]  = dsum / (double)(ub - lb);
    switch ( isrc_fldtype ) {
    case FLDTYPE_INT : 
      iminptr[b] = iminval;
      imaxptr[b] = imaxval;
      sdptr[b]   = 0; // TODO TO BE IMPLEMENTED
      break;
    case FLDTYPE_FLOAT : 
      fminptr[b] = fminval;
      fmaxptr[b] = fmaxval;
      sdptr[b]   = 0; // TODO TO BE IMPLEMENTED
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }

  //-----------------------------------------------------------
  // Add output field(s) to meta data 
  sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d", minfile,
	  src_fld_meta->fldtype, src_fld_meta->n_sizeof);
  status = add_fld(dst_tbl, "min", str_meta_data, &dst_fld_id);
  cBYE(status);

  sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d", maxfile,
	  src_fld_meta->fldtype,  src_fld_meta->n_sizeof);
  status = add_fld(dst_tbl, "max", str_meta_data, &dst_fld_id);
  cBYE(status);

  sprintf(str_meta_data, "filename=%s:fldtype=long long:n_sizeof=8", cntfile);
  status = add_fld(dst_tbl, "cnt", str_meta_data, &dst_fld_id);
  cBYE(status);

  sprintf(str_meta_data, "filename=%s:fldtype=double:n_sizeof=8", avgfile);
  status = add_fld(dst_tbl, "avg", str_meta_data, &dst_fld_id);
  cBYE(status);

  sprintf(str_meta_data, "filename=%s:fldtype=double:n_sizeof=8", sdfile);
  status = add_fld(dst_tbl, "sd", str_meta_data, &dst_fld_id);
  cBYE(status);

  //-----------------------------------------------------------
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);

  free_if_non_null(minfile); rs_munmap(min_X, n_min_X);
  free_if_non_null(maxfile); rs_munmap(max_X, n_max_X);
  free_if_non_null(cntfile); rs_munmap(cnt_X, n_cnt_X);
  free_if_non_null(avgfile); rs_munmap(avg_X, n_avg_X);
  free_if_non_null(sdfile);  rs_munmap(sd_X,  n_sd_X);

  return(status);
}
