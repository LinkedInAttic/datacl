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
int ylc_num_in_list(
		    char *list,
		    char *mode,
		    int *ptr_num_in_list,
		    long long *ptr_cnt
		    )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_list = false; int list_id = -1, tbl_id = -1, 
			    xlist_id = -1;

  *ptr_num_in_list = -1;
  *ptr_cnt = -1; 
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }
  status = get_num_in_list(list_id, ptr_num_in_list); cBYE(status);
  if ( *ptr_num_in_list <= 0 ) { go_BYE(-1); }

  status = get_sum_fld_cnt(list_id, *ptr_num_in_list, mode, ptr_cnt); cBYE(status);
 BYE:
  return(status);
}

