#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "f_to_s.h"

#include "meta_globals.h"

#include "uniq_int.h"
#include "uniq_longlong.h"
#include "uniq_alldef_int.h"
#include "uniq_alldef_longlong.h"
#include "uniq_cnt_alldef_i_i.h"
#include "uniq_cnt_alldef_ll_ll.h"
#include "uniq_cnt_alldef_ll_dbl.h"
extern bool g_write_to_temp_dir;

//---------------------------------------------------------------
// START FUNC DECL
int 
count_vals(
	   char *src_tbl,
	   char *src_fld,
	   char *in_cnt_fld,
	   char *dst_tbl,
	   char *dst_fld,
	   char *out_cnt_fld
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *in_cnt_fld_X = NULL; size_t in_cnt_fld_nX = 0;
  FLD_TYPE *src_fld_meta = NULL, *in_cnt_fld_meta = NULL;
  FLD_TYPE *cnt_fld_meta = NULL, *nn_src_fld_meta = NULL;
  long long src_nR; 
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, nn_src_fld_id = INT_MIN;
  int dst_fld_id = INT_MIN;
  int cnt_fld_id = INT_MIN, nn_cnt_fld_id = INT_MIN;
  long long dst_nR, n_out;
  char buffer[32];
  char str_meta_data[256];
  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *out_X = NULL; char *cnt_X = NULL; 
  char *opfile = NULL, *tmp_opfile = NULL; FILE *ofp = 0;
  char *cnt_opfile = NULL, *tmp_cnt_opfile = NULL; FILE *cnt_ofp = 0;
  size_t n_out_X = 0, n_cnt_X = 0;
  unsigned int n_sizeof = INT_MAX;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 256);
  zero_string(buffer, 32);
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  src_fld_meta = &(g_fld[src_fld_id]);
  status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0); cBYE(status);
  nn_src_fld_id = src_fld_meta->nn_fld_id;
  if ( nn_src_fld_id >= 0 ) { 
    nn_src_fld_meta = &(g_fld[nn_src_fld_id]);
    status = rs_mmap(nn_src_fld_meta->filename, &nn_src_fld_X, &nn_src_fld_nX, 0); 
    cBYE(status);
  }
  // Get the count field if specified. It cannot have null values
  if ( *in_cnt_fld != '\0' ) { 
    status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
    chk_range(src_fld_id, 0, g_n_fld);
    src_fld_meta = &(g_fld[src_fld_id]);
    status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0); cBYE(status);
    nn_cnt_fld_id = cnt_fld_meta->nn_fld_id;
    chk_range(nn_cnt_fld_id, 0, g_n_fld);
  }
  /* Make sure src_fld field is sorted ascending */
  zero_string(buffer, 32);
  status = f_to_s(src_tbl, src_fld, "is_sorted", buffer);
  cBYE(status);
  if ( strcmp(buffer, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_fld, src_tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  // Create 2 temporary files to store the results. We allocate space
  // differently based on field types
  g_write_to_temp_dir = true;
  status = open_temp_file(&ofp, &tmp_opfile, 0); cBYE(status);
  fclose_if_non_null(ofp);
  status = open_temp_file(&cnt_ofp, &tmp_cnt_opfile, 0); cBYE(status);
  fclose_if_non_null(cnt_ofp);
  //------------------------------------------------------
  n_out = src_nR * sizeof(int); // upper bound 

  if ( strcmp(src_fld_meta->fldtype, "int") == 0 ) {
    status = mk_file(tmp_cnt_opfile, sizeof(int) * src_nR);
    cBYE(status);
    rs_mmap(tmp_cnt_opfile, &cnt_X, &n_cnt_X, 1);
    cBYE(status);

    status = mk_file(tmp_opfile, sizeof(int) * src_nR);
    cBYE(status);
    rs_mmap(tmp_opfile, &out_X, &n_out_X, 1);
    cBYE(status);
    if ( nn_src_fld_X == NULL ) { 
      if ( *in_cnt_fld == '\0' ) { 
	uniq_alldef_int((int *)src_fld_X, src_nR, (int *)out_X, 
			&dst_nR, (int *)cnt_X);
      }
      else {
	uniq_cnt_alldef_i_i((int *)src_fld_X, (int *)in_cnt_fld_X, 
			    src_nR, (int *)out_X, &dst_nR, (int *)cnt_X);
      }
    }
    else {
      uniq_int((int *)src_fld_X, src_nR, nn_src_fld_X,
	       (int *)out_X, &dst_nR, (int *)cnt_X);
    }
  }
  else if ( strcmp(src_fld_meta->fldtype, "long long") == 0 ) {
    status = mk_file(tmp_opfile, sizeof(long long) * src_nR);
    cBYE(status);
    rs_mmap(tmp_opfile, &out_X, &n_out_X, 1);
    cBYE(status);
    if ( nn_src_fld_X == NULL ) { 
      if ( *in_cnt_fld == '\0' ) { 
        status = mk_file(tmp_cnt_opfile, sizeof(long long) * src_nR);
        cBYE(status);
        rs_mmap(tmp_cnt_opfile, &cnt_X, &n_cnt_X, 1);
        cBYE(status);
	uniq_alldef_longlong((long long *)src_fld_X, src_nR, 
			     (long long *)out_X, &dst_nR, (long long *)cnt_X);
      }
      else {
	if ( strcmp(in_cnt_fld_meta->fldtype, "long long") == 0 ) { 
          status = mk_file(tmp_cnt_opfile, sizeof(long long) * src_nR);
          cBYE(status);
          rs_mmap(tmp_cnt_opfile, &cnt_X, &n_cnt_X, 1);
          cBYE(status);
	  uniq_cnt_alldef_ll_ll((long long *)src_fld_X, 
	    (long long *)in_cnt_fld_X, src_nR, (long long *)out_X, &dst_nR, 
	    (long long *)cnt_X);
	}
	else if ( strcmp(in_cnt_fld_meta->fldtype, "double") == 0 ) { 
          status = mk_file(tmp_cnt_opfile, sizeof(double) * src_nR);
          cBYE(status);
          rs_mmap(tmp_cnt_opfile, &cnt_X, &n_cnt_X, 1);
          cBYE(status);
	  uniq_cnt_alldef_ll_dbl((long long *)src_fld_X, 
	    (double *)in_cnt_fld_X, src_nR, (long long *)out_X, &dst_nR, 
	    (double *)cnt_X);
	}
      }
    }
    else {
      if ( *in_cnt_fld == '\0' ) { 
        status = mk_file(tmp_cnt_opfile, sizeof(long long) * src_nR);
        cBYE(status);
        rs_mmap(tmp_cnt_opfile, &cnt_X, &n_cnt_X, 1);
        cBYE(status);
        uniq_longlong((long long *)src_fld_X, src_nR, nn_src_fld_X, 
		    (long long *)out_X, &dst_nR, (long long *)cnt_X);
      }
      else {
	fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
      }
    }
  }
  else { go_BYE(-1); }
  if ( dst_nR == 0 ) { goto BYE; }
  //-----------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  if ( dst_tbl_id >= 0 ) { 
    status = del_tbl(NULL, dst_tbl_id);
  }
  sprintf(buffer, "%lld", dst_nR);
  status = add_tbl(dst_tbl, buffer, &dst_tbl_id);
  cBYE(status);
  //-----------------------------------------------------------
  // Now copy the temporary files over to real files
  status = open_temp_file(&ofp, &opfile, 0);
  cBYE(status);
  if ( strcmp(src_fld_meta->fldtype, "int") == 0 ) {
    fwrite(out_X, sizeof(int), dst_nR, ofp);
    n_sizeof = sizeof(int);
  }
  else if ( strcmp(src_fld_meta->fldtype, "long long") == 0 ) {
    fwrite(out_X, sizeof(long long), dst_nR, ofp);
    n_sizeof = sizeof(long long);
  }
  fclose_if_non_null(ofp);
  // Add output field to meta data 
  sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%u", opfile,
	  src_fld_meta->fldtype, n_sizeof);
  status = add_fld(dst_tbl, dst_fld, str_meta_data, &dst_fld_id);
  cBYE(status);
  // Since src_fld is sorted ascending, so is dst_fld 
  g_fld[dst_fld_id].sorttype = 1; 
  // Now copy the temporary count file to the real one
  status = open_temp_file(&cnt_ofp, &cnt_opfile, 0);
  cBYE(status);
  if ( *in_cnt_fld == '\0' ) { 
    if ( strcmp(src_fld_meta->fldtype, "int") == 0 ) {
      fwrite(cnt_X, sizeof(int), dst_nR, cnt_ofp);
      sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=%lu", 
        cnt_opfile, sizeof(int));
    }
    else if ( strcmp(src_fld_meta->fldtype, "long long") == 0 ) {
      fwrite(cnt_X, sizeof(long long), dst_nR, cnt_ofp);
      sprintf(str_meta_data, "filename=%s:fldtype=long long:n_sizeof=%lu",
        cnt_opfile, sizeof(long long));
    }
    else { go_BYE(-1); }
  }
  else {
    if ( strcmp(in_cnt_fld_meta->fldtype, "int") == 0 ) {
      fwrite(cnt_X, sizeof(int), dst_nR, cnt_ofp);
      sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=%lu",
        cnt_opfile, sizeof(int));
    }
    else if ( strcmp(in_cnt_fld_meta->fldtype, "long long") == 0 ) {
      fwrite(cnt_X, sizeof(long long), dst_nR, cnt_ofp);
      sprintf(str_meta_data, "filename=%s:fldtype=long long:n_sizeof=%lu",
        cnt_opfile, sizeof(long long));
    }
    else if ( strcmp(in_cnt_fld_meta->fldtype, "double") == 0 ) {
      fwrite(cnt_X, sizeof(double), dst_nR, cnt_ofp);
      sprintf(str_meta_data, "filename=%s:fldtype=double:n_sizeof=%lu",
        cnt_opfile, sizeof(double));
    }
    else { go_BYE(-1); }
  }
  fclose_if_non_null(cnt_ofp);
  // Add count field to meta data 
  status = add_fld(dst_tbl, out_cnt_fld, str_meta_data, &cnt_fld_id);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  g_write_to_temp_dir = false;
  rs_munmap(src_fld_X, src_fld_nX);
  free_if_non_null(opfile);
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(in_cnt_fld_X, in_cnt_fld_nX);
  rs_munmap(cnt_X, n_cnt_X);
  rs_munmap(out_X, n_out_X);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  unlink(tmp_opfile);
  unlink(tmp_cnt_opfile);
  free_if_non_null(tmp_opfile);
  free_if_non_null(tmp_cnt_opfile);
  free_if_non_null(cnt_opfile);
  return(status);
}
