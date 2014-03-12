/* Yoni List Creator */
#include <sqlite3.h>
#include "constants.h"
#include "macros.h"
#include "md5global.h"
#include "mddriver.h"
#include "qtypes.h"
#include "auxil.h"
#include "ylc_auxil.h"
#include "ylc_dbauxil.h"
#include "ylc_globals.h"
#include "mmap.h"
#include "f1f2opf3.h"
#include "fld_meta.h"
#include "ext_f_to_s.h"
#include "is_fld.h"
#include "dup_fld.h"
#include "tbl_meta.h"
#include "del_fld.h"
int ylc_exclude(
		char *list,
		char *xfld /* field to exclude */
		)
{
  int status = 0;
  int rc; char *zErrMsg = NULL; bool b_is_list;
  char command[1024];
  char *cum_xcl_fld = NULL;
  int xfld_id, cum_xcl_fld_id, nn_fld_id;
  FLD_REC_TYPE xfld_rec, cum_xcl_fld_rec, nn_fld_rec;

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  if ( ( xfld == NULL ) || ( *xfld == '\0' ) ) { go_BYE(-1); }
  if ( strlen(list) >= MAX_LEN_LIST_NAME ) { go_BYE(-1); }

  status = is_list(list, &b_is_list); cBYE(status);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); return(status);
  }
  int list_id, tbl_id, xlist_id; TBL_REC_TYPE tbl_rec;
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( tbl_id  <  0 ) { go_BYE(-1); }
  if ( list_id <= 0 ) { go_BYE(-1); }
  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);
  char *tbl = tbl_rec.name;


  status = is_fld(NULL, tbl_id, xfld, &xfld_id, &xfld_rec, 
		    &nn_fld_id, &nn_fld_rec);  cBYE(status);
  if ( xfld_id < 0 ) { go_BYE(-1); }
  if ( xfld_rec.fldtype != B ) { go_BYE(-1); }

  status = del_all_lmt_flds(list); cBYE(status); 
  if ( xlist_id <= 0 ) { /* This is the first exclusion field for list */
    // Get field ID 
    char cum_xcl_fld[MAX_LEN_FLD_NAME+1];
    zero_string(cum_xcl_fld, (MAX_LEN_FLD_NAME+1));
    // Make a copy of this field 
    status = mk_name_new_fld(list, tbl, xfld, "cumx_", cum_xcl_fld); cBYE(status);
    status = dup_fld(tbl, xfld, cum_xcl_fld); cBYE(status);
    status = is_fld(NULL, tbl_id, cum_xcl_fld, &cum_xcl_fld_id, &cum_xcl_fld_rec, 
		    &nn_fld_id, &nn_fld_rec);  cBYE(status);
    // Insert the copy of field into xlist
    status = add_to_xlist(list, list_id, cum_xcl_fld, cum_xcl_fld_id); cBYE(status);
    // Get ID of inserted row 
    xlist_id = sqlite3_last_insert_rowid(g_db);
    // Insert the original field into flds_x_xlist
    zero_string(command, 1024); 
    sprintf(command, "insert into flds_x_xlist (fld, fld_id, xlist_id) values ('%s', %d, %d)", xfld, xfld_id, xlist_id);
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if ( rc ) { handle_sqlite_error(); }
    /*--------------------------------------------------*/
    zero_string(command, 1024); 
    sprintf(command, "update list set xlist_id = %d where id = %d ", 
	    xlist_id, list_id);
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if ( rc ) { handle_sqlite_error(); }
  }
  else {
    bool b_exists; int cum_xcl_fld_id;
    /* Make sure that this field does not already occur */
    status = is_fld_in_xlist(xfld, xlist_id, &b_exists); cBYE(status);
    if ( b_exists ) { 
      fprintf(stderr, "[%s] already excluded\n", xfld); goto BYE; 
    }
    /*--------------------------------------------------*/
    // Insert the original field into flds_x_xlist
    zero_string(command, 1024); 
    sprintf(command, "insert into flds_x_xlist (fld, fld_id, xlist_id) values ('%s', %d, %d)", xfld, xfld_id, xlist_id);
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if ( rc ) { handle_sqlite_error(); }
    /*--------------------------------------------------*/
    // Get name of current cum_xcl_fld in Table xlist 
    status = get_xlist_info(xlist_id, &cum_xcl_fld, &cum_xcl_fld_id); cBYE(status);
    /* Update cum_xcl_fld with new field */
    status = f1f2opf3(tbl, xfld, cum_xcl_fld, "||", cum_xcl_fld); cBYE(status);
  }
  /* For each existing field, we do "and not x" with new excl field */
  status = and_not_excl(list); cBYE(status);
 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  free_if_non_null(cum_xcl_fld);
  return(status);
}
