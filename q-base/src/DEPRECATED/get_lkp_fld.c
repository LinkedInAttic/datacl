#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

/* Consider a table that is designated as a lookup table. A lookup table
 * is one that must have the following 2 fields
 (1) an id field, which is an integer field
 (2) a description field, which is a char string field
 It can have other fields but it must have the above. 

 So, what this function does is, given a table and either "id" or
 "description", it returns information about the field that matches that.
 If the table is NOT a lookup table, it will return an error in the
 string result */


// START FUNC DECL
int
get_lkp_fld(
	    char *tbl,
	    char *str_id_or_desc,
	    char *str_result
	    )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, fld_id, id_or_desc;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_id_or_desc == NULL ) || ( *str_id_or_desc == '\0' ) ) { go_BYE(-1); }
#define LKP_ID          100
#define LKP_DESCRIPTION 200
  if ( strcmp(str_id_or_desc, "id") == 0 ) { 
    id_or_desc = LKP_ID;
  }
  else if ( strcmp(str_id_or_desc, "description") == 0 ) { 
    id_or_desc = LKP_DESCRIPTION;
  }
  else { go_BYE(-1); } 
  //------------------------------------------------ 
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  //------------------------------------------------
  fld_id = -1;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_fld[i].tbl_id != tbl_id ) { continue; } 
    if ( g_fld[i].name[0] == '\0' ) {  continue; } 
    if ( ( ( g_fld[i].is_lkp_id ) && ( id_or_desc = LKP_ID ) ) || 
	 ( ( g_fld[i].is_lkp_val ) && ( id_or_desc = LKP_DESCRIPTION )) ) { 
      fld_id = i;
      break;
    }
  }
  if ( fld_id < 0 ) { 
    strcpy(str_result, "\"-1\",\"\",\"\"");
  }
  else {
    sprintf(str_result, "\"%d\",\"%s\",\"%s\"", 
	    fld_id, g_fld[fld_id].name, g_fld[fld_id].dispname);
  }
  //------------------------------------------------
 BYE:
  return(status);
}
