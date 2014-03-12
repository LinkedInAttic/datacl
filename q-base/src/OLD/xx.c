#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "bin_search.h"
#include "sort.h"
#include "mjoin_long_long.h"

typedef struct longlong_int_type {
  long long lnk_val;
  int idx_val;
} LONGLONG_INT_TYPE;

int
mjoin_long_long(
	  char *src_fld_X,
	  char *src_lnk_X,
	  char *dst_lnk_X,
	  char *nn_src_fld_X,
	  char *nn_src_lnk_X,
	  char *nn_dst_lnk_X,
	  int src_nR,
	  int dst_nR,
	  bool *ptr_is_any_null,
	  char *str_meta_data,
	  char **ptr_nn_opfile,
	  int imjoin_op
	  )
{
  int status = 0;
  FILE *tfp = NULL; 
  char *temp_src_file = NULL, *temp_dst_file = NULL;
  int nn_src_nR, nn_dst_nR;
  int src_idx_val, dst_lnk_idx;
  long long *ll_dst_lnk_ptr = NULL, *ll_src_lnk_ptr = NULL;
  char *temp_src_X = NULL; size_t temp_src_nX = 0;
  char *temp_dst_X = NULL; size_t temp_dst_nX = 0;
  LONGLONG_INT_TYPE *src_pair = NULL, *dst_pair = NULL;
  long long *ll_src_fld_ptr = NULL;
  long long *dst_fld_val = NULL; char *nn_dst_fld_val = NULL;
  bool is_any_null = false; // assume no null values
  int src_marker;
  FILE *ofp = NULL, *nn_ofp = NULL;
  char *opfile = NULL, *nn_opfile = NULL;
  long long dst_lnk_val, src_lnk_val, ll_src_fld_val;
  bool found;
  int rec_size = sizeof(long long) + sizeof(int);

  status = open_temp_file(&ofp, &opfile);
  cBYE(status);
  status = open_temp_file(&nn_ofp, &nn_opfile);
  cBYE(status);
  //--------------------------------------------------------
  nn_dst_fld_val = (char *)malloc(dst_nR * sizeof(char));
  return_if_malloc_failed(nn_dst_fld_val);
  //--------------------------------------------------------
  /* START: Collect nn source values and sort them on link field */
  status = open_temp_file(&tfp, &temp_src_file);
  cBYE(status);
  nn_src_nR = 0;
  ll_src_lnk_ptr = (long long*)src_lnk_X;
  for ( int i = 0; i < src_nR; i++ ) { 
    if ( ( nn_src_fld_X != NULL ) && ( nn_src_fld_X[i] == FALSE ) ) { continue; }
    if ( ( nn_src_lnk_X != NULL ) && ( nn_src_lnk_X[i] == FALSE ) ) { continue; }
    nn_src_nR++;
    fwrite(ll_src_lnk_ptr+i, 1, sizeof(long long), tfp);
    fwrite(&i, 1, sizeof(int), tfp);
  }
  fclose_if_non_null(tfp);
  if ( nn_src_nR == 0 ) { /* No values to process */
    go_BYE(-1);
  }
  status = rs_mmap(temp_src_file, &temp_src_X, &temp_src_nX, 1); 
  cBYE(status);
  qsort(temp_src_X, nn_src_nR, rec_size, sort_Li_aa_compare);
  src_pair = (LONGLONG_INT_TYPE *)temp_src_X;
  /* STOP: Collect nn source values and sort them on link field */
  /*------------------------------------------------*/
  /* START: Sort source and destination link fields */
  status = open_temp_file(&tfp, &temp_dst_file);
  cBYE(status);
  nn_dst_nR = 0;
  ll_dst_lnk_ptr = (long long *)dst_lnk_X;
  for ( int i = 0; i < dst_nR; i++ ) { 
    if ( ( nn_dst_lnk_X != NULL ) && ( nn_dst_lnk_X[i] == FALSE ) ) { continue; }
    nn_dst_nR++;
    fwrite(ll_dst_lnk_ptr+i, 1, sizeof(long long), tfp);
    fwrite(&i, 1, sizeof(int), tfp);
  }
  fclose_if_non_null(tfp);
  if ( nn_dst_nR == 0 ) { /* No values to process */
    go_BYE(-1);
  }
  status = rs_mmap(temp_dst_file, &temp_dst_X, &temp_dst_nX, 1); 
  cBYE(status);
  qsort(temp_dst_X, nn_dst_nR, rec_size, sort_Li_aa_compare);
  dst_pair = (LONGLONG_INT_TYPE *)temp_dst_X;
  /*------------------------------------------------*/
  /* STOP : Sort source and destination link fields */
  //--------------------------------------------------------
  ll_dst_lnk_ptr = (long long *)dst_lnk_X;
  ll_src_lnk_ptr = (long long *)src_lnk_X;
  ll_src_fld_ptr = (long long *)src_fld_X;
  dst_fld_val = (long long *)malloc(dst_nR * sizeof(long long));
  return_if_malloc_failed(dst_fld_val);
  for ( int i = 0; i < dst_nR; i++ ) {
    nn_dst_fld_val[i] = FALSE;
    switch ( imjoin_op ) { 
    case MJOIN_OP_SUM : 
      dst_fld_val[i] = 0;
      break;
    case MJOIN_OP_MIN : 
      dst_fld_val[i] = INT_MAX;
      break;
    case MJOIN_OP_MAX : 
      dst_fld_val[i] = INT_MIN;
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  src_marker = 0;
  for ( int i = 0; i < nn_dst_nR; i++ ) {
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
	default : 
	  go_BYE(-1);
	  break;
	}
      }
    }
    else {
      is_any_null = true;
      nn_dst_fld_val[dst_lnk_idx] = FALSE;
    }
  }
  strcat(str_meta_data, opfile);
  fwrite(dst_fld_val, dst_nR, sizeof(long long), ofp);
  fclose_if_non_null(ofp); // primary field
  fwrite(nn_dst_fld_val, dst_nR, sizeof(char), nn_ofp);
  fclose_if_non_null(nn_ofp); // auxiliary field 

  *ptr_is_any_null = is_any_null;
  *ptr_nn_opfile = nn_opfile;
 BYE:
  rs_munmap(temp_src_X, temp_src_nX);
  rs_munmap(temp_dst_X, temp_dst_nX);
  unlink(temp_src_file);
  unlink(temp_dst_file);
  free_if_non_null(temp_src_file);
  free_if_non_null(temp_dst_file);
  free_if_non_null(dst_fld_val);
  free_if_non_null(nn_dst_fld_val);

  free_if_non_null(opfile);
  fclose_if_non_null(ofp);

  fclose_if_non_null(nn_ofp);

  return(status);
}
