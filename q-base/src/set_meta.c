#include <limits.h>
#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "auxil.h"
#include "aux_meta.h"
#include "set_meta.h"
#include "dbauxil.h"
#include "meta_globals.h"

// START FUNC DECL
int set_min_max_sum(
		    const char *attr, 
		    const char *value, 
		    int fld_id
		    )
// STOP FUNC DECL
{
  int status = 0;
  double valF8; long long valI8; 
  char strbuf[32]; zero_string(strbuf, 32);
  status = read_nth_val(value, ":", 1, strbuf, 32); cBYE(status);
  status = stoI8(strbuf, &valI8); cBYE(status);
  if ( valI8 < 0 ) { go_BYE(-1); }
  g_flds[fld_id].num_nn = valI8;


  status = read_nth_val(value, ":", 0, strbuf, 32); cBYE(status);
  switch ( g_flds[fld_id].fldtype ) {
  case B : case I1 : case I2 : case I4 : case I8 :
    status = stoI8(strbuf, &valI8); cBYE(status);
    if ( strcmp(attr, "sum") == 0 ) { 
      g_flds[fld_id].sumval.sumI8 = valI8;
      g_flds[fld_id].is_sum_nn = true;
    }
    else if ( strcmp(attr, "min") == 0 ) { 
      g_flds[fld_id].minval.minI8 = valI8;
      g_flds[fld_id].is_min_nn = true;
    }
    else if ( strcmp(attr, "max") == 0 ) { 
      g_flds[fld_id].maxval.maxI8 = valI8;
      g_flds[fld_id].is_max_nn = true;
    }
    else {
      go_BYE(-1);
    }
    break;
  case F4 : case F8 : 
    status = stoF8(strbuf, &valF8); cBYE(status);
    if ( strcmp(attr, "sum") == 0 ) { 
      g_flds[fld_id].sumval.sumF8 = valF8;
      g_flds[fld_id].is_sum_nn = true;
    }
    else if ( strcmp(attr, "min") == 0 ) { 
      g_flds[fld_id].minval.minF8 = valF8;
      g_flds[fld_id].is_min_nn = true;
    }
    else if ( strcmp(attr, "max") == 0 ) { 
      g_flds[fld_id].maxval.maxF8 = valF8;
      g_flds[fld_id].is_max_nn = true;
    }
    else {
      go_BYE(-1);
    }
    break;
  default : 
    go_BYE(-1);
    break; 
  }
  if ( ( g_flds[fld_id].is_min_nn == true ) &&
       ( g_flds[fld_id].is_max_nn == true ) ) {
    switch ( g_flds[fld_id].fldtype ) {
    case I1 : case I2 : case I4 : case I8 :
      if ( ( g_flds[fld_id].minval.minI8 > g_flds[fld_id].maxval.maxI8 ) ) {
	go_BYE(-1);
      }
      break;
    case F4 : case F8 : 
      if ( ( g_flds[fld_id].minval.minF8 > g_flds[fld_id].maxval.maxF8 ) ) {
	go_BYE(-1);
      }
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  if ( g_flds[fld_id].is_min_nn == true ) {
    switch ( g_flds[fld_id].fldtype ) {
    case I1 : 
      if ( ( g_flds[fld_id].minval.minI8 < SCHAR_MIN )  ||
           ( g_flds[fld_id].minval.minI8 > SCHAR_MAX ) ) {
	go_BYE(-1);
      }
      break;
    case I2 : 
      if ( ( g_flds[fld_id].minval.minI8 < SHRT_MIN )  ||
           ( g_flds[fld_id].minval.minI8 > SHRT_MAX ) ) {
	go_BYE(-1);
      }
    case I4 : 
      if ( ( g_flds[fld_id].minval.minI8 < INT_MIN )  ||
           ( g_flds[fld_id].minval.minI8 > INT_MAX ) ) {
	go_BYE(-1);
      }
    default : 
      break;
    }
  }
  if ( g_flds[fld_id].is_max_nn == true ) {
    switch ( g_flds[fld_id].fldtype ) {
    case I1 : 
      if ( ( g_flds[fld_id].maxval.maxI8 < SCHAR_MIN )  ||
           ( g_flds[fld_id].maxval.maxI8 > SCHAR_MAX ) ) {
	go_BYE(-1);
      }
      break;
    case I2 : 
      if ( ( g_flds[fld_id].maxval.maxI8 < SHRT_MIN )  ||
           ( g_flds[fld_id].maxval.maxI8 > SHRT_MAX ) ) {
	go_BYE(-1);
      }
    case I4 : 
      if ( ( g_flds[fld_id].maxval.maxI8 < INT_MIN )  ||
           ( g_flds[fld_id].maxval.maxI8 > INT_MAX ) ) {
	go_BYE(-1);
      }
    default : 
      break;
    }
  }

 BYE:
  return status ;
}
// START FUNC DECL
int
int_set_meta(
	     int tbl_id,
	     int fld_id,
	     const char *attr,
	     const char *value,
	     bool is_internal
	     )
// STOP FUNC DECL
{
  int status = 0;
  FLD_TYPE fldtype;

  if ( ( attr  == NULL ) || ( *attr  == '\0' ) ) { go_BYE(-1); }
  if ( ( value == NULL ) || ( *value == '\0' ) ) { go_BYE(-1); }
  if ( tbl_id < 0 ) { go_BYE(-1); }
  if ( strcmp(attr, "tbltype") != 0 ) {
    if ( fld_id < 0 ) { go_BYE(-1); }
  }
  //-------------------------------------------------
  if ( strcmp(attr, "tbltype") == 0 ) {
    TBL_TYPE tbltype;
    status = unstr_tbltype(value, &tbltype); cBYE(status);
    int txt_fld_id, nn_txt_fld_id;
    FLD_REC_TYPE txt_fld_rec, nn_txt_fld_rec; 
    switch ( tbltype ) { 
      case undef_tbl : 
	go_BYE(-1); 
	break;
      case txt_lkp : 
	status = is_fld(NULL, tbl_id, "txt", &txt_fld_id, &txt_fld_rec, 
	    &nn_txt_fld_id, &nn_txt_fld_rec); 
	if ( txt_fld_id < 0 ) { go_BYE(-1); }
	if ( txt_fld_rec.fldtype != SV ) { go_BYE(-1); }
	break;
      default : 
	go_BYE(-1); 
	break;
    }
    g_tbls[tbl_id].tbltype = tbltype; 
  }
  else if ( strcmp(attr, "dict_fld") == 0 ) { 
#define BUFLEN 128
    int dict_tbl_id = -1;
    int dict_fld_id, nn_fld_id;
    FLD_REC_TYPE dict_fld_rec, nn_fld_rec; 
    TBL_REC_TYPE dict_tbl_rec;
    char strtbl[BUFLEN], strfld[BUFLEN];
    /* value is tbl:fld where both tbl and fld can be specified as
     * strings or integers */
    status = read_nth_val(value, ":", 0, strtbl, BUFLEN);
    if ( *strtbl == '\0' ) { go_BYE(-1); }
    status = read_nth_val(value, ":", 1, strfld, BUFLEN);
    if ( *strfld == '\0' ) { go_BYE(-1); }

    // get dict_tbl_id
    if ( alldigits(strtbl) ) {
      status = stoI4(strtbl, &dict_tbl_id);
    }
    else {
      status = is_tbl(strtbl, &dict_tbl_id, &dict_tbl_rec);
    }
    chk_range(dict_tbl_id, 0, g_n_tbl);
    // TODO P2 THINK IF THIS IS NECESSARY
    // if ( g_tbls[dict_tbl_id].tbltype != txt_lkp ) { go_BYE(-1); }
    // get dict_fld_id
    if ( alldigits(strfld) ) {
      status = stoI4(strfld, &dict_fld_id);
      chk_range(dict_fld_id, 0, g_n_fld);
      dict_fld_rec = g_flds[dict_fld_id];
    }
    else {
      status = is_fld(NULL, dict_tbl_id, strfld, &dict_fld_id, &dict_fld_rec, 
	    &nn_fld_id, &nn_fld_rec); 
      if ( dict_fld_id < 0 ) { go_BYE(-1); }
    }
    if ( ( dict_fld_rec.fldtype != SV ) && ( dict_fld_rec.fldtype != SC ) ) { 
      go_BYE(-1); 
    }
    status = add_to_fld_info(fld_id, fk_fld_txt_lkp, dict_fld_id);
    cBYE(status);
  }
  else if ( ( strcmp(attr, "sum") == 0 ) ||
            ( strcmp(attr, "min") == 0 ) ||
            ( strcmp(attr, "max") == 0 ) ) {
    status = set_min_max_sum(attr, value, fld_id); cBYE(status);
  }
  /* This is a dangerous operation. Use only for "casting" from I4 to
   * F4 and back or from I8 to F8 and back */
  else if ( strcmp(attr, "fldtype") == 0 ) {
    status = unstr_fldtype(value, &fldtype); cBYE(status);
    if ( is_internal == false ) { 
    switch ( g_flds[fld_id].fldtype ) { 
      case I4 :  if ( fldtype != F4 ) { go_BYE(-1); break; }
      case F4 :  if ( fldtype != I4 ) { go_BYE(-1); break; }
      case I8 :  if ( fldtype != F8 ) { go_BYE(-1); break; }
      case F8 :  if ( fldtype != I8 ) { go_BYE(-1); break; }
      default : go_BYE(-1); break;
    }
    }
    g_flds[fld_id].fldtype = fldtype;
  }
  else if ( strcmp(attr, "srttype") == 0 ) {
    if ( strcmp(value, "unknown") == 0 ) { 
      g_flds[fld_id].srttype = unknown; 
    }
    else if ( strcmp(value, "ascending") == 0 ) { 
      if ( is_internal == false ) { go_BYE(-1); }
      g_flds[fld_id].srttype = ascending; 
    }
    else if ( strcmp(value, "descending") == 0 ) { 
      if ( is_internal == false ) { go_BYE(-1); }
      g_flds[fld_id].srttype = descending; 
    }
    else if ( strcmp(value, "unsorted") == 0 ) { 
      if ( is_internal == false ) { go_BYE(-1); }
      g_flds[fld_id].srttype = unsorted; 
    }
    else { 
      go_BYE(-1);
    }
  }
  else if ( strcmp(attr, "external") == 0 ) {
    if ( is_internal == false ) { go_BYE(-1); }
    if ( strcmp(value, "true") == 0 ) { 
      g_flds[fld_id].is_external = true;
    }
    else if ( strcmp(value, "false") == 0 ) { 
      g_flds[fld_id].is_external = false;
    }
    else { go_BYE(-1); }
  }
  else {
    fprintf(stderr, "NOT IMPLEMENTED. attr = [%s] \n", attr);
    go_BYE(-1);
  }
  status = chk_tbl_meta(tbl_id); cBYE(status);
  long long nR = g_tbls[tbl_id].nR;
  if ( fld_id >= 0 ) { 
    status = chk_fld_meta(fld_id, nR); cBYE(status);
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
set_meta(
	 const char *tbl,
	 const char *fld,
	 const char *attr,
	 const char *value
	 )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id;
  FLD_REC_TYPE fld_rec; int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 

  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status); 
  chk_range(tbl_id, 0, g_n_tbl); 
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { 
    fld_id = INT_MIN;
  }
  else { 
    status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, 
		    &nn_fld_id, &nn_fld_rec);
    cBYE(status); 
    chk_range(fld_id, 0, g_n_fld);
  }
  bool is_internal = false;
  status = int_set_meta(tbl_id, fld_id, attr, value, is_internal); cBYE(status);
 BYE:
  return status ;
}
