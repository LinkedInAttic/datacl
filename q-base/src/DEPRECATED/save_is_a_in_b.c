#include <stdio.h>
#include <unistd.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "bin_search.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_join.h"
#include "bin_search.h"
#include "get_nR.h"
#include "aux_fld_meta.h"
#include "mk_file.h"
#include "is_tbl.h"
#include "is_nn_fld.h"

/* This is used when f2 is sorted ascending but f1 is not */
/* Also assume that f2 has no null values */
/* Implementation uses binary search which I do not like */
/* Usually used when T1 is large and T2 is small */

//---------------------------------------------------------------
// START FUNC DECL
int 
is_a_in_b(
	    char *docroot,
	    sqlite3 *in_db,
	    char *t1,
	    char *f1,
	    char *t2,
	    char *f2,
	    char *cfld,
	    char *src_f2, /* fld_to_fetch_from_t2 */
	    char *dst_f1 /* fld_to_place_in_t1 */
	    )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *src_f2_X = NULL; size_t src_f2_nX = 0;
  char *dst_f1_X = NULL; size_t dst_f1_nX = 0;
  char *cfX = NULL; size_t cf_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *nn_src_f2_X = NULL; size_t nn_src_f2_nX = 0;
  FLD_META_TYPE f1_meta, f2_meta, src_f2_meta;
  FLD_META_TYPE nn_f1_meta, nn_src_f2_meta;
  int *if1 = NULL, *if2 = NULL, *isrc_f2, *idst_f1 = NULL; 
  int i1val, prev_i1val, isrc_val;
  long long *llf1 = NULL, *llf2 = NULL, *llsrc_f2, *lldst_f1 = NULL; 
  long long ll1val, prev_ll1val, llsrc_val;
  long long nR1, nR2;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *dst_ofp = NULL; char *dst_opfile = NULL;
  char str_meta_data[4096];
  int t1_id, t2_id, f2_id, ifldtype, src_f2_ifldtype;
  int iminval, imaxval; long long llminval, llmaxval;
  bool b_is_tbl, b_write_dst;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( cfld == NULL ) || ( *cfld == '\0' ) ) {  
    /* condition field NOT provided */
    if ( ( src_f2 == NULL ) || ( *src_f2 == '\0' ) || 
         ( dst_f1 == NULL ) || ( *dst_f1 == '\0' ) ) {
      go_BYE(-1);
    }
  }
  else {
    if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
    if ( ( src_f2 != NULL ) && ( *src_f2 != '\0' ) ) { go_BYE(-1); }
    if ( ( dst_f1 != NULL ) && ( *dst_f1 != '\0' ) ) { go_BYE(-1); }
  }
  //--------------------------------------------------------
  zero_string(str_meta_data, 4096);
  zero_fld_meta(&f1_meta); zero_fld_meta(&nn_f1_meta);
  zero_fld_meta(&f2_meta); /* Recall no null values for f2 */
  zero_fld_meta(&src_f2_meta); zero_fld_meta(&nn_src_f2_meta);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //--------------------------------------------------------
  status = is_tbl(docroot, db, t1, &b_is_tbl, &t1_id);
  cBYE(status);
  if ( !b_is_tbl ) { go_BYE(-1); }
  status = internal_get_nR(db, t1_id, &nR1);
  cBYE(status);
  status = fld_meta(docroot, db, t1, f1, -1, &f1_meta);
  cBYE(status);
  status = rs_mmap(f1_meta.filename, &f1_X, &f1_nX, 0);
  cBYE(status);
  status = mk_ifldtype(f1_meta.fldtype, &ifldtype);
  cBYE(status);

  status = is_tbl(docroot, db, t2, &b_is_tbl, &t2_id);
  cBYE(status);
  if ( !b_is_tbl ) { go_BYE(-1); }
  status = internal_get_nR(db, t2_id, &nR2);
  cBYE(status);
  status = fld_meta(docroot, db, t2, f2, -1, &f2_meta);
  cBYE(status);
  status = rs_mmap(f2_meta.filename, &f2_X, &f2_nX, 0);
  cBYE(status);
  if ( ( src_f2 != NULL ) && ( *src_f2 != '\0' ) ) { 
    status = fld_meta(docroot, db, t2, src_f2, -1, &src_f2_meta);
    cBYE(status);
    status = rs_mmap(src_f2_meta.filename, &src_f2_X, &src_f2_nX, 0);
    cBYE(status);
    status = get_aux_field_if_it_exists( docroot, db, t2, src_f2_meta.id, 
	"nn", &nn_src_f2_meta, &nn_src_f2_X, &nn_src_f2_nX);
    cBYE(status);
    status = mk_ifldtype(src_f2_meta.fldtype, &src_f2_ifldtype);
    cBYE(status);
  }
  /*------------------------------------------------------------*/
  if ( strcmp(f1_meta.fldtype, f2_meta.fldtype) != 0 ) {
    fprintf(stderr, "Fields being compared must be same type \n");
    go_BYE(-1); 
  } 
  /*------------------------------------------------------------*/
  // There can be null values in f1 but not in f2
  status = get_aux_field_if_it_exists(docroot, db, t1, f1_meta.id, "nn", 
      &nn_f1_meta, &nn_f1_X, &nn_f1_nX);
  cBYE(status);
  status = is_nn_fld(docroot, db, t2, f2, &f2_id, NULL);
  cBYE(status);
  if ( f2_id > 0 ) { 
    fprintf(stderr, "Field [%s] in Table [%s] has null values\n", f2, t2);
    fprintf(stderr, "NOT IMPLEMENTED\n");
    go_BYE(-1);
  }
  /* Determine whether a destination value needs to be written in T1 */
  if ( ( dst_f1 != NULL ) && ( *dst_f1 != '\0' ) ) {
    b_write_dst = true;
  }
  else { 
    b_write_dst = false;
  }
  /*------------------------------------------------------------*/
  /* Make space for output condition field */
  status = open_temp_file(&ofp, &opfile); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, nR1 * sizeof(char)); cBYE(status);
  status = rs_mmap(opfile, &cfX, &cf_nX, 1); cBYE(status);
  /* Make space for output value field */
  if ( ( dst_f1 != NULL ) && ( *dst_f1 != '\0' ) ) { 
    status = open_temp_file(&dst_ofp, &dst_opfile);
    cBYE(status);
    fclose_if_non_null(dst_ofp);
    status = mk_file(dst_opfile, nR1 * src_f2_meta.n_sizeof); cBYE(status);
    status = rs_mmap(dst_opfile, &dst_f1_X, &dst_f1_nX, 0);
  }
  //--------------------------------------------------------
  /* We can finally start with the computations */ 
  if1  = (int *)f1_X; llf1 = (long long *)f1_X;
  if2  = (int *)f2_X; llf2 = (long long *)f2_X;
  isrc_f2 = (int *)src_f2_X; llsrc_f2 = (long long *)src_f2_X;
  idst_f1 = (int *)dst_f1_X; lldst_f1 = (long long *)dst_f1_X;
  /* We find the smallest and largest values of f2 which allow us to
   * skip the binary search over the values of f2 */
  switch ( ifldtype ) { 
  case FLDTYPE_INT : 
    iminval = if2[0];
    imaxval = if2[nR2-1];
    break;
  case FLDTYPE_LONGLONG : 
    llminval = llf2[0];
    llmaxval = llf2[nR2-1];
    break;
  default : 
    go_BYE(-1);
    break;
  }
  /*------------------------------------------------------------------*/
  bool prev_defined = false; 
  /* prev_defined tells us whetrher prev_i1val or prev_ll1val are defined */
  long long prev_pos = -1; // undefined value 
  for ( long long i1 = 0; i1 < nR1; i1++ ) { 
    bool is_found;
    long long pos;
    is_found = false;
    /* If f1 is null, then cfld and dst_f1 must also be null */
    if ( ( nn_f1_X != NULL ) && ( nn_f1_X[i1] == FALSE ) ) {
      isrc_val = 0;
      cfX[i1] = FALSE;
      if ( b_write_dst ) { 
	lldst_f1[i1] = 0;
      }
      continue;
    }
    /*----------------------------------------------------------------*/
    switch ( ifldtype ) { 
    case FLDTYPE_INT : 
      i1val = if1[i1];
      if ( ( i1val < iminval ) || ( i1val > imaxval ) ) {
	pos = -1;
      }
      else {
	if ( ( prev_defined ) && ( i1val == prev_i1val ) ) { 
	  if ( prev_pos >= 0 ) { 
	    pos = prev_pos;
	  }
	  else {
	    status = bin_search_i(if2, nR2, i1val, &pos, "");
	    cBYE(status);
	  }
	}
	else {
	  status = bin_search_i(if2, nR2, i1val, &pos, "");
	  cBYE(status);
	}
      }
      if ( pos >= 0 ) { is_found = true; }
      if ( b_write_dst ) { 
	if ( pos >= 0 ) { 
	  isrc_val = isrc_f2[pos];
	}
	else {
	  isrc_val = 0;
	}
	idst_f1[i1] = isrc_val;
      }
      break;
    case FLDTYPE_LONGLONG : 
      ll1val = llf1[i1];
      if ( ( ll1val < llminval ) || ( ll1val > llmaxval ) ) {
	pos = -1;
      }
      else {
	if ( ( prev_defined ) && ( ll1val == prev_ll1val ) ) { 
	  if ( prev_pos >= 0 ) { 
	    pos = prev_pos;
	  }
	  else {
	    status = bin_search_ll(llf2, nR2, ll1val, &pos, "");
	    cBYE(status);
	  }
	}
	else {
	  status = bin_search_ll(llf2, nR2, ll1val, &pos, "");
	  cBYE(status);
	}
      }
      if ( pos >= 0 ) { is_found = true; } 
      if ( b_write_dst ) { 
	if ( pos >= 0 ) { 
	  llsrc_val = llsrc_f2[pos];
	}
	else {
	  llsrc_val = 0;
	}
	lldst_f1[i1] = llsrc_val;
      }
      break;
    default :
      go_BYE(-1);
      break;
    }
    prev_i1val = i1val;
    prev_ll1val = ll1val;
    prev_defined = true;
    if ( is_found ) { 
      cfX[i1] = TRUE;
    }
    else {
      cfX[i1] = FALSE;
    }
  }
  fclose_if_non_null(ofp);
  fclose_if_non_null(dst_ofp);
  // Add output field to meta data 
  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
    sprintf(str_meta_data, "n_sizeof=1:fldtype=bool:filename=%s", opfile);
    status = add_fld(docroot, db, t1, cfld, str_meta_data);
    cBYE(status);
  }
  else {
    switch ( src_f2_ifldtype ) { 
    case FLDTYPE_INT : 
      sprintf(str_meta_data, "n_sizeof=%lu:fldtype=int:filename=%s", 
	      sizeof(int), dst_opfile);
      break;
    case FLDTYPE_LONGLONG : 
      sprintf(str_meta_data, "n_sizeof=%lu:fldtype=int:filename=%s", 
	      sizeof(long long), dst_opfile);
      break;
    }
    status = add_fld(docroot, db, t1, dst_f1, str_meta_data);
    cBYE(status);
    status = add_aux_fld(docroot, db, t1, dst_f1, opfile, "nn");
    cBYE(status);
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(src_f2_X, src_f2_nX);
  rs_munmap(dst_f1_X, dst_f1_nX);
  rs_munmap(cfX, cf_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(nn_src_f2_X, nn_src_f2_nX);

  if ( in_db == NULL ) { sqlite3_close(db); }
  free_if_non_null(opfile);
  fclose_if_non_null(ofp);
  free_if_non_null(dst_opfile);
  fclose_if_non_null(dst_ofp);
  return(status);
}
