#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <limits.h>
#include <values.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "dbauxil.h"
#include "fld_meta.h"
#include "get_nR.h"
#include "add_fld.h"
#include "is_fld.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
f_to_s(
       char *docroot,
       sqlite3 *in_db,
       char *tbl,
       char *fld,
       char *op,
       char *str_result
       )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *X = NULL; size_t nX = 0;
  FLD_META_TYPE x_fld_meta;
  int *iptr = NULL; char *cptr = NULL;
  bool *bptr = NULL; long long *llptr = NULL; float *fptr = NULL;
  int tbl_id, fld_id; long long nR; 
  bool exists;
  char *cached_rslt = NULL;
  int itemp;  long long lltemp; double dtemp; float ftemp;
              long long llavg;  double davg;
              long long llsum;  double dsum;

  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //--------------------------------------------------------
  status = is_tbl_fld(docroot, db, tbl, fld, &tbl_id, &fld_id, &exists);
    if ( !exists )  { go_BYE(-1); }
  status = internal_get_nR(db, tbl_id, &nR);
  cBYE(status);
  //--------------------------------------------------------
  // See if you have data cached in fld_stats
  if ( ( strcmp(op, "max") == 0 ) || ( strcmp(op, "min") == 0 ) ) {
    char qstr[4096]; 
    zero_string(qstr, 4096);
    sprintf(qstr, "select count(*) from fld_stats where fld_id = %d \n", fld_id);
    status = db_get_ival(db, qstr, &itemp);
    cBYE(status);
    if ( itemp == 1 ) { 
      sprintf(qstr, "select %s from fld_stats where fld_id = %d \n", op, fld_id);
      status = db_get_sval(db, qstr, &cached_rslt);
      cBYE(status);
      strcpy(str_result, cached_rslt);
      free_if_non_null(cached_rslt);
      goto BYE;
    }
    else {
      /* We do not have cached results */
    }
  }
  //--------------------------------------------------------
  status = fld_meta(docroot, db, tbl, NULL, fld_id, &x_fld_meta);
  cBYE(status);
  status = rs_mmap(x_fld_meta.filename, &X, &nX, 1); // modifying
  cBYE(status);
  /* Now we can start with computation */
  iptr = (int *)X; 
  fptr = (float *)X; 
  llptr = (long long *)X; 

  if ( strcmp(op, "sum") == 0 ) {
    if ( strcmp(x_fld_meta.fldtype, "int") == 0 ) { 
      lltemp = 0;
      for ( long long i = 0; i < nR; i++ ) { 
	lltemp += iptr[i];
      }
      sprintf(str_result, "%lld", lltemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "float") == 0 ) { 
      dtemp = 0;
      for ( long long i = 0; i < nR; i++ ) { 
	dtemp += fptr[i];
      }
      sprintf(str_result, "%f", dtemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "long long") == 0 ) { 
      lltemp = 0;
      for ( long long i = 0; i < nR; i++ ) { 
	lltemp += llptr[i];
      }
      sprintf(str_result, "%lld", lltemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "char") == 0 ) { 
      lltemp = 0;
      cptr = (char *)X; 
      for ( long long i = 0; i < nR; i++ ) { 
	lltemp += cptr[i];
      }
      sprintf(str_result, "%lld", lltemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "bool") == 0 ) { 
      lltemp = 0;
      bptr = (bool *)X; 
      for ( long long i = 0; i < nR; i++ ) { 
	if ( bptr[i] ) { lltemp++; } 
      }
      sprintf(str_result, "%lld", lltemp);
    }
    else {
      fprintf(stderr, "DBG: Not implemented: fldtype = %s \n",  x_fld_meta.fldtype);
      go_BYE(-1);
    }
  }
  else if ( strcmp(op, "max") == 0 ) {
    if ( strcmp(x_fld_meta.fldtype, "int") == 0 ) { 
      itemp = INT_MIN;
      for ( long long i = 0; i < nR; i++ ) { 
	if ( iptr[i] > itemp ) { itemp = iptr[i]; }
      }
      sprintf(str_result, "%d", itemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "float") == 0 ) { 
      ftemp = FLT_MIN;
      for ( long long i = 0; i < nR; i++ ) { 
	if ( fptr[i] > ftemp ) { ftemp = fptr[i]; }
      }
      sprintf(str_result, "%f", ftemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "long long") == 0 ) { 
      llptr = (long long *)X; 
      lltemp = LLONG_MIN;
      for ( long long i = 0; i < nR; i++ ) { 
	if ( llptr[i] > lltemp ) { lltemp = llptr[i]; }
      }
      sprintf(str_result, "%lld", lltemp);
    }
  }
  else if ( strcmp(op, "min") == 0 ) {
    if ( strcmp(x_fld_meta.fldtype, "int") == 0 ) { 
      itemp = INT_MAX;
      for ( long long i = 0; i < nR; i++ ) { 
	if ( iptr[i] < itemp ) { itemp = iptr[i]; }
      }
      sprintf(str_result, "%d", itemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "float") == 0 ) { 
      ftemp = FLT_MAX;
      for ( long long i = 0; i < nR; i++ ) { 
	if ( fptr[i] < ftemp ) { ftemp = fptr[i]; }
      }
      sprintf(str_result, "%f", ftemp);
    }
    else if ( strcmp(x_fld_meta.fldtype, "long long") == 0 ) { 
      llptr = (long long *)X; 
      lltemp = LLONG_MAX;
      for ( long long i = 0; i < nR; i++ ) { 
	if ( llptr[i] < lltemp ) { lltemp = llptr[i]; }
      }
      sprintf(str_result, "%lld", lltemp);
    }
  }
  else if ( strcmp(op, "avg") == 0 ) {
    if ( strcmp(x_fld_meta.fldtype, "int") == 0 ) { 
      llsum = 0;
      for ( long long i = 0; i < nR; i++ ) { 
	llsum += iptr[i];
      }
      llavg = llsum / nR;
      sprintf(str_result, "%lld", llavg);
    }
    else if ( strcmp(x_fld_meta.fldtype, "float") == 0 ) { 
      dsum = 0;
      for ( int i = 0; i < nR; i++ ) { 
	dsum += fptr[i];
      }
      davg = dsum / (double)nR;
      sprintf(str_result, "%f", davg);
    }
    else if ( strcmp(x_fld_meta.fldtype, "long long") == 0 ) { 
      llsum = 0;
      for ( int i = 0; i < nR; i++ ) { 
	llsum += llptr[i];
      }
      llavg = llsum / nR;
      sprintf(str_result, "%lld", llavg);
    }
  }
  else {
    go_BYE(-1);
  }
BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(X, nX);
  free_if_non_null(cached_rslt);
  return(status);
}
