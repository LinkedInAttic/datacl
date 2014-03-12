#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "t1f1t2f2op.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "get_nR.h"

//---------------------------------------------------------------
int 
t1f1t2f2op(
       char *docroot,
       sqlite3 *in_db,
       char *t1,
       char *f1,
       char *cf1,
       char *t2,
       char *f2,
       char *op
       )
{
  int status = 0;
  sqlite3 *db = NULL;
  int *szptr = NULL;
  int *iptr = NULL;
  /*
    wchar_t *wcptr;
  */
  char *X = NULL; size_t nX = 0;
  char *cf1_X = NULL; size_t cf1_nX = 0;
  char *sz_X = NULL; size_t sz_nX = 0;
  FLD_META_TYPE f1_fld_meta;
  FLD_META_TYPE sz_fld_meta; int sz_fld_id;
  FLD_META_TYPE cf1_fld_meta;
  int cnt, nw_i;
  int t2_id, nR2 = 0;
  int t1_id, nR1 = 0;
  FILE *ofp = NULL, *cfp = NULL; char *opfile = NULL, *cnt_file = NULL;
  char str_meta_data[256], cnt_str_meta_data[256];
  char buf[16];
  int prev, curr;
  bool is_prev_defined;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( cf1 == NULL ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); } }
  zero_string(str_meta_data, 256);
  zero_string(cnt_str_meta_data, 256);
  zero_string(buf, 16);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  status = open_temp_file(&ofp, &opfile);
  cBYE(status);
  if ( strcmp(op, "histo") == 0 ) {
    status = open_temp_file(&cfp, &cnt_file);
    cBYE(status);
  }
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, t1, &nR1, &t1_id);
  cBYE(status);
  status = fld_meta(docroot, db, t1, f1, -1, &f1_fld_meta);
  cBYE(status);
  status = rs_mmap(f1_fld_meta.filename, &X, &nX, 0);
  cBYE(status);

  if ( *cf1 != '\0' ) { 
    status = fld_meta(docroot, db, t1, cf1, -1, &cf1_fld_meta);
    cBYE(status);
    status = rs_mmap(cf1_fld_meta.filename, &cf1_X, &cf1_nX, 0);
    cBYE(status);
  }

  if ( strcmp(f1_fld_meta.fldtype, "I") == 0 ) { 
    strcpy(str_meta_data, "fldtype=int:sz=4");
    iptr = (int *)X;
    if ( f1_fld_meta.n_sizeof == 0 ) { /* variable length */
      status = get_aux_fld_id(docroot, db, f1_fld_meta.id, "sz", &sz_fld_id);
      zero_fld_meta(&sz_fld_meta);
      zero_fld_meta(&sz_fld_meta);
      status = fld_meta(docroot, db, t1, NULL, sz_fld_id, &sz_fld_meta);
      cBYE(status);
      status = rs_mmap(sz_fld_meta.filename, &sz_X, &sz_nX, 0);
      cBYE(status);
      szptr = (int *)sz_X;
      /*---------- START FUNCTION SPECIFIC */
      if ( strcmp(op, "flatten") == 0 ) {
	if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
        for ( int i = 0; i < nR1; i++ ) {
	  if ( ( *cf1 != '\0' ) && ( cf1_X[i] == FALSE ) ) {
	    // Nothing to write 
	  }
	  else {
	    nR2 += nw_i;
            fwrite(iptr, sizeof(int), nw_i, ofp);
	  }
          nw_i = szptr[i];
	  iptr += nw_i;
	}
      }
    }
    /*---------- STOP  FUNCTION SPECIFIC */
    else if ( f1_fld_meta.n_sizeof == 4 ) { 
      if ( strcmp(op, "histo") == 0 ) {
	if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
	if ( strcmp(f2, "cnt") == 0 ) { go_BYE(-1); }
	cnt = 1;
        is_prev_defined = false;
        for ( int i = 0; i < nR1; i++ ) {
	  if ( ( *cf1 != '\0' ) && ( cf1_X[i] == FALSE ) ) {
	    // Nothing to write 
	  }
	  else {
	    curr = iptr[i];
	    if ( is_prev_defined == false ) {
	      is_prev_defined = true;
	    }
	    else { 
	      if ( curr == prev ) {
	        cnt++;
	      }
	      else {
	        // output prev and count
                fwrite(&prev, sizeof(int), 1, ofp);
                fwrite(&cnt,  sizeof(int), 1, cfp);
	        nR2++;
	        cnt = 1;
	      }
	    }
	    prev = curr;
	  }
	}
        // Boundary condition for last element: 
        fwrite(&prev, sizeof(int), 1, ofp);
        fwrite(&cnt,  sizeof(int), 1, cfp);
	nR2++;
        fclose_if_non_null(ofp);
        fclose_if_non_null(cfp);
      }
    }
    else { go_BYE(-1); }
  }
  else { 
    fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1); 
  }
  fclose_if_non_null(ofp);
  // Add output field to meta data 
  status = del_tbl(docroot, db, t2);
  cBYE(status);
  if ( nR2 > 0 ) { 
    sprintf(buf, "%d", nR2);
    status = add_tbl(docroot, db, t2, buf, &t2_id);
    cBYE(status);
    strcat(str_meta_data, ":nR=");
    strcat(str_meta_data, buf);
    strcpy(cnt_str_meta_data, str_meta_data);
  fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1);
    /*
    status = add_fld(docroot, db, t2, f2, str_meta_data, opfile);
    cBYE(status);
    if ( strcmp(op, "histo") == 0 ) {
      status = add_fld(docroot, db, t2, "cnt", cnt_str_meta_data, cnt_file);
      cBYE(status);
    }
    */
  }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  free_if_non_null(opfile);
  free_if_non_null(cnt_file);
  fclose_if_non_null(ofp);
  fclose_if_non_null(cfp);
  return(status);
}
