#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "add_tbl.h"
#include "is_fld.h"
#include "f_to_s.h"
#include "is_nn_fld.h"
#include "open_temp_file.h"
#include "mk_file.h"
#include "drop_null_fld.h"
#include "del_fld.h"
#include "del_tbl.h"
#include "fop.h"
#include "meta_globals.h"

// START FUNC DECL
int 
copy_range(
	   char *src_tbl,
	   char *src_fld,
	   char *dst_tbl,
	   char *dst_fld,
	   char *aux_tbl,
	   char *aux_fld_lb,
	   char *aux_fld_ub,
	   char *order
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;

  char *lb_X  = NULL; size_t lb_nX = 0;
  char *ub_X  = NULL; size_t ub_nX = 0;

  char *dst_X = NULL; size_t dst_nX = 0;
  char *nn_dst_X = NULL; size_t nn_dst_nX = 0;

  FLD_TYPE *src_fld_meta = NULL;
  FLD_TYPE *nn_src_fld_meta = NULL;
  FLD_TYPE *aux_lb_meta = NULL;
  FLD_TYPE *aux_ub_meta = NULL;

  long long src_nR = INT_MIN, dst_nR = INT_MIN, aux_nR = INT_MIN;
#define BUFLEN (MAX_LEN_FLD_NAME + 32)
  char str_meta_data[1024]; char buffer[BUFLEN];
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN, aux_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, dst_fld_id = INT_MIN; 
  int nn_src_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN; 
  int lb_id = INT_MIN;
  int ub_id = INT_MIN;
  int itemp;

  FILE *ofp = NULL, *nn_ofp = NULL;
  char *opfile = NULL, *nn_opfile = NULL;
  //----------------------------------------------------------------
  zero_string(str_meta_data, 1024);
  zero_string(buffer, BUFLEN);
  if ( strcmp(src_tbl, aux_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(aux_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  //----------------------------------------------------------------
  // src_tbl
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;

  // Delete dst_tbl if it exists
  status = del_tbl(dst_tbl, -1); cBYE(status);

  // src_fld
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  src_fld_meta = &(g_fld[src_fld_id]);
  if ( ( strcmp(src_fld_meta->fldtype, "bool") == 0 )  ||
       ( strcmp(src_fld_meta->fldtype, "char") == 0 ) ||
       ( strcmp(src_fld_meta->fldtype, "int") == 0 )  ||
       ( strcmp(src_fld_meta->fldtype, "float") == 0 ) ||
       ( strcmp(src_fld_meta->fldtype, "double") == 0 ) ) {
    /* all is well */
  }
  else { go_BYE(-1); }
  status = rs_mmap(src_fld_meta->filename, &X, &nX, 0); cBYE(status);
  nn_src_fld_id = src_fld_meta->nn_fld_id;
  if ( nn_src_fld_id >= 0 ) { 
    nn_src_fld_meta = &(g_fld[nn_src_fld_id]);
    status = rs_mmap(nn_src_fld_meta->filename, &nn_X, &nn_nX, 0); cBYE(status);
  }
  //-=======================================================
  status = is_tbl(aux_tbl, &aux_tbl_id); cBYE(status);
  chk_range(aux_tbl_id, 0, g_n_tbl);
  aux_nR = g_tbl[aux_tbl_id].nR;  
  if ( aux_nR == 0 ) { go_BYE(-1); }

  status = is_fld(NULL, aux_tbl_id, aux_fld_lb, &lb_id); cBYE(status);
  chk_range(lb_id, 0, g_n_fld);
  aux_lb_meta = &(g_fld[lb_id]);
  status = rs_mmap(aux_lb_meta->filename, &lb_X, &lb_nX, 0); cBYE(status);
  // all values of aux_fld_lb must be defined
  status = is_nn_fld(aux_tbl, aux_fld_lb, &itemp, buffer); cBYE(status);
  if ( itemp >= 0 ) { go_BYE(-1); }
  if ( strcmp(aux_lb_meta->fldtype, "long long") != 0 ) { go_BYE(-1); }
  //----------------------------------------------------------------

  status = is_fld(NULL, aux_tbl_id, aux_fld_ub, &ub_id); cBYE(status);
  chk_range(ub_id, 0, g_n_fld);
  aux_ub_meta = &(g_fld[ub_id]);
  status = rs_mmap(aux_ub_meta->filename, &ub_X, &ub_nX, 0); cBYE(status);
  // all values of aux_fld_ub must be defined
  status = is_nn_fld(aux_tbl, aux_fld_ub, &itemp, buffer); cBYE(status);
  if ( itemp >= 0 ) { go_BYE(-1); }
  if ( strcmp(aux_ub_meta->fldtype, "long long") != 0 ) { go_BYE(-1); }
  //----------------------------------------------------------------
  long long *ll_lb = (long long *)lb_X;
  long long *ll_ub = (long long *)ub_X;
  long long lb, ub;
  dst_nR = 0;
  for ( long long i = 0; i < aux_nR; i++ ) {
    lb = ll_lb[i];
    ub = ll_ub[i];
    if ( ub <= lb    ) { go_BYE(-1); }
    if ( lb < 0      ) { go_BYE(-1); }
    if ( ub > src_nR ) { go_BYE(-1); }
    dst_nR += (ub - lb );
  }

  long long filesz = src_fld_meta->n_sizeof * dst_nR;
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status); 
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &dst_X, &dst_nX, 1); cBYE(status);

  if ( nn_src_fld_id >= 0 ) { 
    long long nnfilesz = sizeof(char) * dst_nR;
    status = open_temp_file(&nn_ofp, &nn_opfile, nnfilesz); cBYE(status); 
    fclose_if_non_null(nn_ofp);
    status = mk_file(nn_opfile, nnfilesz); cBYE(status);
    status = rs_mmap(nn_opfile, &nn_dst_X, &nn_dst_nX, 1); cBYE(status);
  }
#define CUTOFF 32
  if ( ( order == NULL ) || 
      ( *order == '\0' ) ||
      ( ( strcmp(order, "sortA") == 0 ) && 
	( ( aux_nR > CUTOFF ) || ( aux_nR == 1 ) ) ) ) {
    /* preserve order */
    for ( long long i = 0; i < aux_nR; i++ ) {
      lb = ll_lb[i];
      ub = ll_ub[i];
      int sz = src_fld_meta->n_sizeof;
      memcpy(dst_X, X + (lb * sz), (sz * (ub - lb)));
      dst_X += sz * (ub - lb );
      if ( nn_src_fld_id >= 0 ) { 
	int nnsz = sizeof(char);
	memcpy(nn_dst_X, nn_X + (lb * nnsz), (nnsz * (ub - lb)));
	nn_dst_X += sizeof(char) * (ub - lb );
      }
    }
  }
  else if ( strcmp(order, "sortA") == 0 ) {
    if ( strcmp(src_fld_meta->fldtype, "int") != 0 ) { 
      fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
    }
    if ( ( aux_nR == 1 ) || ( aux_nR > CUTOFF ) ) { go_BYE(-1); }
    int *iptrs[CUTOFF];
    long long num_to_consume[CUTOFF]; 
    long long num_consumed[CUTOFF]; 
    for ( long long i = 0; i < aux_nR; i++ ) {
      lb = ll_lb[i];
      ub = ll_ub[i];
      num_to_consume[i] = ub - lb;
      num_consumed[i]   = 0;
      iptrs[i] = (int *)X;
      iptrs[i] += lb;
    }
    int min_val, min_range, alt_val;
    int *outptr = (int *)dst_X;
    for ( long long i = 0; i  < dst_nR; i++ ) { 
      /* Find the smallest */
      min_val = INT_MAX;
      min_range = INT_MIN;
      bool some_one = false;
      for ( int j = 0; j < aux_nR; j++ ) { 
	if ( num_consumed[j] >= num_to_consume[j] ) {
	  continue;
	}
        some_one = true;
        alt_val = *(iptrs[j]); 
	if ( alt_val < min_val ) {
	  min_val = alt_val;
	  min_range = j;
	}
      }
      if ( some_one == false ){ go_BYE(-1); }
      *outptr = min_val;
      outptr++;
      iptrs[min_range]++;
      num_consumed[min_range]++;
    }
    for ( int j = 0; j < aux_nR; j++ ) { 
      if ( num_consumed[j] != num_to_consume[j] ) { go_BYE(-1); }
    }
  }
  else { go_BYE(-1); }
  char str_result[32];
  int *dbgptr = (int *)dst_X;
  sprintf(str_result, "%lld", dst_nR);
  status = add_tbl(dst_tbl, str_result, &dst_tbl_id); cBYE(status);
  sprintf(str_meta_data, "n_sizeof=%d:fldtype=%s:filename=%s", 
	  src_fld_meta->n_sizeof, src_fld_meta->fldtype, opfile);
  status = add_fld(dst_tbl, dst_fld, str_meta_data, &dst_fld_id); cBYE(status);
  if ( nn_src_fld_id >= 0 ) { 
    status = add_aux_fld(dst_tbl, dst_fld, nn_opfile, "nn", &nn_dst_fld_id);
    cBYE(status);
    strcpy(buffer, "_nn_"); strcat(buffer, dst_fld);
    status = f_to_s(dst_tbl, buffer, "sum", str_result); cBYE(status); 
    char *endptr = NULL;
    long long nn_dst_nR = strtoll(str_result, &endptr, 10);
    if ( nn_dst_nR == dst_nR ) { /* nn field not needed */
      status = drop_null_fld(dst_tbl, dst_fld); cBYE(status);
    }
  }
  //--------------------------------------------
  if ( ( order != NULL ) && ( strcmp(order, "sortA") == 0 ) ) { 
    status = f_to_s(dst_tbl, dst_fld, "is_sorted", str_result);
    cBYE(status);
    if ( strcmp(str_result, "ascending") != 0 ) { 
      status = fop(dst_tbl, dst_fld, "sortA"); cBYE(status);
    }
  }
  //--------------------------------------------
 BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(dst_X, dst_nX);
  rs_munmap(nn_dst_X, nn_dst_nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  free_if_non_null(nn_opfile);
  free_if_non_null(opfile);
  return(status);
}
