#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <math.h>
#include "spooky_hash.h"
#include "macros.h"
#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "dbauxil.h"
#include "assign_I4.h"
#include "qsort_asc_I4.h"
#include "determine_rho_loc.h"
#include "approx_unique.h"
// #include <malloc.h>

#ifdef IPP
#include "ipp.h"
#include "ippi.h"
#endif

// START FUNC DECL
int 
ext_approx_unique(
   char *tbl,
   char *fld,
   char *cfld,
   long long *ptr_num_unique,
   double *ptr_estimate_accuracy_percent,
   int *ptr_estimate_is_good  
   )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id;
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_REC_TYPE cfld_rec; int cfld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id;
  char *fld_X = NULL; size_t fld_nX = 0;
  char *nn_fld_X = NULL; size_t nn_fld_nX = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;
  long long nR;

  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }

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
  status = approx_unique((int *)fld_X, (char *)cfld_X, nR, ptr_num_unique,
   ptr_estimate_accuracy_percent, ptr_estimate_is_good  );
  cBYE(status);

BYE:
  rs_munmap(fld_X, fld_nX);
  return status ;
}

// START FUNC DECL
int 
approx_unique (
	       int *x,
	       char *cfld,
	       long long siz,
	       long long *y,
	       double *ptr_estimate_accuracy_percent,
	       int *ptr_estimate_is_good 
	       )
