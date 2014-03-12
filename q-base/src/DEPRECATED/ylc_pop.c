/* Yoni List Creator */
#include <sqlite3.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "auxil.h"
#include "mmap.h"
#include "ylc_pop.h"
#include "ylc_auxil.h"
#include "ylc_dbauxil.h"
#include "ylc_globals.h"
#include "del_fld.h"
/*
#include "meta_data.h"
#include "aux_meta.h"
#include "fld_meta.h"
#include "tbl_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f1f2opf3.h"
#include "ext_f_to_s.h"
#include "dup_fld.h"
#include "fop.h"
*/
int ylc_pop(
	    char *list
	    )
{
  int status = 0;
  int rc, n, n_before, n_after, list_id = -1, tbl_id = -1, xlist_id = -1;
  char *zErrMsg = NULL;
  char *cum_fld = NULL; int cum_fld_id;
  char command[1024];

  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }

  status = get_num_in_list(list_id, &n_before); cBYE(status);

  if ( n_before == 0 ) { 
    fprintf(stderr, "List [%s] is empty \n", list); return(status);
  }
  /* Delete exclusion list if exists */
  if ( xlist_id >= 0 ) { 
    status = get_xlist_info(xlist_id, &cum_fld, &cum_fld_id);
    cBYE(status);
    status = del_fld(NULL, tbl_id,  NULL, cum_fld_id, true); cBYE(status);

    zero_string(command, 1024);
    sprintf(command, "delete from xlist where id = %d ", xlist_id);
    status = db_get_num_rslts(g_db, command, &n);

    zero_string(command, 1024);
    sprintf(command, "delete from flds_x_xlist where xlist_id = %d ", xlist_id);
    status = db_get_num_rslts(g_db, command, &n);

  }

  zero_string(command, 1024);
  sprintf(command, "select count(*) from flds_x_list where list_id = %d and position = %d \n", list_id, n_before);
  status = db_get_num_rslts(g_db, command, &n);
  if ( n != 1 ) { fprintf(stderr, "Data Integrity Violated\n"); go_BYE(-1); }
  int position = n_before;

  // Delete Q fields if needed 
  status = del_aux_fld_if_exists(tbl_id, list_id, position, "new_"); 
  cBYE(status);
  // Delete fld entry in meta data 
  zero_string(command, 1024);
  sprintf(command, "delete from flds_x_list where list_id = %d and position = %d \n", list_id, n_before);
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }

  status = get_num_in_list(list_id, &n_after); cBYE(status);
  if ( n_after != (n_before - 1) ) { go_BYE(-1); }

 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  free_if_non_null(cum_fld);
  return(status);
}
