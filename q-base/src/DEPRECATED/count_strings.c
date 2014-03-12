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
#include "sort.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "f_to_s.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "meta_globals.h"
extern bool g_write_to_temp_dir;

//---------------------------------------------------------------
// START FUNC DECL
int 
count_strings(
	   char *src_tbl,
	   char *src_fld,
	   char *str_trunc_len, 
	   // truncate strings beyond this len
	   char *dst_tbl
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *sz_src_fld_X = NULL; size_t sz_src_fld_nX = 0;
  FLD_TYPE *src_fld_meta = NULL; 
  FLD_TYPE *nn_src_fld_meta = NULL; 
  FLD_TYPE *sz_src_fld_meta = NULL;
  long long src_nR; 
  int src_tbl_id = INT_MIN, src_fld_id = INT_MIN;
  int dst_tbl_id = INT_MIN;
  int nn_src_fld_id = INT_MIN, sz_src_fld_id = INT_MIN;
  int itemp;
  char str_dst_nR[32]; long long dst_nR = 0;
  char str_meta_data[4096];
  char *out_X = NULL; char *cnt_X = NULL; 
  char *dst_fk_X = NULL; size_t dst_fk_nX = 0;
  FILE *ofp = NULL; char *opfile = NULL; 
  FILE *tmp_ofp = NULL; char *tmp_opfile = NULL; 
  FILE *nn_ofp = NULL; char *nn_opfile = NULL; 
  FILE *sz_ofp = NULL; char *sz_opfile = NULL; 
  FILE *cnt_ofp = NULL; char *cnt_opfile = NULL; 
  FILE *from_dst_ofp = NULL; char *from_dst_opfile = NULL; 
  FILE *dst_fk_ofp = NULL; char *dst_fk_opfile = NULL; int *idst_fk = NULL;
  FILE *to_dst_ofp = NULL; char *to_dst_opfile = NULL; 
  size_t out_nX = 0, cnt_nX = 0;
  int *szptr = NULL; char *nnptr = NULL;
  char  *cptr = NULL, *prev_cptr = NULL; 
  char *sz_src_fld = NULL; char *str_maxlen = NULL; 
  char *from_dst_fld = NULL, *to_dst_fld = NULL, *dst_fk_fld = NULL;
  char *null_str = NULL, *endptr = NULL;
  int maxlen, rec_size, trunc_len = -1; // no truncation by default
  long long cnt = 1; int icnt = 1;
  bool is_any_null = false;
      char is_nn; int sz;
  //----------------------------------------------------------------
  if ( str_trunc_len != NULL ) { 
    trunc_len = strtol(str_trunc_len, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( trunc_len <= 1 ) { go_BYE(-1); } 
    trunc_len++; // to include null character
  }
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 4096);
  zero_string(str_dst_nR, 32);
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  src_nR = g_tbl[src_tbl_id].nR;
  if ( src_nR > INT_MAX ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
  }
  //--------------------------------------------------------
  src_fld_meta = &(g_fld[src_fld_id]);
  status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0);
  cBYE(status);
  //--------------------------------------------------------

  if ( strcmp(src_fld_meta->fldtype, "char string") != 0 ) { go_BYE(-1); }
  cptr = src_fld_X;
  // Get nn field for source if if it exists
  nn_src_fld_id = g_fld[src_fld_id].nn_fld_id;
  if ( nn_src_fld_id >= 0 ) { 
    nn_src_fld_meta = &(g_fld[nn_src_fld_id]);
    status = rs_mmap(nn_src_fld_meta->filename, &nn_src_fld_X, &nn_src_fld_nX, 0);
    cBYE(status);
  }
  nnptr = (char *)nn_src_fld_X;
  // Get sz field for source 
  sz_src_fld_id = g_fld[src_fld_id].sz_fld_id;
  chk_range(sz_src_fld_id, 0, g_n_fld);
  sz_src_fld_meta = &(g_fld[sz_src_fld_id]);
  status = rs_mmap(sz_src_fld_meta->filename, &sz_src_fld_X, &sz_src_fld_nX, 0);
  cBYE(status);
  szptr = (int *)sz_src_fld_X;
  //------------------------------------
  mcr_alloc_null_str(from_dst_fld, (MAX_LEN_FILE_NAME+16));
  mcr_alloc_null_str(to_dst_fld, (MAX_LEN_FILE_NAME+16));
  mcr_alloc_null_str(dst_fk_fld, (MAX_LEN_FILE_NAME+16));
  mcr_alloc_null_str(sz_src_fld, (MAX_LEN_FILE_NAME+16));
  mcr_alloc_null_str(str_maxlen, 32);

  strcpy(sz_src_fld, "_sz_"); strcat(sz_src_fld, src_fld);

  strcpy(from_dst_fld, src_tbl); strcat(from_dst_fld, "_from");
  strcpy(to_dst_fld,   dst_tbl); strcat(to_dst_fld,   "_to");
  strcpy(dst_fk_fld,   dst_tbl); strcat(dst_fk_fld,   "_id");
  
  // Get max length of src field 
  status = f_to_s(src_tbl, sz_src_fld, "max", str_maxlen);
  cBYE(status);
  maxlen = strtol(str_maxlen, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( maxlen <= 0 ) { go_BYE(-1); }
  if ( trunc_len < 0 ) { 
    trunc_len = maxlen;
  }
  mcr_alloc_null_str(null_str, trunc_len);
  //--------------------------------------------------------
  // Write out src field as constant length field 
  status = open_temp_file(&tmp_ofp, &tmp_opfile, 0);
  cBYE(status);
  for ( int i = 0; i < src_nR; i++ ) { 
    int sz, fill_sz; 
    sz = szptr[i]; // includes terminating null character
    if ( sz <= trunc_len ) { 
      fwrite(cptr, sizeof(char), sz, tmp_ofp);
      fill_sz = trunc_len - sz;
      fwrite(null_str, sizeof(char), fill_sz, tmp_ofp);
    }
    else {
      fwrite(cptr, sizeof(char), trunc_len-1, tmp_ofp);
      fwrite(null_str, sizeof(char), 1, tmp_ofp);
    }
    // Append origin of this string 
    fwrite(&i, sizeof(int), 1, tmp_ofp);
    cptr += sz;
    // TODO: Check about nullc and sz 
  }
  fclose_if_non_null(tmp_ofp);
  //---------------------------------------------------------
  // Sort output file so that similar titles occur together
  status = rs_mmap(tmp_opfile, &X, &nX, 1); // 1 for writing
  cBYE(status);
  // Note that we added the origin of the string. Hence the size of a
  // record is trunc_len + sizeof(int)
  rec_size = trunc_len + sizeof(int);
  qsort(X, src_nR, rec_size, str_compare);
  rs_munmap(X, nX);
  //--------------------------------------------------------
  // Now we need to compact the sorted output so that it is not padded
  // Open 4 files for 
  // output string, sz(output string), nn(output string), cnt 
  status = open_temp_file(&ofp,     &opfile, 0);     cBYE(status);
  status = open_temp_file(&nn_ofp,  &nn_opfile, 0);  cBYE(status);
  status = open_temp_file(&sz_ofp,  &sz_opfile, 0);  cBYE(status);
  status = open_temp_file(&cnt_ofp, &cnt_opfile, 0); cBYE(status);
  status = open_temp_file(&from_dst_ofp, &from_dst_opfile, 0); cBYE(status);
  status = open_temp_file(&to_dst_ofp, &to_dst_opfile, 0); cBYE(status);

  status = open_temp_file(&dst_fk_ofp, &dst_fk_opfile, 0); cBYE(status);
  fclose_if_non_null(dst_fk_ofp);
  mk_file(dst_fk_opfile, src_nR * sizeof(int)); cBYE(status);
  rs_mmap(dst_fk_opfile,  &dst_fk_X, &dst_fk_nX, 1); cBYE(status);
  idst_fk = (int *)dst_fk_X;


  status = rs_mmap(tmp_opfile, &X, &nX, 0); 
  cBYE(status);
  cptr = X;
  prev_cptr = cptr;
  cptr += rec_size;
  cnt = icnt = 1;
  for ( int i = 0; i < src_nR; i++ ) { 
    char *xptr; int *iptr; 
    xptr = prev_cptr + trunc_len;
    iptr = (int *)xptr;
    if ( ( *iptr < 0 ) || ( *iptr >= src_nR ) ) { go_BYE(-1); }
    fwrite(iptr,    sizeof(int), 1, from_dst_ofp);
    fwrite(&dst_nR, sizeof(int), 1, to_dst_ofp);
    idst_fk[*iptr] = dst_nR;
    if ( strcmp(cptr, prev_cptr) != 0 ) { 

      sz = strlen(prev_cptr);
      if ( sz == 0 ) { is_nn = FALSE; } else { is_nn = TRUE; }
      sz++; // for null character termination

      fwrite(prev_cptr,   sizeof(char), sz, ofp);
      fwrite(&sz,         sizeof(int),  1, sz_ofp);
      fwrite(&is_nn,      sizeof(char), 1,  nn_ofp);

      if ( cnt > INT_MAX ) { go_BYE(-1); }
      icnt = cnt;
      fwrite(&icnt,   sizeof(int), 1,  cnt_ofp);
      cnt = 1;
      dst_nR++;
    }
    else {
      cnt++;
    }
    prev_cptr = cptr;
    cptr += rec_size;
  }
  // START: Now for the last guy
  cnt--; // this is needed
  sz = strlen(prev_cptr);
  if ( sz == 0 ) { is_nn = FALSE; } else { is_nn = TRUE; }
  if ( is_nn == FALSE ) { 
    is_any_null = true;
  }
  sz++; // for null character termination

  fwrite(prev_cptr,   sizeof(char), sz, ofp);
  fwrite(&sz,         sizeof(int),  1, sz_ofp);
  fwrite(&is_nn,      sizeof(char), 1,  nn_ofp);

  if ( cnt > INT_MAX ) { go_BYE(-1); }
  icnt = cnt;
  fwrite(&icnt,   sizeof(int), 1,  cnt_ofp);
  dst_nR++;
  // STOP: Now for the last guy
  //---------------------------
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  fclose_if_non_null(sz_ofp);
  fclose_if_non_null(cnt_ofp);
  fclose_if_non_null(from_dst_ofp);
  fclose_if_non_null(to_dst_ofp);
  //-----------------------------------------------
  // Delete temp file
  unlink(tmp_opfile);
  //-----------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  if ( dst_tbl_id >= 0 ) { /* Delete table */
    status = del_tbl(NULL, dst_tbl_id);
  }
  sprintf(str_dst_nR, "%lld", dst_nR);
  status = add_tbl(dst_tbl, str_dst_nR, &dst_tbl_id);
  cBYE(status);
  //-----------------------------------------------------------
  /* TODO I think we can dispense with from_dst_fld and to_dst_fld */
  sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=4", from_dst_opfile);
  status = add_fld(src_tbl, from_dst_fld, str_meta_data, &itemp);
  cBYE(status);

  sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=4", to_dst_opfile);
  status = add_fld(src_tbl, to_dst_fld, str_meta_data, &itemp);
  cBYE(status);

  sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=4", dst_fk_opfile);
  status = add_fld(src_tbl, dst_fk_fld, str_meta_data, &itemp);
  cBYE(status);

  // Add output field to meta data 
  sprintf(str_meta_data, "filename=%s:fldtype=char string:n_sizeof=0", opfile);
  status = add_fld(dst_tbl, src_fld, str_meta_data, &itemp);
  cBYE(status);
  fclose_if_non_null(cnt_ofp);
  // Add nn field if needed
  if ( is_any_null ) { 
    status = add_aux_fld(dst_tbl, src_fld, nn_opfile, "nn", &itemp);
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
  }
  // Add sz field 
  status = add_aux_fld(dst_tbl, src_fld, sz_opfile, "sz", &itemp);
  cBYE(status);
  // Add count field to meta data 
  sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=4",
      cnt_opfile);
  status = add_fld(dst_tbl, "cnt", str_meta_data, &itemp);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  free_if_non_null(null_str);
  rs_munmap(X, nX);
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(cnt_X, cnt_nX);
  rs_munmap(out_X, out_nX);
  rs_munmap(dst_fk_X, dst_fk_nX);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  fclose_if_non_null(ofp);     free_if_non_null(opfile);
  fclose_if_non_null(nn_ofp);  free_if_non_null(nn_opfile);
  fclose_if_non_null(sz_ofp);  free_if_non_null(sz_opfile);
  fclose_if_non_null(cnt_ofp); free_if_non_null(cnt_opfile);
  fclose_if_non_null(tmp_ofp); free_if_non_null(tmp_opfile);
  fclose_if_non_null(from_dst_ofp); free_if_non_null(from_dst_opfile);
  fclose_if_non_null(to_dst_ofp); free_if_non_null(to_dst_opfile);
  free_if_non_null(sz_src_fld);
  free_if_non_null(dst_fk_fld);
  free_if_non_null(str_maxlen);
  return(status);
}
