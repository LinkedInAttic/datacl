//
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
// START FUNC DECL
int ylc_descr_excl(
			    char *list
			    )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_list = false; int list_id = -1, tbl_id = -1, xlist_id = -1;
  int rc; char *zErrMsg = NULL;
  char command[1024];
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); 
    return(status);
  }
  else {
    status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
    if ( xlist_id <= 0 ) { 
      fprintf(stderr, "No exclusions for List [%s] \n", list); 
    }
    else {
      char *flds1 = "cum_fld,cum_fld_id";
      zero_string(command, 1024);
      fprintf(stderr,"%s\n", flds1);
      sprintf(command, "select %s from xlist where id = %d ", 
	      flds1, xlist_id);
      rc = sqlite3_exec(g_db, command, callback_pr, 0, &zErrMsg);
      if ( rc ) { handle_sqlite_error(); }

      char *flds2 = "fld,fld_id";
      zero_string(command, 1024);
      fprintf(stderr,"%s\n", flds2);
      sprintf(command, "select %s from flds_x_xlist where xlist_id = %d ", 
	      flds2, xlist_id);
      rc = sqlite3_exec(g_db, command, callback_pr, 0, &zErrMsg);
      if ( rc ) { handle_sqlite_error(); }
    }
  }
  
 BYE:
  // TODO P3: sqlite3_free(zErrMsg);
  return(status);
}

