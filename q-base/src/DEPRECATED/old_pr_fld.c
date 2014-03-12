#include <stdio.h>
#include <wchar.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "dbauxil.h"
#include "pr_fld.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "sort.h"

#define FLDTYPE_WCHAR_T 100
#define FLDTYPE_CHAR    200
#define DBG_BUF_LEN 32
//---------------------------------------------------------------
int 
pr_fld(
       char *docroot,
       sqlite3 *in_db,
       char *tbl,
       char *fld,
       char *cfld,
       char *ordr_fld,
       FILE *ofp
       )
{
  int status = 0;
  sqlite3 *db = NULL;
  int *uiptr = NULL, *szptr = NULL;
  wchar_t *wcptr; char *cptr; 
  char *X = NULL; size_t nX = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;
  char *ordr_X = NULL; size_t ordr_nX = 0;
  char *sz_X = NULL; size_t sz_nX = 0;
  FLD_META_TYPE x_fld_meta, cfld_meta, ordr_fld_meta;
  FLD_META_TYPE sz_fld_meta; int sz_fld_id;
  int *ordr = NULL, *chk_ordr = NULL; int idx;
  bool is_fixed_len, pr_done;
  int ifldtype, nw_i, nC = INT_MAX, nR;
  wchar_t *wcformat = L"%c";
  wchar_t wceoln = L'\n'; wchar_t wcdquote = L'"'; 
  int *offset = NULL;
  wchar_t wcbuf[DBG_BUF_LEN];
  char cbuf[DBG_BUF_LEN];
  int wcbufptr, cbufptr;
  char qstr[4096];
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ofp == NULL ) { go_BYE(-1); }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  for ( int i = 0; i < DBG_BUF_LEN; i++ ) { wcbuf[i] = L'\0'; }
  for ( int i = 0; i < DBG_BUF_LEN; i++ ) { cbuf[i] = '\0'; }
  zero_string(qstr, 4096);
  //--------------------------------------------------------
  status = fld_meta(docroot, db, tbl, fld, -1, &x_fld_meta);
  cBYE(status);
  status = rs_mmap(x_fld_meta.filename, &X, &nX, 0);
  cBYE(status);
  sprintf(qstr, "select nR from tbl where name = '%s' ", tbl);
  status = db_get_ival(db, qstr, &nR);
  cBYE(status);
  if ( nR <= 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = mk_ifldtype(x_fld_meta.fldtype, &ifldtype);
  cBYE(status);
  //--------------------------------------------------------
  if ( ( ordr_fld != NULL ) && ( *ordr_fld != '\0' ) ) {
    status = fld_meta(docroot, db, tbl, ordr_fld, -1, &ordr_fld_meta);
    cBYE(status);
    status = rs_mmap(ordr_fld_meta.filename, &ordr_X, &ordr_nX, 0);
    cBYE(status);
    ordr = (int *)ordr_X;
#ifdef DEBUG
    /* Make sure that ordr is a permutation of 0, ... nR-1 */
    chk_ordr = (int*)malloc(nR * sizeof(int));
    return_if_malloc_failed(chk_ordr);
    memcpy(chk_ordr, ordr, ordr_nX);
    qsort(chk_ordr, nR, sizeof(int), sort_int_a_compare);
    for ( int i = 0; i < nR; i++ ) { 
      if ( chk_ordr[i] != i ) { go_BYE(-1); }
    }
    return_if_malloc_failed(chk_ordr);
#endif
    }
  //--------------------------------------------------------
  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
    status = fld_meta(docroot, db, tbl, cfld, 01, &cfld_meta);
    cBYE(status);
    status = rs_mmap(cfld_meta.filename, &cfld_X, &cfld_nX, 0);
    cBYE(status);
  }
  //--------------------------------------------------------
  if ( strcmp(x_fld_meta.fldtype, "I") == 0 ) {
    uiptr = (int *)X;
    if ( x_fld_meta.n_sizeof == 0 ) { /* variable length */
      zero_fld_meta(&sz_fld_meta);
      status = get_aux_fld_id(docroot, db, x_fld_meta.id, "sz", &sz_fld_id);
      cBYE(status);
      status = fld_meta(docroot, db, tbl, NULL, sz_fld_id, &sz_fld_meta);
      cBYE(status);
      status = rs_mmap(sz_fld_meta.filename, &sz_X, &sz_nX, 0);
      cBYE(status);
      szptr = (int *)sz_X;
      for ( int i = 0; i < nR; i++ ) {
        idx = i;
        if ( ordr != NULL ) { idx = ordr[idx]; }
	nw_i = szptr[idx];
	for ( int j = 0; j < nw_i; j++ ) { 
	  fprintf(ofp, "%d ", *uiptr++); // FIX 
	}
        fprintf(ofp, "\n");
      }
    }
    else if ( x_fld_meta.n_sizeof == 4 ) { 
      chk_file_size(nX, sizeof(int));
      for ( int i = 0; i < nR; i++ ) {
        idx = i;
        if ( ordr != NULL ) { idx = ordr[idx]; }
        if ( ( cfld_X == NULL ) || ( cfld_X[idx] == TRUE ) ) { 
	  fprintf(ofp, "%d\n", uiptr[idx]);
	}
      }
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(x_fld_meta.fldtype, "B") == 0 ) {
    if ( x_fld_meta.n_sizeof != 1 ) { go_BYE(-1); }
    chk_file_size(nX, sizeof(char));
    for ( int i = 0; i < nR; i++ ) {
      idx = i;
      if ( ordr != NULL ) { idx = ordr[idx]; }
      if ( X[idx] == TRUE ) { 
	fprintf(ofp, "1\n");
      }
      else  if ( X[idx] == FALSE ) { 
	fprintf(ofp, "0\n");
      }
      else { go_BYE(-1); }
    }
  }
  else if ( ( strcmp(x_fld_meta.fldtype, "W") == 0 ) ||
            ( strcmp(x_fld_meta.fldtype, "C") == 0 ) ) {
    if ( x_fld_meta.n_sizeof == 0 ) { /* variable length */
      is_fixed_len = false;
      status = get_aux_fld_id(docroot, db, x_fld_meta.id, "sz", &sz_fld_id);
      cBYE(status);
      zero_fld_meta(&sz_fld_meta);
      status = fld_meta(docroot, db, tbl, NULL, sz_fld_id, &sz_fld_meta);
      cBYE(status);
      status = rs_mmap(sz_fld_meta.filename, &sz_X, &sz_nX, 0);
      cBYE(status);
      szptr = (int *)sz_X;
      offset = (int *)malloc(nR * sizeof(int));
      return_if_malloc_failed(offset);
      offset[0] = 0;
      for ( int i = 1; i < nR; i++ ) { 
	offset[i] = offset[i-1] + szptr[i-1];
      }
    }
    else {
      is_fixed_len = true;
      switch ( ifldtype ) { 
      case FLDTYPE_WCHAR_T : 
        chk_file_size(nX, sizeof(wchar_t));
        nC = x_fld_meta.n_sizeof / sizeof(wchar_t);
	break;
      case FLDTYPE_CHAR : 
        chk_file_size(nX, sizeof(char));
        nC = x_fld_meta.n_sizeof / sizeof(char);
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    for ( int i = 0; i < nR; i++ ) {
      idx = i;
      if ( ordr != NULL ) { idx = ordr[idx]; }
      if ( ( cfld_X != NULL ) && ( cfld_X[idx] == FALSE ) ) { 
	continue; // condition field says do not print this row
      }
      if ( is_fixed_len == true ) { 
	nw_i = nC;
	wcptr = (wchar_t *)X + (nC * idx);
	cptr  = (char    *)X + (nC * idx);
      }
      else { 
	nw_i = szptr[idx];
	wcptr = (wchar_t *)X + offset[idx]; 
	cptr  = (char    *)X + offset[idx]; 
      }
      pr_done = false;
      wcbufptr = cbufptr = 0;
      for ( int i = 0; i < DBG_BUF_LEN; i++ ) { wcbuf[i] = L'\0'; }
      for ( int i = 0; i < DBG_BUF_LEN; i++ ) { cbuf[i] = '\0'; }
      switch ( ifldtype ) { 
      case FLDTYPE_WCHAR_T : 
	fwprintf(ofp, wcformat, wcdquote);
      case FLDTYPE_CHAR : 
	fprintf(ofp, "\"");
	break;
      default : 
	go_BYE(-1);
	break;
      }
      for ( int j = 0; ((j < nw_i) && (pr_done == false)); j++ ) { 
	switch ( ifldtype ) { 
	case FLDTYPE_WCHAR_T : 
	  if ( *wcptr == '\0' )  { 
	    pr_done = true; 
	  }
	  else {
	    if ( ( *wcptr == '"' ) || ( *wcptr == '"' ) ) {
	      fprintf(ofp, "\\");
	    }
	    wcbuf[wcbufptr++] = *wcptr; // for debugging
	    fwprintf(ofp, wcformat, *wcptr++); 
	  }
	  break;
	case FLDTYPE_CHAR : 
	  if ( *cptr == '\0' ) {
	    pr_done = true;
	  }
	  else { 
	    cbuf[cbufptr++] = *cptr; // for debugging
	    if ( ( *cptr == '"' ) || ( *cptr == '\\' ) ) {
	      fprintf(ofp, "\\");
	    }
	    fprintf(ofp, "%c", *cptr++); 
	  }
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
      switch ( ifldtype ) { 
      case FLDTYPE_WCHAR_T : 
	fwprintf(ofp, wcformat, wceoln);
      case FLDTYPE_CHAR : 
	fprintf(ofp, "\"\n");
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
  }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(X, nX);
  rs_munmap(sz_X, sz_nX);
  free_if_non_null(chk_ordr);
  free_if_non_null(offset);
  rs_munmap(cfld_X, cfld_nX);
  rs_munmap(ordr_X, ordr_nX);
  return(status);
}
