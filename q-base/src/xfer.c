#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "vec_f_to_s.h"
#include "xfer.h"
#include "add_fld.h"
#include "get_meta.h"
#include "set_meta.h"
#include "add_aux_fld.h"
#include "meta_globals.h"

#include "./AUTOGEN/GENFILES/xfer_I1_I4.h"
#include "./AUTOGEN/GENFILES/xfer_I2_I4.h"
#include "./AUTOGEN/GENFILES/xfer_I4_I4.h"
#include "./AUTOGEN/GENFILES/xfer_I8_I4.h"
#include "./AUTOGEN/GENFILES/xfer_F4_I4.h"
#include "./AUTOGEN/GENFILES/xfer_F8_I4.h"
#include "xfer_SC_I4.h"

// last review 9/5/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
xfer(
     char *src_tbl,
     char *src_fld,
     char *dst_tbl,
     char *dst_idx, // this is an index into the source table 
     char *dst_fld
     )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE src_fld_rec, dst_idx_meta, dst_fld_rec; 
  FLD_REC_TYPE nn_src_fld_rec, nn_dst_idx_meta, nn_dst_fld_rec; 

  char strbuf[32]; int dict_tbl_id;

  long long src_nR, dst_nR;

  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *dst_fld_X = NULL; size_t dst_fld_nX = 0;
  char *dst_idx_X = NULL; size_t dst_idx_nX = 0;

  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *nn_dst_idx_X = NULL; size_t nn_dst_idx_nX = 0;
  char *nn_dst_fld_X = NULL; size_t nn_dst_fld_nX = 0;

  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, nn_src_fld_id = INT_MIN;
  int dst_idx_id = INT_MIN, nn_dst_idx_id = INT_MIN;
  int dst_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) )  { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_idx == NULL ) || ( *dst_idx == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) )  { go_BYE(-1); }
  if ( strcmp(dst_idx, dst_fld) == 0 ) { go_BYE(-1); }
  //----------------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
      &nn_src_fld_id, &nn_src_fld_rec);
  cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);

  status = get_data(src_fld_rec, &src_fld_X, &src_fld_nX, false); cBYE(status);

  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbls[dst_tbl_id].nR;

  status = is_fld(NULL, dst_tbl_id, dst_idx, &dst_idx_id, &dst_idx_meta, 
      &nn_dst_idx_id, &nn_dst_idx_meta);
  cBYE(status);
  chk_range(dst_idx_id, 0, g_n_fld);
  status = get_data(dst_idx_meta, &dst_idx_X, &dst_idx_nX, false); cBYE(status);

  if ( nn_src_fld_id >= 0 ) { 
    status = get_data(nn_src_fld_rec, &nn_src_fld_X, &nn_src_fld_nX, false); 
    cBYE(status);
  }
  if ( nn_dst_idx_id >= 0 ) { 
    status = get_data(nn_dst_idx_meta, &nn_dst_idx_X, &nn_dst_idx_nX, false); 
    cBYE(status);
  }
  //----------------------------------------------------------------
  // allocate space for output
  int fldsz = 0; size_t filesz = 0;
  int ddir_id    = -1, fileno    = -1; 
  int nn_ddir_id = -1, nn_fileno = -1; 

  if ( src_fld_rec.fldtype == SC ) {
    int len = src_fld_rec.len; if ( len <= 0 ) { go_BYE(-1); }
    filesz = (len+1) * dst_nR;
  }
  else if ( ( src_fld_rec.fldtype == SV ) ||( src_fld_rec.fldtype == B ) ) {
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
  }
  else {
    status = get_fld_sz(src_fld_rec.fldtype, &fldsz); cBYE(status);
    filesz = fldsz * dst_nR;
  }
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &dst_fld_X, &dst_fld_nX, true); cBYE(status);

  if ( nn_src_fld_id < 0 ) { 
    // No nulls in source means no nulls possible in destination
  }
  else {
    // may not need the nn field but will discover this only later
    status = get_fld_sz(I1, &fldsz); cBYE(status);
    filesz = fldsz * dst_nR;
    status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno); cBYE(status);
    status = q_mmap(nn_ddir_id, nn_fileno, &nn_dst_fld_X, &nn_dst_fld_nX,true); 
    cBYE(status);
  }
  
  //----------------------------------------------------------------
  switch ( src_fld_rec.fldtype ) { 
#include "incl_xfer_I1.c"
#include "incl_xfer_I2.c"
#include "incl_xfer_I4.c"
#include "incl_xfer_I8.c"
#include "incl_xfer_F4.c"
#include "incl_xfer_F8.c"
    case SC : 
      switch ( dst_idx_meta.fldtype ) { 
	case I4 : 
	  xfer_SC_I4(src_fld_X, src_fld_rec.len, src_nR, dst_idx_X, nn_dst_idx_X, 
	      dst_nR, dst_fld_X);
	  break;
	default : go_BYE(-1); break;
      }
      break;
    default : 
      go_BYE(-1);
      break;
  }
#define BUFLEN 32
  char buf[BUFLEN], buf2[BUFLEN]; long long nn_dst_nR;
  zero_fld_rec(&dst_fld_rec); dst_fld_rec.fldtype = src_fld_rec.fldtype;
  status = add_fld(dst_tbl_id, dst_fld, ddir_id, fileno, 
      &dst_fld_id, &dst_fld_rec);
  cBYE(status);
  // Find out whether an nn field is really needed
  status = vec_f_to_s(nn_dst_fld_X, I1, NULL, dst_nR, "sum", buf, BUFLEN);
  cBYE(status);
  status = read_nth_val(buf, ":", 0, buf2, BUFLEN); cBYE(status);
  status = stoI8(buf2, &nn_dst_nR); cBYE(status);
  if ( dst_nR == nn_dst_nR ) { 
    // nn field not needed
    status = q_delete(nn_ddir_id, nn_fileno); cBYE(status);
  }
  else {
    zero_fld_rec(&nn_dst_fld_rec); nn_dst_fld_rec.fldtype = I1; 
    status = add_aux_fld(NULL, dst_tbl_id, NULL, dst_fld_id, nn_ddir_id, 
	nn_fileno, nn, &nn_dst_fld_id, &nn_dst_fld_rec);
  }
  status = int_get_meta(src_tbl_id, src_fld_id, "dict_tbl_id", strbuf);
  cBYE(status);
  status = stoI4(strbuf, &dict_tbl_id); cBYE(status);
  if ( dict_tbl_id >= 0 ) {
    status = int_set_meta(dst_tbl_id, dst_fld_id, "dict_tbl_id", strbuf, true);
    cBYE(status);
  }
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(dst_idx_X, dst_idx_nX);
  rs_munmap(dst_fld_X, dst_fld_nX);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  rs_munmap(nn_dst_idx_X, nn_dst_idx_nX);
  rs_munmap(nn_dst_fld_X, nn_dst_fld_nX);
  return status ;
}
