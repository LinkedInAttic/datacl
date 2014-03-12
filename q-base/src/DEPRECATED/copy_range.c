#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "add_tbl.h"
#include "is_fld.h"
#include "f_to_s.h"
#include "is_nn_fld.h"
#include "mk_temp_file.h"
#include "drop_nn_fld.h"
#include "del_fld.h"
#include "del_tbl.h"
#include "fop.h"
#include "meta_globals.h"


int
is_range(
	 char *mat_tbl, 
	 char *mat_fld, 
	 long long *ptr_mat_lb, 
	 long long *ptr_mat_ub
	 )
{
  /* lower bound is inclusive, upper bound is exclusive */
  char *endptr = NULL;
  if ( ( mat_tbl == NULL ) || ( *mat_tbl == '\0' ) ) { return(false); }
  *ptr_mat_lb = strtoll(mat_tbl, &endptr, 10);
  if ( *endptr != '\0' ) { return(false); }
  if ( *ptr_mat_lb < 0 ) { return(false); }

  if ( ( mat_fld == NULL ) || ( *mat_fld == '\0' ) ) { return(false); }
  *ptr_mat_ub = strtoll(mat_fld, &endptr, 10);
  if ( *endptr != '\0' ) { return(false); }
  if ( *ptr_mat_ub < 0 ) { return(false); }
  if ( *ptr_mat_lb >= *ptr_mat_ub ) { return(false); }
  return(true);
}
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
	   char *order,
	   /* If mat_fld provided, then we are interested only in
	    * those values that are also present in mat_fld */
	   /* Alternatively, mat_tbl/mat_fld arguments could be used to
	    * provide a lower bound/upper bound for useful indexes */
	   char *mat_tbl, // mat is abbreviation for match 
	   char *mat_fld
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;

  char *lb_X  = NULL; size_t lb_nX = 0;
  char *ub_X  = NULL; size_t ub_nX = 0;

  char *dst_X = NULL; size_t dst_nX = 0;
  char *mat_X = NULL; size_t mat_nX = 0;
  char *nn_dst_X = NULL; size_t nn_dst_nX = 0;

  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec, aux_tbl_rec, mat_tbl_rec;
  FLD_REC_TYPE mat_fld_rec, nn_mat_fld_rec;
  FLD_REC_TYPE src_fld_rec, nn_src_fld_rec;
  FLD_REC_TYPE dst_fld_rec, nn_dst_fld_rec;
  FLD_REC_TYPE aux_fld_lb_rec, nn_aux_fld_lb_rec; 
  FLD_REC_TYPE aux_fld_ub_rec, nn_aux_fld_ub_rec;

  long long src_nR = LLONG_MIN, dst_nR = LLONG_MIN; 
  long long mat_dst_nR = 0;
  long long aux_nR = LLONG_MIN, mat_nR = LLONG_MIN;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int aux_tbl_id = INT_MIN, mat_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, mat_fld_id = INT_MIN, dst_fld_id = INT_MIN; 
  int nn_src_fld_id = INT_MIN, nn_mat_fld_id, nn_dst_fld_id = INT_MIN; 
  int aux_fld_lb_id = INT_MIN, nn_aux_fld_lb_id = INT_MIN;
  int aux_fld_ub_id = INT_MIN, nn_aux_fld_ub_id = INT_MIN;
  char **Y = NULL; int nY = 0;

