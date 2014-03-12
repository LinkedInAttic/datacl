#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "dbauxil.h"
#include "auxil.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_tbl.h"
#include "f_to_s.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "mk_temp_file.h"
#include "vec_f_to_s.h"
#include "meta_globals.h"

// last review 9/8/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
copy_fld_ranges(
		char *t1,
		char *f1,
		char *rng_tbl,
		char *lb,
		char *ub,
		char *t2
		)
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0; 
  char *op_X = NULL; size_t op_nX = 0; 
  char *nn_op_X = NULL; size_t nn_op_nX = 0; 
  char *lb_X = NULL; size_t lb_nX = 0; 
  char *ub_X = NULL; size_t ub_nX = 0; 
  char *nn_X = NULL; size_t nn_nX = 0; 
  TBL_REC_TYPE t1_rec, t2_rec, rng_tbl_rec;
  FLD_REC_TYPE t1f1_rec, nn_t1f1_rec;
  FLD_REC_TYPE t2f1_rec, nn_t2f1_rec;
  FLD_REC_TYPE lb_rec, nn_lb_rec;
  FLD_REC_TYPE ub_rec, nn_ub_rec;
  long long nR1 = -1, rng_tbl_nR = -1;
  long long nn_nR2 = -1, exp_nR2 = -1, nR2  = -1;
  long long lbidx = -1, ubidx = -1;

  int t1_id = -1, t2_id = -1, rng_tbl_id = -1;
  int t1f1_id = -1, nn_t1f1_id = -1;
  int t2f1_id = -1, nn_t2f1_id = -1;
  int lb_id = -1,   nn_lb_id = -1; 
  int ub_id = -1,   nn_ub_id = -1;

