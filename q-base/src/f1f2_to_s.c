#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "is_tbl.h"
#include "assign_I8.h"
#include "nn_f_to_s_sum_I1.h"
#include "nn_f_to_s_sum_I2.h"
#include "nn_f_to_s_sum_I4.h"
#include "nn_f_to_s_sum_I8.h"

extern int g_num_cores;

// START FUNC DECL
int
core_f1f2_to_s(
    const char *op,
    char *f1_X, 
    FLD_TYPE f1type,
    char *f2_X, 
    FLD_TYPE f2type,
    long long nR,
    char *rslt_buf, 
    int sz_rslt_buf
    )
// STOP FUNC DECL
{
  int status = 0;

  int nT = g_num_cores;
  long long block_size = nR / g_num_cores; 
  long long *rslt = NULL, *cnt = NULL;
  if ( strcmp(op, "sum") == 0 ) {
    rslt = malloc(g_num_cores * sizeof(long long));
    cnt  = malloc(g_num_cores * sizeof(long long));
    assign_const_I8(rslt, g_num_cores, 0);
    assign_const_I8(cnt , g_num_cores, 0);

    for ( int tid = 0; tid < nT; tid++ ) {
      if ( status < 0 ) { continue; }
      long long lb = tid * block_size;
      long long ub = lb + block_size;
      if ( tid == (nT-1) ) { ub = nR; }
      long long tnR = ub - lb;
      char      *f1I1 = (char *)     f1_X; f1I1 += lb;

      char      *f2I1 = (char *)     f2_X; f2I1 += lb;
      short     *f2I2 = (short *)    f2_X; f2I2 += lb;
      int       *f2I4 = (int *)      f2_X; f2I4 += lb;
      long long *f2I8 = (long long *)f2_X; f2I8 += lb;
      long long trsltI8 = 0, tcntI8 = 0;
      switch ( f2type ) {
      case I1 : 
	nn_f_to_s_sum_I1(f2I1, f1I1, tnR, &tcntI8, &trsltI8);
	break;
      case I2 : 
	nn_f_to_s_sum_I2(f2I2, f1I1, tnR, &tcntI8, &trsltI8);
	break;
      case I4 : 
	nn_f_to_s_sum_I4(f2I4, f1I1, tnR, &tcntI8, &trsltI8);
	break;
      case I8 : 
	nn_f_to_s_sum_I8(f2I8, f1I1, tnR, &tcntI8, &trsltI8);
	break;
      default : 
	status = -1; 
	break;
      }
      rslt[tid] = trsltI8;
      cnt[tid]  = tcntI8;
    }
    cBYE(status);
    long long final_rslt = 0, final_cnt = 0;
    for ( int tid = 0; tid < nT; tid++ ) { 
      final_rslt += rslt[tid];
      final_cnt  += cnt[tid];
    }
    sprintf(rslt_buf, "%lld:%lld:%lld\n", final_rslt, final_cnt, nR);
  }
  else {
    go_BYE(-1);
  }
  free_if_non_null(rslt);
  free_if_non_null(cnt);
BYE:
  return(status);
}

//---------------------------------------------------------------
// START FUNC DECL
int 
f1f2_to_s(
	  const char *tbl,
	  const char *f1,
	  const char *f2,
	  const char *op,
	  char *rslt_buf,
	  int sz_rslt_buf
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
#define MAX_LEN 32
  long long nR;
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE f1_rec; int f1_id; 
  FLD_REC_TYPE f2_rec; int f2_id; 
  FLD_REC_TYPE nn_f1_rec; int nn_f1_id; 
  FLD_REC_TYPE nn_f2_rec; int nn_f2_id; 
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1  == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2  == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  status = is_fld(tbl, -1, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  //-------------------------------------------------------------
  status = is_fld(tbl, -1, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec);
  cBYE(status);
  if ( f2_id < 0 ) { go_BYE(-1); }
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);

  if ( strcmp(op, "sum") == 0 ) {
    if ( f1_rec.fldtype != I1 ) { go_BYE(-1); }
    if ( f1_rec.nn_fld_id >= 0 ) { go_BYE(-1); }
    if ( f2_rec.nn_fld_id >= 0 ) { go_BYE(-1); }
  }
  //-------------------------------------------------------------
  status = core_f1f2_to_s(op, f1_X, f1_rec.fldtype, f2_X, f2_rec.fldtype, 
      nR, rslt_buf, sz_rslt_buf);
  cBYE(status);

 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  return status ;
}
