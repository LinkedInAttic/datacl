#include <pthread.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "s_to_f.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "ext_s_to_f.h"
#include "par_s_to_f.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
ext_s_to_f(
       const char *tbl,
       const char *fld,
       const char *str_scalar
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  FILE  *ofp = NULL; char opfile[MAX_LEN_FILE_NAME+1];
#define MAX_LEN 32
  char str_fldtype[MAX_LEN];
  long long nR;
  int fldsz  = INT_MIN;
  TBL_REC_TYPE tbl_rec; int tbl_id; long long tbl_magic_val;
  FLD_REC_TYPE fld_rec; int fld_id; long long fld_magic_val;
  FLD_TYPE fldtype;
  bool is_null = true;
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd = false;
  //----------------------------------------------------------------
  zero_string(str_fldtype, MAX_LEN);
  zero_string(opfile, MAX_LEN_FILE_NAME);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  nR = tbl_rec.nR;
  tbl_magic_val = tbl_rec.magic_val;
  //--------------------------------------------------------
  status = extract_S(str_scalar, "fldtype=[", "]", str_fldtype, MAX_LEN, 
      &is_null);
  cBYE(status);
  if ( is_null ) { go_BYE(-1); }
  if ( ( str_fldtype == NULL ) || ( *str_fldtype == '\0' ) ) { go_BYE(-1); }
  status = unstr_fldtype(str_fldtype, &fldtype); cBYE(status);
  status = get_fld_sz(fldtype, &fldsz); cBYE(status); 
  if ( fldtype == xstring ) { go_BYE(-1); }
  //--------------------------------------------------------
  // Create storage and update meta data 
  long long filesz = nR * fldsz;
  status = mk_temp_file(opfile, filesz); cBYE(status);
  status = add_fld(tbl_id, tbl_magic_val, fld, opfile, fldtype, 
      &fld_id, &fld_rec);
  cBYE(status);
  fld_magic_val = fld_rec.magic_val;
  mcr_cd;
  status = rs_mmap(opfile, &X, &nX, 1); cBYE(status);
  mcr_uncd;
  //--------------------------------------------------------
  status = par_s_to_f(X, 0, nR, str_scalar);
  cBYE(status);
  rs_munmap(X, nX);
  
  status = mark_fld_done(tbl_id, tbl_magic_val, fld_id, fld_magic_val);
  cBYE(status);
  fprintf(stderr,"fld_id=%d\n", fld_id);
 BYE:
  rs_munmap(X, nX);
  fclose_if_non_null(ofp);
  return(status);
}
