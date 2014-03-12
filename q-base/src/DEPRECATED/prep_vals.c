#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "s_to_f.h"
#include "f_to_s.h"
#include "dup_fld.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "open_temp_file.h"
#include "mk_file.h"
#include "qsort_asc_val_int_idx_int.h"
#include "qsort_asc_val_int_idx_longlong.h"
#include "qsort_asc_val_longlong_idx_int.h"
#include "qsort_asc_val_longlong_idx_longlong.h"

// START FUNC DECL
int
prep_vals(
	  char *docroot,
	  sqlite3 *db,
	  char *tbl,
	  char *fld,
	  long long nR, /* number of values */
	  long long *ptr_nn_nR, /* number of non-null values */
	  FLD_META_TYPE val_meta, // used only for source, not dest
	  FLD_META_TYPE nn_val_meta, // used only for source, not dest
	  FLD_META_TYPE lnk_meta,
	  FLD_META_TYPE nn_lnk_meta,
	  char **ptr_cpy_lnk_X,
	  size_t *ptr_cpy_lnk_nX,
	  char **ptr_cpy_idx_X,
	  size_t *ptr_cpy_idx_nX,
	  int *ptr_sizeof_idx,
	  char **ptr_cpy_lnk_file,
	  char **ptr_cpy_idx_file
	  )
