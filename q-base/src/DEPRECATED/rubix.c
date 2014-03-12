#include <limits.h>
#include <values.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "set_meta.h"
#include "is_fld.h"
#include "mk_file.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

//---------------------------------------------------------------

// START FUNC DECL
int 
rubix(
	   const char *t1,
	   const char *str_lb,
	   const char *str_ub,
	   const char *bmask_fld,
	   const char *str_bmask_val,
	   char *rsltbuf,
	   int sz_rsltbuf
    )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
#define MAX_LEN 32
  long long nR1;
  TBL_REC_TYPE t1_rec; int t1_id; 
  FLD_REC_TYPE f1_rec; int f1_id; 
  FLD_REC_TYPE nn_f1_rec; int nn_f1_id; 
  long long lb, ub; char *endptr;
  unsigned int bmask;
#define MAX_NUM_CHUNKS 16
  int partial_sums[MAX_NUM_CHUNKS];
  // long long dbg_cnt[MAX_NUM_CHUNKS];

  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( bmask_fld == NULL ) || ( *bmask_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_lb == NULL ) || ( *str_lb == '\0' ) ) { go_BYE(-1); }
  if ( ( str_ub == NULL ) || ( *str_ub == '\0' ) ) { go_BYE(-1); }
  if ( ( str_bmask_val == NULL ) || ( *str_bmask_val == '\0' ) ) { go_BYE(-1); }

  const char *f1 = bmask_fld;

  lb = strtoll(str_lb, &endptr, 10); 
  if ( *endptr != '\0' ) { go_BYE(-1); }
  ub = strtoll(str_ub, &endptr, 10); 
  if ( *endptr != '\0' ) { go_BYE(-1); }
  bmask = strtoll(str_bmask_val, &endptr, 10); 
  if ( *endptr != '\0' ) { go_BYE(-1); }

  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  nR1 = t1_rec.nR;
  if ( ( lb < 0 ) || ( ub > nR1 ) || ( lb >= ub ) ) { go_BYE(-1); }
  status = is_fld(t1, -1, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  cBYE(status);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }

  unsigned int *f1I4ptr = (unsigned int *)f1_X;
  long long niter = ub - lb;
  f1I4ptr += lb;

  int nT; long long block_size;
  status = partition(niter, 1024, MAX_NUM_CHUNKS, &block_size, &nT);
  cBYE(status);
  // fprintf(stderr, "niter = %lld \n", niter);
  // fprintf(stderr, "block_size = %lld \n", block_size);
  // fprintf(stderr, "nT = %d \n", nT);

  for ( int tid = 0; tid < nT; tid++ ) { partial_sums[tid] = 0; }

  cilkfor ( int tid = 0; tid < nT; tid++ ) {
    long long tlb = (tid * block_size);
    long long tub = tlb + block_size;
    if ( tid == (nT-1) ) { tub = niter; }
    long long local_sum = 0; // long long local_cnt = 0;
    register unsigned int *tf1I4ptr = f1I4ptr + tlb; // local copy
    for ( long long i = tlb; i < tub; i++ ) { 
      unsigned int f1I4 = *tf1I4ptr++;
      if ( ( f1I4 & bmask ) > 0 ) {
        local_sum++;
      }
    }
    partial_sums[tid] = local_sum;
  }
  long long global_sum = 0;
  for ( int tid = 0; tid < nT; tid++ ) { 
    global_sum += partial_sums[tid];
  }
  /* 
  if ( niter != chk_niter ) { 
    fprintf(stderr, "%lld:%lld", niter, chk_niter);
    go_BYE(-1); 
  }
  */
  sprintf(rsltbuf, "%lld:%lld", global_sum, niter);
 BYE:
  rs_munmap(f1_X, f1_nX);
  return(status);
}
