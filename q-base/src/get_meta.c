#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mmap.h"
#include "aux_meta.h"
#include "get_meta.h"
#include "dbauxil.h"
#include "meta_globals.h"

// START FUNC DECL
int
int_get_meta(
	     int tbl_id,
	     int fld_id,
	     char *attr,
	     char *value
	     )
// STOP FUNC DECL
{
  int status = 0;

  chk_range(tbl_id, 0, g_n_tbl);
  chk_range(fld_id, 0, g_n_fld);
  if ( g_flds[fld_id].tbl_id != tbl_id ) { go_BYE(-1); }
  if ( attr == NULL ) { go_BYE(-1); }
  if ( value == NULL ) { go_BYE(-1); }

  if ( strcmp(attr, "srttype") == 0 ) {
    status = mk_str_srttype(g_flds[fld_id].srttype, value); cBYE(status);
  }
  else if ( strcmp(attr, "is_external") == 0 ) {
    sprintf(value, "%d", g_flds[fld_id].is_external);
  }
  else if ( strcmp(attr, "dict_fld_id") == 0 ) {
    int dict_fld_id;
    status = get_fld_info(fk_fld_txt_lkp, fld_id, &dict_fld_id); cBYE(status);
    sprintf(value, "%d", dict_fld_id);
  }
  else if ( strcmp(attr, "len_fld_id") == 0 ) {
    int len_fld_id; 
    status = get_fld_info(fk_fld_len, fld_id, &len_fld_id); cBYE(status);
    sprintf(value, "%d", len_fld_id);
  }
  else if ( strcmp(attr, "off_fld_id") == 0 ) {
    int off_fld_id; 
    status = get_fld_info(fk_fld_off, fld_id, &off_fld_id); cBYE(status);
    sprintf(value, "%d", off_fld_id);
  }
  else { go_BYE(-1); }
BYE:
  return status ;
}
// START FUNC DECL
int
get_meta(
	 char *tbl,
	 char *fld,
	 char *attr,
	 char *rslt_buf,
	 int sz_rslt_buf // TODO P3: Use this to check for buffer overflow
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
  status = int_get_meta(tbl_id, fld_id, attr, rslt_buf); cBYE(status);
 BYE:
  return status ;
}
