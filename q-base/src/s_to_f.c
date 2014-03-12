#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "s_to_f.h"
#include "fld_meta.h"
#include "set_meta.h"
#include "extract_I8.h"
#include "vec_s_to_f.h"
#include "meta_globals.h"

// last review 9/4/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
s_to_f(
       const char *tbl,
       const char *fld,
       const char *str_scalar
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  int fileno = -1, ddir_id = -1;
#define MAX_LEN 32
  char str_fldtype[MAX_LEN];
  char op[MAX_LEN];
  long long nR;
  int fldsz  = INT_MIN;
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_TYPE fldtype;
  bool is_null = true;
  //----------------------------------------------------------------
  zero_string(str_fldtype, MAX_LEN);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  zero_string(op, MAX_LEN);
  status = extract_S(str_scalar, "op=[", "]", op, MAX_LEN, &is_null); 
  cBYE(status);
  if ( is_null ) { go_BYE(-1); }
  if ( ( strcmp(op, "const") == 0 ) || 
       ( strcmp(op, "seq") == 0 ) ) {
    /* all is well */
  }
  else {
    go_BYE(-1);
  }

  status = extract_S(str_scalar, "fldtype=[", "]", str_fldtype, MAX_LEN, &is_null); 
  if ( is_null ) { go_BYE(-1); }
  if ( ( str_fldtype == NULL ) || ( *str_fldtype == '\0' ) ) { go_BYE(-1); }
  status = unstr_fldtype(str_fldtype, &fldtype); cBYE(status);
  //--------------------------------------------------------
  // Create storage 
  status = get_fld_sz(fldtype, &fldsz); cBYE(status); 
  size_t filesz = nR * fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &X, &nX, 1); cBYE(status);
  //--------------------------------------------------------
  status = vec_s_to_f(X, nR, str_scalar); cBYE(status);
  rs_munmap(X, nX);
  // Update meta data 
  zero_fld_rec(&fld_rec); fld_rec.fldtype = fldtype;
  status = add_fld(tbl_id, fld, ddir_id, fileno, &fld_id, &fld_rec);
  cBYE(status);
  if ( strcmp(op, "seq") == 0 ) {
    long long incrI8; bool is_null;
    status = extract_I8(str_scalar, "incr=[", "]", &incrI8, &is_null); 
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( incrI8 < 0 ) { 
      status = int_set_meta(tbl_id, fld_id, "srttype", "descending", true);
      cBYE(status);
    }
    else if ( incrI8 > 0 ) { 
      status = int_set_meta(tbl_id, fld_id, "srttype", "ascending", true);
      cBYE(status);
    }
  }

 BYE:
  rs_munmap(X, nX);
  return(status);
}
