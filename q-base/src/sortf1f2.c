#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "fop.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "set_meta.h"

#include "qsort_asc_val_I4_idx_I4.h"
#include "qsort_asc_val_I4_idx_I8.h"
#include "qsort_asc_val_I8_idx_I4.h"
#include "qsort_asc_val_I8_idx_I8.h"
#include "qsort_dsc_val_I4_idx_I4.h"
#include "qsort_dsc_val_I4_idx_I8.h"
#include "qsort_dsc_val_I8_idx_I4.h"
#include "qsort_dsc_val_I8_idx_I8.h"

/* str_op_spec can be one of following
   A => f2 is null
   D => f2 is null
   AA => sort f1 A primary and sort f2 A secondary
   A_ => sort f1 A primary and f2 is drag along
   AD => sort f1 A primary and sort f2 D secondary

   DA => sort f1 D primary and sort f2 A secondary
   D_ => sort f1 D primary and f2 is drag along
   DD => sort f1 D primary and sort f2 D secondary
   */

// last review 9/22/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
sortf1f2(
       char *tbl,
       char *f1,
       char *f2, 
       char *str_op_spec 
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec, f2_rec, nn_f2_rec;
  int tbl_id, f1_id, f2_id, nn_f1_id, nn_f2_id;
  long long nR; 

  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id< 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  // Requirements (1) no null values (2) not external field 
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( nn_f1_id >= 0 ) { go_BYE(-1); } 
  if ( f1_rec.is_external ) { go_BYE(-1); }
  status = get_data(f1_rec, &X1, &nX1, true); cBYE(status);
  //--------------------------------------------------------
  // Requirements (1) no null values (2) not external field 
  status = is_fld(NULL, tbl_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec); 
  cBYE(status);
  if ( f2_id < 0 ) { go_BYE(-1); }
  if ( nn_f2_id >= 0 ) { go_BYE(-1); } 
  if ( f2_rec.is_external ) { go_BYE(-1); }
  status = get_data(f2_rec, &X2, &nX2, true); cBYE(status);
  //--------------------------------------------------------
  if ( strcmp(str_op_spec,"A_" ) == 0 ) { 
    if ( ( f1_rec.fldtype == I4 ) &&  ( f2_rec.fldtype == I4 ) ) {
      qsort_asc_val_I4_idx_I4((int *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( f1_rec.fldtype == I4 ) &&  ( f2_rec.fldtype == I8 ) ) {
      qsort_asc_val_I4_idx_I8((long long *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( f1_rec.fldtype == I8 ) &&  ( f2_rec.fldtype == I4 ) ) {
      qsort_asc_val_I8_idx_I4((int *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( f1_rec.fldtype == I8 ) &&  ( f2_rec.fldtype == I8 ) ) {
      qsort_asc_val_I8_idx_I8((long long *)X2, (void *)X1, nR, NULL);
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"AD" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
  }
  else if ( strcmp(str_op_spec,"DA" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
  }
  else if ( strcmp(str_op_spec,"D_" ) == 0 ) { 
    if ( ( f1_rec.fldtype == I4 ) &&  ( f2_rec.fldtype == I4 ) ) {
      qsort_dsc_val_I4_idx_I4((int *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( f1_rec.fldtype == I4 ) &&  ( f2_rec.fldtype == I8 ) ) {
      qsort_dsc_val_I4_idx_I8((long long *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( f1_rec.fldtype == I8 ) &&  ( f2_rec.fldtype == I4 ) ) {
      qsort_dsc_val_I8_idx_I4((int *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( f1_rec.fldtype == I8 ) &&  ( f2_rec.fldtype == I8 ) ) {
      qsort_dsc_val_I8_idx_I8((long long *)X2, (void *)X1, nR, NULL);
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"DD" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
  }
  else { go_BYE(-1); }
  // Set meta data indicating sort status of A
  if ( *str_op_spec == 'A' ) { 
    status = int_set_meta(tbl_id, f1_id, "srttype", "ascending", true); 
    cBYE(status);
  }
  else if ( *str_op_spec == 'D' ) { 
    status = int_set_meta(tbl_id, f1_id, "srttype", "descending", true); 
    cBYE(status);
  }
  else { go_BYE(-1);  }
  //-------------------------------------------------------
BYE:
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  return status ;
}
