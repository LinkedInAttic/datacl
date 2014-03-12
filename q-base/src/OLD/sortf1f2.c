#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "fop.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

#include "qsort_asc_int.h"
#include "qsort_asc_longlong.h"
#include "qsort_dsc_int.h"
#include "qsort_dsc_longlong.h"

#include "qsort_asc_val_int_idx_int.h"
#include "qsort_asc_val_longlong_idx_int.h"
#include "qsort_asc_val_int_idx_longlong.h"
#include "qsort_asc_val_longlong_idx_longlong.h"

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
  FLD_TYPE *f1_meta = NULL, *f2_meta = NULL;
  int tbl_id, f1_id, f2_id;
  long long nR; 

  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  if ( strlen(str_op_spec) >= 1 ) { 
    if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  }
  else if ( strlen(str_op_spec) == 2 ) { 
    if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
    if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }
  }
  else { go_BYE(-1); }
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  /* Since we are sorting in place, field must not be external */
  f1_meta = &(g_fld[f1_id]);
  if ( f1_meta->is_external ) { go_BYE(-1); }
  status = rs_mmap(f1_meta->filename, &X1, &nX1, 1); // modifying
  cBYE(status);

  if ( ( f2 != NULL )  && ( *f2 != '\0' ) ) { 
    status = is_fld(NULL, tbl_id, f2, &f2_id); cBYE(status);
    chk_range(f2_id, 0, g_n_fld);
    f2_meta = &(g_fld[f2_id]);
    /* Since we are sorting in place, field must not be external */
    if ( f2_meta->is_external ) { go_BYE(-1); }
    status = rs_mmap(f2_meta->filename, &X2, &nX2, 1); // modifying
    cBYE(status);
  }
  if ( strcmp(str_op_spec,"A" ) == 0 ) { 
    if ( strcmp(f1_meta->fldtype, "int") == 0 ) {
      qsort_asc_int(X1, nR, sizeof(int), NULL);
    }
    else if ( strcmp(f1_meta->fldtype, "long long") == 0 ) {
      qsort_asc_int(X1, nR, sizeof(long long), NULL);
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"D" ) == 0 ) { 
    if ( strcmp(f1_meta->fldtype, "int") == 0 ) {
      qsort_dsc_int(X1, nR, sizeof(int), NULL);
    }
    else if ( strcmp(f1_meta->fldtype, "long long") == 0 ) {
      qsort_dsc_longlong(X1, nR, sizeof(int), NULL);
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"AA" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
    if ( strcmp(f1_meta->fldtype, "int") == 0 ) {
    }
    else if ( strcmp(f1_meta->fldtype, "long long") == 0 ) {
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"A_" ) == 0 ) { 
    if ( ( strcmp(f1_meta->fldtype, "int") == 0 ) && 
         ( strcmp(f2_meta->fldtype, "int") == 0 ) ) {
      qsort_asc_val_int_idx_int((int *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( strcmp(f1_meta->fldtype, "int") == 0 ) && 
         ( strcmp(f2_meta->fldtype, "long long") == 0 ) ) {
      qsort_asc_val_int_idx_longlong((long long *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( strcmp(f1_meta->fldtype, "long long") == 0 ) && 
         ( strcmp(f2_meta->fldtype, "int") == 0 ) ) {
      qsort_asc_val_longlong_idx_int((int *)X2, (void *)X1, nR, NULL);
    }
    else if ( ( strcmp(f1_meta->fldtype, "long long") == 0 ) && 
         ( strcmp(f2_meta->fldtype, "long long") == 0 ) ) {
      qsort_asc_val_longlong_idx_longlong((long long *)X2, (void *)X1, nR, NULL);
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"AD" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
    if ( strcmp(f1_meta->fldtype, "int") == 0 ) {
    }
    else if ( strcmp(f1_meta->fldtype, "long long") == 0 ) {
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"DA" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
    if ( strcmp(f1_meta->fldtype, "int") == 0 ) {
    }
    else if ( strcmp(f1_meta->fldtype, "long long") == 0 ) {
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(str_op_spec,"D_" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
  }
  else if ( strcmp(str_op_spec,"DD" ) == 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
    if ( strcmp(f1_meta->fldtype, "int") == 0 ) {
    }
    else if ( strcmp(f1_meta->fldtype, "long long") == 0 ) {
    }
    else { go_BYE(-1); }
  }
  else { go_BYE(-1); }
  // Set meta data indicating sort status of A
  if ( *str_op_spec == 'A' ) { 
    g_fld[f1_id].sorttype = 1;
  }
  else if ( *str_op_spec == 'D' ) { 
    g_fld[f1_id].sorttype = 1;
  }
  else { go_BYE(-1);  }
  //-------------------------------------------------------
BYE:
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  return(status);
}
