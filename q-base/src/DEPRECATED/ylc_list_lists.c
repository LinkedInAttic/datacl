/* Yoni List Creator */
#include <sqlite3.h>
#include "constants.h"
#include "macros.h"
#include "md5global.h"
#include "mddriver.h"
#include "qtypes.h"
#include "auxil.h"
#include "ylc_auxil.h"
#include "ylc_globals.h"
int ylc_list_lists(
	    )
{
  int status = 0;
  int rc; char *zErrMsg = NULL;
  rc = sqlite3_exec(g_db, "select * from list", callback_pr, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  // TODO P3 free_if_non_null(zErrMsg);
  return(status);
} 

