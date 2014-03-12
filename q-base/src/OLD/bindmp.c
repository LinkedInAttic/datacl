#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "aux_fld_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "f_to_s.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
bindmp(
       char *tbl,
       char *str_flds,
       char *cfld,
       char *opfile
       )
// STOP FUNC DECL
{
  int status = 0;
  char **Xs = NULL; size_t *nXs = 0;
  char *X = NULL;  size_t nX = 0;
  FLD_TYPE **fld_metas = NULL;
  char **flds = NULL; int *fld_size = NULL;
  long long nR, nn_nR; int n_flds = -1;
  int tbl_id = INT_MIN, fld_id = INT_MIN, cfld_id = INT_MIN;
  FLD_TYPE *cfld_meta = NULL; char *cfld_X = NULL; size_t cfld_nX = 0;
  int rec_size;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( opfile == NULL ) || ( *opfile == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = explode(str_flds, ':', &flds, &n_flds);
  cBYE(status);
  //--------------------------------------------------------
  fld_metas = (FLD_TYPE **)malloc(n_flds * sizeof(FLD_TYPE *));
  return_if_malloc_failed(fld_metas);
  for ( int i = 0; i < n_flds; i++ ) { 
    fld_metas[i] = NULL;
  }
  Xs = (char **)malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(Xs);
  nXs = (size_t *)malloc(n_flds * sizeof(size_t));
  return_if_malloc_failed(nXs);
  for ( int i = 0; i < n_flds; i++ ) { 
    Xs[i] = NULL; 
    nXs[i] = 0;
  }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  /* If there is a condition field for the export, get a handle on it */
  if ( ( cfld != NULL ) && ( *cfld != '\0' ) )  {
    status = is_fld(NULL, tbl_id, cfld, &cfld_id); cBYE(status);
    chk_range(cfld_id, 0, g_n_fld);
    cfld_meta = &(g_fld[cfld_id]);
    status = rs_mmap(cfld_meta->filename, &cfld_X, &cfld_nX, 0);
    cBYE(status);
  }
  //--------------------------------------------------------
  /* Get meta data for all fields to export. Also, get record size */
  rec_size = 0;
  fld_size = (int *)malloc(n_flds * sizeof(int));
  for ( int i = 0; i < n_flds; i++ ) { 
    status = is_fld(NULL, tbl_id, flds[i], &fld_id); cBYE(status);
    chk_range(fld_id, 0, g_n_fld);
    fld_metas[i] = &(g_fld[fld_id]);
    status = rs_mmap(fld_metas[i]->filename, &(Xs[i]), &(nXs[i]), 0);
    cBYE(status);
    rec_size += fld_metas[i]->n_sizeof;
    fld_size[i] = fld_metas[i]->n_sizeof;
    if ( ( strcmp(fld_metas[i]->fldtype, "int") == 0 ) || 
         ( strcmp(fld_metas[i]->fldtype, "bool") == 0 ) || 
         ( strcmp(fld_metas[i]->fldtype, "char") == 0 ) || 
         ( strcmp(fld_metas[i]->fldtype, "long long") == 0 ) || 
         ( strcmp(fld_metas[i]->fldtype, "float") == 0 ) || 
         ( strcmp(fld_metas[i]->fldtype, "double") == 0 ) ) {
      /* all is well */
    }
    else {
      fprintf(stderr, "Cannot bindmp fldtype = %s \n", fld_metas[i]->fldtype);
      go_BYE(-1);
    }
  }
  /* allocate space for output */
  if ( cfld_X == NULL ) { 
    nn_nR = nR;
  }
  else {
    char *endptr; 
    char str_rslt[16];
    zero_string(str_rslt, 16);
    status = f_to_s(tbl, cfld, "sum", str_rslt); cBYE(status);
    nn_nR = strtol(str_rslt, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( nn_nR == 0 ) {
      fprintf(stderr, "WARNING! Nothing to dump\n"); goto BYE;
    }
  }
  status = mk_file(opfile, (rec_size * nn_nR)); cBYE(status);
  status = rs_mmap(opfile, &X, &nX, 1); cBYE(status);
  //------------------------
  long long chk_nR = 0;
  for ( long long i = 0; i < nR; i++ ) {
    /* Skip this row if condition field asks you to do so */
    if ( ( cfld_X == NULL ) || ( cfld_X[i] == TRUE ) ) {
      chk_nR++;
      for ( int j = 0; j < n_flds; j++ ) {
        int fldsz = fld_size[j];
        memcpy(X, Xs[j], fldsz);
        X += fldsz;
        Xs[j] += fldsz;
      }
    }
    else {
      for ( int j = 0; j < n_flds; j++ ) {
        int fldsz = fld_size[j];
        Xs[j] += fldsz;
      }
    }
  }
  if ( chk_nR != nn_nR ) { go_BYE(-1); }
 BYE:
  rs_munmap(X, nX);
  for ( int i = 0; i < n_flds; i++ ) { 
    rs_munmap(Xs[i], nXs[i]);
    free_if_non_null(flds[i]);
  }
  rs_munmap(cfld_X, cfld_nX);
  free_if_non_null(Xs);
  free_if_non_null(nXs);
  free_if_non_null(fld_metas);
  free_if_non_null(flds);
  free_if_non_null(fld_size);
  return(status);
}
