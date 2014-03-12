#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "macros.h"
#include "qtypes.h"
#include "top_n.h"
#include "is_tbl.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_fld.h"
#include "add_fld.h"
#include "add_tbl.h"
#include "aux_meta.h"
#include "mk_temp_file.h"


/* helper for qsort() */
int compare (const void * a, const void * b)
{
  return ( *(long long*)a - *(long long*)b );
}

// START FUNC DECL
int 
ext_top_n(
   char *tbl,
   char *fld,
   char *cfld,
   char *outtbl,
   char *str_topnI4
   )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE outtbl_rec; int outtbl_id;
  TBL_REC_TYPE tbl_rec; int tbl_id;
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_REC_TYPE cfld_rec; int cfld_id;
  FLD_REC_TYPE out_val_fld_rec; int out_val_fld_id;
  FLD_REC_TYPE out_idx_fld_rec; int out_idx_fld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id;
  char *fld_X = NULL; size_t fld_nX = 0;
  char *nn_fld_X = NULL; size_t nn_fld_nX = 0;
  char *out_val_X = NULL; size_t out_val_nX = 0;
  char *out_idx_X = NULL; size_t out_idx_nX = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;
  int topnI4; int fldsz = 0; size_t filesz = 0;
  long long nR;
  int out_val_ddir_id = -1, out_val_fileno = -1;
  int out_idx_ddir_id = -1, out_idx_fileno = -1;
  char fk_fld[MAX_LEN_FLD_NAME+1];

  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( outtbl == NULL ) || ( *outtbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_topnI4 == NULL ) || ( *str_topnI4 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(tbl, outtbl) == 0 ) { go_BYE(-1); }
  status = stoI4(str_topnI4, &topnI4); cBYE(status);
  // TODO P4 Document below
  if ( ( topnI4 < 1 ) || ( topnI4 > 1024 ) )  { go_BYE(-1); } 

  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id,&nn_fld_rec);
  cBYE(status);
  if ( fld_id < 0 ) { go_BYE(-1); }
  status = get_data(fld_rec, &fld_X, &fld_nX, false); cBYE(status);
  if ( fld_rec.fldtype != I4 ) { go_BYE(-1); }
  // Get nn field for f1 if if it exists
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_fld_X, &nn_fld_nX, false); 
    cBYE(status);
    if ( ( cfld == NULL ) && ( *cfld == '\0' ) ) { go_BYE(-1); }
    cfld_X = nn_fld_X;
  } 
  else {
    if ( ( cfld == NULL ) && ( *cfld == '\0' ) ) { 
      status = is_fld(NULL, tbl_id, cfld, &cfld_id, &cfld_rec, 
	  &nn_cfld_id,&nn_cfld_rec);
      cBYE(status);
      if ( cfld_id < 0 ) { go_BYE(-1); }
      status = get_data(cfld_rec, &cfld_X, &cfld_nX, false); cBYE(status);
      if ( cfld_rec.fldtype != I1 ) { go_BYE(-1); }
    }
    else {
      cfld_X = NULL;
    }
  }
  zero_string(fk_fld, MAX_LEN_FLD_NAME+1);

  status = add_tbl(outtbl, str_topnI4, &outtbl_id, &outtbl_rec); cBYE(status);
  /* what we are creating is an index in outtbl, Hence I8 */
  zero_fld_rec(&out_idx_fld_rec); 
  out_idx_fld_rec.fldtype = I8; 
  status = get_fld_sz(out_idx_fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = topnI4 * fldsz;
  status = mk_temp_file(filesz, &out_idx_ddir_id, &out_idx_fileno); cBYE(status);
  status = q_mmap(out_idx_ddir_id, out_idx_fileno, &out_idx_X, &out_idx_nX, true); 
  cBYE(status);
  if ( ( strlen(tbl) + strlen("fk_") ) >= MAX_LEN_FLD_NAME ) { go_BYE(-1); }
  strcpy(fk_fld, "fk_"); strcat(fk_fld, tbl);
  status = add_fld(outtbl_id, fk_fld, out_idx_ddir_id, out_idx_fileno, 
      &out_idx_fld_id, &out_idx_fld_rec);
  cBYE(status);

  /* Now we create the values that are the top N */
  zero_fld_rec(&out_val_fld_rec); 
  out_val_fld_rec.fldtype = fld_rec.fldtype;
  status = get_fld_sz(out_val_fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = topnI4 * fldsz;
  status = mk_temp_file(filesz, &out_val_ddir_id, &out_val_fileno); cBYE(status);
  status = q_mmap(out_val_ddir_id, out_val_fileno, &out_val_X, &out_val_nX, true); 
  cBYE(status);
  status = add_fld(outtbl_id, fld, out_val_ddir_id, out_val_fileno, 
      &out_val_fld_id, &out_val_fld_rec);
  cBYE(status);

  status = top_n((int *)fld_X, (char *)cfld_X, nR, topnI4, 
      (long long *)out_idx_X, (long long *)out_val_X);
  cBYE(status);

BYE:
  rs_munmap(fld_X, fld_nX);
  rs_munmap(out_idx_X, out_idx_nX);
  rs_munmap(out_val_X, out_val_nX);
  return(status);
}


// START FUNC DECL
int 
top_n (
       int * x,
       char *cfld, 
       long long siz,
       int n,
       long long * idx_out,
       long long * val_out 
       )
// STOP FUNC DECL
//-----------------------------------------------------------------------
/* README:

top_n: takes as input an array "x" of "siz" elements, also takes as input an integer "n", and outputs a set "idx_out" which contains the indices of the top - n values in x (the indices will be sorted in increasing order). 

For ex, say n=3 and the top 3 values were x[27], x[10] and x[36] respectively. idx_out would be {10,27,36} (NOTE: idx_out will always be in increasing order, so the first element of idx_out does not necessarily point to the top value)

INPUTS:

x: input array

cfld: optional character array such that cfld[i] = 0 => skip; else, use value.
cfld = NULL => all input values are defined

siz: number of elements in the input array

n: locations of the top n values desired as output (not necessarily in sorted order, i.e., the first location need not point to the top value, see example above).

OUTPUTS:

idx_out: contains the indices where the top-n values are located in the input ( idx_out will be sorted)

NOTE: idx_out has to be pre-malloced with n * (long long) size 

Return values:
0: computations successful
-1: something fundamentally wrong

 */
//-----------------------------------------------------------------------
{
  int status = 0;  

  int       * temp_top_val = NULL; 
  long long * temp_idx_out = NULL; /* temporary arrays */

  int temp;

  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( idx_out == NULL ) { go_BYE(-1); }
  if ( ( siz <= 0 ) || ( n <= 0 ) || ( n > siz-1 ) ) { go_BYE(-1); }

  int m = n;
  if ( n % 2 == 0 ) { m++; }
  /* binary min heap will be implemented later. Need the total number of nodes to be odd so that every one has either no child or exactly 2 children (makes coding easier). So, if n is even, we compute the top n+1 values and return only top n.*/
   
  temp_top_val = malloc ( m * sizeof(int) );
  return_if_malloc_failed(temp_top_val);
  temp_idx_out = malloc ( m * sizeof(long long) );
  return_if_malloc_failed(temp_idx_out);

  for ( int jj = 0; jj < m; jj++ ) {
    temp_top_val[jj]      = INT_MIN; // minimum possible 4 byte integer
    temp_idx_out[jj] = -1;
  }

  /*  min-heap to obtain top-m (we have ensured m to be odd for coding conenvience) quickly */

  for ( long long ii = 0; ii < siz ; ii++ ) {

    if ( ( cfld != NULL ) && ( cfld[ii] == 0 ) ) { continue; }
    int kk = 0;

    /* if the current value is greater than the minimum value stored by our top-m array */
    if ( x[ii] >= temp_top_val[0] ) {
      
      temp_top_val[0] = x[ii];
      temp_idx_out[0] = ii;

      while ( 2*kk+2 < m ) {

	/* maintaining min-heap property */
	if ( ( temp_top_val[kk] > temp_top_val[2*kk+1] ) && 
	     ( temp_top_val[2*kk+2] >= temp_top_val[2*kk+1] ) ) {

	  temp = temp_top_val[kk];
	  temp_top_val[kk] = temp_top_val[2*kk+1];
	  temp_top_val[2*kk+1] = temp;

	  temp = temp_idx_out[kk];
	  temp_idx_out[kk] = temp_idx_out[2*kk+1];
	  temp_idx_out[2*kk+1] = temp;

	  kk = 2*kk+1;

	}

	else if ( ( temp_top_val[kk] > temp_top_val[2*kk+2] ) && 
	    ( temp_top_val[2*kk+1] >= temp_top_val[2*kk+2] ) ) {

	  temp = temp_top_val[kk];
	  temp_top_val[kk] = temp_top_val[2*kk+2];
	  temp_top_val[2*kk+2] = temp;

	  temp = temp_idx_out[kk];
	  temp_idx_out[kk] = temp_idx_out[2*kk+2];
	  temp_idx_out[2*kk+2] = temp;

	  kk = 2*kk+2;

	}

	else
	  break;

      }

      
    }
 

  }

  if ( n % 2 == 0 ) {

    /* finding the minimum value stored in top_val and removing it if n was even originally, then printing the remaining values from heap */

    int min_val = INT_MIN;
    int min_val_loc= -1;

    for ( int jj = 0; jj < m; jj++) {
      if ( temp_top_val[jj] <=  min_val ) { 
	min_val = temp_top_val[jj];
	min_val_loc = jj;
      }
    }
    cBYE(min_val_loc);

    /* storing the top n locations in idx_out */

    for ( int jj = 0; jj < min_val_loc ; jj++ ) {
      idx_out[jj] = temp_idx_out[jj];
    }
    for ( int jj = min_val_loc+1; jj < m; jj++ ) {
      idx_out[jj-1] = temp_idx_out[jj];
    }

  }
  else {
    
    /* storing the top n locations in idx_out */

    for ( int jj = 0; jj < n ; jj++ ) {
      idx_out[jj] = temp_idx_out[jj];
      val_out[jj] = temp_top_val[jj];
    }

  }
  /* for debugging only 
  for ( int jj = 0; jj < n; jj ++ ) {
    printf("%lld\n", idx_out[jj]);
  }
  */

BYE:
  free_if_non_null (temp_idx_out);
  free_if_non_null (temp_top_val);
  return (status);
}
