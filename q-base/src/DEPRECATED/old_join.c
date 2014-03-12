#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "dbauxil.h"
#include "fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "bin_search.h"
#include "sort.h"
#include "join.h"
#include "get_nR.h"

//---------------------------------------------------------------
int 
join(
     char *docroot,
     sqlite3 *in_db,
     char *src_tbl,
     char *src_lnk,
     char *src_fld,
     char *dst_tbl,
     char *dst_lnk,
     char *dst_fld
     )
{
  int status = 0;
  sqlite3 *db = NULL;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *src_sz_X = NULL;  size_t src_sz_nX = 0;
  char *src_lnk_X = NULL; size_t src_lnk_nX = 0;
  char *dst_lnk_X = NULL; size_t dst_lnk_nX = 0;
  FLD_META_TYPE src_fld_meta, src_lnk_meta;
  FLD_META_TYPE               dst_lnk_meta;
  FLD_META_TYPE src_nn_fld_meta, src_sz_fld_meta;
  int rc, n_sizeof, src_idx, len; int *src_idxs = NULL;
  int src_nR, src_tbl_id;
  int dst_nR, dst_tbl_id;
  FILE *ofp = NULL, *nn_ofp = NULL, *sz_ofp = NULL, *hash_ofp = NULL;
  char *opfile = NULL, *nn_opfile = NULL, *sz_opfile = NULL;
  char str_meta_data[256];
  int *dst_lnk_ptr = NULL, *src_lnk_ptr = NULL;
  int *offset = NULL, *szptr = NULL;
  bool is_any_null = false; // assume no null values
  char *nn_dst_fld = NULL, *sz_dst_fld = NULL, *hash_dst_fld = NULL;
  char nullc = '\0', *cptr = NULL;
  char *zErrMsg = NULL;
  char buf[32];
  int *src_ptr = NULL; int dst_val;
  char *csrc_ptr = NULL; int cdst_val;
  char *nn_dst_src_val = NULL;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_lnk == NULL ) || ( *src_lnk == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_lnk == NULL ) || ( *dst_lnk == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 256);
  zero_string(buf, 32);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //--------------------------------------------------------
  rc = sqlite3_exec(db, "select * from tbl", NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  //--------------------------------------------------------
  status = open_temp_file(&ofp, &opfile);
  cBYE(status);
  status = open_temp_file(&nn_ofp, &nn_opfile);
  cBYE(status);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, src_tbl, &src_nR, &src_tbl_id);
  cBYE(status);
  status = fld_meta(docroot, db, src_tbl, src_fld, -1, &src_fld_meta);
  cBYE(status);
  status = rs_mmap(src_fld_meta.filename, &src_fld_X, &src_fld_nX, 0);
  cBYE(status);

  status = fld_meta(docroot, db, src_tbl, src_lnk, -1, &src_lnk_meta);
  cBYE(status);
  status = rs_mmap(src_lnk_meta.filename, &src_lnk_X, &src_lnk_nX, 0);
  cBYE(status);
  //--------------------------------------------------------
  status = fld_meta_2(db, src_fld_meta.id, "sz", &src_sz_fld_meta);
  cBYE(status);
  status = fld_meta_2(db, src_fld_meta.id, "nn", &src_nn_fld_meta);
  cBYE(status);

  //--------------------------------------------------------

  status = external_get_nR(docroot, db, dst_tbl, &src_nR, &dst_tbl_id);
  status = fld_meta(docroot, db, dst_tbl, dst_lnk, -1, &dst_lnk_meta);
  cBYE(status);
  status = rs_mmap(dst_lnk_meta.filename, &dst_lnk_X, &dst_lnk_nX, 0);
  cBYE(status);
  //--------------------------------------------------------
  // Find out if there is a nn_fld for the source. 
  //--------------------------------------------------------
  // Link field must be integer
  if ( strcmp(src_lnk_meta.fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( strcmp(dst_lnk_meta.fldtype, "int") != 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  dst_lnk_ptr = (int *)dst_lnk_X;
  src_lnk_ptr = (int *)src_lnk_X;
  is_any_null = false; // assume no null values

  /* SIMPLE CODE 
  status = simple_find_matches_if_any(dst_lnk_ptr, dst_nR, 
      src_lnk_ptr, src_nR, &src_idxs, &is_any_null, nn_ofp);
  cBYE(status);
  */
  status = fast_find_matches_if_any(dst_lnk_ptr, dst_nR, 
      src_lnk_ptr, src_nR, &src_idxs, &is_any_null, &nn_dst_src_val);
  cBYE(status);
  fwrite(nn_dst_src_val, dst_nR, sizeof(char), nn_ofp);

  if ( src_fld_meta.n_sizeof < 0 ) { go_BYE(-1); }
  if ( src_fld_meta.n_sizeof == 0 ) { // variable length field
    if ( strcmp(src_fld_meta.fldtype, "char") != 0 ) { 
      fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1);
      go_BYE(-1);
    }
    status = open_temp_file(&sz_ofp, &sz_opfile);
    cBYE(status);
    status = rs_mmap(src_sz_fld_meta.filename, &src_sz_X, &src_sz_nX, 0);
    cBYE(status);
    szptr = (int *)src_sz_X;
    status = mk_offset((int *)src_sz_X, src_nR, &offset);
    strcpy(str_meta_data,"fldtype=char:sz=0:nR=");
    sprintf(buf, "%d", dst_nR); 
    strcat(str_meta_data, buf);
    for ( int i = 0; i < dst_nR; i++ ) {
      src_idx = src_idxs[i];
      if ( src_idx < 0 ) {
	len = 1;
	fwrite(&nullc, 1, 1, ofp);
      }
      else {
	cptr = src_fld_X + offset[src_idx];
	fwrite(cptr, szptr[src_idx], sizeof(char), ofp);
      }
      fwrite(szptr + src_idx, 1, sizeof(int), sz_ofp);
    }
    free_if_non_null(offset);
  }
  else {
    n_sizeof = src_fld_meta.n_sizeof;
    if ( strcmp(src_fld_meta.fldtype, "wchar_t") == 0 ) { 
      strcpy(str_meta_data,"fldtype=wchar_t");

      strcat(str_meta_data, ":sz="); 
      sprintf(buf, "%d", n_sizeof); 
      strcat(str_meta_data, buf);

      strcat(str_meta_data, ":nR="); 
      sprintf(buf, "%d", dst_nR); 
      strcat(str_meta_data, buf);

      if ( strcmp(src_fld_meta.fldtype, "wchar_t") == 0 ) {
	for ( int i = 0; i < dst_nR; i++ ) {
	  src_idx = src_idxs[i];
	  if ( src_idx < 0 ) {
	    for ( int j = 0; j < n_sizeof; j++ ) { 
	      fwrite(&nullc, 1, 1, ofp);
	    }
	  }
	  else {
	    cptr = src_fld_X + (n_sizeof * src_idx);
	    fwrite(cptr, 1, n_sizeof, ofp);
	  }
	
	}
      }
    }
    else if ( strcmp(src_fld_meta.fldtype, "int") == 0 ) {
      strcpy(str_meta_data,"fldtype=int:sz=4");
      src_ptr = (int *)src_fld_X;
      for ( int i = 0; i < dst_nR; i++ ) {
        src_idx = src_idxs[i];
        if ( src_idx < 0 ) {
  	  dst_val = 0;
        }
        else {
  	  dst_val = src_ptr[src_idx];
        }
        fwrite(&dst_val, 1, sizeof(int), ofp);
      }
    }
    else if ( ( strcmp(src_fld_meta.fldtype, "char") == 0 ) || 
              ( strcmp(src_fld_meta.fldtype, "bool") == 0 ) ) {
      if ( strcmp(src_fld_meta.fldtype, "bool") == 0 ) {
        strcpy(str_meta_data,"fldtype=bool:sz=1");
      }
      else if ( strcmp(src_fld_meta.fldtype, "char") == 0 ) {
        strcpy(str_meta_data,"fldtype=char:sz=1");
      }
      csrc_ptr = (char *)src_fld_X;
      for ( int i = 0; i < dst_nR; i++ ) {
        src_idx = src_idxs[i];
        if ( src_idx < 0 ) {
  	  cdst_val = 0;
        }
        else {
  	  cdst_val = csrc_ptr[src_idx];
        }
        fwrite(&cdst_val, 1, sizeof(char), ofp);
      }
    }
    else { go_BYE(-1); }
  }

  fclose_if_non_null(ofp); // primary field
  fclose_if_non_null(nn_ofp); // auxiliary field 
  fclose_if_non_null(sz_ofp); // auxiliary field 
  fclose_if_non_null(hash_ofp); // auxiliary field 
  // Add output field to meta data 
  if ( ( str_meta_data == NULL ) || ( *str_meta_data == '\0' ) ) { go_BYE(-1); }
  fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1);
  /*
  status = add_fld(docroot, db, dst_tbl, dst_fld, str_meta_data, opfile);
  cBYE(status);
  */
  if ( src_fld_meta.n_sizeof == 0 ) { // variable length field
    len = strlen(dst_fld) + strlen(".sz") + 4;
    sz_dst_fld = malloc(len * sizeof(char));
    return_if_malloc_failed(sz_dst_fld);
    zero_string(sz_dst_fld, len);
    strcpy(sz_dst_fld, dst_fld);
    strcat(sz_dst_fld, ".sz");
    status = add_aux_fld(docroot, db, dst_tbl, dst_fld, sz_dst_fld,
			 sz_opfile, "sz");
    cBYE(status);
    free_if_non_null(sz_dst_fld);
  }
  if ( is_any_null == true ) {
    len = strlen(dst_fld) + strlen(".nn") + 4;
    nn_dst_fld = malloc(len * sizeof(char));
    return_if_malloc_failed(nn_dst_fld);
    zero_string(nn_dst_fld, len);
    strcpy(nn_dst_fld, dst_fld);
    strcat(nn_dst_fld, ".nn");
    status = add_aux_fld(docroot, db, dst_tbl, dst_fld, nn_dst_fld,
			 nn_opfile, "nn");
    cBYE(status);
    free_if_non_null(nn_dst_fld);
  }
  else {
    unlink(nn_opfile);
  }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(dst_lnk_X, dst_lnk_nX);

  free_if_non_null(opfile);
  fclose_if_non_null(ofp);

  free_if_non_null(nn_opfile);
  fclose_if_non_null(nn_ofp);

  free_if_non_null(sz_opfile);
  fclose_if_non_null(sz_ofp);

  free_if_non_null(nn_dst_fld);
  free_if_non_null(sz_dst_fld);
  free_if_non_null(hash_dst_fld);
  free_if_non_null(src_idxs);
  free_if_non_null(offset);
  free_if_non_null(nn_dst_src_val);
  return(status);
}