#define BUFLEN (MAX_LEN_FLD_NAME+1+32)
  char strbuf[BUFLEN];

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( aux_tbl == NULL ) || ( *aux_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  if ( order == NULL ) { go_BYE(-1); }
  if ( strcmp(src_tbl, aux_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(aux_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  if ( ( strcmp(order, "sortA") == 0 ) || ( *order == '\0' ) ) { 
    // all is well 
  }
  else {
    go_BYE(-1);
  }
  //----------------------------------------------------------------
  // src_tbl
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;

  // Delete dst_tbl if it exists
  status = del_tbl(dst_tbl, -1); cBYE(status);

  // src_fld
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);

  status = get_data(src_fld_rec, &X, &nX, false); cBYE(status);
  if ( nn_src_fld_id >= 0 ) { 
    status = get_data(nn_src_fld_rec, &nn_X, &nn_nX, 0); cBYE(status);
  }
  if ( src_fld_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( strcmp(order, "sortA") == 0 ) {
    if ( nn_src_fld_id >= 0 ) { go_BYE(-1); }
  }
  //-=======================================================
  // mat_tbl
  // Check whether we have been given a range or a table
  long long mat_lb = LLONG_MIN, mat_ub = LLONG_MAX; 
  bool is_mat_range = false, is_mat_tbl = false;
  is_mat_range = is_range(mat_tbl, mat_fld, &mat_lb, &mat_ub);
  if ( is_mat_range ) { 
    /* nothing more to do */
  }
  else {
    if ( ( mat_tbl != NULL ) && ( *mat_tbl != '\0' ) ) {
      status = is_tbl(mat_tbl, &mat_tbl_id, &mat_tbl_rec); cBYE(status);
      chk_range(mat_tbl_id, 0, g_n_tbl);
      mat_nR = g_tbls[mat_tbl_id].nR;
      status = is_fld(NULL, mat_tbl_id, mat_fld, &mat_fld_id, &mat_fld_rec, 
		      &nn_mat_fld_id, &nn_mat_fld_rec); cBYE(status);
      chk_range(mat_fld_id, 0, g_n_fld);

      status = get_data(mat_fld_rec, &mat_X, &mat_nX, 0); cBYE(status);
      if ( nn_mat_fld_id >= 0 ) { go_BYE(-1); }
      if ( mat_fld_rec.fldtype != I4 ) { go_BYE(-1); }
      if ( mat_fld_rec.srttype != ascending ) { go_BYE(-1); }
      is_mat_tbl = true;
    }
  }
  //-=======================================================
  // aux tbl 
  status = is_tbl(aux_tbl, &aux_tbl_id, &aux_tbl_rec); cBYE(status);
  chk_range(aux_tbl_id, 0, g_n_tbl);
  aux_nR = g_tbls[aux_tbl_id].nR;  
  if ( aux_nR == 0 ) { go_BYE(-1); }

  // aux fld lb 
  status = is_fld(NULL, aux_tbl_id, aux_fld_lb, &aux_fld_lb_id, 
		  &aux_fld_lb_rec, &nn_aux_fld_lb_id, &nn_aux_fld_lb_rec); cBYE(status);
  chk_range(aux_fld_lb_id, 0, g_n_fld);
  status = get_data(aux_fld_lb_rec, &lb_X, &lb_nX, 0); cBYE(status);
  // all values of aux_fld_lb must be defined
  if ( nn_aux_fld_lb_id >= 0 ) { go_BYE(-1); }
  if ( aux_fld_lb_rec.fldtype != I8 ) { go_BYE(-1); }
  //----------------------------------------------------------------
  // aux fld lb 
  status = is_fld(NULL, aux_tbl_id, aux_fld_ub, &aux_fld_ub_id, 
		  &aux_fld_ub_rec, &nn_aux_fld_ub_id, &nn_aux_fld_ub_rec); cBYE(status);
  chk_range(aux_fld_ub_id, 0, g_n_fld);
  status = get_data(aux_fld_ub_rec, &ub_X, &ub_nX, 0); cBYE(status);
  // all values of aux_fld_ub must be defined
  if ( nn_aux_fld_ub_id >= 0 ) { go_BYE(-1); }
  if ( aux_fld_ub_rec.fldtype != I8 ) { go_BYE(-1); }
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
  // What we have computed here is an uppe bound on dst_nR. If there is
  // a mat_tbl or a mat_range provided, this value may decrease and we 
  // need to adjust for it.

  int fldsz = 0; size_t filesz = 0;
  int ddir_id = -1, fileno = -1;
  int nn_ddir_id = -1, nn_fileno = -1;
  int nn_sz = sizeof(char);
  status = get_fld_sz(src_fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = fldsz * dst_nR;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &dst_X, &dst_nX, 1); cBYE(status);

  if ( nn_src_fld_id >= 0 ) { 
    status = get_fld_sz(I1, &fldsz); cBYE(status);
    filesz = fldsz * dst_nR;
    status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno); cBYE(status);
    status = q_mmap(nn_ddir_id, nn_fileno, &nn_dst_X, &nn_dst_nX, 1); cBYE(status);
  }
#define CUTOFF 32
  /* If 
   * (i) there are more than <CUTOFF> ranges to copy or 
   * (ii) there is exactly 1 range to copy or 
   * (iii) we don't care about oder, then just blast them out one by one */
  bool is_simple_copy = false;
  if ( ( aux_nR > CUTOFF ) || ( aux_nR == 1 )  || ( *order == '\0' ) )  {
    is_simple_copy = true;
  }
  if ( ( is_mat_tbl ) || ( is_mat_range ) ) { is_simple_copy = false; }
  if ( is_simple_copy ) {
    for ( long long i = 0; i < aux_nR; i++ ) {
      lb = ll_lb[i];
      ub = ll_ub[i];
      memcpy(dst_X, X + (lb * fldsz), (fldsz * (ub - lb)));
      dst_X += fldsz * (ub - lb );
      if ( nn_src_fld_id >= 0 ) { 
	memcpy(nn_dst_X, nn_X + (lb * nn_sz), (nn_sz * (ub - lb)));
	nn_dst_X += nn_sz * (ub - lb );
      }
    }
  }
  else if ( strcmp(order, "sortA") == 0 ) {
    int *iptrs[CUTOFF];
    int *mat_ptr = NULL, mat_val, mat_idx = INT_MIN;
    long long num_to_consume[CUTOFF]; 
    long long num_consumed[CUTOFF]; 
    if ( mat_fld_id >= 0 ) { 
      mat_idx = 0;
      mat_ptr = (int *)mat_X;
      mat_val = mat_ptr[mat_idx];
    }
    for ( long long i = 0; i < aux_nR; i++ ) {
      lb = ll_lb[i];
      ub = ll_ub[i];
      num_to_consume[i] = ub - lb;
      num_consumed[i]   = 0;
      iptrs[i] = (int *)X;
      iptrs[i] += lb;
    }
    int min_val, min_range, alt_val, out_val;
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
      out_val = min_val;
      bool add_val = false;
      // Check whether out_val is in match values 
      if ( ( is_mat_tbl ) || ( is_mat_range ) )  {
	if ( is_mat_tbl ) { 
	  for ( ; mat_idx < mat_nR; mat_idx++) { 
	    mat_val = mat_ptr[mat_idx];
	    if ( out_val > mat_val ) {
	      // Need to increment mat_idx smartly so that we don't do a
	      // sequential scan of the input data
	    }
	    else if ( out_val == mat_val ) {
	      add_val = true; break;
	    }
	    else if ( out_val < mat_val ) {
	      add_val = false; break;
	    }
	  }
	}
	else {
	  if ( ( out_val >= mat_lb ) && ( out_val < mat_ub ) ) {
	    add_val = true;
	  }
	  else {
	    add_val = false;
	  }
	}
      }
      else {
	add_val = true;
      }
      //---------------------------------------------------------
      if ( add_val == true ) { 
        *outptr = out_val;
        outptr++;
	mat_dst_nR++;
      }
      iptrs[min_range]++;
      num_consumed[min_range]++;
    }
    for ( int j = 0; j < aux_nR; j++ ) { 
      if ( num_consumed[j] != num_to_consume[j] ) { go_BYE(-1); }
    }
    // printf("mat_dst_nR = %lld \n", mat_dst_nR);
    // printf("    dst_nR = %lld \n", dst_nR);
    if ( mat_dst_nR > dst_nR ) { go_BYE(-1); }
    if ( mat_dst_nR < dst_nR ) {
      dst_nR = mat_dst_nR;
      rs_munmap(dst_X, dst_nX);
      q_trunc(ddir_id, fileno, (dst_nR * fldsz));
    }
  }
  else { go_BYE(-1); }
  // TODO: P1 Delete nn file if not needed
  if ( dst_nR == 0 ) {
    fprintf(stderr, "Nothing to copy\n");
    go_BYE(-1);
  }

  sprintf(strbuf, "%lld", dst_nR);
  status = add_tbl(dst_tbl, strbuf, &dst_tbl_id, &dst_tbl_rec); 
  cBYE(status);
  zero_fld_rec(&dst_fld_rec); dst_fld_rec.fldtype = src_fld_rec.fldtype;
  status = add_fld(dst_tbl_id, dst_fld, ddir_id, fileno, 
		   &dst_fld_id, &dst_fld_rec); 
  cBYE(status);
  if ( nn_src_fld_id >= 0 ) { 
    zero_fld_rec(&nn_dst_fld_rec); nn_dst_fld_rec.fldtype = I1;
    status = add_aux_fld(NULL, dst_tbl_id, NULL, dst_fld_id, nn_ddir_id, 
			 nn_fileno, nn, &nn_dst_fld_id, &nn_dst_fld_rec);
    cBYE(status);
    strcpy(strbuf, "_nn_"); strcat(strbuf, dst_fld);
    status = f_to_s(dst_tbl, strbuf, "sum", strbuf, BUFLEN ); cBYE(status); 
    status = explode(strbuf, ':', &Y, &nY); cBYE(status);
    if ( nY != 2 ) { go_BYE(-1); }
    if ( strcmp(Y[0], Y[1]) == 0 ) { /* nn field not needed */
      status = drop_nn_fld(NULL, NULL, dst_fld_id); cBYE(status);
    }
  }
  //--------------------------------------------
  if ( ( order != NULL ) && ( strcmp(order, "sortA") == 0 ) ) { 
    status = f_to_s(dst_tbl, dst_fld, "is_sorted", strbuf, BUFLEN);
    cBYE(status);
    if ( strcmp(strbuf, "ascending") != 0 ) { 
      // This is the case when user requested sortA as order but the
      // number of ranges exceeded our cutoff
      status = fop(dst_tbl, dst_fld, "sortA"); cBYE(status);
    }
  }
  //--------------------------------------------
 BYE:
  for ( int i = 0; i < nY; i++ ) {
    free_if_non_null(Y[i]);
  }
  free_if_non_null(Y);
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(dst_X, dst_nX);
  rs_munmap(mat_X, mat_nX);
  rs_munmap(nn_dst_X, nn_dst_nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  return(status);
}
