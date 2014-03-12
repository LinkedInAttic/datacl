#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "dbauxil.h"
#include "del_tbl.h"
#include "meta_globals.h"
//---------------------------------------------------------------
// START FUNC DECL
int
del_all(
	)
// STOP FUNC DECL
{
  int status = 0;
  for ( int i = 0; i < g_n_tbl; i++ ) { 
    if ( g_tbl[i].name[0] != '\0' ) {
      status = del_tbl(NULL, i);
      cBYE(status);
    }
  }
  for ( int i = 0; i < g_n_ht_tbl; i++ ) { 
    g_ht_tbl[i].key = 0;
    g_ht_tbl[i].val = -1;
  }
BYE:
  return(status);
}
