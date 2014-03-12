#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "assign_I4.h"
#include "qsort_asc_I4.h"
#include "approx_frequent.h"
#include "sorted_array_to_id_freq.h"
#include "update_counter.h"

#ifdef IPP
#include "ipp.h"
#include "ippi.h"
#endif

#define MAX_SZ 200*1048576
/* Will not use more than (4*200) MB of memory, can change if you want */

// START FUNC DECL
int 
ext_approx_frequent(
   char *tbl,
   char *fld,
   char *cfld,
   char *outtbl,
   char *str_min_freq,
   char *str_max_err,
   int *ptr_is_good
   )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE outtbl_rec; int outtbl_id;
  TBL_REC_TYPE tbl_rec; int tbl_id;
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_REC_TYPE cfld_rec; int cfld_id;
  FLD_REC_TYPE out_val_fld_rec; int out_val_fld_id;
  FLD_REC_TYPE out_cnt_fld_rec; int out_cnt_fld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id;
  char *fld_X = NULL; size_t fld_nX = 0;
  char *nn_fld_X = NULL; size_t nn_fld_nX = 0;
  char *out_val_X = NULL; size_t out_val_nX = 0;
  char *out_cnt_X = NULL; size_t out_cnt_nX = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;
  int fldsz = 0; size_t filesz = 0;
  long long nR = 0, nR_out = 0, actual_nR_out = 0;
  int out_val_ddir_id = -1, out_val_fileno = -1;
  int out_cnt_ddir_id = -1, out_cnt_fileno = -1;
  long long min_freq; long long max_err;

  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( outtbl == NULL ) || ( *outtbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_min_freq == NULL ) || ( *str_min_freq == '\0' ) ) { go_BYE(-1); }
  if ( ( str_max_err == NULL ) || ( *str_max_err == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(tbl, outtbl) == 0 ) { go_BYE(-1); }
  status = stoI8(str_min_freq, &min_freq); cBYE(status);
  status = stoI8(str_max_err, &max_err); cBYE(status);
  if ( min_freq <= 1 ) { go_BYE(-1); }
  if ( max_err  <= 1 ) { go_BYE(-1); }
  if ( ( min_freq - max_err ) < 1 ) { go_BYE(-1); }

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
  // TODO P3: Compute sum of cfld here and send to Kishore.
  // Compute worst case for nR_out
  nR_out = nR / ( min_freq - max_err );

  zero_fld_rec(&out_cnt_fld_rec); 
  out_cnt_fld_rec.fldtype = I4; // Note that count is I4
  status = get_fld_sz(out_cnt_fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = nR_out * fldsz;
  status = mk_temp_file(filesz, &out_cnt_ddir_id, &out_cnt_fileno); cBYE(status);
  status = q_mmap(out_cnt_ddir_id, out_cnt_fileno, &out_cnt_X, &out_cnt_nX, true); 
  cBYE(status);

  zero_fld_rec(&out_val_fld_rec); 
  out_val_fld_rec.fldtype = fld_rec.fldtype;
  status = get_fld_sz(out_val_fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = nR_out * fldsz;
  status = mk_temp_file(filesz, &out_val_ddir_id, &out_val_fileno); cBYE(status);
  status = q_mmap(out_val_ddir_id, out_val_fileno, &out_val_X, &out_val_nX, true); 
  cBYE(status);
  // Now the real work begins
  status = approx_frequent((int *)fld_X, (char *)cfld_X, nR, min_freq, max_err,
      (int *)out_val_X, (int *)out_cnt_X, nR_out, &actual_nR_out, ptr_is_good);
  cBYE(status);
  // truncate output as needed 
  if ( actual_nR_out <= 0 ) { go_BYE(-1); }
  status = get_fld_sz(out_val_fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = actual_nR_out * fldsz;
  status = q_trunc(out_val_ddir_id, out_val_fileno, filesz); cBYE(status);

  status = get_fld_sz(I4, &fldsz); cBYE(status);
  filesz = actual_nR_out * fldsz;
  status = q_trunc(out_cnt_ddir_id, out_cnt_fileno, filesz); cBYE(status);

  //----------------------------------------
  char strbuf[32]; zero_string(strbuf, 32);
  sprintf(strbuf, "%lld", actual_nR_out);
  status = add_tbl(outtbl, strbuf, &outtbl_id, &outtbl_rec); cBYE(status);

  status = add_fld(outtbl_id, "cnt", out_cnt_ddir_id, out_cnt_fileno, 
      &out_cnt_fld_id, &out_cnt_fld_rec);
  cBYE(status);
  status = add_fld(outtbl_id, fld, out_val_ddir_id, out_val_fileno, 
      &out_val_fld_id, &out_val_fld_rec);
  cBYE(status);


BYE:
  rs_munmap(fld_X, fld_nX);
  rs_munmap(out_val_X, out_val_nX);
  rs_munmap(out_cnt_X, out_cnt_nX);
  return status ;
}

// START FUNC DECL
int 
approx_frequent (
		 int * x, 
		 char * cfld,
		 long long siz, 
		 long long min_freq, 
		 long long err, 
		 int * y, 
		 int * f, 
		 long long out_siz,
		 long long * ptr_len,
		 int * ptr_estimate_is_good
		 )
// STOP FUNC DECL
//-----------------------------------------------------------------------------
/* README: 

status =
approx_frequent(x,cfld,siz,min_freq,err,y,f,out_siz,ptr_len,ptr_estimate_is_good)
: The algorithm takes as input an array of integers, and lists out the
"frequent" elements in the set approximately, where "frequent"
elements are defined as elements occuring greater than or equal to
"min_freq" number of times in the input. The approximated output has
the following properties:

(1) all elements in x occuring greater than or equal to min_freq
number of times will definitely be listed in y (THESE ARE THE FREQUENT
ELEMENTS (definition) )

(2) their corresponding frequency in f will be greater than or equal
to (min_freq-err), i.e., the maximum error in estimating their
frequencies is err.

(3) no elements in x occuring less than (min_freq-err) number of times
will be listed in y


The approximation is two fold: 

(i) the estimated frequencies of the "frequent" elements can be off by
a maximum of err.

(ii) elements occuring between (min_freq-err) and (min_freq) number of
times can also be listed in y.


For example: say min_freq = 500 and err = 100.  y will contain the id
of all the elements occuring >= 500 definitely, and their
corresponding estimated frequency in f would definitely be >=
(500-100) = 400. No element in x which occurs less than 400 times will
occur in y. Note that elements with frequency between 400 and 500
"can" be listed in y.

Author: Kishore Jaganathan

Algorithm: FREQUENT algorithm (refer to Cormode's paper "Finding
Frequent Items in Data Streams")

NOTE: This implementation is a slight variant of the algorithm
mentioned in the paper, so that some steps can be parallelized.

INPUTS: 

x: The input array 

cfld: two options - 

(1) NULL: All elements of x are processed.

(2) non-NULL: Array of same size as x. Acts as a select vector (only
those elements with non-zero values in cfld are processed). ex: If x
has 10 elements and cfld is {0,0,1,0,0,0,1,0,1,0}, then only the 3rd,
7th and 9th element are chosen for processing.

siz: Number of elements in the input array x

min_freq: elements occuring greater than or equal to min_freq times in
x (among the ones selected for processing) are considered frequent
elements. All of their id's will definitely be stored in y.

err: the measured frequencies of the "frequent" elements in x (i.e.,
occuring >= min_freq times in x, among the ones selected for
processing) will definitely be greater than or equal to min_freq-err,
and will be stored in f (corresponding to the id stored in y). Also,
no element with frequency lesser than (min_freq-err) in x (among the
ones selected for processing) will occur in y. Note: Lesser the error,
more memory is needed for computation

out_siz: number of integers that can be written in y and f (prealloced
memory). See y and f for how much to allocate.


OUTPUTS:

y: array containing the id's of the "frequent" elements. Need to
malloc beforehand by atleast (number of elements to be
processed)/(min_freq-err) * sizeof(int). If cfld is NULL, number of
elements to be processed is siz, else it is equal to the number of
non-zero entries in cfld.

f: array containing the corresponding frequencies of the "frequent"
elements. Need to malloc beforehand by atleast (number of elements to
be processed)/(min_freq-err) * sizeof(int). If cfld is NULL, number of
elements to be processed is siz, else it is equal to the number of
non-zero entries in cfld.

out_siz: number of integers that can be written in y and f (prealloced
memory). See y and f for how much to allocate.

ptr_len: the size of y and f used by the algorithm to write the ids
and frequencies of estimated approximate "frequent" elements

ptr_estimate_is_good: pointer to a location which stores 1, -1, -2 or -3

1: approximate calculations were successful, results stored in y,f and ptr_len
-1: something wrong with the input data. Check if sufficient malloc was done beforehand to y and f, in case you forgot.

-2: need too much memory, hence didn't do the calculations. Can retry with one of the following two things

(i) increase MAX_SZ if you are sure you have more RAM available 
(ii) increase err (the approximation parameter). Increasing err will result
in more approximation (hence answer being less accurate) but memory
requirements will be lesser.

status: will return 0 or -1
0: two cases - 

(i) calculations are successful, ptr_estimate_is_good will be set to 1 

(ii) need too much memory and hence didn't do the calculations, ptr_estimate_is_good will be set to -2.

-1: Something wrong with inputs, ptr_estimate_is_good will also be set
 to -1

 */
//-----------------------------------------------------------------------------
{

  int status = 0;

  int flag = 0; /* used to assist freeing mallocs */ 

  *ptr_estimate_is_good = -1; /* default */

  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( siz <= 0 ) { go_BYE(-1); }
  if ( err <= 0 ) { go_BYE(-1); } 
  if ( min_freq <= 0 ) { go_BYE(-1); }
  if ( min_freq - err <= 0 ) { go_BYE(-1); }
  if ( y == NULL ) { go_BYE(-1); }
  if ( f == NULL ) { go_BYE(-1); }
  if ( ptr_len == NULL ) { go_BYE(-1); }


  long long eff_siz = 0; /* number of entries to be considered */
  if ( cfld == NULL ) { eff_siz = siz; }
  else {

    for ( long long ii = 0; ii < siz; ii++ ) {
      if ( cfld[ii] == 0 ) { continue; }
      eff_siz++;
    }
    if ( eff_siz == 0 ) { go_BYE(-1); } /* cfld has all 0 entries */

  }

  double eps = (double) err/eff_siz; 
  /* parameter of FREQUENT algorithm, decides the error in approximation */
  if ( eps < pow(2,-50) ) { 
    *ptr_estimate_is_good = -2; 
    go_BYE(0); /* need too much memory */
  }

  if ( out_siz < eff_siz/(min_freq - err) ) { 
    *ptr_estimate_is_good = -1;
    go_BYE(-1);
    /* insufficient memory allocated to the outputs y and f */
  }

  //-------------------------------------------------------------------------

  /* The algorithm will be using (long long)(1/eps)+1 counters: stored in (cntr_id, cntr_freq) */

  int * cntr_id = NULL;
  int * cntr_freq = NULL;
  long long cntr_siz = (long long) (1/eps)+1;

  if ( cntr_siz < 10000 ) { cntr_siz = 10000; } /* can be removed */

  flag = 1; /* defined cntr_id and cntr_freq */

  if ( ( cntr_siz*(1+2+6) ) > MAX_SZ ) {
    *ptr_estimate_is_good = -2;
    go_BYE(0);
    /* Quitting if too much memory needed. Retry by doing one of the following:
       (i) Increase MAX_SZ if you think you have more RAM
       (ii) Increase eps (the approximation percentage) so that computations can be done within RAM
     */
  }
  
  int NUM_THREADS = 128;
  while ( (cntr_siz*(NUM_THREADS+2+6)) > MAX_SZ ) { NUM_THREADS = NUM_THREADS/2; } /* to promote parallel computing when possible, adapting NUM_THREADS to meet memory requirements */
 
  cntr_id = (int *)malloc( cntr_siz * sizeof(int) );
  return_if_malloc_failed(cntr_id);
  cntr_freq = (int *)malloc( cntr_siz * sizeof(int) );
  return_if_malloc_failed(cntr_freq);

  long long active_cntr_siz = 0; /* no of counters with non-zero frequencies */

#ifdef IPP
  ippsZero_32s((int *)cntr_id, cntr_siz);
  ippsZero_32s((int *)cntr_freq, cntr_siz);
#else
  assign_const_I4(cntr_id,cntr_siz,0);
  assign_const_I4(cntr_freq,cntr_siz,0);
#endif

  //-------------------------------------------------------------------------

  /* We will look at the incoming data as packets of size cntr_siz
     with sorted data (this would help speed up the update process a
     lot, this step is not mentioned in the paper - it's my
     improvization). Since the sorting has to be done within each
     packet separately, we can parallelize this step as follows: we
     divide the incoming data into blocks of size =
     NUM_THREADS*cntr_siz (so that NUM_THREADS threads can be
     generated for each block and sorted separately in parallel */

  /* "inputPacket" is a 2d array of size NUM_THREADS *cntr_siz: stores
     and sortes packets belonging to the same block in parallel */

  int ** inputPackets = NULL;
  long long * inputPacketsUsedSiz = NULL;

  flag = 2;  /* inputPackets and inputPacketsUsedSiz are defined */

  inputPackets = malloc ( NUM_THREADS * sizeof(int*) );
  return_if_malloc_failed(inputPackets); 

  inputPacketsUsedSiz = malloc ( NUM_THREADS * sizeof(long long) );
  return_if_malloc_failed(inputPacketsUsedSiz);

  for ( long long ii = 0; ii < NUM_THREADS; ii++) {
    inputPacketsUsedSiz[ii] = 0;
  }

  for ( int ii = 0; ii < NUM_THREADS; ii++ ) {
    inputPackets[ii] =  (int *) malloc( cntr_siz * sizeof(int) );
  }

  flag = 3; /* inputPackets[ii] defined for ii = 0 to NUM_THREADS-1 */

  for ( int ii = 0; ii < NUM_THREADS; ii++ ) {
    return_if_malloc_failed(inputPackets[ii]);
#ifdef IPP
    ippsZero_32s((int *)inputPackets[ii],cntr_siz);
#else
    assign_const_I4(inputPackets[ii],cntr_siz,0);
#endif
  }

  //------------------------------------------------------------------------
  
  int * bf_id = NULL;
  int * bf_freq = NULL; /* temporary counters for processing */

  flag = 4;  /* bf_id and bf_freq are defined */

  bf_id = (int *)malloc( cntr_siz * sizeof(int) );
  return_if_malloc_failed(bf_id);

  bf_freq = (int *)malloc( cntr_siz * sizeof(int) );
  return_if_malloc_failed(bf_freq);

  long long current_loc_in_x = 0; /* start of input data */

  /* Do the following for each block, till you reach the end of input */
  while ( current_loc_in_x < siz ) { 

    /* A block of data ( containing NUM_THREADS packets, i.e NUM_THREADS * cntr_siz integers ) is processed inside this loop. For each packet, the following operations are done: 
     (1): Sort the packet (can be done in parallel)
     (2): Convert each sorted packet into (id, freq) i.e (key, count) format using sorted_array_to_id_freq(). 
     (3): Update the counter array using update_counter()
     
     Steps (1) and (2) can be done in parallel, but for some reason trying to do (2) in parallel is slowing down the code. So doing only (1) in parallel. */

    /* Copying input data into "inputPackets" buffers */

    if ( cfld == NULL || eff_siz == siz ) {

      //------------------------------------------------------------------
      for ( long long ii = 0; ii < NUM_THREADS; ii++) {
	inputPacketsUsedSiz[ii] = 0;
      }

      cilkfor ( int tid = 0; tid < NUM_THREADS; tid++ ) {

	long long lb = current_loc_in_x + tid * cntr_siz; 
	long long ub = lb + cntr_siz;
	if ( lb >= siz ) { continue; }
	if ( ub >= siz ) { ub = siz; }

	memcpy(inputPackets[tid], x+lb, (ub-lb)*sizeof(int));
	inputPacketsUsedSiz[tid] = (ub-lb);

      }

      for ( int tid = 0; tid < NUM_THREADS; tid++ ) {
	current_loc_in_x += inputPacketsUsedSiz[tid];
      }
      //------------------------------------------------------------------

    }
    else {

      //------------------------------------------------------------------
      /* NOTE: if cfld input is non-null, it means we are not interested in all the elements. In every iteration, we keep filling inputPackets buffer with only those data we are interested in using the helper variable "current_loc_in_x". */

      for ( long long ii = 0; ii < NUM_THREADS; ii++) {
	inputPacketsUsedSiz[ii] = 0;
      }
      int tid = 0;
      
      while ( current_loc_in_x < siz  && tid < NUM_THREADS ) {

	if ( cfld[current_loc_in_x] == 0 ) { current_loc_in_x++; }
	else {
	  inputPackets[tid][inputPacketsUsedSiz[tid]] = x[current_loc_in_x];
	  current_loc_in_x++; inputPacketsUsedSiz[tid]++;
	  if ( inputPacketsUsedSiz[tid] == cntr_siz ) { tid++; }
	}

      }
      //------------------------------------------------------------------

    }


    /* Step (1) can be done here in parallel  */
    cilkfor ( int tid = 0; tid < NUM_THREADS; tid++ ) {
    
      if ( inputPacketsUsedSiz[tid] == 0 ) { continue; }

#ifdef IPP
      ippsSortAscend_32s_I(inputPackets[tid], inputPacketsUsedSiz[tid]);
#else
      qsort_asc_I4(inputPackets[tid], inputPacketsUsedSiz[tid], sizeof(int), NULL);
#endif

    }

    /* Steps (2) and (3) done here */

    for ( int tid = 0; tid < NUM_THREADS; tid++ ) {
    
      if ( inputPacketsUsedSiz[tid] == 0 ) { break; }
    
      long long bf_siz = 0;
      status = sorted_array_to_id_freq(inputPackets[tid],inputPacketsUsedSiz[tid],bf_id,bf_freq,&bf_siz); cBYE(status);

      status = update_counter(cntr_id,cntr_freq,cntr_siz,&active_cntr_siz,bf_id,bf_freq,bf_siz);
      cBYE(status);

    }


  }

  //----------------------------------------------------------------------
  /* Post-processing, writing the outputs */
  
  long long jj = 0;
  for ( long long ii = 0; ii < active_cntr_siz; ii++ ) {

    if ( cntr_freq[ii] >= (min_freq-err) ) {
      y[jj] = cntr_id[ii]; f[jj] = cntr_freq[ii];
      jj++;
    }

  }

  *ptr_len = jj;
  *ptr_estimate_is_good = 1;

 BYE:

  if ( flag >= 4 ) {
    free_if_non_null(bf_id);
    free_if_non_null(bf_freq);
  }
  if ( flag >= 3 ) {
    for ( int ii = 0; ii < NUM_THREADS; ii++ ) {
      free_if_non_null(inputPackets[ii]);
    }
  }
  if ( flag >= 2 ) {
    free_if_non_null(inputPackets);
    free_if_non_null(inputPacketsUsedSiz);
  }
  if ( flag >= 1 ) {
    free_if_non_null(cntr_id);
    free_if_non_null(cntr_freq);
  }

  return status ;
}
