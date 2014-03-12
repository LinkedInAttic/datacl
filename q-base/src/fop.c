/*
  #ifdef IPP
  #include "ipps.h"
  #endif
  * Strangely, the IPP functions do not work as well as mine.
  ippsSortAscend_32s_I((int *)X, nR);
  ippsSortDescend_32s_I((int *)X, nR);
  * I have taken them out but this warrants further investigation.
  * TODO P2
  */

// last review 9/9/2013
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "fop.h"
#include "mk_temp_file.h"
#include "vec_f_to_s.h"
#include "permute_I4.h"
#include "permute_I8.h"
#include "set_meta.h"
#include "aux_meta.h"

#include "extract_I8.h"
#include "extract_S.h"
#include "zero_after_n.h"

#include "sort_asc_F4.h"
#include "sort_dsc_F4.h"

#include "qsort_asc_I4.h"
#include "qsort_dsc_I4.h"

#include "qsort_asc_I8.h"
#include "qsort_dsc_I8.h"

#include "saturate_I1.h"
#include "saturate_I2.h"
#include "saturate_I4.h"
#include "saturate_I8.h"

#include "srt_uniform_I4.h"
#include "srt_uniform_I8.h"

#include "par_sort_with_bins.h"
#include "stride.h"
#include "mk_bins.h"

#include "meta_globals.h"

#define BUFLEN 32

// START FUNC DECL
int
decide_on_parallelism(
		      char *distribution,
		      char *X,
		      int tbl_id,
		      int fld_id,
		      long long nR,
		      long long *ptr_minI8,
		      long long *ptr_maxI8,
		      bool *ptr_use_par_sort
		      )
