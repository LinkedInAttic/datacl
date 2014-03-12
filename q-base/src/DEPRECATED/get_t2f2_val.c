#ifdef ICC
#include <cilk/cilk_api.h>
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "fop.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "vec_f_to_s.h"
#include "meta_globals.h"
#include "assign_I4.h"
#include "assign_I1.h"

/* START FUNC DECL */
int 
get_t2f2_val(
	     char *t1,
	     char *lnk1,
	     char *t2,
	     char *lnk2,
	     char *t2f2, /* field in t2 */
	     char *t1f2 /* field in t1 */
	     )
/* STOP FUNC DECL */
{
  int status = 0;
  char *t2f2_X = NULL; size_t n_t2f2_X = 0;
  char *nn_t2f2_X = NULL; size_t nn_n_t2f2_X = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  char *nn_X1 = NULL; size_t nn_nX1 = 0;
  char *nn_X2 = NULL; size_t nn_nX2 = 0;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE lnk1_rec, nn_lnk1_rec; int lnk1_id, nn_lnk1_id;
  FLD_REC_TYPE lnk2_rec, nn_lnk2_rec;int lnk2_id, nn_lnk2_id; 
  FLD_REC_TYPE t2f2_rec, nn_t2f2_rec; int t2f2_id, nn_t2f2_id;
  FLD_REC_TYPE t1f2_rec, nn_t1f2_rec; int t1f2_id, nn_t1f2_id;
  int t1_id, t2_id; 
  long long nR1, nR2;
#define BUFLEN 32
  char rslt_buf[BUFLEN];
  char **Y = NULL; int nY = 0;
  HT_I8_TYPE *ht = NULL;
  int sz_ht;    // size of hash table 
  int n_ht = 0;  // number of occupied entries in hash table 

  /*-------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( strcmp(t1, t2) == 0 ) && ( strcmp(t1f2, t2f2) == 0 ) ) { go_BYE(-1); }

  if ( ( lnk1 == NULL ) || ( *lnk1 == '\0' ) ) { go_BYE(-1); }
  if ( ( lnk2 == NULL ) || ( *lnk2 == '\0' ) ) { go_BYE(-1); }
  /*-------------------------------------------------------------*/

  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR;

  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbls[t2_id].nR;
  /*-------------------------------------------------------------*/
  status = is_fld(NULL, t1_id, lnk1, &lnk1_id, &lnk1_rec, 
		  &nn_lnk1_id, &nn_lnk1_rec); 
  cBYE(status);
  chk_range(lnk1_id, 0, g_n_fld);
  if ( nn_lnk1_id >= 0 ) { 
    status = get_data(nn_lnk1_rec, &nn_X1, &nn_nX1, false); cBYE(status);
  }
  status = get_data(lnk1_rec, &X1, &nX1, false); cBYE(status);
  /*-------------------------------------------------------------*/
  status = is_fld(NULL, t2_id, lnk2, &lnk2_id, &lnk2_rec, 
		  &nn_lnk2_id, &nn_lnk2_rec); 
  cBYE(status);
  chk_range(lnk2_id, 0, g_n_fld);
  if ( nn_lnk2_id >= 0 ) { go_BYE(-1); } 
  status = get_data(lnk2_rec, &X2, &nX2, false); cBYE(status);
  if ( nn_lnk2_id >= 0 ) { 
    status = get_data(nn_lnk2_rec, &nn_X2, &nn_nX2, false); cBYE(status);
  }
  /*-------------------------------------------------------- */
  if ( lnk1_rec.fldtype != lnk2_rec.fldtype ) { go_BYE(-1); }
  /*-------------------------------------------------------- */
  status = is_fld(NULL, t2_id, t2f2, &t2f2_id, &t2f2_rec, 
		  &nn_t2f2_id, &nn_t2f2_rec); 
  cBYE(status);
  chk_range(t2f2_id, 0, g_n_fld);
  status = get_data(t2f2_rec, &t2f2_X, &n_t2f2_X, false); cBYE(status);
  if ( nn_t2f2_id >= 0 ) { 
    status = get_data(nn_t2f2_rec, &nn_t2f2_X, &nn_n_t2f2_X, false); 
    cBYE(status);
  }
  /*--------------------------------------------------------*/
  /* Check supported field types */
  if ( lnk1_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( lnk2_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( t2f2_rec.fldtype != I4 ) { go_BYE(-1); }
  /*-------------------------------------------------------- */
  /* Create space for output */
  int fldsz = 0; size_t filesz = 0;
  int ddir_id    = -1, fileno    = -1; 
  int nn_ddir_id = -1, nn_fileno = -1;

  status = get_fld_sz(I4, &fldsz); cBYE(status);
  filesz = fldsz * nR1; 
  status = mk_temp_file(filesz, &ddir_id, &fileno);
  status = q_mmap(ddir_id, fileno, &X, &nX, 1); cBYE(status);

  status = get_fld_sz(I1, &fldsz); cBYE(status);
  filesz = fldsz * nR1; 
  status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno);
  status = q_mmap(nn_ddir_id, nn_fileno, &nn_X, &nn_nX, 1); cBYE(status);

  /*-------------------------------------------------------- */
  bool is_fast = false;
  bool definitely_has_null_vals = false;
  long long nn_nR2 = 0;
  if ( nn_X2 != NULL ) { 
    for ( long long i = 0; i < nR2; i++ ) { 
      if ( nn_X2[i] == TRUE ) { 
	nn_nR2++;
      }
    }
  }
  else {
    nn_nR2 = nR2;
  }
  if ( nn_nR2 == 0 ) {
    definitely_has_null_vals = true;
    assign_const_I4((int  *)X,    nR1, 0);
    assign_const_I1((char *)nn_X, nR1, 0);
  }
  else {
    cilkfor ( long long i = 0; i < nR1; i++ ) {
      int *it2f2 = (int *)t2f2_X;
      int *I4_lnk1 = (int *)X1; int I4val1;
      int *I4_lnk2 = (int *)X2; int I4val2;
      int *out = (int *)X; int outval;
      char *out_nn = (char *)nn_X;
      char c_nn;
      I4val1 = I4_lnk1[i];
      /* If the link value is null, then the output value is null */
      if ( ( nn_X1 != NULL ) && ( nn_X1[i] == FALSE ) ) {
	out_nn[i] = FALSE;
	out[i]    = 0;
      }
      else {
	/* We are now looking for I4val1 in the link field of t2 */
	if ( is_fast == false ) { 
	  outval = 0;
	  c_nn = FALSE;
	  for ( long long j = 0; j < nR2; j++ ) {
	    if ( ( nn_X2 != NULL ) && ( nn_X2[j] == FALSE ) ) {
	      continue;
	    }
	    I4val2 = I4_lnk2[j];
	    if ( I4val1 == I4val2 ) {
	      if ( ( nn_t2f2_X == NULL ) || (nn_t2f2_X[j] == TRUE ) ) {
		c_nn = TRUE;
	      }
	      outval = it2f2[j];
	      break;
	    }
	  }
	  out_nn[i] = c_nn;
	  out[i]    = outval;
	}
	else {
	  WHEREAMI; status = -1; continue;
	}
      }
    }
    cBYE(status);
#ifdef CILK
    __cilkrts_end_cilk();
#endif
  }
  rs_munmap(X, nX);
  zero_fld_rec(&t1f2_rec); t1f2_rec.fldtype = I4;
  status = add_fld(t1_id, t1f2, ddir_id, fileno, &t1f2_id, &t1f2_rec);
  cBYE(status);
  if ( definitely_has_null_vals == true ) {
    /* No need to check */
    rs_munmap(nn_X, nn_nX);
    zero_fld_rec(&nn_t1f2_rec); nn_t1f2_rec.fldtype = I1;
    status = add_aux_fld(NULL, t1_id, NULL, t1f2_id, nn_ddir_id, 
	nn_fileno, nn, &nn_t1f2_id, &nn_t1f2_rec);
    cBYE(status);
  }
  else {
    status = vec_f_to_s(nn_X, I1, NULL, nR1, "sum", rslt_buf, BUFLEN);
    cBYE(status);
    status = explode(rslt_buf, ':', &Y, &nY);
    if ( nY != 2 ) { go_BYE(-1); }
    rs_munmap(nn_X, nn_nX);
    if ( strcmp(Y[0], Y[1]) == 0 ) {
      /* num non null values == nR => nn field not needed */
      status = q_delete(nn_ddir_id, nn_fileno); cBYE(status);
    }
    else {
      zero_fld_rec(&nn_t1f2_rec); nn_t1f2_rec.fldtype = I1;
      status = add_aux_fld(NULL, t1_id, NULL, t1f2_id, nn_ddir_id, 
			   nn_fileno, nn, &nn_t1f2_id, &nn_t1f2_rec);
      cBYE(status);
    }
  }
  /*-------------------------------------------------------*/
 BYE:
  if ( Y != NULL ) { 
    for ( int i = 0; i < nY; i++ ) { 
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  rs_munmap(nn_X1, nn_nX1);
  rs_munmap(nn_X2, nn_nX2);
  rs_munmap(t2f2_X, n_t2f2_X);
  rs_munmap(nn_t2f2_X, nn_n_t2f2_X);
  return(status);
}
