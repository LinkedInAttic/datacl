#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "parse_attrs.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "aux_fld_meta.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
binld(
       char *tbl,
       char *str_flds,
       char *str_flds_spec,
       char *infile
       )
// STOP FUNC DECL
{
  int status = 0;
  char **flds = NULL;  char **spec = NULL;
  FILE *ofp = NULL; char **opfiles = NULL;
  int rec_size = -1, n_flds = -1, alt_n;
  int tbl_id = INT_MIN, fld_id = INT_MIN;
  int *fldsz = NULL;
  long long nR;
  FILE *ifp = NULL;
  char  *X  = NULL; size_t  nX  = 0;
  char **Xs = NULL; size_t *nXs = 0;
  char str_meta_data[1024]; char str_nR[32]; 
  int *fldtype = NULL;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds_spec == NULL ) || ( *str_flds_spec == '\0' ) ) { go_BYE(-1); }
  if ( ( infile == NULL ) || ( *infile == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  zero_string(str_nR, 32);
  //--------------------------------------------------------
  if ( str_flds == NULL ) { go_BYE(-1); }
  if ( strlen(str_flds) == 0 ) { go_BYE(-1); }
  status = explode(str_flds, ':', &flds, &n_flds);
  cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) { 
    status = chk_fld_name(flds[i], 0); cBYE(status);
    for ( int j = i+1; j < n_flds; j++ ) { 
      if ( strcmp(flds[i], flds[j]) == 0 ) { 
	fprintf(stderr, "Duplicate field names not allowed \n");
	go_BYE(-1);
      }
    }
  }
  //--------------------------------------------------------
  if ( strlen(str_flds_spec) != ((n_flds-1)*2+1) ) { go_BYE(-1); }
  status = explode(str_flds_spec, ':', &spec, &alt_n);
  cBYE(status);
  if ( alt_n != n_flds ) { go_BYE(-1); }
  fldtype = (int *)malloc(n_flds * sizeof(int));
  return_if_malloc_failed(fldtype);
  fldsz = (int *)malloc(n_flds * sizeof(int));
  return_if_malloc_failed(fldsz);

  rec_size = 0;
  for ( int j = 0; j < n_flds; j++ ) { 
    if  ( strcmp(spec[j], "B") == 0 ) { 
      fldtype[j] = FLDTYPE_BOOL;
      rec_size  += sizeof(char);
      fldsz[j]   = sizeof(char);
    }
    else if  ( strcmp(spec[j], "C") == 0 ) { 
      fldtype[j] = FLDTYPE_CHAR;
      rec_size  += sizeof(char);
      fldsz[j]   = sizeof(char);
    }
    else if  ( strcmp(spec[j], "I") == 0 ) { 
      fldtype[j] = FLDTYPE_INT;
      rec_size  += sizeof(int);
      fldsz[j]   = sizeof(int);
    }
    else if  ( strcmp(spec[j], "L") == 0 ) { 
      fldtype[j] = FLDTYPE_LONGLONG;
      rec_size += sizeof(long long);
      fldsz[j]   = sizeof(long long);
    }
    else if  ( strcmp(spec[j], "F") == 0 ) { 
      fldtype[j] = FLDTYPE_FLOAT;
      rec_size += sizeof(float);
      fldsz[j]   = sizeof(float);
    }
    else if  ( strcmp(spec[j], "D") == 0 ) { 
      fldtype[j] = FLDTYPE_DOUBLE;
      rec_size += sizeof(double);
      fldsz[j]   = sizeof(double);
    }
    else {
      go_BYE(-1);
    }
  }
  /* mmap input file and get number of rows */
  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  nR = nX / rec_size;
  if ( nR * rec_size != nX ) { go_BYE(-1); }
  sprintf(str_nR, "%lld", nR);
  /* Create output files for each field */
  opfiles = (char **)malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(opfiles);
  Xs = (char **)malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(Xs);
  nXs = (size_t *)malloc(n_flds * sizeof(size_t));
  return_if_malloc_failed(nXs);
  for ( int j = 0; j < n_flds; j++ ) { 
    status = open_temp_file(&ofp, &(opfiles[j]), 0); cBYE(status);
    fclose_if_non_null(ofp);
    status = mk_file(opfiles[j], (fldsz[j] * nR)); cBYE(status);
    status = rs_mmap(opfiles[j], &(Xs[j]), &(nXs[j]), 1); cBYE(status);
  }
  //--------------------------------------------------------
  //--------------------------------------------------------
  for ( long long i = 0; i < nR; i++ ) { 
    for ( int j = 0; j < n_flds; j++ ) { 
      memcpy(Xs[j], X, fldsz[j]);
      Xs[j] += fldsz[j];
      X     += fldsz[j];
    }
  }
  for ( int j = 0; j < n_flds; j++ ) { 
    rs_munmap(Xs[j], nXs[j]);
  }
  // Delete table if it exists 
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  if ( tbl_id >= 0 ) { 
    status = del_tbl(NULL, tbl_id);
    cBYE(status);
  }
  // Create an empty table
  status = add_tbl(tbl, str_nR, &tbl_id); cBYE(status);
  // Add fields to table 
  for ( int j = 0; j < n_flds; j++ ) { 
    sprintf(str_meta_data, "filename=%s", opfiles[j]);
    switch ( fldtype[j] ) { 
      case FLDTYPE_BOOL : 
      strcat(str_meta_data, ":fldtype=int:n_sizeof=1");
      break;
      case FLDTYPE_CHAR : 
      strcat(str_meta_data, ":fldtype=int:n_sizeof=1");
      break;
      case FLDTYPE_INT : 
      strcat(str_meta_data, ":fldtype=int:n_sizeof=4");
      break;
      case FLDTYPE_FLOAT : 
      strcat(str_meta_data, ":fldtype=float:n_sizeof=4");
      break;
      case FLDTYPE_LONGLONG : 
      strcat(str_meta_data, ":fldtype=long long:n_sizeof=8");
      break;
      case FLDTYPE_DOUBLE : 
      strcat(str_meta_data, ":fldtype=double:n_sizeof=8");
      break;
      default : 
      go_BYE(-1);
      break;
    }
    status = add_fld(tbl, flds[j], str_meta_data, &fld_id);
    cBYE(status);
  }
  fclose_if_non_null(ifp);
BYE:
  fclose_if_non_null(ifp);
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( opfiles != NULL ) { free_if_non_null(opfiles[i]); }
    if ( spec != NULL ) { free_if_non_null(spec[i]); }
    if ( flds != NULL ) { free_if_non_null(flds[i]); }
  }
  free_if_non_null(opfiles);
  free_if_non_null(spec);
  free_if_non_null(flds);
  free_if_non_null(fldtype);
  free_if_non_null(fldsz);
  free_if_non_null(Xs);
  free_if_non_null(nXs);
  return(status);
}
