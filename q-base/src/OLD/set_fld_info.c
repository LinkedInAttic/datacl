#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

// START FUNC DECL
int
set_fld_info(
	char *tbl,
	char *fld,
	char *in_str_fld_info
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, fld_id, isort;
  char *endptr = NULL, *cptr = NULL, *attr = NULL, *val = NULL;
  char *bak_str_fld_info = NULL, *str_fld_info = NULL;

  str_fld_info = malloc(strlen(in_str_fld_info) + 1);
  strcpy(str_fld_info, in_str_fld_info);
  bak_str_fld_info = str_fld_info;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_fld_info == NULL ) || ( *str_fld_info == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  //------------------------------------------------
  for ( ; ; ) { 
    if ( ( str_fld_info == NULL ) || ( *str_fld_info == '\0' ) ) { break; }
    // Read
    cptr = strsep(&str_fld_info, ":");
    if ( cptr == NULL ) { break; }
    // Break it into LHS of equals sign an RHS
    attr = strsep(&cptr, "=");
    if ( attr == NULL ) { go_BYE(-1); }
    val = strsep(&cptr, "=");
    if ( val == NULL ) {  go_BYE(-1); }
    if ( strcmp(attr, "sort") == 0 ) {
      isort = strtoll(val, &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
      if ( ( isort < -1 ) || ( isort > 2 ) ) { go_BYE(-1); }
      g_fld[fld_id].sorttype = isort;
    }
  }
  //------------------------------------------------
 BYE:
  free_if_non_null(bak_str_fld_info);
  return(status);
}
