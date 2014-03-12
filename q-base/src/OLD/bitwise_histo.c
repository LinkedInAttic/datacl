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
#include "bitwise_histo.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "meta_globals.h"
/* Input: Field f1 in Table t1. fldtype(f1) must be int or long long.
   Creates a new table t2. Deletes it if it exists. Creates a single
   field cnt in t2 of type long long. Number of columns in t2 is specified 
   by input num_bits. The i^{th} row of field cnt in table t2 is the number
   of rows of t1 where f1 had a 1 in the i^{th} bit (LSB). 
   */

//---------------------------------------------------------------
// START FUNC DECL
int 
bitwise_histo(
	   char *src_tbl,
	   char *src_fld,
	   char *str_num_bits,
	   char *dst_tbl
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *out_X = NULL; size_t out_nX = 0;
  FLD_TYPE *src_fld_meta = NULL, *nn_src_fld_meta = NULL;
  long long src_nR; 
  int src_tbl_id = INT_MIN, src_fld_id = INT_MIN, nn_src_fld_id = INT_MIN;
  int dst_tbl_id = INT_MIN, dst_fld_id = INT_MIN;
  char str_meta_data[256];
  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *opfile = NULL; FILE *ofp = 0;
  int num_bits; char *endptr = NULL;
  long long *outptr = NULL;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_num_bits == NULL ) || ( *str_num_bits == '\0' ) ) { go_BYE(-1); }
  num_bits = strtol(str_num_bits, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  zero_string(str_meta_data, 256);
  //-------------------------------------------------------- 
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  src_fld_meta = &(g_fld[src_fld_id]);
  status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0);
  cBYE(status);
  //--------------------------------------------------------
  nn_src_fld_id = g_fld[src_fld_id].nn_fld_id;
  if ( nn_src_fld_id >= 0 ) { 
    nn_src_fld_meta = &(g_fld[nn_src_fld_id]);
    status = rs_mmap(nn_src_fld_meta->filename, &nn_src_fld_X, &nn_src_fld_nX, 0);
    cBYE(status);
  }
  //--------------------------------------------------------
  // Create output file 
  long long filesz = num_bits * sizeof(long long);
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &out_X, &out_nX, 1); cBYE(status);
  outptr = (long long *)out_X;
  for ( int i = 0; i < num_bits; i++ ) { 
    outptr[i] = 0;
  }
  //------------------------------------------------------
  if ( strcmp(src_fld_meta->fldtype, "int") == 0 ) {
    int *i_inptr; unsigned int inval, bit;
    i_inptr = (int *)src_fld_X;
    for ( long long i = 0; i < src_nR; i++ ) { 
      if ( ( num_bits < 0 ) || ( num_bits > 32 ) ) { go_BYE(-1); }
      unsigned int bmask = 1;
      inval = *i_inptr;
      for ( int j = 0; j < num_bits; j++ ) { 
	bit = inval & bmask;
	if ( bit != 0 ) { 
	  outptr[j] ++;
	}
	bmask = bmask << 1;
      }
      i_inptr++;
    }
  }
  else if ( strcmp(src_fld_meta->fldtype , "long long") == 0 ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1); 
  }
  else { go_BYE(-1); }
  //-----------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  if ( dst_tbl_id >= 0 ) { /* Delete table */
    status = del_tbl(NULL, dst_tbl_id);
  }
  status = add_tbl(dst_tbl, str_num_bits, &dst_tbl_id); cBYE(status);
  //-----------------------------------------------------------
  // Add output field to meta data 
  sprintf(str_meta_data, "filename=%s:fldtype=long long:n_sizeof=8", opfile);
  status = add_fld(dst_tbl, "cnt", str_meta_data, &dst_fld_id);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(out_X, out_nX);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  free_if_non_null(opfile);
  return(status);
}
