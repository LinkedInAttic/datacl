/* READ */
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "set_meta.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "count.h"
#include "count_I1.h"
#include "count_I2.h"
#include "count_I4.h"
#include "count_I8.h"
#include "count_nn_I1.h"
#include "count_nn_I2.h"
#include "count_nn_I4.h"
#include "count_nn_I8.h"
#include "assign_I4.h"

extern int g_num_cores;

//---------------------------------------------------------------
// START FUNC DECL
int 
count(
      char *src_tbl,
      char *fk_dst,
      char *cfld,
      char *dst_tbl,
      char *cnt_fld
      )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL;    size_t nX = 0;
  char *op_X = NULL; size_t n_op_X = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;


  TBL_REC_TYPE src_tbl_rec; int src_tbl_id; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id; 

  FLD_REC_TYPE fk_dst_rec; int fk_dst_id; 
  FLD_REC_TYPE nn_fk_dst_rec; int nn_fk_dst_id; 

  FLD_REC_TYPE cfld_rec; int cfld_id;
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id;

  FLD_REC_TYPE cnt_fld_rec; int cnt_fld_id; 

  char opfile[MAX_LEN_FILE_NAME+1];

  long long src_nR, dst_nR;
#define MAX_LEN 32
  char str_dst_nR[MAX_LEN];
  int **partial_counts = NULL; int nT = 0;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fk_dst == NULL ) || ( *fk_dst == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt_fld == NULL ) || ( *cnt_fld == '\0' ) ) { go_BYE(-1); }
  zero_string(str_dst_nR, MAX_LEN);
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbls[dst_tbl_id].nR;
  if ( dst_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, fk_dst, &fk_dst_id, &fk_dst_rec, 
		  &nn_fk_dst_id, &nn_fk_dst_rec); 
  cBYE(status);
  chk_range(fk_dst_id, 0, g_n_fld);

  status = get_data(fk_dst_rec, &X, &nX, 0); cBYE(status);
  if ( nn_fk_dst_id >= 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
  }

  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) { 
    status = is_fld(NULL, src_tbl_id, cfld, &cfld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec); 
    if ( cfld_id >= 0 ) { 
      if ( cfld_rec.fldtype != I1 ) { go_BYE(-1); }
      if ( nn_cfld_id >= 0 ) { go_BYE(-1); }
    }
    status = get_data(cfld_rec, &cfld_X, &cfld_nX, 0); cBYE(status);
  }
  //------------------------------------------------------
  int ddir_id = INT_MAX;
  status = mk_temp_file(opfile, (dst_nR * sizeof(int)), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &op_X, &n_op_X, true); cBYE(status);
  int *cntI4 = (int *)op_X;
  for ( int i = 0; i < dst_nR ; i++ ) {
    cntI4[i] = 0;
  }
  //------------------------------------------------------
  if ( dst_nR > INT_MAX ) { go_BYE(-1); } /* required by count_In */

  bool is_sequential = true;
  if ( dst_nR > 1048576 ) {
    fprintf(stderr, "Count(%s)  = %lld > 1048576. Use another algorithm\n", 
	    dst_tbl, dst_nR); go_BYE(-1);
  }
  // TODO: Need to adjust parallelism better than current hack
  if ( ( dst_nR <= 32768 ) && ( src_nR > 1048576 ) ) {
    is_sequential = false;
  }
  // Initialize counters to 0 
#ifdef IPP
  ippsZero_32s(cntI4, dst_nR); 
#else
  for ( int i = 0; i < dst_nR; i++ ) { cntI4[i] = 0; }
#endif

  //  TODO: Parallelism does not seem to provide any speedup at all
//  is_sequential = true; /* TODO P1: parallel version taking longer !!! */
//  fprintf(stderr, "forcing sequential execution in count() \n");

  int max_num_chunks = g_num_cores; 
  long long min_block_size = 8192, block_size; 
  if ( is_sequential == false ) { 
    status = partition(src_nR, min_block_size, max_num_chunks, &block_size, &nT);
    cBYE(status);
    partial_counts = (int **)malloc(nT * sizeof(int *));
    return_if_malloc_failed(partial_counts);
    for ( int tid = 0; tid < nT; tid++ ) {
      partial_counts[tid] = (int *)malloc(dst_nR * sizeof(int));
      return_if_malloc_failed(partial_counts[tid]);
    }
  }
