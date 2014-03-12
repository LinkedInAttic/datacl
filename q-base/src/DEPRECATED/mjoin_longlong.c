#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "bin_search.h"
#include "assign_I1.h"
#include "mjoin_longlong.h"
#include "sort2_asc_longlong.h"

int
mjoin_longlong(
	  char *src_fld_X,
	  char *src_lnk_X,
	  char *dst_lnk_X,
	  char *nn_src_fld_X,
	  char *nn_src_lnk_X,
	  char *nn_dst_lnk_X,
	  long long src_nR,
	  long long dst_nR,
	  bool *ptr_is_all_def,
	  char *str_meta_data,
	  char **ptr_nn_opfile,
	  int imjoin_op
	  )
{
  int status = 0;
  long long nn_src_nR, nn_dst_nR;
  long long src_idx_val, dst_lnk_idx;
  LONGLONG_LONGLONG_TYPE *src_pair = NULL, *dst_pair = NULL;
  long long *ll_src_fld_ptr = NULL;
  long long *dst_fld_val = NULL; char *nn_dst_fld_val = NULL;
  bool is_all_def = true; // assume no null values
  int src_marker;
  FILE *ofp = NULL, *nn_ofp = NULL;
  char *opfile = NULL, *nn_opfile = NULL;
  long long dst_lnk_val, src_lnk_val, ll_src_fld_val;
  bool found;
  long long *ll_dst_lnk_ptr = NULL, *ll_src_lnk_ptr = NULL;
  char *temp_src_X = NULL; size_t temp_src_nX = 0;
  char *temp_dst_X = NULL; size_t temp_dst_nX = 0;

  status = open_temp_file(&ofp, &opfile, 0); cBYE(status);
  status = open_temp_file(&nn_ofp, &nn_opfile, 0); cBYE(status);
  //--------------------------------------------------------
  nn_dst_fld_val = (char *)malloc(dst_nR * sizeof(char));
  return_if_malloc_failed(nn_dst_fld_val);
  if ( nn_dst_lnk_X == NULL ) { 
    assign_const_I1(nn_dst_fld_val, dst_nR, 0);
  }
  else {
    is_all_def = false;
    assign_I1(nn_dst_fld_val, nn_dst_lnk_X, dst_nR);
  }
  //--------------------------------------------------------
  status = collect_lnk_idx_vals_longlong(
      src_lnk_X, nn_src_lnk_X, nn_src_fld_X, src_nR, 
      dst_lnk_X, nn_dst_lnk_X,               dst_nR, 
      &src_pair, &nn_src_nR, &dst_pair, &nn_dst_nR,
      &temp_src_X, &temp_src_nX, &temp_dst_X, &temp_dst_nX);
  cBYE(status);

  ll_dst_lnk_ptr = (long long *)dst_lnk_X;
  ll_src_lnk_ptr = (long long *)src_lnk_X;
  ll_src_fld_ptr = (long long *)src_fld_X;
  dst_fld_val = (long long *)malloc(dst_nR * sizeof(long long));
  return_if_malloc_failed(dst_fld_val);
  for ( long long i = 0; i < dst_nR; i++ ) {
    nn_dst_fld_val[i] = FALSE;
    switch ( imjoin_op ) { 
    case MJOIN_OP_REG : 
      dst_fld_val[i] = 0;
      break;
    case MJOIN_OP_SUM : 
      dst_fld_val[i] = 0;
      break;
    case MJOIN_OP_MIN : 
      dst_fld_val[i] = LLONG_MAX;
      break;
    case MJOIN_OP_MAX : 
      dst_fld_val[i] = LLONG_MIN;
      break;
    case MJOIN_OP_OR : 
      dst_fld_val[i] = 0;
      break;
    case MJOIN_OP_AND : 
      dst_fld_val[i] = 0xFFFFFFFFFFFFFFFF;
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  src_marker = 0;
  for ( long long i = 0; i < nn_dst_nR; i++ ) {
    dst_lnk_val = dst_pair[i].lnk_val;
    dst_lnk_idx = dst_pair[i].idx_val;
    if ( ( dst_lnk_idx < 0 ) || ( dst_lnk_idx >= dst_nR ) ) { go_BYE(-1); }
    found = false;
    /* Advance src marker until src_lnk_val matches dst_lnk_val */
    for ( ; src_marker < nn_src_nR; src_marker++ ) {
      src_lnk_val = src_pair[src_marker].lnk_val;
      if ( src_lnk_val == dst_lnk_val ) {
	found = true;
	break;
      }
      else if ( src_lnk_val > dst_lnk_val ) {
	found = false;
	break;
      }
    }
    if ( found == true ) { 
      nn_dst_fld_val[dst_lnk_idx] = TRUE;
      for ( ; src_marker < src_nR; src_marker++ ) { 
        src_lnk_val = src_pair[src_marker].lnk_val;
        src_idx_val = src_pair[src_marker].idx_val;
        if ( ( src_idx_val < 0 ) || ( src_idx_val >= src_nR ) ) { go_BYE(-1); }
	if ( src_lnk_val != dst_lnk_val ) {
	  break;
	}
	ll_src_fld_val = ll_src_fld_ptr[src_idx_val];
	switch ( imjoin_op ) { 
	case MJOIN_OP_SUM : 
	  dst_fld_val[dst_lnk_idx] += ll_src_fld_val;
	  break;
	case MJOIN_OP_MAX : 
	  if ( ll_src_fld_val > dst_fld_val[dst_lnk_idx] ) { 
	    dst_fld_val[dst_lnk_idx] = ll_src_fld_val;
	  }
	  break;
	case MJOIN_OP_MIN : 
	  if ( ll_src_fld_val < dst_fld_val[dst_lnk_idx] ) { 
	    dst_fld_val[dst_lnk_idx] = ll_src_fld_val;
	  }
	  break;
	case MJOIN_OP_OR : 
          dst_fld_val[dst_lnk_idx] = dst_fld_val[dst_lnk_idx] | ll_src_fld_val;
	  break;
	case MJOIN_OP_AND : 
          dst_fld_val[dst_lnk_idx] = dst_fld_val[dst_lnk_idx] & ll_src_fld_val;
	  break;
	case MJOIN_OP_REG : 
          dst_fld_val[dst_lnk_idx] = ll_src_fld_val;
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
    }
    else {
      is_all_def = false;
      nn_dst_fld_val[dst_lnk_idx] = FALSE;
    }
    if ( imjoin_op == MJOIN_OP_REG ) { // TODO: Document reason for this
      src_marker--;
      if ( src_marker < 0 ) { src_marker = 0; }
    }
  }
  strcat(str_meta_data, opfile);
  fwrite(dst_fld_val, dst_nR, sizeof(long long), ofp);
  fclose_if_non_null(ofp); // primary field
  fwrite(nn_dst_fld_val, dst_nR, sizeof(char), nn_ofp);
  fclose_if_non_null(nn_ofp); // auxiliary field 

  *ptr_is_all_def = is_all_def;
  *ptr_nn_opfile = nn_opfile;
 BYE:
  rs_munmap(temp_src_X, temp_src_nX);
  rs_munmap(temp_dst_X, temp_dst_nX);
  free_if_non_null(dst_fld_val);
  free_if_non_null(nn_dst_fld_val);

  free_if_non_null(opfile);
  fclose_if_non_null(ofp);

  fclose_if_non_null(nn_ofp);

  return(status);
}

int
collect_lnk_idx_vals_longlong(
    char *src_lnk_X,
    char *nn_src_lnk_X,
    char *nn_src_fld_X,
    long long src_nR,
    char *dst_lnk_X,
    char *nn_dst_lnk_X,
    long long dst_nR,
      LONGLONG_LONGLONG_TYPE **ptr_src_pair,
      long long *ptr_nn_src_nR,
      LONGLONG_LONGLONG_TYPE **ptr_dst_pair,
      long long *ptr_nn_dst_nR,
      char **ptr_temp_src_X,
      size_t *ptr_temp_src_nX,
      char **ptr_temp_dst_X,
      size_t *ptr_temp_dst_nX
      )
  {
    int status = 0;
    long long nn_src_nR, nn_dst_nR;
    LONGLONG_LONGLONG_TYPE *src_pair = NULL, *dst_pair = NULL;
  int rec_size = sizeof(long long) + sizeof(long long);
  char *temp_src_file = NULL, *temp_dst_file = NULL;
  FILE *tfp = NULL;
  long long *ll_dst_lnk_ptr = NULL, *ll_src_lnk_ptr = NULL;
  char *temp_src_X = NULL; size_t temp_src_nX = 0;
  char *temp_dst_X = NULL; size_t temp_dst_nX = 0;

  /* START: Collect nn source values and sort them on link field */
  status = open_temp_file(&tfp, &temp_src_file, 0);
  cBYE(status);
  nn_src_nR = 0;
  ll_src_lnk_ptr = (long long*)src_lnk_X;
  for ( long long i = 0; i < src_nR; i++ ) { 
    if ( ( nn_src_fld_X != NULL ) && ( nn_src_fld_X[i] == FALSE ) ) { continue; }
    if ( ( nn_src_lnk_X != NULL ) && ( nn_src_lnk_X[i] == FALSE ) ) { continue; }
    nn_src_nR++;
    fwrite(ll_src_lnk_ptr+i, 1, sizeof(long long), tfp);
    fwrite(&i, 1, sizeof(long long), tfp);
  }
  fclose_if_non_null(tfp);
  if ( nn_src_nR == 0 ) { /* No values to process */
    go_BYE(-1);
  }
  status = rs_mmap(temp_src_file, &temp_src_X, &temp_src_nX, 1); 
  cBYE(status);
  if ( ( ( temp_src_nX/ sizeof(LONGLONG_LONGLONG_TYPE) ) * sizeof(LONGLONG_LONGLONG_TYPE) ) != temp_src_nX) {
    go_BYE(-1);
  }
  qsort(temp_src_X, nn_src_nR, rec_size, sort2_asc_longlong);
  src_pair = (LONGLONG_LONGLONG_TYPE *)temp_src_X;
  /* STOP: Collect nn source values and sort them on link field */
  /*------------------------------------------------*/
  /* START: Sort destination link fields */
  status = open_temp_file(&tfp, &temp_dst_file, 0);
  cBYE(status);
  nn_dst_nR = 0;
  ll_dst_lnk_ptr = (long long *)dst_lnk_X;
  for ( long long  i = 0; i < dst_nR; i++ ) { 
    if ( ( nn_dst_lnk_X != NULL ) && ( nn_dst_lnk_X[i] == FALSE ) ) { continue; }
    nn_dst_nR++;
    fwrite(ll_dst_lnk_ptr+i, 1, sizeof(long long), tfp);
    fwrite(&i, 1, sizeof(long long), tfp);
  }
  fclose_if_non_null(tfp);
  if ( nn_dst_nR == 0 ) { /* No values to process */
    go_BYE(-1);
  }
  status = rs_mmap(temp_dst_file, &temp_dst_X, &temp_dst_nX, 1); 
  cBYE(status);
  if ( ( ( temp_dst_nX/ sizeof(LONGLONG_LONGLONG_TYPE) ) * sizeof(LONGLONG_LONGLONG_TYPE) ) != temp_dst_nX) {
    go_BYE(-1);
  }
  qsort(temp_dst_X, nn_dst_nR, rec_size, sort2_asc_longlong);
  dst_pair = (LONGLONG_LONGLONG_TYPE *)temp_dst_X;
  /*------------------------------------------------*/
  /* STOP : Sort destination link fields */
  //--------------------------------------------------------
  *ptr_nn_src_nR = nn_src_nR;
  *ptr_nn_dst_nR = nn_dst_nR;

  *ptr_src_pair = src_pair;
  *ptr_dst_pair = dst_pair;
  *ptr_temp_src_X = temp_src_X;
  *ptr_temp_src_nX = temp_src_nX;
  *ptr_temp_dst_X = temp_dst_X;
  *ptr_temp_dst_nX = temp_dst_nX;
BYE:
  unlink(temp_src_file);
  unlink(temp_dst_file);
  free_if_non_null(temp_src_file);
  free_if_non_null(temp_dst_file);
  return(status);
  }
