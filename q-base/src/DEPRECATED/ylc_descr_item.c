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
int ylc_descr_item(
		   char *list,
		   char *str_pos,
		   char *mode,
		   char *what,
		   int *ptr_fld_id,
		   char **ptr_fld,
		   long long *ptr_fld_cnt,
		   char *opbuf
		   )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_list = false;  char *endptr;
  int list_id = -1, xlist_id = -1, tbl_id = -1, num_in_list = -1, pos = -1;

  *ptr_fld_id = -1;
  *ptr_fld_cnt = -1;
  *ptr_fld = NULL;

  if ( ( list == NULL ) || ( *list == '\0' ) )  { go_BYE(-1); }
  if ( ( str_pos  == NULL ) || ( *str_pos  == '\0' ) )  { go_BYE(-1); }
  if ( ( what == NULL ) || ( *what == '\0' ) )  { go_BYE(-1); }
  chk_mode(mode);
  pos = strtol(str_pos, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( pos <= 0 ) { go_BYE(-1); }

  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }
  status = get_num_in_list(list_id, &num_in_list); cBYE(status);
  if ( num_in_list <= 0 ) { go_BYE(-1); }
  if ( pos > num_in_list ) { go_BYE(-1); }

  chk_mode(mode);
  if ( strcmp(what, "id") == 0 ) {
    status = get_fld_id(list_id, pos, mode, ptr_fld_id); cBYE(status);
    sprintf(opbuf, "%d", *ptr_fld_id);
  }
  else if ( strcmp(what, "name") == 0 ) {
    status = get_fld_name(list_id, pos, mode, ptr_fld); cBYE(status);
    sprintf(opbuf, "%s", *ptr_fld);
  }
  else if ( strcmp(what, "cnt") == 0 ) {
    status = get_fld_cnt(list_id, pos, mode, ptr_fld_cnt); cBYE(status);
    sprintf(opbuf, "%lld", *ptr_fld_cnt);
  }
  else {
    go_BYE(-1);
  }
  
 BYE:
  return(status);
}
