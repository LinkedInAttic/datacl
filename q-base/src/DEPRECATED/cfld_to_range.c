#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "cfld_to_range.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "is_nn_fld.h"
#include "mk_temp_file.h"
#include "add_tbl.h"
#include "meta_globals.h"

// START FUNC DECL
int 
cfld_to_range(
	      char *src_tbl,
	      char *src_fld,
	      char *dst_tbl,
	      char *lb_fld,
	      char *ub_fld
	      )
// STOP FUNC DECL
{
  int status = 0;
  char *src_X = NULL; size_t src_nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0; 
  int lb_ddir_id = -1, lb_fileno = -1;
  char *ub_X = NULL; size_t ub_nX = 0; 
  int ub_ddir_id = -1, ub_fileno = -1;

  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE lb_fld_rec, ub_fld_rec;
  FLD_REC_TYPE src_fld_rec,    nn_src_fld_rec;

  long long src_nR = LLONG_MIN, dst_nR = LLONG_MIN;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, nn_src_fld_id = INT_MIN;
  int lb_fld_id = INT_MIN;
  int ub_fld_id = INT_MIN;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }

  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_fld  == NULL ) || ( *lb_fld  == '\0' ) ) { go_BYE(-1); }
  if ( ( ub_fld  == NULL ) || ( *ub_fld  == '\0' ) ) { go_BYE(-1); }
  //----------------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = src_tbl_rec.nR;

  // Create space for output
  long long max_dst_nR = src_nR;
  long long filesz = max_dst_nR * sizeof(long long); 

  status = mk_temp_file(filesz, &lb_ddir_id, &lb_fileno); cBYE(status);
  status = q_mmap(lb_ddir_id, lb_fileno, &lb_X, &lb_nX, true); cBYE(status);
  long long *lbI8 = (long long *)lb_X;

  status = mk_temp_file(filesz, &ub_ddir_id, &ub_fileno); cBYE(status);
  status = q_mmap(ub_ddir_id, ub_fileno, &ub_X, &ub_nX, true); cBYE(status);
  long long *ubI8 = (long long *)ub_X;

  // src_fld
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, 
		  &src_fld_rec, &nn_src_fld_id, &nn_src_fld_rec); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  if ( nn_src_fld_id >= 0 ) { go_BYE(-1); }
  status = get_data(src_fld_rec, &src_X, &src_nX, false); cBYE(status);
  if ( src_fld_rec.fldtype != I1 ) { go_BYE(-1); }


  // TODO P0: Parallelize outer loop 
  // Looking for the start of the range 
  long long rangelb, rangeub;
  dst_nR = 0;
  for ( int i = 0; i < src_nR; i++ ) { 
    if ( src_X[i] == 0 ) { continue; }
    rangelb = i; // inclusive 
    rangeub = -1; 
    // Looking for the end of the range 
    for ( ; i < src_nR; i++ ) { 
      if ( src_X[i] == 0 ) {
	rangeub = i; // exclusive 
	break;
      }
      if ( rangeub < 0 ) { 
	rangeub = src_nR;
      }
    }
    lbI8[dst_nR] = rangelb;
    ubI8[dst_nR] = rangeub;
    dst_nR++;
  }
  if ( dst_nR == 0 ) { goto BYE; } // Nothing todo 
  // truncate file if necessary
  rs_munmap(lb_X,  lb_nX);
  rs_munmap(ub_X,  ub_nX);
  if ( dst_nR < src_nR ) { 
    q_trunc(lb_ddir_id, lb_fileno,  (dst_nR * sizeof(long long)));
    q_trunc(ub_ddir_id, ub_fileno,  (dst_nR * sizeof(long long)));
  }

  char strbuf[32];
  sprintf(strbuf, "%lld", dst_nR);
  status = add_tbl(dst_tbl, strbuf, &dst_tbl_id, &dst_tbl_rec); cBYE(status);

  zero_fld_rec(&lb_fld_rec); lb_fld_rec.fldtype = I8;
  status = add_fld(dst_tbl_id, lb_fld, lb_ddir_id, lb_fileno, 
      &lb_fld_id, &lb_fld_rec); cBYE(status);
  zero_fld_rec(&ub_fld_rec); ub_fld_rec.fldtype = I8;
  status = add_fld(dst_tbl_id, ub_fld, ub_ddir_id, ub_fileno, 
      &ub_fld_id, &ub_fld_rec); cBYE(status);
 BYE:
  rs_munmap(src_X, src_nX);
  rs_munmap(lb_X,  lb_nX);
  rs_munmap(ub_X,  ub_nX);
  return(status);
}
