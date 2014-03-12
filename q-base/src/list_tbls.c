#include "qtypes.h"
#include "dbauxil.h"
#include "meta_globals.h"
#include "auxil.h"
#include "aux_meta.h"

// START FUNC DECL
int
list_tbls(
	 )
// STOP FUNC DECL
{
  int status = 0;
  for ( int i = 0; i < g_n_tbl; i++ ) { 
    if ( g_tbls[i].name[0] != '\0' ) { /* entry in use */
      char str_tbltype[16]; zero_string(str_tbltype, 16);
      status = mk_str_tbltype(g_tbls[i].tbltype, str_tbltype); cBYE(status);
      fprintf(stdout, "%d,%lld,%s,%s\n", i, g_tbls[i].nR, 
	  str_tbltype, g_tbls[i].name); 
    }
  }
BYE:
  return(status);
}
