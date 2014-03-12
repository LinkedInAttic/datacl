#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "mk_temp_file.h"
#include "get_nR.h"
#include "regex_match.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
// last review 9/22/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
regex_match(
	    char *tbl,
	    char *f1,
	    char *regex,
	    char *matchtype,
	    char *f2
	    )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *len_X = NULL; size_t len_nX = 0;
  char *off_X = NULL; size_t off_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec, f2_rec;
  long long nR; 
  int tbl_id = INT_MIN; 
  int f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int len_fld_id = INT_MIN ; short     *lenI2 = NULL;
  int off_fld_id = INT_MIN;  long long *offI8 = NULL;
  int f2_id = INT_MIN;
  int fldsz, ddir_id = -1, fileno = -1;
  short f1len = -1;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( regex == NULL ) || ( *regex == '\0' ) ) { go_BYE(-1); }
  if ( ( matchtype == NULL ) || ( *matchtype == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;

  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  if ( ( f1_rec.fldtype != SC ) && ( f1_rec.fldtype != SV ) ) { go_BYE(-1); }
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  //---------------------------------------------
  // make space for output 
  status = get_fld_sz(I1, &fldsz); cBYE(status);
  size_t filesz = nR * fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &f2_X, &f2_nX, true); cBYE(status);
  //---------------------------------------------
  switch ( f1_rec.fldtype ) { 
  case SC : 
    f1len = f1_rec.len;
    if ( f1len < 1 ) { go_BYE(-1); }
    break;
  case SV : 
    status = get_fld_info(fk_fld_len, f1_id, &len_fld_id); cBYE(status);
    if ( len_fld_id < 0 ) { go_BYE(-1); }
    FLD_REC_TYPE len_fld_rec = g_flds[len_fld_id]; 
    if ( len_fld_rec.fldtype != I2 )  { go_BYE(-1); }
    if ( len_fld_rec.nn_fld_id >= 0 ) { go_BYE(-1); }
    status = get_data(len_fld_rec, &len_X, &len_nX, false); cBYE(status);
    lenI2 = (short *)len_X;

    status = get_fld_info(fk_fld_off, f1_id, &off_fld_id); cBYE(status);
    if ( off_fld_id < 0 ) { go_BYE(-1); }
    FLD_REC_TYPE off_fld_rec = g_flds[off_fld_id]; 
    if ( off_fld_rec.fldtype != I8 )  { go_BYE(-1); }
    if ( off_fld_rec.nn_fld_id >= 0 ) { go_BYE(-1); }
    status = get_data(off_fld_rec, &off_X, &off_nX, false); cBYE(status);
    offI8 = (long long *)off_X;


    break;
  default : 
    go_BYE(-1); 
    break;
  }
  // TODO: Implement for SC and SV both P1
  if ( strcmp(matchtype, "exact") == 0 ) {
    char *l_f1_X = f1_X;
    for ( long long i = 0; i < nR; i++ ) { 
      if ( strcmp(l_f1_X, regex) == 0 ) {
	f2_X[i] = 1;
      }
      else {
	f2_X[i] = 0;
      }
      switch ( f1_rec.fldtype ) { 
      case SC : 
	l_f1_X += f1len;
	l_f1_X++; // to jump over nullc
	break;
      case SV : 
	l_f1_X += lenI2[i];
	l_f1_X++; // to jump over nullc
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
  }
  else {
    go_BYE(-1); // TO BE IMPLEMENTED
  }
  //---------------------------------------
  rs_munmap(f2_X, f2_nX);
  zero_fld_rec(&f2_rec); f2_rec.fldtype = I1;
  status = add_fld(tbl_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
  cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(len_X, len_nX);
  rs_munmap(off_X, off_nX);
  rs_munmap(f2_X, f2_nX);
  return(status);
}
