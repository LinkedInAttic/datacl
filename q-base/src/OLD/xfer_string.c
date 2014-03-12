#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/mman.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "mk_file.h"
#include "meta_globals.h"

#include "conv_int_to_longlong.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
xfer_string(
	    FLD_TYPE *dst_idx_meta,
	    char *src_fld_X,
	    char *sz_src_fld_X,
	    long long src_nR,
	    char *dst_tbl,
	    char *dst_fld,
	    char *dst_idx_X,
	    char *nn_dst_idx_X,
	    long long dst_nR
	    )
// STOP FUNC DECL
{
  int status = 0;
  char str_meta_data[256];

  char *szX = NULL; size_t n_szX= 0; int *szptr = NULL;
  char *nnX = NULL; size_t n_nnX= 0; char *nnptr = NULL;

  int dst_fld_id, nn_dst_fld_id, sz_dst_fld_id;

  FILE *ofp    = NULL; char *opfile    = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  FILE *sz_ofp = NULL; char *sz_opfile = NULL;

  long long *offset_src_fld = NULL;
  long long *ll_dst_idx = NULL;
  bool is_any_null = false; 

  //--------------------------------------------------------
  status = open_temp_file(&ofp, &opfile, 0); cBYE(status);
  /* We cannot allocate storage for this since we do not know how much
   * to allocate a-priori */
  /* Open output fields for size and nn in mmmap mode */
  long long filesz = dst_nR * sizeof(int);
  status = open_temp_file(&sz_ofp, &sz_opfile, filesz); cBYE(status);
  fclose_if_non_null(sz_ofp);
  status = mk_file(sz_opfile, filesz); cBYE(status);
  status = rs_mmap(sz_opfile, &szX, &n_szX, 1); cBYE(status);
  szptr = (int *)szX;

  long long nnfilesz = dst_nR * sizeof(char);
  status = open_temp_file(&nn_ofp, &nn_opfile, nnfilesz); cBYE(status);
  fclose_if_non_null(nn_ofp);
  status = mk_file(nn_opfile, nnfilesz); cBYE(status);
  status = rs_mmap(nn_opfile, &nnX, &n_nnX, 1); cBYE(status);
  nnptr = (char *)nnX;
  /*-------------------------------------------------------------*/
  status = mk_offset((int *)sz_src_fld_X, src_nR, &offset_src_fld);
  cBYE(status);
  /* Convert from int to longlong if needed */
  if ( strcmp(dst_idx_meta->fldtype, "int") == 0 ) {
    ll_dst_idx = malloc(dst_nR * sizeof(long long));
    return_if_malloc_failed(ll_dst_idx);
    if ( dst_nR >= INT_MAX ) {
      fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1);
    }
    conv_int_to_longlong((int *)dst_idx_X, (int)dst_nR, ll_dst_idx);
  }
  else {
    ll_dst_idx = (long long *)dst_idx_X;
  }
  /*-------------------------------------------------------------*/
  for ( long long i = 0; i < dst_nR; i++ ) {
    int *i_sz_src_fld_ptr = (int *)sz_src_fld_X;
    long long idx;
    size_t nw;
    char *testptr;
    char nullc = 0;
    long long offset; int sz;
    if ( ( nn_dst_idx_X != NULL ) && ( nn_dst_idx_X[i] == FALSE ) ) { 
      // Output null value
      is_any_null = true;
      nnptr[i] = FALSE;
      szptr[i] = 1;
      nw = fwrite(&nullc, sizeof(char), 1,  ofp); // Null terminate
      if ( nw != 1 ) { go_BYE(-1); }
    }
    else { 
      idx = ll_dst_idx[i];
      if ( ( idx < 0 ) || ( idx >= src_nR ) ) { go_BYE(-1); }
      sz = i_sz_src_fld_ptr[idx];
      offset = offset_src_fld[idx];
      // This checks that strings are null terminated
      testptr = src_fld_X + offset + sz - 1;
      if ( *testptr != '\0' ) {
	go_BYE(-1);
      }
      szptr[i] = sz;
      nnptr[i] = TRUE;
      nw = fwrite(src_fld_X + offset, sizeof(char), sz, ofp);
      if ( nw != sz ) { go_BYE(-1); }
    }
  }
  fclose_if_non_null(ofp);
  rs_munmap(szX, n_szX);
  rs_munmap(nnX, n_nnX);


  // Add output field to meta data 
  zero_string(str_meta_data, 256);
  strcpy(str_meta_data, "fldtype=char string:n_sizeof=0");
  strcat(str_meta_data, ":filename="); 
  strcat(str_meta_data, opfile);
  status = add_fld(dst_tbl, dst_fld, str_meta_data, &dst_fld_id); cBYE(status);
  // Add size field 
  status = add_aux_fld(dst_tbl, dst_fld, sz_opfile, "sz", &sz_dst_fld_id);
  cBYE(status);
  if ( is_any_null ) {
    status = add_aux_fld(dst_tbl, dst_fld, nn_opfile, "nn", &nn_dst_fld_id);
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
  }
 BYE:
  if ( strcmp(dst_idx_meta->fldtype, "int") == 0 ) {
    free_if_non_null(ll_dst_idx);
  }
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  fclose_if_non_null(sz_ofp);

  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  free_if_non_null(sz_opfile);
  free_if_non_null(offset_src_fld);
  return(status);
}
