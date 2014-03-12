#include <limits.h>
#include "qtypes.h"
#include "dbauxil.h"
#include "del_fld.h"
#include "is_tbl.h"
#include "auxil.h"
#include "tbl_meta.h"
#include "aux_meta.h"
#include "meta_globals.h"
// START FUNC DECL
int
del_tbl(
	const char *tbl,
	int in_tbl_id
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id;
  TBL_REC_TYPE tbl_rec;
  // Basic arg checks 
  if ( in_tbl_id < 0 ) { 
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  }
  else {
    tbl_id = in_tbl_id;
  }
  if ( tbl_id < 0 ) { /* Nothing to do */
    goto BYE;
  }
  //------------------------------------------------------
  switch ( g_tbls[tbl_id].tbltype ) { 
  case regular : 
  case txt_lkp : 
    // Delete all the primary fields in this table (aux will happen)
    for ( int i = 0; i < g_n_fld; i++ ) { 
      if ( ( g_flds[i].tbl_id == tbl_id ) &&
	   ( g_flds[i].auxtype == undef_auxtype ) ) { 
	status = del_fld( NULL, tbl_id, NULL, i); cBYE(status);
      }
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }

  //------------------------------------------------
  // Delete table itself
  zero_tbl_rec(&(g_tbls[tbl_id]));
 BYE:
  return status ;
}
// START FUNC DECL
int  
iter_del_tbl(
	     const char *tbl
	     )
// STOP FUNC DECL
{
  int status = 0;
  char **Y = NULL; int nY = 0; bool is_multiple = false;

  for ( int i = 0; ; i++ ) { 
    if ( tbl[i] == '\0' ) { break; }
    if ( tbl[i] == ':' ) { is_multiple = true; break; }
  }
  if ( is_multiple ) {
    status = break_str(tbl, ":", &Y, &nY); cBYE(status);
    for ( int i = 0; i < nY; i++ ) { 
      status = del_tbl(Y[i], -1); cBYE(status);
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  else {
    status = del_tbl(tbl, -1); cBYE(status);
  }
 BYE:
  return status ;
}
