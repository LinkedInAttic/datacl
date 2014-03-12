#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

// START FUNC DECL
int
get_disp_name_fld(
	char *tbl,
	char *fld,
	char *str_result,
	int len_str_result
	)
// STOP FUNC DECL
{
  int status = 0;
  int i = 0, tbl_id, fld_id;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  char *cptr1 = g_fld[fld_id].dispname;
  char *cptr2 = str_result;
  *cptr2 = '"'; cptr2++; i++;
  for ( ; *cptr1 != '\0'; i++ ) { 
    if ( ( *cptr1 == '\\' ) ||  ( *cptr1 == '"' ) ) {
      if ( i >= len_str_result ) { go_BYE(-1); }
      *cptr2 = '\\';
      cptr2++;
      i++;
    }
    *cptr2 = *cptr1;
    cptr1++;
    cptr2++;
    i++;
  }
  *cptr2 = '"'; i++;
 BYE:
  return(status);
}
