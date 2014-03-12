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
#include "meta_globals.h"

extern bool g_write_to_temp_dir;

//---------------------------------------------------------------
// START FUNC DECL
int 
count_range(
	   char *src_tbl,
	   char *src_fld,
	   char *dst_tbl,
	   char *lb_fld, /* inclusive */
	   char *ub_fld, /* exclusive */
	   char *cnt_fld
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *lb_fld_X = NULL; size_t lb_fld_nX = 0;
  char *ub_fld_X = NULL; size_t ub_fld_nX = 0;
  char *cnt_fld_X = NULL; size_t cnt_fld_nX = 0;
  FLD_TYPE *src_fld_meta = NULL;
  FLD_TYPE *lb_fld_meta = NULL;
  FLD_TYPE *ub_fld_meta = NULL;
  long long src_nR; 
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN;
  int lb_fld_id = INT_MIN; 
  int ub_fld_id = INT_MIN;
  int cnt_fld_id = INT_MIN;
  long long dst_nR;
  char str_dst_nR[32];
  char str_meta_data[1024];
  char *opfile = NULL; FILE *ofp = 0;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_fld == NULL ) || ( *ub_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb_fld, ub_fld) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb_fld, cnt_fld) == 0 ) { go_BYE(-1); }
  if ( strcmp(ub_fld, cnt_fld) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  zero_string(str_dst_nR, 32);
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  src_fld_meta = &(g_fld[src_fld_id]);
  status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0); cBYE(status);
  // Cannot yet deal with following cases
  if ( strcmp(src_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( src_fld_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbl[dst_tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, dst_tbl_id, lb_fld, &lb_fld_id); cBYE(status);
  chk_range(lb_fld_id, 0, g_n_fld);
  lb_fld_meta = &(g_fld[lb_fld_id]);
  status = rs_mmap(lb_fld_meta->filename, &lb_fld_X, &lb_fld_nX, 0); cBYE(status);
  if ( strcmp(lb_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_fld(NULL, dst_tbl_id, ub_fld, &ub_fld_id); cBYE(status);
  chk_range(ub_fld_id, 0, g_n_fld);
  ub_fld_meta = &(g_fld[ub_fld_id]);
  status = rs_mmap(ub_fld_meta->filename, &ub_fld_X, &ub_fld_nX, 0); cBYE(status);
  if ( strcmp(ub_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  // Create output
  status = open_temp_file(&ofp, &opfile, dst_nR*sizeof(int)); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, dst_nR * sizeof(int)); cBYE(status);
  status = rs_mmap(opfile, &cnt_fld_X, &cnt_fld_nX, 1); cBYE(status);
  //------------------------------------------------------
  /* Sequential scan can be replaced with logarithmic operation when lb
   * and ub are sorted ascending as is src_fld. FOR LATER */
  // TODO I think this needs input to be sorted. Check it out
  int range_idx = 0;
  int *in = NULL, *lb = NULL, *ub = NULL, *out = NULL;
  int *lbmax = NULL, *ubmax = NULL;
  int inval, lbval, ubval;
  in = (int *)src_fld_X;
  lb = (int *)lb_fld_X;
  ub = (int *)ub_fld_X;
  out = (int *)cnt_fld_X;
  lbmax = lb + dst_nR;
  ubmax = ub + dst_nR;
  for ( long long i = 0; i < src_nR; i++ ) { 
    inval = *in; lbval = *lb; ubval = *ub;
    if ( ub <= lb ) { go_BYE(-1); }
    if ( inval >= ubval ) { /* ub is exclusive */
      do { 
        if ( lb == lbmax ) { go_BYE(-1); }
        if ( ub == ubmax ) { go_BYE(-1); }
        lbval = *lb++;
        ubval = *ub++;
        range_idx++;
      } while ( inval < lbval ) ; /* lb is inclusive */
    }
    out[range_idx]++;
    in++;
  }
  // Add count field to meta data 
  sprintf(str_meta_data, "fldtype=int:n_sizeof=4:filename=%s", opfile);
  status = add_fld(dst_tbl, cnt_fld, str_meta_data, &cnt_fld_id);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(lb_fld_X, lb_fld_nX);
  rs_munmap(ub_fld_X, ub_fld_nX);
  rs_munmap(cnt_fld_X, cnt_fld_nX);
  free_if_non_null(opfile);
  return(status);
}
