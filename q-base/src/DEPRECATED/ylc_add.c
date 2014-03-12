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
#include "is_tbl.h"

int ylc_add(
	    char *list,
	    char *tbl
	    )
{
  int status = 0;
  int rc; char *zErrMsg = NULL; bool b_is_list;
  char command[1024];

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  if ( ( tbl  == NULL ) || ( *tbl  == '\0' ) ) { go_BYE(-1); }
  if ( strlen(list) >= MAX_LEN_LIST_NAME ) { go_BYE(-1); }

  status = is_list(list, &b_is_list);
  if ( b_is_list ) { 
    fprintf(stderr, "List [%s] already exists \n", list); go_BYE(-1);
  }
  int tbl_id; TBL_REC_TYPE tbl_rec;
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) {
    fprintf(stderr, "Table [%s] does not exist \n", tbl); go_BYE(-1); 
  }

  zero_string(command, 1024); 
  sprintf(command, "insert into list (name, tbl, tbl_id) values ('%s', '%s', %d)", 
	  list, tbl, tbl_id);
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { 
    fprintf(stderr,"ERROR=[%s] Command=[%s] \n", zErrMsg, command);
    go_BYE(-1); 
  }
 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  return(status);
}