// STOP FUNC DECL
{
  int status = 0;
  long long minI8, maxI8;
  chk_range(fld_id, 0, g_n_fld);
  FLD_REC_TYPE fld_rec = g_flds[fld_id];
  char buf1[BUFLEN], buf2[BUFLEN];

  if ( ( distribution == NULL ) || ( *distribution == '\0' ) ) {
    *ptr_use_par_sort = false; return status;
  }
  if ( ( fld_rec.fldtype != I4 ) && ( fld_rec.fldtype != I8 ) ) {
    fprintf(stderr, "Not using par sort \n"); WHEREAMI; 
    *ptr_use_par_sort = false; return status;
  }
  if ( nR < 16*1048576 )  {
    fprintf(stderr, "Not using par sort \n"); WHEREAMI; 
    *ptr_use_par_sort = false; return status;
  }
  if ( strcmp(distribution, "uniform_random") != 0 ) {
    //-----------------------------------------------------------------
    // Get min value 
    if ( fld_rec.is_min_nn == true ) { 
      minI8 = fld_rec.minval.minI8;
    }
    else {
      zero_string(buf1, BUFLEN); zero_string(buf2, BUFLEN);
      status = vec_f_to_s(X, fld_rec.fldtype, NULL, nR, "min", buf1, BUFLEN);
      cBYE(status);
      status = read_nth_val(buf1, ":", 0, buf2, BUFLEN);
      status = stoI8(buf2, &minI8); cBYE(status);
      // Set for subsequent use
      status = int_set_meta(tbl_id, fld_id, "min", buf2, true); 
      cBYE(status);
    }
    *ptr_minI8 = minI8;
    if ( minI8 < 0 ) {
      fprintf(stderr, "Not using par sort \n"); WHEREAMI; 
      *ptr_use_par_sort = false;  return status;
    }
    // Get max value 
    if ( fld_rec.is_max_nn == true ) { 
      maxI8 = fld_rec.maxval.maxI8;
    }
    else {
      zero_string(buf1, BUFLEN); zero_string(buf2, BUFLEN);
      status = vec_f_to_s(X, fld_rec.fldtype, NULL, nR, "max", buf1, BUFLEN);
      cBYE(status);
      status = read_nth_val(buf1, ":", 0, buf2, BUFLEN);
      status = stoI8(buf2, &maxI8); cBYE(status);
      // Set for subsequent use
      status = int_set_meta(tbl_id, fld_id, "max", buf2, true); 
      cBYE(status);
    }
    *ptr_maxI8 = maxI8;
    if ( ( maxI8/(minI8+1) ) <= 64 ) { /* We use 2^6 "threads" */
      fprintf(stderr, "Not using par sort \n"); WHEREAMI; 
      *ptr_use_par_sort = false; return status;
    }
  }
  else if ( strcmp(distribution, "random") == 0 ) { 
  }
 BYE:
  return status;
}
//---------------------------------------------------------------
// START FUNC DECL
int 
fop(
    char *tbl,
    char *fld,
    char *str_op_spec
    )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *Y = NULL; size_t nY = 0;
  TBL_REC_TYPE tbl_rec;    int tbl_id; 
  FLD_REC_TYPE fld_rec;    int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 
  long long nR; bool is_null;
  char op[BUFLEN], srt_ordr[BUFLEN], distribution[BUFLEN];
  char       *I1ptr = NULL;
  short      *I2ptr = NULL;
  int        *I4ptr = NULL;
  long long  *I8ptr = NULL;

  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  zero_string(op, BUFLEN); 
  zero_string(srt_ordr, BUFLEN);
  zero_string(distribution, BUFLEN);
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( fld_id < 0 ) { go_BYE(-1); }
  if ( fld_rec.is_external ) { go_BYE(-1); }
  if ( nn_fld_id >= 0 ) { go_BYE(-1); }

  status = get_data(fld_rec, &X, &nX, true); cBYE(status);
  if ( nX == 0 ) { go_BYE(-1); }

  status = extract_S(str_op_spec, "op=[", "]", op, BUFLEN-1, &is_null);
  if ( is_null == false ) {
    status = extract_S(str_op_spec, "order=[", "]", srt_ordr, BUFLEN-1, 
		       &is_null);
    if ( is_null == true ) { go_BYE(-1); }
    if ( ( strcmp(srt_ordr, "asc") != 0 ) && ( strcmp(srt_ordr, "dsc") != 0 ) ){
      go_BYE(-1);
    }
    status = extract_S(str_op_spec, "distribution=[", "]", distribution, 
		       BUFLEN-1, &is_null);
    if ( is_null == false ) { 
      if ( strcmp(distribution, "uniform_random") == 0 ) {
	// all is well 
      }
      else {
	go_BYE(-1);
      }
    }
  }

  long long maxI8 = LLONG_MIN, minI8 = LLONG_MAX; int y, shift;
  bool use_par_sort = true;
  if ( ( strcmp(str_op_spec, "sortA") == 0 ) ||
       ( ( strcmp(op, "sort") == 0 ) && ( strcmp(srt_ordr, "asc") == 0 ) ) ) {
    status = decide_on_parallelism(distribution, X, tbl_id, fld_id, nR, 
				   &minI8, &maxI8, &use_par_sort); 
    cBYE(status);
    if ( use_par_sort == true ) {
      long long filesz = 0; int fldsz = 0, ddir_id = -1, fileno = -1;
      status = get_fld_sz(fld_rec.fldtype, &fldsz);
      filesz = nR * fldsz;
      status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
      status = q_mmap(ddir_id, fileno, &Y, &nY, 1); cBYE(status);
      if ( strcmp(distribution, "uniform_random") == 0 ) {
	if ( maxI8 < 0 ) { go_BYE(-1); }
	// Determine shift 
	switch ( fld_rec.fldtype ) { 
	case I4 : 
	  y = __builtin_clz((unsigned int)maxI8);
	  shift = (32 - y) -  6; /* 6 because we use 2^6 threads */
	  fprintf(stderr, "shift = %d, minI8 = %lld, maxI8 = %lld \n", 
		  shift, minI8, maxI8);
	  status = srt_uniform_I4((int *)X, nR, (int *)Y, shift, "asc"); 
	  cBYE(status);
	  break;
	case I8 : 
	  y = __builtin_clzll((unsigned long long)maxI8);
	  shift = (64 - y) - 6; /* 6 because we use 2^6 threads */
	  fprintf(stderr, "shift = %d, minI8 = %lld, maxI8 = %lld \n", 
		  shift, minI8, maxI8);
	  status = srt_uniform_I8((long long *)X, nR, (long long *)Y, shift, "asc"); 
	  cBYE(status);
	  break;
	default : 
	  go_BYE(-1);
	}
      }
      else if ( strcmp(distribution, "random") == 0 ) {
	char str_stride[BUFLEN];
	char str_start[BUFLEN];
	char str_nR2[BUFLEN];
	char str_num_bins[BUFLEN];

	int lb_fld_id; FLD_REC_TYPE lb_fld_rec; 
	int ub_fld_id; FLD_REC_TYPE ub_fld_rec;
	int nn_fld_id; FLD_REC_TYPE nn_fld_rec;

	char *lb_X = NULL; size_t lb_nX = 0;
	char *ub_X = NULL; size_t ub_nX = 0;

	int num_samples = 1048576;
	int stride = nR / num_samples;
	int num_bins = 9999;

	sprintf(str_stride, "%d", stride);
	sprintf(str_start,  "%d", 0);
	sprintf(str_nR2,    "%d", 1048576);
	status = ext_stride(tbl, fld, 0, str_stride, "__tempt", fld, str_nR2);
	cBYE(status);
	status = fop("__tempt", fld, "sortA"); cBYE(status);

	sprintf(str_num_bins,    "%d", num_bins);
	status = mk_bins("__tempt", "fld", "__bintbl", str_num_bins);
	cBYE(status);
	long long bin_nR; int bin_tbl_id; TBL_REC_TYPE bin_tbl_rec;
	status = is_tbl("__bintbl", &bin_tbl_id, &bin_tbl_rec); cBYE(status);
	if ( bin_tbl_id < 0 ) { go_BYE(-1); }
	bin_nR = bin_tbl_rec.nR;


	status = is_fld("__bintbl", -1, "lb", &lb_fld_id, &lb_fld_rec, 
			&nn_fld_id, &nn_fld_rec); 
	if ( lb_fld_id < 0 ) { go_BYE(-1); }
	status = get_data(lb_fld_rec, &lb_X, &lb_nX, false); cBYE(status);

	status = is_fld("__bintbl", -1, "ub", &ub_fld_id, &ub_fld_rec, 
			&nn_fld_id, &nn_fld_rec); 
	if ( ub_fld_id < 0 ) { go_BYE(-1); }
	status = get_data(ub_fld_rec, &ub_X, &ub_nX, false); cBYE(status);

	/* TODO P1 
	status = par_sort_with_bins(X, nR, lb_X, ub_X, bin_nR, 
	    fld_rec.fldtype, Y);
	cBYE(status);
	*/
      }
      else {
	go_BYE(-1);
      }
	fprintf(stderr, "Completed parallel sort \n");
	rs_munmap(X, nX); rs_munmap(Y, nY);
	/* TODO: P4: You are screwing around with meta data at a fairly 
	 * low level here.  Dangerous. Fix this some time */
	int old_ddir_id = fld_rec.ddir_id;
	int old_fileno  = fld_rec.fileno;
	status = q_delete(old_ddir_id, old_fileno); cBYE(status);
	g_flds[fld_id].ddir_id = ddir_id;
	g_flds[fld_id].fileno = fileno;
    }
    else {
      fprintf(stderr, "Not using par sort \n"); WHEREAMI; 
      if ( fld_rec.fldtype == I4 ) {
	// OLD qsort(X, nR, sizeof(int), sort_asc_I4);
	qsort_asc_I4(X, nR, sizeof(int), NULL);
      }
      else if ( fld_rec.fldtype == F4 ) { 
        qsort(X, nR, sizeof(int), sort_asc_F4);
      }
      else if ( fld_rec.fldtype == I8 ) { 
	// OLD qsort(X, nR, sizeof(long long), sort_asc_I8);
	qsort_asc_I8(X, nR, sizeof(long long), NULL);
      }
      else { 
	fprintf(stderr, "Not implemented\n"); go_BYE(-1);
      }
    }
    status = int_set_meta(tbl_id, fld_id, "srttype", "ascending", true);
    cBYE(status);
  }
  else if ( ( strcmp(str_op_spec, "sortD") == 0 ) || 
	    ( ( strcmp(op, "sort") == 0 ) && ( strcmp(srt_ordr, "dsc") == 0 ) ) ) {
    if ( fld_rec.fldtype == I4 ) { 
      // OLD qsort(X, nR, sizeof(int), sort_dsc_I4);
      qsort_dsc_I4(X, nR, sizeof(int), NULL);
    }
    else if ( fld_rec.fldtype == F4 ) { 
      qsort(X, nR, sizeof(float), sort_dsc_F4);
    }
    else if ( fld_rec.fldtype == I8 ) { 
      // OLD qsort(X, nR, sizeof(long long), sort_dsc_I8);
      qsort_dsc_I8(X, nR, sizeof(long long), NULL);
    }
    else { 
      fprintf(stderr, "Not implemented\n");
      go_BYE(-1);
    }
    status = int_set_meta(tbl_id, fld_id, "srttype", "descending", true);
    cBYE(status);
  }
  else if ( ( strcmp(str_op_spec, "permute") == 0 ) ||
	    ( strcmp(op, "permute") == 0 ) ) {
    switch ( fld_rec.fldtype ) { 
    case I4 : permute_I4((int *)X, nR);  break;
    case I8 : permute_I8((long long *)X, nR);  break;
    default : go_BYE(-1); break;
    }
    status = int_set_meta(tbl_id, fld_id, "srttype", "unsorted", true);
    cBYE(status);
  }
  else if ( strcmp(op, "zero_after_n") == 0 ) {
    long long one_cnt;
    status = chk_aux_info(str_op_spec); cBYE(status);
    status = extract_I8(str_op_spec, "limit=[", "]", &one_cnt, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( fld_rec.fldtype != B ) { go_BYE(-1); }
    if ( ( one_cnt < 1 ) || ( one_cnt >= nR ) ) { go_BYE(-1); }
    status = zero_after_n(X, nR, one_cnt); cBYE(status);
    rs_munmap(X, nX);
  }
  else if ( strcmp(op, "saturate") == 0 ) {
    long long maxval;
    status = chk_aux_info(str_op_spec); cBYE(status);
    status = extract_I8(str_op_spec, "maxval=[", "]", &maxval, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    switch ( fld_rec.fldtype ) { 
    case I1 : 
      I1ptr = (char *)X;
      if ( ( maxval < SCHAR_MIN ) || ( maxval > SCHAR_MAX ) ) { go_BYE(-1); }
      saturate_I1(I1ptr, nR, (char)maxval); cBYE(status);
      break;
    case I2 : 
      I2ptr = (short *)X;
      if ( ( maxval <  SHRT_MIN ) || ( maxval >  SHRT_MAX ) ) { go_BYE(-1); }
      saturate_I2(I2ptr, nR, (short)maxval); cBYE(status);
      break;
    case I4 : 
      I4ptr = (int *)X;
      if ( ( maxval <   INT_MIN ) || ( maxval >   INT_MAX ) ) { go_BYE(-1); }
      saturate_I4(I4ptr, nR, (int)maxval); cBYE(status);
      break;
    case I8 : 
      I8ptr = (long long *)X;
      if ( ( maxval < LLONG_MIN ) || ( maxval > LLONG_MAX ) ) { go_BYE(-1); }
      saturate_I8(I8ptr, nR, maxval); cBYE(status);
      break;
    default : go_BYE(-1); break; 
    }
    rs_munmap(X, nX);
  }
  else {
    go_BYE(-1);
  }
  //-------------------------------------------------------
 BYE:
  rs_munmap(X, nX);
  rs_munmap(Y, nY);
  return status ;
}
