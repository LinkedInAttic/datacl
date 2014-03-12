#include <stdio.h> 
#include <unistd.h>
#include <wchar.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "dbauxil.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_tbl.h"
#include "f_to_s.h"
#include "aux_fld_meta.h"
#include "add_aux_fld.h"
#include "mk_file.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
copy_fld(
	 char *t1,
	 char *f1,
	 char *cfld,
	 char *t2
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0; 
  char *nnopX = NULL;  size_t n_nnopX = 0; 
  char *opX = NULL;  size_t n_opX = 0; 
  char *nnX = NULL; size_t n_nnX = 0; 
  char *cfldX = NULL; size_t n_cfldX = 0;
  char *nncfldX = NULL; size_t n_nncfldX = 0;
  FLD_TYPE *f1_meta = NULL, *nn_f1_meta = NULL;
  FLD_TYPE *cfld_meta = NULL, *nn_cfld_meta = NULL;
  long long nR1, alt_nR2, nR2 = 0; 
  int t1_id = INT_MIN, t2_id = INT_MIN; 
  int t1f1_id = INT_MIN, t2f1_id = INT_MIN;
  int cfld_id = INT_MIN, nn_cfld_id = INT_MIN; 
  int nn_t1f1_id = INT_MIN, nn_t2f1_id = INT_MIN;
  char *endptr;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  char str_result[32]; char str_meta_data[1024]; 
  bool is_null_val_in_op = false;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  zero_string(str_result, 32);
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbl[t1_id].nR;

  status = is_tbl(t2, &t2_id); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbl[t2_id].nR;

  status = is_fld(NULL, t1_id, f1, &t1f1_id); cBYE(status);
  chk_range(t1f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[t1f1_id]);
  rs_mmap(f1_meta->filename, &X, &nX, 0);

  if ( *cfld != '\0' ) { 
    status = f_to_s(t1, cfld, "sum", str_result);
    cBYE(status);
    alt_nR2 = strtoll(str_result, &endptr, 10);
  }
  else {
    alt_nR2 = nR1;
  }
  if ( alt_nR2 != nR2 ) { 
    fprintf(stderr, "Incompatible number of rows %lld --> %lld \n",
	    alt_nR2, nR2);
    go_BYE(-1); 
  }
  // Get nn field for f1 if if it exists
  nn_t1f1_id = f1_meta->nn_fld_id;
  if ( nn_t1f1_id >= 0 ) { 
    nn_f1_meta = &(g_fld[nn_t1f1_id]);
    status = rs_mmap(nn_f1_meta->filename, &nnX, &n_nnX, 0);
  }
  /* Get a handle on the condition field if specified  */
  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
  status = is_fld(NULL, t1_id, cfld, &cfld_id); cBYE(status);
  if ( cfld_id >= 0 ) { 
    cfld_meta = &(g_fld[cfld_id]);
    status = rs_mmap(cfld_meta->filename, &cfldX, &n_cfldX, 0); cBYE(status);
    // Get nn field for cfld if if it exists
    nn_cfld_id = cfld_meta->nn_fld_id;
    if ( nn_cfld_id >= 0 ) { 
      nn_cfld_meta = &(g_fld[nn_cfld_id]);
      status = rs_mmap(nn_cfld_meta->filename, &nncfldX, &n_nncfldX, 0);
    }
  }
  }
  //--------------------------------------------------------
  // Check that you can handle the input 
  if ( ( strcmp(f1_meta->fldtype, "char string") == 0 ) || 
       ( f1_meta->n_sizeof == 0 ) ) { 
    go_BYE(-1);
  }
  /* Allocate space for output */
  long long filesz = f1_meta->n_sizeof * nR2;
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &opX, &n_opX, 1); cBYE(status);
  //--------------------------------------------------------
  /* Allocate space for output condition field if necessary */
  if ( nnX != NULL ) { 
    long long filesz = sizeof(char) * nR2;
    status = open_temp_file(&nn_ofp, &nn_opfile, filesz); cBYE(status);
    fclose_if_non_null(nn_ofp);
    status = mk_file(nn_opfile, filesz); cBYE(status);
    status = rs_mmap(nn_opfile, &nnopX, &n_nnopX, 1); cBYE(status);
  }
  //--------------------------------------------------------
  // Get started
  if ( cfldX == NULL ) { 
    /* Optimization when no condition field */
    memcpy(opX, X, nR1 * f1_meta->n_sizeof);
    if ( nnX != NULL ) { 
      memcpy(nnopX, nnX, nR1 * sizeof(char));
      is_null_val_in_op = true;
    }
  }
  else {
    int rec_size = f1_meta->n_sizeof;
    char *bak_X = X, *bak_nnX = nnX, *bak_opX = opX, *bak_nnopX = nnopX;

#undef TESTING
#ifdef TESTING
    long long dbg_opi = 0;
    long long itemp = 0;
    for ( long long i = 0; i < nR1; i++ ) {
      if ( nncfldX == NULL ) { 
	if ( cfldX[i] == TRUE ) {
	  itemp++;
	}
      }
      else {
	if ( ( cfldX[i] == TRUE ) && ( nncfldX[i] == TRUE ) ) { 
	  itemp++;
	}
      }
    }
    if ( itemp != nR2 ) { 
      fprintf(stderr, "itemp = %lld, nR2 = %lld \n",  itemp, nR2);
      go_BYE(-1); 
    }
#endif
    for ( long long i = 0; i < nR1; i++ ) {
      bool is_copy = false;
      if ( nncfldX == NULL ) {
	if ( cfldX[i] == TRUE ) {
	  is_copy = true;
	}
      }
      else {
        if ( ( cfldX[i] == TRUE ) && ( nncfldX[i] == TRUE ) ) { 
	  is_copy = true;
	}
      }
      if ( is_copy ) { 
	memcpy(opX, X, rec_size);
        opX += rec_size;
	if ( nnX != NULL ) { 
	  memcpy(nnopX, nnX, sizeof(char));
          nnopX += sizeof(char);
	  if ( *nnopX == FALSE ) { 
	    is_null_val_in_op = true;
	  }
	}
#ifdef TESTING
	dbg_opi++;
#endif
      }
      X += rec_size;
      if ( nnX != NULL ) { nnX++;  }
    }
#ifdef TESTING
    if ( dbg_opi != nR2 ) { 
      fprintf(stderr, "dbg_opi = %lld, nR2 = %lld \n",  dbg_opi, nR2);
      go_BYE(-1); 
    } 
#endif
    X      = bak_X;
    nnX    = bak_nnX;
    opX   = bak_opX;
    nnopX = bak_nnopX;
  }

  // Add field to meta data store 
  sprintf(str_meta_data, "fldtype=%s:n_sizeof=%d:filename=%s",
	  f1_meta->fldtype, f1_meta->n_sizeof, opfile);
  status = add_fld(t2, f1, str_meta_data, &t2f1_id);
  cBYE(status);
  /* Sort status stays the same */
  g_fld[t2f1_id].sorttype = g_fld[t1f1_id].sorttype;
  if ( ( nnX != NULL ) && ( is_null_val_in_op ) ) {
    status = add_aux_fld(t2, f1, nn_opfile, "nn", &nn_t2f1_id);
    cBYE(status);
  }
  else {
    if ( nn_opfile != NULL ) { unlink(nn_opfile); }
  }
 BYE:
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  rs_munmap(X, nX);
  rs_munmap(nnX, n_nnX);
  rs_munmap(opX, n_opX);
  rs_munmap(nnopX, n_nnopX);
  rs_munmap(cfldX, n_cfldX);
  rs_munmap(nncfldX, n_nncfldX);
  return(status);
}

// START FUNC DECL
int
copy_nn_vals(
	     char *X_in,
	     long long n_in,
	     char *nn_X_in,
	     char *X_out,
	     long long n_out,
	     int rec_size
	     )
// STOP FUNC DECL
{
  int status = 0;
  long long idx = 0;
  for ( long long i = 0; i < n_in; i++ ) { 
    if ( nn_X_in[i] == TRUE ) { 
      memcpy(X_out, X_in, rec_size);
      X_out += rec_size;
      idx++;
    }
    X_in += rec_size;
  }
  if ( idx != n_out ) { go_BYE(-1); }
 BYE:
  return(status);
}