// STOP FUNC DECL
{
  int status = 0;
  long long nn_nR = -1;
  bool idx_is_longlong;
  FLD_META_TYPE cpy_lnk_meta, cpy_idx_meta;
  char *lnk_X = NULL; size_t lnk_nX = 0; 
  char *nn_lnk_X = NULL; size_t nn_lnk_nX = 0; 
  char *nn_val_X = NULL; size_t nn_val_nX = 0; 
  char *cpy_lnk_X = NULL; size_t cpy_lnk_nX = 0; 
  char *cpy_idx_X = NULL; size_t cpy_idx_nX = 0; 
  FILE *slfp = NULL, *sifp = NULL;
  char *cpy_lnk_file = NULL, *cpy_idx_file = NULL;
  int *iptr;
  bool is_idx_fld; int itemp;

  *ptr_sizeof_idx = -1;
  /* START: Check to see whether we can minimize work */
  /* Our best case is when 
      (a) no null values 
      (b) val field is sorted ascending order
      (c) idx field exists */
  /* Check whether there are null values in src (val or lnk) */
  if ( ( strlen(nn_lnk_meta.filename) == 0 ) && 
       ( strlen(nn_val_meta.filename) == 0 ) ) {
    is_non_null_in_src = true;
  }
  else 
    is_non_null_in_src = false;
  }
  /* Check whether an index field exists */
  status = is_fld(docroot, db, src_tbl, "id", &is_idx_fld, &itemp, &itemp);
  cBYE(status);
  if ( is_idx_fld ) { 
    status = fld_meta(docroot, db, src_tbl, "id", -1, &idx_meta);
    cBYE(status);
  }
  /* Determine type and size of index field */
  if ( nR >= INT_MAX ) { 
    idx_is_longlong = true;
    *ptr_sizeof_idx = sizeof(long long);
  }
  else {
    idx_is_longlong = false;
    *ptr_sizeof_idx = sizeof(int);
  }
  /* Check to see if val fld is sorted in ascending order */
  status = is_srt(docroot, db, src_tbl, "id", &srt_type);
  if ( srt_type == 1 ) { 
    is_val_fld_srt_asc = true; 
  } 
  else { 
    is_val_fld_srt_asc = false; 
  } 
  cBYE(status);
  /* STOP: Check to see whether we can minimize work */

  if ( ( is_no_null_in_src ) && ( is_val_fld_srt_asc ) && ( is_idx_fld ) ) {
    status = rs_mmap(lnk_meta.file_name, &cpy_lnk_X, &cpy_lnk_nX, 0);
    status = rs_mmap(idx_meta.file_name, &cpy_idx_X, &cpy_idx_nX, 0);
    *ptr_cpy_lnk_file = NULL; /* was not created */
    *ptr_cpy_idx_file = NULL; /* was not created */
  }
  else if ( ( is_no_null_in_src ) && ( is_val_fld_srt_asc ) && ( !is_idx_fld ) ) {
  }
  else if ( ( is_no_null_in_src ) && ( !is_val_fld_srt_asc ) && ( is_idx_fld ) ) {
  }
  else if ( ( is_no_null_in_src ) && ( !is_val_fld_srt_asc ) && ( !is_idx_fld ) ) {
  }
  else if ( !is_no_null_in_src ) { 
  }
  else { go_BYE(-1); /* Control should never come here */ }
    /* If src lnk and src val have no NULL values, do following */
    /* number of non-null vals = number of rows */
    nn_nR = nR; 
    /* If no index field exists, make one */
    if ( !is_idx_fld ) {
      if ( nR >= INT_MAX ) { 
        status = s_to_f(docroot, db, tbl, "_cpy_idx", 
		      "op=seq:start=0:incr=1:fldtype=long long"); 
        cBYE(status);
      }
      else { 
        status = s_to_f(docroot, db, tbl, "_cpy_idx", 
		      "op=seq:start=0:incr=1:fldtype=int"); 
        cBYE(status);
      }
      zero_fld_meta(&cpy_idx_meta);
    }
    else {
    }
    status = dup_fld(docroot, db, tbl, fld, "_cpy_lnk");
    cBYE(status);
    zero_fld_meta(&cpy_lnk_meta);
    status = fld_meta(docroot, db, tbl, "_cpy_lnk", -1, &cpy_lnk_meta);
    cBYE(status);
    status = rs_mmap(cpy_lnk_meta.filename, &cpy_lnk_X, &cpy_lnk_nX, 1);
    cBYE(status);
    status = rs_mmap(cpy_idx_meta.filename, &cpy_idx_X, &cpy_idx_nX, 1);
    cBYE(status);
  }
  else {
    register int sz = lnk_meta.n_sizeof;
    register char *cptr = NULL;
    nn_nR = 0;
    // slfp = source link file pointer 
    status = open_temp_file(&slfp, &cpy_lnk_file); cBYE(status);
    // sifp = source index file pointer 
    status = open_temp_file(&sifp, &cpy_idx_file); cBYE(status);
    //------------------------------------------
    status = rs_mmap(lnk_meta.filename, &lnk_X, &lnk_nX, 0);
    cBYE(status);
    if ( strlen(nn_lnk_meta.name) == 0 ) {
      nn_lnk_X = NULL;
    }
    else {
      status = rs_mmap(nn_lnk_meta.filename, &nn_lnk_X, &nn_lnk_nX, 0);
      cBYE(status);
    }
    if ( strlen(val_meta.name) == 0 ) {
      nn_val_X = NULL;
    }
    else {
      status = rs_mmap(nn_val_meta.filename, &nn_val_X, &nn_val_nX, 0);
      cBYE(status);
    }
    //------------------------------------------
    if ( nR >= INT_MAX ) { 
      idx_is_longlong = true;
      *ptr_sizeof_idx = sizeof(long long);
      cptr = lnk_X;
      for ( long long i = 0; i < nR; i++ ) { 
	if ( ( nn_lnk_X != NULL ) && ( nn_lnk_X[i] == FALSE ) ) { 
	  cptr+= sz; continue; 
	}
	if ( ( nn_val_X != NULL ) && ( nn_val_X[i] == FALSE ) ) { 
	  cptr+= sz; continue; 
	}
	fwrite(&i, sizeof(long long), 1, sifp);
	fwrite(cptr, sz, 1, slfp);
	cptr += sz;
	nn_nR++;
      }
    }
    else  {
      idx_is_longlong = false;
      *ptr_sizeof_idx = sizeof(int);
      cptr = lnk_X;
      for ( int i = 0; i < nR; i++ ) { 
	if ( ( nn_lnk_X != NULL ) && ( nn_lnk_X[i] == FALSE ) ) { 
	  cptr+= sz; continue; 
	}
	if ( ( nn_val_X != NULL ) && ( nn_val_X[i] == FALSE ) ) { 
	  cptr+= sz; continue; 
	}
	fwrite(&i, sizeof(int), 1, sifp);
	fwrite(cptr, sz, 1, slfp);
	cptr += sz;
	nn_nR++;
      }
    }
    // close and reopen using mmap with write privileges
    fclose_if_non_null(slfp);
    fclose_if_non_null(sifp);
    status = rs_mmap(cpy_lnk_file, &cpy_lnk_X, &cpy_lnk_nX, 1);
    cBYE(status);
    status = rs_mmap(cpy_idx_file, &cpy_idx_X, &cpy_idx_nX, 1);
    cBYE(status);
  }
  // STOP: Create non-null values of src link field and an index field 
  // Sort values created above
  if ( strcmp(lnk_meta.fldtype, "int") == 0 ) { 
    if ( idx_is_longlong ) { 
      qsort_asc_val_int_idx_longlong((long long *)cpy_idx_X, 
				     cpy_lnk_X, nn_nR, NULL);
      if ( (2*cpy_lnk_nX) != cpy_idx_nX ) { go_BYE(-1); }
    }
    else  {
    iptr = (int *)cpy_lnk_X;
    iptr = (int *)cpy_idx_X;
      qsort_asc_val_int_idx_int((int *)cpy_idx_X, cpy_lnk_X, 
				nn_nR, NULL);
      if ( cpy_lnk_nX != cpy_idx_nX ) { go_BYE(-1); }
    }
    iptr = (int *)cpy_lnk_X;
    iptr = (int *)cpy_idx_X;
  }
  else {
    if ( idx_is_longlong ) { 
      qsort_asc_val_longlong_idx_longlong((long long *)cpy_idx_X, 
					  cpy_lnk_X, nn_nR, NULL);
      if ( cpy_lnk_nX != cpy_idx_nX ) { go_BYE(-1); }
    }
    else  {
      qsort_asc_val_longlong_idx_int((int *)cpy_idx_X, cpy_lnk_X,
				     nn_nR, NULL);
      if ( cpy_lnk_nX != (2*cpy_idx_nX) ) { go_BYE(-1); }
    }
  }
  //--------------------------------------------------------
  *ptr_nn_nR = nn_nR;
  *ptr_cpy_lnk_X = cpy_lnk_X;
  *ptr_cpy_lnk_nX = cpy_lnk_nX;
  *ptr_cpy_idx_X = cpy_idx_X;
  *ptr_cpy_idx_nX = cpy_idx_nX;
  *ptr_cpy_lnk_file = cpy_lnk_file;
  *ptr_cpy_idx_file = cpy_idx_file;
 BYE:
  rs_munmap(nn_lnk_X, nn_lnk_nX);
  return(status);
}