#define BUFLEN 32
  char buf1[BUFLEN], buf2[BUFLEN];
  int f1_fldsz, I1_fldsz; size_t filesz;
  int ddir_id = INT_MAX, nn_ddir_id = INT_MAX;
  int fileno  = INT_MAX, nn_fileno  = INT_MAX;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  if ( ( lb      == NULL ) || ( *lb      == '\0' ) ) { go_BYE(-1); }
  if ( ( ub      == NULL ) || ( *ub      == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(lb, ub) == 0 ) { go_BYE(-1); }

  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = t1_rec.nR;

  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  if ( t2_id >= 0 ) {
    nR2 = t2_rec.nR;
  }

  status = is_fld(NULL, t1_id, f1, &t1f1_id, &t1f1_rec, 
		  &nn_t1f1_id, &nn_t1f1_rec); cBYE(status);
  chk_range(t1f1_id, 0, g_n_fld);
  status = get_data(t1f1_rec, &X, &nX, false); cBYE(status);
  if ( nn_t1f1_id >= 0 ) { 
    status = get_data(nn_t1f1_rec, &nn_X, &nn_nX, 0);
  }
  switch ( t1f1_rec.fldtype ) { 
  case I1 : case I2 : case I4 : case I8 : case F4 : case F8 : break; 
  default : go_BYE(-1); break;
  }

  long long *lbptr = NULL, *ubptr = NULL;
  if ( ( rng_tbl == NULL ) || ( *rng_tbl == '\0' ) ) { 
    /* In this case, lb and ub refer to actual indexes */
    status = stoI8(lb, &lbidx); cBYE(status); 
    status = stoI8(ub, &ubidx); cBYE(status); 
    if ( lbidx >= ubidx ) { go_BYE(-1); }
    if ( lbidx < 0    ) { go_BYE(-1); }
    if ( ubidx >= nR1 ) { go_BYE(-1); }
    exp_nR2 = ubidx - lbidx;
  }
  else {
    status = is_tbl(rng_tbl, &rng_tbl_id, &rng_tbl_rec); cBYE(status);
    if ( rng_tbl_id < 0 ) { go_BYE(-1); }
    rng_tbl_nR = rng_tbl_rec.nR;

    status = is_fld(NULL, rng_tbl_id, lb, &lb_id, &lb_rec, 
		    &nn_lb_id, &nn_lb_rec); cBYE(status);
    chk_range(lb_id, 0, g_n_fld);
    status = get_data(lb_rec, &lb_X, &lb_nX, false); cBYE(status);
    if ( lb_rec.fldtype != I8 ) { go_BYE(-1); }
    if ( nn_lb_id >= 0 ) { go_BYE(-1); }
    lbptr = (long long*)lb_X;

    status = is_fld(NULL, rng_tbl_id, ub, &ub_id, &ub_rec, 
		    &nn_ub_id, &nn_ub_rec); cBYE(status);
    chk_range(ub_id, 0, g_n_fld);
    status = get_data(ub_rec, &ub_X, &ub_nX, false); cBYE(status);
    if ( ub_rec.fldtype != I8 ) { go_BYE(-1); }
    if ( nn_ub_id >= 0 ) { go_BYE(-1); }
    ubptr = (long long*)ub_X;

    exp_nR2 = 0;
    for ( long long i = 0; i < rng_tbl_nR; i++ ) { 
      if ( lbptr[i] < 0 ) { go_BYE(-1); }
      if ( ubptr[i] >= nR1 ) { go_BYE(-1); }
      if ( lbptr[i] >= ubptr[i] ) { go_BYE(-1); }
      exp_nR2 += (ubptr[i] - lbptr[i]);
    }
    if ( exp_nR2 == 0 ) { go_BYE(-1); }
  }
  if ( t2_id < 0 ) { 
    nR2 = exp_nR2;
    sprintf(buf1, "%lld", nR2);
    status = add_tbl(t2, buf1, &t2_id, &t2_rec); cBYE(status);
  }
  else {
    if ( exp_nR2 != nR2 ) { go_BYE(-1); }
  }

  //--------------------------------------------------------
  /* Allocate space for output */
  status = get_fld_sz(t1f1_rec.fldtype, &f1_fldsz); cBYE(status);
  filesz = f1_fldsz * nR2; 
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &op_X, &op_nX, 1); cBYE(status);
  //--------------------------------------------------------
  /* Allocate space for output condition field if necessary */
  if ( nn_X != NULL ) { 
    status = get_fld_sz(I1, &I1_fldsz); cBYE(status);
    filesz = I1_fldsz * nR2; 
    status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno);
    status = q_mmap(nn_ddir_id, nn_fileno, &nn_op_X, &nn_op_nX, true);cBYE(status);
  }
  //--------------------------------------------------------
  // Get started
  if ( rng_tbl_id < 0 ) { 
    char *l_X   = X    + ( lbidx  * f1_fldsz );
    char *l_opX = op_X;
    size_t num_bytes_to_copy = f1_fldsz * (ubidx - lbidx);
    memcpy(l_opX, l_X, num_bytes_to_copy);
    if ( nn_X != NULL ) { 
      l_X   = nn_X    + ( lbidx  * I1_fldsz );
      l_opX = nn_op_X;
      num_bytes_to_copy = I1_fldsz * (ubidx - lbidx);
      memcpy(l_opX, l_X, num_bytes_to_copy);
    }
  }
  else {
  long long t2_offset = 0;
  for ( int i = 0; i < rng_tbl_nR; i++ ) {  
    char *l_X   = X    + ( lbptr[i]  * f1_fldsz );
    char *l_opX = op_X + ( t2_offset * f1_fldsz );
    size_t num_bytes_to_copy = f1_fldsz * (ubptr[i] - lbptr[i]);
    memcpy(l_opX, l_X, num_bytes_to_copy);
    if ( nn_X != NULL ) { 
      l_X   = nn_X    + ( lbptr[i]  * I1_fldsz );
      l_opX = nn_op_X + ( t2_offset * I1_fldsz );
      num_bytes_to_copy = I1_fldsz * (ubptr[i] - lbptr[i]);
      memcpy(l_opX, l_X, num_bytes_to_copy);
    }
    t2_offset += (ubptr[i] - lbptr[i]);
  }
  }

  // Add field to meta data store 
  zero_fld_rec(&t2f1_rec); t2f1_rec.fldtype = t1f1_rec.fldtype;
  status = add_fld(t2_id, f1, ddir_id, fileno, &t2f1_id, &t2f1_rec);
  cBYE(status);
  /*- Some other meta data to take care of --------------------*/
  g_flds[t2f1_id].srttype     = g_flds[t1f1_id].srttype;
  // Transfer meta information from t1f1 to t2f1
  status = copy_meta(t2f1_id, t1f1_id, 1); cBYE(status);
  /*-----------------------------------------------------------*/
  if ( nn_X != NULL ) {
    status = vec_f_to_s(nn_op_X, I1, NULL, nR2, "sum", buf1, BUFLEN);
    status = read_nth_val(buf1, ":", 0, buf2, BUFLEN); cBYE(status);
    status = stoI8(buf2, &nn_nR2); cBYE(status);
    if ( nn_nR2 == nR2 ) { 
      status = q_delete(nn_ddir_id, nn_fileno); cBYE(status);
    }
    else {
      zero_fld_rec(&nn_t2f1_rec); nn_t2f1_rec.fldtype = I1;
      status = add_aux_fld(NULL, t2_id, NULL, t2f1_id, nn_ddir_id, nn_fileno, 
			   nn, &nn_t2f1_id, &nn_t2f1_rec);
      cBYE(status);
    }
  }
 BYE:
  rs_munmap(X,       nX);
  rs_munmap(lb_X,    lb_nX);
  rs_munmap(ub_X,    ub_nX);
  rs_munmap(nn_X,    nn_nX);
  rs_munmap(op_X,    op_nX);
  rs_munmap(nn_op_X, nn_op_nX);
  return status ;
}
