#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "mk_temp_file.h"
#include "get_nR.h"
#include "f1opf2_cum.h"
#include "f1opf2_idx_with_reset.h"
#include "f1opf2_shift.h"
#include "vec_f1opf2.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "extract_S.h"
#include "extract_I4.h"
#include "smear.h"
#include "get_type_op_fld.h"
#include "assign_I1.h"
#include "meta_globals.h"

// last review 9/5/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
f1opf2(
       char *tbl,
       char *f1,
       char *str_op_spec,
       char *f2
       )
// STOP FUNC DECL
{
  int status = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE f2_rec, nn_f2_rec;
  long long nR; 
  int tbl_id = INT_MIN, f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int                   f2_id = INT_MIN, nn_f2_id = INT_MIN;
  FLD_TYPE f1type = undef_fldtype, f2type = undef_fldtype;
#define BUFLEN 32
  char op[BUFLEN];
  int fldsz = INT_MAX; size_t filesz = 0, nn_filesz = 0;
  int ddir_id = -1,   fileno = -1; 
  int nn_ddir_id = -1, nn_fileno = -1;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = chk_aux_info(str_op_spec); cBYE(status);
  bool is_null;
  status = extract_S(str_op_spec, "op=[", "]", op, BUFLEN, &is_null);
  cBYE(status);
  if ( is_null ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbls[tbl_id].nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1type = f1_rec.fldtype;
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  // Get nn field for f1 if if it exists
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); cBYE(status);
  }
  //---------------------------------------------
  /* Get destination field type */
  status = get_type_op_fld("f1opf2", op, f1type, f1type, str_op_spec, &f2type);
  cBYE(status);
  if ( f2type == undef_fldtype ) { go_BYE(-1); }
  //------------------------------------------
  if ( f2type == B ) { 
    status = get_file_size_B(nR, &filesz); cBYE(status);
    // Since B cannot have a nn field, if output field is B, then input
    // field cannot have an nn field.
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  }
  else {
    status = get_fld_sz(f2type, &fldsz); cBYE(status);
    filesz    = nR * fldsz;
  }
  status = get_fld_sz(I1, &fldsz); cBYE(status);
  nn_filesz = nR * fldsz;

  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &f2_X, &f2_nX, 1); cBYE(status);
  //--------------------------------------------
  // If f1 has a nn field, then f2 has a nn field. 
  // If f1 does not have a nn field, then f2 does not have a nn field. 
  // TODO: P3. Above assumption should be refined.
  if ( ( nn_f1_X != NULL ) && ( f2type != B ) ) { 
    status = mk_temp_file(nn_filesz, &nn_ddir_id, &nn_fileno); 
    cBYE(status);
    status = q_mmap(nn_ddir_id, nn_fileno, &nn_f2_X, &nn_f2_nX, 1); 
    cBYE(status);
  }
  /* There is a chance that the files created (ddir_id, fileno) and
   * (nn_ddir_id, nn_fileno) will not be used. If we use them, we set
   * them to -1. At end, if they are still non-negative, they are
   * deleted
   * */
  //--------------------------------------------
  if ( ( strcmp(op, "conv") == 0 ) || 
       ( strcmp(op, "bitcount" ) == 0 ) || 
       ( strcmp(op, "sqrt" ) == 0 ) || 
       ( strcmp(op, "abs" ) == 0 ) || 
       ( strcmp(op, "normal_cdf_inverse" ) == 0 ) || 
       ( strcmp(op, "reciprocal" ) == 0 ) || 
       ( strcmp(op, "!" ) == 0 ) || 
       ( strcmp(op, "++" ) == 0 ) || 
       ( strcmp(op, "--" ) == 0 ) || 
       ( strcmp(op, "~" ) == 0 )
       ) 
  {
    status = vec_f1opf2(nR, f1type, f1_X, nn_f1_X, op, 
			f2_X, nn_f2_X, f2type); 
    cBYE(status);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = f2type;
    status = add_fld(tbl_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
    cBYE(status);
    ddir_id = fileno = -1; // TO INDICATE USAGE
    if ( nn_fileno > 0 ) { 
      zero_fld_rec(&nn_f2_rec); nn_f2_rec.fldtype = I1; 
      status = add_aux_fld(NULL, tbl_id, NULL, f2_id, nn_ddir_id, 
			   nn_fileno, nn, &nn_f2_id, &nn_f2_rec);
      cBYE(status);
      nn_ddir_id = nn_fileno = -1; // TO INDICATE USAGE
    }
  }
  else if ( strcmp(op, "cum") == 0 ) {
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    // No easy way to parallelize this. Should not be part of a compound
    // operation
    switch ( f1type ) { 
      case I1 : case I2 : case I4 : case I8 : break;
      default : go_BYE(-1); break;
    }
    status = f1opf2_cum(f1_X, nR, f1type, f2type, f2_X); cBYE(status);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = f2type; 
    status = add_fld(tbl_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
    cBYE(status);
    ddir_id = fileno = -1; // TO INDICATE USAGE
  }
  else if ( strcmp(op, "idx_with_reset") == 0 ) {
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    if ( f1type != I1 ) { go_BYE(-1); }
    // No easy way to parallelize this. Should not be part of compound op
    status = f1opf2_idx_with_reset(f1_X, nR, (long long *)f2_X); 
    cBYE(status);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = f2type; 
    status = add_fld(tbl_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
    cBYE(status);
    ddir_id = fileno = -1; // TO INDICATE USAGE
  }
  else if ( strcmp(op, "smear") == 0 ) {
    if ( f1type != I1 ) { go_BYE(-1); }
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    int plus = -1, minus = -1; 
    status = extract_I4(str_op_spec, "plus=[", "]", &plus, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( plus < 0 ) { go_BYE(-1); }
    status = extract_I4(str_op_spec, "minus=[", "]", &minus, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( minus < 0 ) { go_BYE(-1); }
    // TODO: P1. Parallelize this. Also deal with complexity of compound
    // operations and the fact that first or last dealt with
    // differently.
    if ( ( plus > MAX_SMEAR ) || ( minus > MAX_SMEAR ) ) {
      go_BYE(-1);
    }

    status = smear(f1_X, nR, plus, minus, f2_X); cBYE(status);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = f1_rec.fldtype;
    status = add_fld(tbl_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
    cBYE(status);
    ddir_id = fileno = -1; // TO INDICATE USAGE
  }
  else if ( strcmp(op, "shift") == 0 ) {
    int shift_by = 0;
    char str_newval[BUFLEN];
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    status = extract_I4(str_op_spec, "shift=[", "]", &shift_by, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( shift_by == 0 ) { go_BYE(-1); }

    status = extract_S(str_op_spec, "newval=[", "]", str_newval, BUFLEN, 
		       &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }

    // TODO: P1. Parallelize this. Also deal with complexity of compound
    // operations and the fact that first or last dealt with
    // differently.
    status = f1opf2_shift(f1_X, nR, f1_rec.fldtype, shift_by, str_newval, f2_X);
    cBYE(status);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = f1_rec.fldtype; 
    status = add_fld(tbl_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
    cBYE(status);
    ddir_id = fileno = -1; // TO INDICATE USAGE
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
 BYE:
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  if ( ( ddir_id >= 0 ) && ( fileno >= 0 ) ) {
    q_delete(ddir_id, fileno);
  }
  if ( ( nn_ddir_id >= 0 ) && ( nn_fileno >= 0 ) ) {
    q_delete(nn_ddir_id, nn_fileno);
  }
  return status ;
}
