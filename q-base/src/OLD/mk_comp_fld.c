#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "open_temp_file.h"
#include "aux_fld_meta.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
mk_comp_fld(
	 char *tbl,
	 char *str_flds,
	 char *str_sizes,
	 char *newfldtype,
	 char *fout
	 )
// STOP FUNC DECL
{
  int status = 0;
  char **X = NULL;    size_t *nX = 0;
  char *out_X = NULL; size_t out_nX = 0;
  FLD_TYPE **flds_meta = NULL; int *flds_id = NULL;
  char *filename = NULL;
  long long nR; 
  int tbl_id = INT_MIN; 
  int fout_id = INT_MIN;
  char str_meta_data[1024];
  FILE *fp = NULL;
  char **flds = NULL; int *sizes = NULL; int n_flds = -1;
  char *buffer = NULL;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_sizes == NULL ) || ( *str_sizes == '\0' ) ) { go_BYE(-1); }
  if ( ( newfldtype == NULL ) || ( *newfldtype == '\0' ) ) { go_BYE(-1); }
  if ( ( fout == NULL ) || ( *fout == '\0' ) ) { go_BYE(-1); }
  if ( ( strcmp(newfldtype, "long long" ) != 0 ) &&
       ( strcmp(newfldtype, "int" ) != 0 ) ) { go_BYE(-1); }
  cBYE(status);
  zero_string(str_meta_data, 1024);
  zero_string(buffer, 32);
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  //--------------------------------------------------------
  /* Break up str_flds */
  n_flds = 1; 
  for ( char *cptr = str_flds; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) { n_flds++; }
  }
  if ( n_flds <= 1 ) { go_BYE(-1); }
  flds = malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(flds);
  for ( int i = 0; i < n_flds; i++ ) { 
    flds[i] = malloc((MAX_LEN_FLD_NAME+1) * sizeof(char *));
    return_if_malloc_failed(flds[i]);
    zero_string(flds[i], (MAX_LEN_FLD_NAME+1));
  }
  int fld_idx = 0; int i = 0;
  for ( char *cptr = str_flds; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) { 
      fld_idx++; i = 0; 
    }
    else {
      flds[fld_idx][i++] = *cptr;
    }
  }
  /* Break up str_sizes */
  int n_sizes = 1; 
  int buflen = 0;
  for ( char *cptr = str_sizes; *cptr != '\0'; cptr++ ) { 
    buflen++;
    if ( *cptr == ':' ) { n_sizes++; }
  }
  if ( n_sizes != n_flds ) { go_BYE(-1); }
  buffer = malloc(buflen * sizeof(char));
  return_if_malloc_failed(buffer);
  zero_string(buffer, buflen);
  sizes = malloc(n_flds * sizeof(int));
  return_if_malloc_failed(sizes);
  fld_idx = 0; i = 0;
  int sum_sizes = 0;
  char *endptr;
  for ( char *cptr = str_sizes; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) { 
      sizes[fld_idx] = strtoll(buffer, &endptr, 10);
      zero_string(buffer, buflen);
      if ( sizes[fld_idx] < 0 ) { go_BYE(-1); }
      sum_sizes += sizes[fld_idx];
      fld_idx++;
      i = 0; 
    }
    else {
      buffer[i++] = *cptr;
    }
  }
  long long filesz;
  int n_sizeof;
  /* NOTE IMPLEMENTATION DEPENDENT */
  if ( strcmp(newfldtype, "long long") == 0 ) {
    if ( sum_sizes > 64 ) { go_BYE(-1); }
    filesz = nR * sizeof(long long);
    n_sizeof = 8;
  }
  else if ( strcmp(newfldtype, "int") == 0 ) {
    if ( sum_sizes > 32 ) { go_BYE(-1); }
    filesz = nR * sizeof(int);
    n_sizeof = 4;
  }
  X = malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(X);
  nX = malloc(n_flds * sizeof(size_t));
  return_if_malloc_failed(nX);
  flds_meta = malloc(n_flds * sizeof(FLD_TYPE *));
  return_if_malloc_failed(flds_meta);
  for ( int i = 0; i < n_flds; i++ ) { 
    X[i] = NULL; nX[i] = 0;
    flds_meta[i] = NULL;
  }
  //--------------------------------------------------------
  for ( int i = 0; i < n_flds; i++ ) { 
    status = is_fld(NULL, tbl_id, flds[i], &(flds_id[i])); cBYE(status);
    chk_range(flds_id[i], 0, g_n_fld);
    flds_meta[i] = &(g_fld[flds_id[i]]);
    status = rs_mmap(flds_meta[i]->filename, &(X[i]), &(nX[i]), 0); cBYE(status);
    if ( g_fld[flds_id[i]].nn_fld_id >= 0 ) { go_BYE(-1); }
    if ( strcmp(flds_meta[i]->fldtype, "int") != 0 ) { go_BYE(-1); }
  }
  // Make space to write 
  status = open_temp_file(&fp, &filename, -1); cBYE(status);
  status = mk_file(filename, filesz); cBYE(status);
  status = rs_mmap(filename, &out_X, &out_nX, 1); // for writing
  //--------------------------------------------------------
  sprintf(str_meta_data, "fldtype=%s:n_sizeof=%d:filename=%s",
      newfldtype, n_sizeof, filename);
  status = add_fld(tbl, flds[i], str_meta_data, &fout_id);
  cBYE(status);
 BYE:
  if ( flds != NULL ) { 
    for ( int i = 0; i  < n_flds; i++ ) { free_if_non_null(flds[i]); }
    free_if_non_null(flds); 
  }
  if ( X != NULL ) { 
    for ( int i = 0; i  < n_flds; i++ ) { rs_munmap(X[i], nX[i]); }
    free_if_non_null(X); 
    free_if_non_null(nX); 
  }

  free_if_non_null(flds_meta); 
  free_if_non_null(filename);
  free_if_non_null(buffer);
  return(status);
}
