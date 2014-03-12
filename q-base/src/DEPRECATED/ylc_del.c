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
#include "del_fld.h"
// START FUNC DECL
int ylc_del(
	    char *list
	    )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_list; int rc, list_id = -1, tbl_id = -1, xlist_id = -1;
  int num_in_list;
  char *zErrMsg = NULL;
  char *cum_fld = NULL; int cum_fld_id;
  char command[1024]; 

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); return(status); 
  }
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }
  // Delete xlist if any
  if ( xlist_id > 0 ) { 
    status = get_xlist_info(xlist_id, &cum_fld, &cum_fld_id);
    cBYE(status);
    status = del_fld(NULL, tbl_id, NULL, cum_fld_id, true); cBYE(status);

    zero_string(command, 1024);
    sprintf(command, "delete  from xlist where id = %d \n", xlist_id); 
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK ){ handle_sqlite_error(); }
    // delete entries corresponding to this xlist 
    zero_string(command, 1024);
    sprintf(command, "delete  from flds_x_xlist where xlist_id = %d \n", xlist_id); 
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK ){ handle_sqlite_error(); }
  }
  status = get_num_in_list(list_id, &num_in_list); cBYE(status);
  for ( int position = num_in_list; position > 0 ; position-- ) { 
    status = del_aux_fld_if_exists(tbl_id, list_id, position, "lmt_");
    cBYE(status);
    status = del_aux_fld_if_exists(tbl_id, list_id, position, "xcl_");
    cBYE(status);
    status = del_aux_fld_if_exists(tbl_id, list_id, position, "new_");
    cBYE(status);
  }

  zero_string(command, 1024);
  sprintf(command, "delete  from flds_x_list where list_id = %d \n", list_id); 
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }

  zero_string(command, 1024);
  sprintf(command, "delete  from list where name = '%s' \n", list); 
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }
  
 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  free_if_non_null(cum_fld);
  return(status);
}

