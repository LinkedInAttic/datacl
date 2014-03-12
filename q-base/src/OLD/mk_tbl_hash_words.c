#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "mk_tbl_hash_words.h"
#include "get_nR.h"
#include "is_tbl.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "hash_string.h"
#include "dup_fld.h"
#include "del_fld.h"
#include "count_vals.h"
#include "fop.h"
#include "mjoin.h"
#include "sortbindmp.h"
#include "bindmp.h"
#include "binld.h"
#include "f1opf2.h"

#include "scan_words_int.h"
#include "scan_words_longlong.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
mk_tbl_hash_words(
		  char *docroot,
		  sqlite3 *in_db,
		  char *src_tbl,
		  char *src_fld,
		  char *dst_tbl,
		  char *hash_tbl,
		  char *options
		  )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *hash_fld_X = NULL; size_t hash_fld_nX = 0;
  FLD_META_TYPE src_fld_meta, nn_src_fld_meta, sz_src_fld_meta, hash_fld_meta;
  int src_tbl_id, itemp;
  long long src_nR, hash_nR, dst_nR;
  char str_dst_nR[32];
  char str_meta_data[256];
  char qstr[4096];
  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *sz_src_fld_X = NULL; size_t sz_src_fld_nX = 0;
  FILE *ofp1 = NULL, *ofp2 = NULL, *ofp3 = NULL, *ofp4 = NULL, *ofp5 = NULL;
  char *fname1 = NULL, *fname2 = NULL, *fname3 = NULL, *fname4 = NULL,
    *fname5 = NULL;
  char *tempfile = NULL; FILE *tfp = NULL;
  char *fldname = NULL;
  int *szptr = NULL; 
  bool is_break_on_wspace = true; bool to_pr = true;
  char *str_is_break_on_wspace = NULL, *str_len = NULL;
  char hash_fldtype[16]; int hash_n_sizeof;
  char fld_spec_1[16], fld_spec_2[16];
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 256);
  zero_string(str_dst_nR, 32);
  zero_string(qstr, 4096);
  zero_fld_meta(&src_fld_meta);
  zero_fld_meta(&nn_src_fld_meta);
  zero_fld_meta(&sz_src_fld_meta);
  zero_fld_meta(&hash_fld_meta);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  // Set default hash type 
  zero_string(hash_fldtype, 16); 
  zero_string(fld_spec_1, 16); 
  zero_string(fld_spec_2, 16); 
  //--------------------------------------------------------
  //--------------------------------------------------------
  // Process options
  extract_name_value(options, "break_on_ws=", ":",
      &str_is_break_on_wspace);
  if ( str_is_break_on_wspace != NULL ) { 
    if ( strcmp(str_is_break_on_wspace, "false") == 0 ) {
      is_break_on_wspace = false;
      to_pr = false;
    }
    else if ( strcmp(str_is_break_on_wspace, "false") == 0 ) {
      is_break_on_wspace = true;
      to_pr = false;
    }
  }
  if ( to_pr ) {
    fprintf(stderr, "DBG: Using default of break on white space = TRUE\n");
  }
  //--------------------------------------------------------
  to_pr = true;
  extract_name_value(options, "len=", ":", &str_len);
  if ( str_len != NULL ) { 
    if ( strcmp(str_len, "8") == 0 ) {
      strcpy(hash_fldtype, "long long");
      hash_n_sizeof = 8;
      strcpy(fld_spec_1, "LLL");
      strcpy(fld_spec_2, "LL:LL:LL");
      to_pr = false;
    }
    else if ( strcmp(str_len, "4") == 0 ) {
      strcpy(hash_fldtype, "int");
      hash_n_sizeof = 4;
      strcpy(fld_spec_1, "III");
      strcpy(fld_spec_2, "I:I:I");
      to_pr = false;
    }
    else {
      go_BYE(-1);
    }
  }
  else {
    strcpy(hash_fldtype, "int");
    hash_n_sizeof = 4;
    strcpy(fld_spec_1, "III");
    strcpy(fld_spec_2, "I:I:I");
    if ( to_pr ) {
      fprintf(stderr, "DBG: Using default type int for hash \n");
    }
  }
  //--------------------------------------------------------
  free_if_non_null(str_is_break_on_wspace);
  free_if_non_null(str_len);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, src_tbl, &src_nR, &src_tbl_id);
  cBYE(status);
  status = fld_meta(docroot, db, src_tbl, src_fld, -1, &src_fld_meta);
  cBYE(status);
  if ( strcmp(src_fld_meta.fldtype, "char string") != 0 ) { go_BYE(-1); }
  status = rs_mmap(src_fld_meta.filename, &src_fld_X, &src_fld_nX, 0);
  cBYE(status);
  // Get nn field for source if if it exists
  status = get_aux_field_if_it_exists(
				      docroot, db, src_tbl, src_fld_meta.id, "nn", &nn_src_fld_meta, 
				      &nn_src_fld_X, &nn_src_fld_nX);
  cBYE(status);
  // Get sz field for source if if is string
  status = get_aux_field_if_it_exists(
				      docroot, db, src_tbl, src_fld_meta.id, "sz", &sz_src_fld_meta, 
				      &sz_src_fld_X, &sz_src_fld_nX);
  cBYE(status);
  szptr = (int *)sz_src_fld_X;
  //-- START: Create temp files for dst_tbl
  status = open_temp_file(&ofp1, &fname1);
  status = open_temp_file(&ofp2, &fname2);
  status = open_temp_file(&ofp3, &fname3);
  //-- STOP: Create temp files for dst_tbl

  status = del_tbl(docroot, in_db, dst_tbl);
  cBYE(status);
  status = del_tbl(docroot, in_db, hash_tbl);
  cBYE(status);
  if ( strcmp(hash_fldtype,"int") == 0 ) {
    status = scan_words_int(true, is_break_on_wspace, src_fld_X, 
      src_nR, szptr, NULL, 1, &ofp1, &ofp2, &ofp3, &dst_nR);
    cBYE(status); 
  }
  else if ( strcmp(hash_fldtype,"long long") == 0 ) {
    status = scan_words_longlong(true, is_break_on_wspace, 
	src_fld_X, src_nR, szptr, NULL, 1, &ofp1, &ofp2, &ofp3, &dst_nR);
    cBYE(status); 
  }
  else {
    go_BYE(-1);
  }
  sprintf(str_dst_nR, "%lld", dst_nR);
  status = add_tbl(docroot, db, dst_tbl, str_dst_nR, &itemp);
  cBYE(status);
  sprintf(str_meta_data,
      "fldtype=%s:n_sizeof=%d:filename=%s", 
      hash_fldtype, hash_n_sizeof, fname1);
  status = add_fld(docroot, db, dst_tbl, "hash", str_meta_data);
  cBYE(status);

  fldname = (char *)malloc(strlen(src_tbl) + strlen("_id") + 4);
  return_if_malloc_failed(fldname);
  sprintf(str_meta_data,
      "fldtype=%s:n_sizeof=%d:filename=%s", 
      hash_fldtype, hash_n_sizeof, fname2);
  strcpy(fldname, src_tbl);
  strcat(fldname, "_id");
  status = add_fld(docroot, db, dst_tbl, fldname, str_meta_data);
  cBYE(status);

  sprintf(str_meta_data,
      "fldtype=%s:n_sizeof=%d:filename=%s", 
      hash_fldtype, hash_n_sizeof, fname3);
  status = add_fld(docroot, db, dst_tbl, "pk", str_meta_data);
  cBYE(status);
  /*  Now, we create a table that has the unique hashes */
  status = dup_fld(docroot, db, dst_tbl, "hash", "alt_hash");
  cBYE(status);
  status = fop(docroot, db, dst_tbl, "alt_hash", "sortA");
  cBYE(status);
  status = count_vals(docroot, db, dst_tbl, "alt_hash", "", hash_tbl,
		      "hash", "hash_cnt");
  cBYE(status);
  /* Find the first occurrence of the hash */
  status = mjoin(docroot, db, dst_tbl, "hash", "pk", hash_tbl,
		 "hash", "min_pk", "min");
  cBYE(status);
  /* Sort hash_tbl in the order in which the hashes occurred */
  status = open_temp_file(&tfp, &tempfile);
  cBYE(status);
  fclose_if_non_null(tfp);
  /* WHY DID WE HAVE THIS ? TODO
  if ( strcmp(hash_fldtype, "long long") == 0 ) {
    status = new_f1opf2(docroot, db, hash_tbl, "hash_cnt", 
	"op=conv:newtype=long long", "hash_cnt");
  }
  */
  status = bindmp(docroot, db, hash_tbl, "min_pk:hash:hash_cnt", "", tempfile);
  cBYE(status);
  status = sortbindmp(tempfile, fld_spec_1, "AAA");
  cBYE(status);
  status = del_tbl(docroot, db, hash_tbl);
  cBYE(status);

  status = binld(docroot, db, hash_tbl, "min_pk:hash:hash_cnt", fld_spec_2, tempfile);
  cBYE(status);
  /* Now we need to re-process src_tbl to match hashes with their strings */
  status = fld_meta(docroot, db, hash_tbl, "hash", -1, &hash_fld_meta);
  cBYE(status);
  status = rs_mmap(hash_fld_meta.filename, &hash_fld_X, &hash_fld_nX, 0);
  cBYE(status);
  status = external_get_nR(docroot, db, hash_tbl, &hash_nR, &itemp);

  status = open_temp_file(&ofp4, &fname4);
  cBYE(status);
  status = open_temp_file(&ofp5, &fname5);
  cBYE(status);
  if ( strcmp(hash_fldtype, "int") == 0 ) {
    status = scan_words_int(false, is_break_on_wspace, src_fld_X, 
		      src_nR, szptr, (int *)hash_fld_X, hash_nR, 
		      &ofp4, &ofp5, NULL, &dst_nR);
    cBYE(status);
  }
  else if ( strcmp(hash_fldtype, "long long") == 0 ) {
    status = scan_words_longlong(false, is_break_on_wspace, src_fld_X, 
		      src_nR, szptr, (long long *)hash_fld_X, hash_nR, 
		      &ofp4, &ofp5, NULL, &dst_nR);
    cBYE(status);
  }
  else { go_BYE(-1); }
  zero_string_to_nullc(str_meta_data);
  sprintf(str_meta_data, 
      "fldtype=char string:n_sizeof=0:filename=%s", fname4);
  status = add_fld(docroot, db, hash_tbl, "hash_string", str_meta_data);
  cBYE(status);
  status = add_aux_fld(docroot, db, hash_tbl, "hash_string", fname5, "sz");
  cBYE(status);

  /* Delete fields that are no longer needed */
  status = del_fld(docroot, db, dst_tbl, "pk");
  cBYE(status);
  status = del_fld(docroot, db, dst_tbl, "alt_hash");
  cBYE(status);
  status = del_fld(docroot, db, hash_tbl, "min_pk");
  cBYE(status);

 BYE:
  free_if_non_null(str_is_break_on_wspace);
  free_if_non_null(str_len);
  if ( tempfile != NULL ) { unlink(tempfile); }
  free_if_non_null(tempfile);
  free_if_non_null(fldname);
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(src_fld_X, src_fld_nX);
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);
  fclose_if_non_null(ofp3);
  fclose_if_non_null(ofp4);
  fclose_if_non_null(ofp5);
  free_if_non_null(fname1);
  free_if_non_null(fname2);
  free_if_non_null(fname3);
  free_if_non_null(fname4);
  free_if_non_null(fname5);
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  rs_munmap(sz_src_fld_X, sz_src_fld_nX);
  return(status);
}

