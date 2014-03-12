#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "set_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "meta_globals.h"
#include "t1f1t2f2opt3f3.h"

#include "assign_I4.h"
#include "extract_I4.h"
#include "extract_I8.h"
#include "extract_UI8.h"
#include "extract_S.h"

#include "union_I4.h"
#include "union_I8.h"

#include "intersection_I4.h"
#include "intersection_I8.h"

#include "a_minus_b_I4.h"
#include "a_minus_b_I8.h"

/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
t1f1t2f2opt3f3(
	       char *t1,
	       char *f1,
	       char *t2,
	       char *f2,
	       char *op,
	       char *options,
	       char *t3,
	       char *f3
	       )
/* STOP FUNC DECL */
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *f3_X = NULL; size_t f3_nX = 0;
  int t1_id = INT_MIN, t2_id = INT_MIN, t3_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN, f3_id = INT_MIN;
  int nn_f1_id = INT_MIN, nn_f2_id = INT_MIN;
  TBL_REC_TYPE t1_rec, t2_rec, t3_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE f2_rec, nn_f2_rec;
  FLD_REC_TYPE f3_rec;
  FLD_TYPE f3_type = undef_fldtype;
  long long nR1 = INT_MIN, nR2 = INT_MIN; 
  long long nR3 = INT_MIN, nR3_allocated = INT_MIN;
  int f3_fldsz = INT_MAX; size_t filesz;
  int ddir_id    = -1, fileno    = -1; 
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t3) == 0 ) { go_BYE(-1); }
  if ( ( t3 == NULL ) || ( *t3 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t2, t3) == 0 ) { go_BYE(-1); }

  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  if ( t1_id < 0 ) { go_BYE(-1); }
  nR1 = t1_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  /*--------------------------------------------------------*/
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  if ( t2_id < 0 ) { go_BYE(-1); }
  nR2 = t2_rec.nR; 
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec); 
  cBYE(status);
  if ( f2_id < 0 ) { go_BYE(-1); }
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  /*--------------------------------------------------------*/
  /* f1, f2 must be sorted ascending  */
  if ( f1_rec.srttype != ascending ) { go_BYE(-1); }
  if ( f2_rec.srttype != ascending ) { go_BYE(-1); }
  /* Have not implemented case where f1 has null field  */
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  if ( nn_f2_id >= 0 ) { go_BYE(-1); }
  if ( f1_rec.fldtype != f2_rec.fldtype ) { go_BYE(-1); }
  switch ( f1_rec.fldtype ) { 
    case I4 : case I8 : break;
    default : go_BYE(-1); break;
  }
  f3_type = f1_rec.fldtype;
  status = get_fld_sz(f3_type, &f3_fldsz); cBYE(status);

  if ( strcmp(op, "intersection") == 0 ) {
    nR3_allocated = min(nR1 , nR2);
  }
  else if ( strcmp(op, "a_minus_b") == 0 ) {
    nR3_allocated = nR1; /* upper bound. Truncate this later */
  }
  else if ( ( strcmp(op, "union") == 0 ) || 
            ( strcmp(op, "pvalcalc") == 0 ) ) {
    nR3_allocated = nR1 + nR2; /* nR1 + nR2 is upper bound. Reduce later */
  }
  else {
    go_BYE(-1);
  }

  filesz = f3_fldsz * nR3_allocated; 
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &f3_X, &f3_nX, true); cBYE(status);

  if ( strcmp(op, "intersection") == 0 ) {
    switch ( f1_rec.fldtype ) { 
      case I4 : 
	status = intersection_I4((int *)f1_X, nR1, (int *)f2_X, nR2, 
	    (int *)f3_X, &nR3);
	break;
      case I8 : 
	status = intersection_I8((long long *)f1_X, nR1, 
	    (long long *)f2_X, nR2, (long long *)f3_X, &nR3);
	break;
      default : go_BYE(-1); break;
    }
    cBYE(status);
  }
  else if ( strcmp(op, "a_minus_b") == 0 ) {
    switch ( f1_rec.fldtype ) { 
      case I4 : 
	a_minus_b_I4((int *)f1_X, nR1, (int *)f2_X, nR2, (int *)f3_X, &nR3);
	break;
      case I8 : 
	a_minus_b_I8((long long *)f1_X, nR1, (long long *)f2_X, nR2, (long long *)f3_X, &nR3);
	break;
      default : go_BYE(-1); break;
    }
    cBYE(status);
  }
  else if ( ( strcmp(op, "union") == 0 ) || 
            ( strcmp(op, "pvalcalc") == 0 ) ) {
    unsigned long long mask = 0;
    if ( strcmp(op, "pvalcalc") == 0 ) {
      bool is_null = false;
      status = extract_UI8(options, "mask=[", "]", &mask, &is_null);
      if ( is_null ) { go_BYE(-1); }
    }
    switch ( f1_rec.fldtype ) { 
      case I4 : 
        status = union_I4((int *)f1_X, nR1, (int *)f2_X, nR2, 
           (int *)f3_X, &nR3, (unsigned int)mask);
        cBYE(status);
      break;
      case I8 : 
        status = union_I8((long long *)f1_X, nR1, (long long *)f2_X, nR2, 
          (long long *)f3_X, &nR3, (unsigned long long)mask);
      cBYE(status);
      break;
      default : go_BYE(-1); break;
    }
  }
  else { go_BYE(-1); }
  rs_munmap(f3_X, f3_nX);
  if ( nR3 == 0 ) { 
    status = q_delete(ddir_id, fileno); cBYE(status); 
    ddir_id = fileno = -1; 
    go_BYE(0);
  }
  else if ( nR3 < nR3_allocated ) {
    status = q_trunc(ddir_id, fileno, (nR3 * f3_fldsz)); cBYE(status);
  }
  else if ( nR3 > nR3_allocated ) {
    go_BYE(-1);
  }

  char str_nR3[32]; sprintf(str_nR3, "%lld", nR3);
  status = add_tbl(t3, str_nR3, &t3_id, &t3_rec); cBYE(status);
  zero_fld_rec(&f3_rec); f3_rec.fldtype = f3_type; 
  status = add_fld(t3_id, f3, ddir_id, fileno, &f3_id, &f3_rec); cBYE(status);
  /* Since f1 and f2 are sorted ascending, so must f3 be  */
  if ( ( strcmp(op, "union") == 0 ) || ( strcmp(op, "intersection") == 0 ) || 
         ( strcmp(op, "a_minus_b") == 0 ) ) {
    status = int_set_meta(t3_id, f3_id, "srttype", "ascending", true); cBYE(status);
  }
  /*-----------------------------------------------------------*/
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(f3_X, f3_nX);
  return status ;
}
