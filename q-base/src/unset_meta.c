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
int unset_min_max_sum(
		    const char *attr, 
		    int fld_id
		    )
// STOP FUNC DECL
{
  int status = 0;
  if ( strcmp(attr, "sum") == 0 ) { 
    g_flds[fld_id].sumval.sumI8 = g_flds[fld_id].sumval.sumF8 = 0;
    g_flds[fld_id].is_sum_nn = false;
  }
  else if ( strcmp(attr, "min") == 0 ) { 
    g_flds[fld_id].minval.minI8 = g_flds[fld_id].minval.minF8 = 0;
    g_flds[fld_id].is_min_nn = false;
  }
  else if ( strcmp(attr, "max") == 0 ) { 
    g_flds[fld_id].maxval.maxI8 = g_flds[fld_id].maxval.maxF8 = 0;
    g_flds[fld_id].is_max_nn = false;
  }
  else {
    go_BYE(-1);
  }
  if ( ( g_flds[fld_id].is_max_nn == false )  && 
       ( g_flds[fld_id].is_max_nn == false )  && 
       ( g_flds[fld_id].is_max_nn == false ) ) {
     g_flds[fld_id].num_nn = -1;
  }

 BYE:
  return status ;
}
// START FUNC DECL
int
int_unset_meta(
	     int tbl_id,
	     int fld_id,
	     const char *attr
	     )
// STOP FUNC DECL
{
  int status = 0;

  if ( ( attr == NULL ) || ( *attr == '\0' ) ) { go_BYE(-1); }
  if ( tbl_id < 0 ) { go_BYE(-1); }
  if ( strcmp(attr, "tbltype") != 0 ) {
    if ( fld_id < 0 ) { go_BYE(-1); }
  }
  //---------------------------------------------
  if ( strcmp(attr, "tbltype") == 0 ) {
    g_tbls[tbl_id].tbltype = undef_tbl; 
  }
  else if ( strcmp(attr, "dict_fld") == 0 ) { 
    status = del_from_fld_info_1(fld_id, fk_fld_txt_lkp); cBYE(status);
  }
  else if ( ( strcmp(attr, "sum") == 0 ) ||
            ( strcmp(attr, "min") == 0 ) ||
            ( strcmp(attr, "max") == 0 ) ) {
    status = unset_min_max_sum(attr, fld_id); cBYE(status);
  }
  else if ( strcmp(attr, "srttype") == 0 ) {
    g_flds[fld_id].srttype = unknown; 
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
unset_meta(
	 const char *tbl,
	 const char *fld,
	 const char *attr
	 )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id;
  FLD_REC_TYPE fld_rec; int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 

  if ( ( tbl  == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( attr == NULL ) || ( * tbl == '\0' ) ) { go_BYE(-1); }

  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status); 
  chk_range(tbl_id, 0, g_n_tbl); 
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { 
    fld_id = -1;
  }
  else { 
    status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, 
		    &nn_fld_id, &nn_fld_rec);
    cBYE(status); 
    chk_range(fld_id, 0, g_n_fld);
  }
  status = int_unset_meta(tbl_id, fld_id, attr); cBYE(status);
 BYE:
  return status ;
}
