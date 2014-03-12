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
int ylc_describe(
		 char *list
		 )
{
  int status = 0;
  bool b_is_list = false; int list_id = -1, tbl_id = -1, xlist_id = -1;
  int rc; char *zErrMsg = NULL;
  char command[1024];
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); 
    go_BYE(-1);
  }
  else {
    status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
    if ( list_id <= 0 ) { go_BYE(-1); }

    char *flds="id,fld,fld_id,fld_cnt,cum_fld_id,cum_fld_cnt,new_fld_id,new_fld_cnt,xcl_fld_id,xcl_fld_cnt,lmt_fld_id,lmt_fld_cnt,list_id,position";
    zero_string(command, 1024);
    fprintf(stderr,"%s\n", flds);
    sprintf(command, "select %s from flds_x_list where list_id = %d ", 
	    flds, list_id);
    rc = sqlite3_exec(g_db, command, callback_pr, 0, &zErrMsg);
    if ( rc ) { 
      fprintf(stderr,"ERROR %s Command %s \n", zErrMsg, command);
      go_BYE(-1); 
    }
  }
  
 BYE:
  sqlite3_free(zErrMsg);
  return(status);
}