//-----------------------------------------------------------
int
simple_find_matches_if_any(
		    int *dptr, 
		    int dst_nR, 
		    int *sptr, 
		    int src_nR, 
		    int **ptr_src_idxs,
		    bool *ptr_is_any_null,
		    FILE *nn_ofp
		    )
{
  int status = 0;
  int *src_idxs = NULL;
  int dst_lnk_val, src_lnk_val, src_idx;
  char c_found; bool is_any_null;

  src_idxs = (int *)malloc(dst_nR * sizeof(int));
  return_if_malloc_failed(src_idxs);

  for ( int i = 0; i < dst_nR; i++ ) {
    dst_lnk_val = dptr[i];
    // See if this value exists in source. Dumb scan for now
    src_idx = -1; // indicates not found
    for ( int j = 0; j < src_nR; j++ ) {
      src_lnk_val = sptr[j];
      if ( src_lnk_val == dst_lnk_val ) { 
	src_idx = j;
	break;
      }
    }
    if ( src_idx >= 0 ) {
      c_found = TRUE;
    }
    else {
      c_found = FALSE;
      is_any_null = true;
    }
    src_idxs[i] = src_idx;
    fwrite(&c_found, sizeof(char), 1, nn_ofp);
  }
  *ptr_src_idxs = src_idxs;
  *ptr_is_any_null = is_any_null;
 BYE:
  return(status);
}
//-----------------------------------------------------------
int
fast_find_matches_if_any(
		    int *dst_lnk_ptr, 
		    int dst_nR, 
		    int *src_lnk_ptr, 
		    int src_nR, 
		    int **ptr_src_idxs,
		    bool *ptr_is_any_null,
		    char **ptr_nn_dptr
		    )
{
  int status = 0;
  int *src_idxs = NULL;
  bool is_any_null, found;
  FILE *tfp = NULL; char *temp_src_file, *temp_dst_file;
  char *nn_dptr = NULL;
  int src_lnk_val, dst_lnk_val;
  int src_lnk_idx, dst_lnk_idx;
  char *temp_src_X = NULL; size_t temp_src_nX;
  char *temp_dst_X = NULL; size_t temp_dst_nX;
  IITYPE *src_pair = NULL, *dst_pair = NULL;
  int src_marker;

  is_any_null = false;
  if ( dst_nR <= 0 ) { go_BYE(-1); }
  nn_dptr = (char *)malloc(dst_nR * sizeof(char));
  return_if_malloc_failed(nn_dptr);
  *ptr_src_idxs = NULL;
  *ptr_nn_dptr = NULL;

  src_idxs = (int *)malloc(dst_nR * sizeof(int));
  return_if_malloc_failed(src_idxs);
  /* START: Sort source and destination link fields */
  /*------------------------------------------------*/
  status = open_temp_file(&tfp, &temp_src_file);
  cBYE(status);
  for ( int i = 0; i < src_nR; i++ ) { 
    fwrite(src_lnk_ptr+i, 1, sizeof(int), tfp);
    fwrite(&i, 1, sizeof(int), tfp);
  }
  fclose_if_non_null(tfp);
  status = rs_mmap(temp_src_file, &temp_src_X, &temp_src_nX, 1); 
  cBYE(status);
  qsort(temp_src_X, src_nR, 2*sizeof(int), sort_ii_aa_compare);
  src_pair = (IITYPE *)temp_src_X;
  /*------------------------------------------------*/
  status = open_temp_file(&tfp, &temp_dst_file);
  cBYE(status);
  for ( int i = 0; i < dst_nR; i++ ) { 
    fwrite(dst_lnk_ptr+i, 1, sizeof(int), tfp);
    fwrite(&i, 1, sizeof(int), tfp);
  }
  fclose_if_non_null(tfp);
  status = rs_mmap(temp_dst_file, &temp_dst_X, &temp_dst_nX, 1); 
  cBYE(status);
  qsort(temp_dst_X, dst_nR, 2*sizeof(int), sort_ii_aa_compare);
  dst_pair = (IITYPE *)temp_dst_X;
  /*------------------------------------------------*/
  /* STOP : Sort source and destination link fields */

  src_marker = 0;
  for ( int i = 0; i < dst_nR; i++ ) {
    dst_lnk_val = dst_pair[i].lnk_val;
    dst_lnk_idx = dst_pair[i].idx_val;
    found = false;
    /* Advance src marker until src_lnk_val matches dst_lnk_val */
    for ( ; src_marker < src_nR; src_marker++ ) {
      src_lnk_val = src_pair[src_marker].lnk_val;
      src_lnk_idx = src_pair[src_marker].idx_val;
      if ( src_lnk_val == dst_lnk_val ) {
	found = true;
	break;
      }
      else if ( src_lnk_val > dst_lnk_val ) {
	found = false;
	break;
      }
    }
    if ( found == true ) { 
      nn_dptr[dst_lnk_idx] = TRUE;
      src_idxs[dst_lnk_idx] = src_lnk_idx;
    }
    else {
      is_any_null = true;
      nn_dptr[dst_lnk_idx] = FALSE;
      src_idxs[dst_lnk_idx] = INT_MIN; // as a precaution
    }
  }
  *ptr_src_idxs = src_idxs;
  *ptr_is_any_null = is_any_null;
  *ptr_nn_dptr = nn_dptr;
BYE:
  unlink(temp_src_file);
  unlink(temp_dst_file);
  free_if_non_null(temp_src_file);
  free_if_non_null(temp_dst_file);
  rs_munmap(temp_src_X, temp_src_nX);
  rs_munmap(temp_dst_X, temp_dst_nX);
  return(status);
}