//  fprintf(stderr, "nT         = %d \n", nT);
//  fprintf(stderr, "block_size = %lld \n", block_size);
//  fprintf(stderr, "src_nR     = %lld \n", src_nR);
  
  if ( cfld_id >= 0 ) {
    if ( is_sequential ) { 
      switch ( fk_dst_rec.fldtype ) {
      case I1 : 
	status = count_nn_I1((char *)X, src_nR, cfld_X, cntI4, dst_nR); 
	cBYE(status); 
	break;
      case I2 : 
	status = count_nn_I2((short *)X, src_nR, cfld_X, cntI4, dst_nR); 
	cBYE(status); 
	break;
      case I4 : 
	status = count_nn_I4((int *)X, src_nR, cfld_X, cntI4, dst_nR); 
	cBYE(status); 
	break;
      case I8 : 
	status = count_nn_I8((long long *)X, src_nR, cfld_X, cntI4, dst_nR); 
	cBYE(status); 
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    else {
      cilkfor ( int tid = 0; tid < nT; tid++ ) {
	// Initialize counts to 0 
	int *partial_counts_t = partial_counts[tid];
	// Initialize counters to 0 
#ifdef IPP
	ippsZero_32s(partial_counts_t, dst_nR); 
#else
	assign_const_I4(partial_counts_t, 0, dst_nR);
#endif
	long long lb = block_size * tid;
	long long ub = lb + block_size;
	if ( tid == (nT-1) ) { ub = src_nR; }
        char      *inI1 = (char      *)X; inI1 += lb;
        short     *inI2 = (short     *)X; inI2 += lb;
        int       *inI4 = (int       *)X; inI4 += lb;
        long long *inI8 = (long long *)X; inI8 += lb;
	long long t_src_nR = ub - lb;
	switch ( fk_dst_rec.fldtype ) {
	case I1 : 
	  status = count_nn_I1(inI1, t_src_nR, cfld_X, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	case I2 : 
	  status = count_nn_I2(inI2, t_src_nR, cfld_X, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	case I4 : 
	  status = count_nn_I4(inI4, t_src_nR, cfld_X, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	case I8 : 
	  status = count_nn_I8(inI8, t_src_nR, cfld_X, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
    }
  }
  else {
    if ( is_sequential ) { 
      switch ( fk_dst_rec.fldtype ) {
      case I1 : 
	status = count_I1((char *)X, src_nR, cntI4, dst_nR); cBYE(status);
	break;
      case I2 : 
	status = count_I2((short *)X, src_nR, cntI4, dst_nR); cBYE(status); 
	break;
      case I4 : 
	status = count_I4((int *)X, src_nR, cntI4, dst_nR); cBYE(status); 
	break;
      case I8 : 
	status = count_I8((long long *)X, src_nR, cntI4, dst_nR); cBYE(status); 
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    else {
      cilkfor ( int tid = 0; tid < nT; tid++ ) {
	// Initialize counts to 0 
	int *partial_counts_t = partial_counts[tid];
	// Initialize counters to 0 
#ifdef IPP
	ippsZero_32s(partial_counts_t, dst_nR); 
#else
	assign_const_I4(partial_counts_t, 0, dst_nR);
#endif
	long long lb = block_size * tid;
	long long ub = lb + block_size;
	if ( tid == (nT-1) ) { ub = src_nR; }
	long long t_src_nR = ub - lb;
        char      *inI1 = (char      *)X; inI1 += lb;
        short     *inI2 = (short     *)X; inI2 += lb;
        int       *inI4 = (int       *)X; inI4 += lb;
        long long *inI8 = (long long *)X; inI8 += lb;
	switch ( fk_dst_rec.fldtype ) {
	case I1 : 
	  status = count_I1(inI1, t_src_nR, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	case I2 : 
	  status = count_I2(inI2, t_src_nR, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	case I4 : 
	  status = count_I4(inI4, t_src_nR, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	case I8 : 
	  status = count_I8(inI8, t_src_nR, partial_counts_t, dst_nR); 
	  cBYE(status); 
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
    }
  }
  if ( is_sequential == false ) { 
  cilkfor ( int i = 0; i < dst_nR; i++ ) { 
    register int lsumI4 = 0;
    for ( int tid = 0; tid < nT; tid++ ) { 
      lsumI4 += partial_counts[tid][i];
    }
    cntI4[i] = lsumI4;
  }
  }

  rs_munmap(op_X, n_op_X);
  status = add_fld(dst_tbl_id, cnt_fld, ddir_id, opfile, I4, -1, &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
 BYE:
  if ( partial_counts != NULL ) { 
    for ( int tid = 0; tid < nT; tid++ ) {
      free_if_non_null(partial_counts[tid]);
    }
    free_if_non_null(partial_counts);
  }
  rs_munmap(X, nX);
  rs_munmap(op_X, n_op_X);
  return(status);
}