// STOP FUNC DECL
//----------------------------------------------------------------------------
/* README: 

status = approx_unique(x,cfld,siz,y,ptr_estimate_accuracy_percent,ptr_estimate_is_good): Calculates the cardinality (number of unique elements) of an integer set approximately using very little memory. The percentage accuracy in estimation (the approximation basically) will be stored in ptr_estimate_accuracy_percent. 

The approximation percentages observed during extensive testing are roughly as follows:
(i) Data sets with cardinality  > 200,000 will be estimated within 1% accuracy
(ii) Data sets with cardinality > 100,000 will be estimated with 2-3% accuracy
(iii) Data sets with cardinality > 10,000 will be estimated with 6% accuracy
(iv) Data sets with cardinality > 1,000 will be estimated with 30% accuracy
(v) Data sets with cardinality < 1,000: the algorithm will quit setting ptr_estimate_accuracy_percent to 100 and ptr_estimate_is_good will have a value -2.

Author: Kishore Jaganathan

Algorithm: Hyperloglog by Philippe Flajolet 

INPUTS: 

x: array containing the input data to be processed.

cfld: two options - (1) NULL: All elements of x are processed.
(2) non-NULL: Array of same size as x. Acts as a select vector (only those elements with non-zero values in cfld are processed). ex: If x has 10 elements and cfld is {0,0,1,0,0,0,1,0,1,0}, then only the 3rd, 7th and 9th element are chosen for processing.

siz: Number of elements in the input array x. 

OUTPUTS: 

y: Pointer to the location where the calculated cardinality of x will be stored.

ptr_estimate_accuracy_percent: Pointer to the location where the percentage error in the calculated cardinality of x will be stored. ex: 1.22 => +/-1.22% (3 sigma rule used)

ptr_estimate_is_good: Pointer to a location where 1,-1,-2 or -3  will be stored. Use this as a sanity check: 1 is good, everything else is bad.
(1) 1: Hyperloglog algorithm successfully calculated cardinality. Outputs *y, *ptr_estimate_accuracy are reliable/ accurate with very high probability (99%). 
(2) -1: Hyperloglog algorithm failed to calculate the cardinality due to error in inputs (some of them NULL maybe). *y will be set to -1 and *ptr_estimate_accuracy_percent will be set to 100%. 
(3) -2: Hyperloglog algorithm failed to calculate cardinality as input cardinality too small for Hyperloglog to work. I would suggest using (key, value) storage based sorting using binary search trees (example: std::maps in C++, levelDB etc) due to low cardinality. *y will be set to -1 and *ptr_estimate_accuracy_percent will be set to 100%
(4) -3: Hyperloglog algorithm failed to calculate cardinality due to one of the two cases: (a) The odd chance that the hash function produced a value which is divisible by 2^44. This will happen very very very rarely unless you are dealing with datasets with more than 100 billion unique elements, just retrying the code would help solve the issue. (b) The dataset indeed has more than 100 billion unique elements. In that case, you need to use a 128 bit hash function (like "spooky_hash128"). Edit this code and determine_rho_loc code accordingly. Make sure you back up this code before editing. *y will be set to -1 and *ptr_estimate_accuracy_percent will be set to 100%.

status: Takes values 0 or -1. 
0: The algorithm either computed the cardinality and set  *ptr_estimate_is_good to 1 (and ptr_estimate_accuracy_percent to whatever % the accuracy is) or the computations were not possible and set *ptr_estimate_is_good to -2 (low cardinality) or -3 (extremely high cardinality or hash function gave bad outputs). 
-1: Something wrong with the inputs, *ptr_estimate_is_good will be set to -1.

*/
//----------------------------------------------------------------------------
{

  int status = 0;

  int flag = 0;

  *y = -1;
  *ptr_estimate_accuracy_percent = 100;
  *ptr_estimate_is_good = -1; /* default */

  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( y == NULL ) { go_BYE(-1); }
  if ( ptr_estimate_accuracy_percent == NULL ) { go_BYE(-1); }
  if ( ptr_estimate_is_good == NULL ) { go_BYE(-1); }
  if ( siz <= 0 ) { go_BYE(-1); }

  //--------------------------------------------------------------------------

  int m = 65536;      
  if ( (m & (m-1)) != 0 || m < 128 || m > (1<<20) ) { go_BYE(-1); }
  /* m is the number of bins used by hyperloglog. Calculations in this work assume m to be power of 2 and 128 <= m <= 2^20 for convenience. Unless you know what you are doing and it's impact on other lines of this code (incl. determine_rho_loc function), PLEASE DONOT VIOLATE THIS CONDITION.*/

  /* NOTE: Higher m guarantees lesser error (less than {3 * 1.04/sqrt(m)} % with 99% probability). This would mean that m = Inf would give us 0 error. The catch is that hyperloglog algorithm assumes that all bins are used by atleast some input elements (to ensure stochastic averaging). Typical thumb rule for  m: the cardinality of the data set has to be atleast around 3-5 times m (ideally m log(m) so that all the bins are used sufficiently). This can't be checked beforehand as we don't know the cardinality (that's what we are trying to estimate here). The strategy we'll be using to overcome this: Start with a high m (65536 works well: +/- 1.22%). If some bins are unused, keep reducing m by a factor of 2 by combining neigbhoring bins (to maintain independence) till all the bins are used sufficiently. */

#define BLOCK_SIZE 1048576   
  /* Data will be processed in blocks to promote parallel processing */

  int num_blocks = siz / BLOCK_SIZE;
  if ( ( num_blocks * BLOCK_SIZE ) != siz ) {
    num_blocks++;
  }   /* number of blocks in the input data */

  int *max_rho = NULL;  /* stores maximum rho value for each bin */
  uint64_t seed = time(NULL); /* seed for the hashfunction */
  uint64_t *hashval = NULL; /*  64 bit hashvalues stored for a block of data */
  
  flag = 1; /* max_rho and hashval are defined */

  max_rho = malloc( m * sizeof(int) );
  return_if_malloc_failed(max_rho);
#ifdef IPP
  ippsZero_32s((int *)max_rho,m);
#else
  assign_const_I4(max_rho,m,0);
#endif

  hashval = malloc( BLOCK_SIZE * sizeof(uint64_t) );
  return_if_malloc_failed(hashval);
  for ( int ii = 0; ii < BLOCK_SIZE; ii++ ) { 
    hashval[ii] = 0;
  }

  //-------------------------------------------------------------------------

  /* For each element in the input data stream: 
     (1) calculate hash value using spooky_hash64 (or any other hashfunction)
     (2) determine rho and loc (bin id) values using determine_rho_loc 
     (3) update max_rho accordingly 
     Steps (1) and (2) are highly parallelizable, data processed in blocks to leverage this. For some reason, parallelizing (2) is slowing down -- so not doing it. */

 
  for ( int c = 0; c < num_blocks; c++ ) { 


    /* calculating block boundaries */
    long long lb = c * BLOCK_SIZE;
    long long ub = lb + BLOCK_SIZE;
    if ( ub > siz )  { ub = siz; }     

    if ( cfld == NULL ) {
      //--------------------------------------------------------------------
      /* selecting all entries of x as inputs
	 compute hashvalues of blocks in parallel */
      cilkfor ( long long ii = 0 ; ii < (ub-lb) ; ii++ ) {

	int * t_value = &x[lb+ii];
	hashval[ii] = spooky_hash64((void *)t_value, sizeof(int), seed);
	
      }

      /* computing rho and loc for each hashval, update max_rho */

      for ( long long ii = 0; ii < (ub-lb) ; ii++ ) {

	int loc = 0, rho = 0;
	status = determine_rho_loc(hashval[ii],m,&rho,&loc);
	
	if ( status == -1 ) {
	  *ptr_estimate_is_good = -3;
	  *ptr_estimate_accuracy_percent = 100;
	  *y = -1;
	  go_BYE(0);
	}

	if ( max_rho[loc] < rho ) { 
	  max_rho[loc] = rho;
	}

      }
      //--------------------------------------------------------------------
    }
    else {
      //--------------------------------------------------------------------
      /* select only those entries with non-zero cfld
         compute hashvalues of blocks in parallel */
      cilkfor ( long long ii = 0 ; ii < (ub-lb) ; ii++ ) {

	if ( cfld[lb+ii] == 0 ) { continue; }

	int * t_value = &x[lb+ii];
	hashval[ii] = spooky_hash64((void *)t_value, sizeof(int), seed);

      }

      /* computing rho and loc for each hashval, update max_rho */

      for ( long long ii = 0; ii < (ub-lb) ; ii++ ) {

	if ( cfld[lb+ii] == 0 ) { continue; }

	int loc = 0, rho = 0;
	status = determine_rho_loc(hashval[ii], m, &rho, &loc); 

	if ( status == -1 ) {
	  *ptr_estimate_is_good = -3;
	  *ptr_estimate_accuracy_percent = 100;
	  *y = -1;
	  go_BYE(0);
	}

	if ( max_rho[loc] < rho ) { 
	  max_rho[loc] = rho;
	}

      }

      //--------------------------------------------------------------------
    }


  }

  //-------------------------------------------------------------------------

  /* If some max_rho's are zero, it means that all bins were not used sufficiently and hence stochastic averaging (law of large numbers, assumed by hyperloglog) was not done properly (due to using too many bins for too few data). In such a situation, bins will be merged here. */

  /* Bin merging rule (change only if you know what you are doing):
     While ( 1% of the bins were not used by any element ) {
       Combine every adjacent bin: for ex - if m was 65536, bins {0,1} {2,3} {4,5}... {65534,65535} will be merged to get 32768 bins. max_rho of the new bins can be easily calculated by choosing the maximum of the two corresponding bins. 
     } */

  /* If m becomes less than 128, it would mean that the algorithm failed to estimate the cardinality within 30% accuracy {3 * 1.04/sqrt(m) - 3 sigma rule}. This would be due to input cardinality being too less, in which case I would recommend (key, count) based sorting using binary search trees due to low cardinality. The algorithm would quit setting *y to -1, *ptr_estimate_is_good to 0 and *ptr_estimate_accuracy to 100 %. */
  

  int cnt_zero = 0;
  for ( int ii = 0; ii < m; ii++ ) { 
    if ( max_rho[ii] == 0 ) { cnt_zero++; }
  }
 
  while ( cnt_zero > m/100 && m > 128) {
    
    int jj = 0;
    for ( int ii = 0; ii < m; ii+=2) {
      if ( max_rho[ii] >= max_rho[ii+1] ) {
	max_rho[jj++] = max_rho[ii];
      }
      else {
	max_rho[jj++] = max_rho[ii+1];
      }
    }
    m = jj; 

    cnt_zero = 0;
    for ( int ii = 0; ii < m; ii++ ) { 
      if ( max_rho[ii] == 0 ) { cnt_zero++; }
    }

  }
 
  //-------------------------------------------------------------------------
  
  /* Final computations */
  
  if ( cnt_zero < m/100 ) {

    *ptr_estimate_is_good = 1;
    *ptr_estimate_accuracy_percent = (double)1.04/sqrt(m)*3*100;

    double temp_val = 0;   
    for ( int ii = 0; ii < m; ii++ ) {
      temp_val = temp_val + pow(2,-(double)max_rho[ii]);
    }
    temp_val = 1/temp_val;
    double alpha_m = 0.7213/(1+1.079/m); /* true for m >= 128 */
    *y = (int)(alpha_m*pow(m,2)*temp_val);

  }
  else {

    *ptr_estimate_is_good = -2; 
    *ptr_estimate_accuracy_percent = 100;
    *y = -1;
    go_BYE(0);
    /* algorithm failed to estimate cardinality within 30% accuracy */

  }

  //-------------------------------------------------------------------------

 BYE:

  if ( flag == 1) {
    free_if_non_null(max_rho);
    free_if_non_null(hashval);
  }

  return (status);
}
