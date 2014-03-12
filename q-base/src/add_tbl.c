#include <limits.h>
#include "qtypes.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "auxil.h"
#include "is_tbl.h"
#include "hash_string.h"
#include "meta_globals.h"

// START FUNC DECL
int
get_empty_tbl(
	      const char *tbl,
	      TBL_REC_TYPE *tbls,
	      int n_tbl,
	      int *ptr_tbl_id
	      )
// STOP FUNC DECL
{
  int status = 0;
  unsigned int startidx ;

  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  status = hash_string_UI4(tbl, &startidx);
  startidx = startidx % n_tbl;

  *ptr_tbl_id = INT_MIN; /* no empty spot */
  for ( int i = startidx; i < n_tbl; i++ ) { 
    if ( tbls[i].name[0] == '\0' ) { 
      *ptr_tbl_id = i;
      break;
    }
  }
  if ( *ptr_tbl_id < 0 ) {
    for ( int i = 0; i < startidx; i++ ) { 
      if ( tbls[i].name[0] == '\0' ) { 
	*ptr_tbl_id = i;
	break;
      }
    }
  }
  if ( *ptr_tbl_id < 0 ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1);
  }
 BYE:
  return status ;
}

//---------------------------------------------------------------
// START FUNC DECL
int
add_tbl(
	const char *tbl,
	const char *str_nR,
	int *ptr_tbl_id,
	TBL_REC_TYPE *ptr_tbl_rec
	)
// STOP FUNC DECL
{
  int status = 0;
  long long nR; 
  int tbl_id = INT_MIN;
  TBL_REC_TYPE tbl_rec;
  zero_tbl_rec(&tbl_rec);
  //---------------------- Check inputs
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_nR == NULL ) || ( *str_nR == '\0' ) ) { go_BYE(-1); }
  status = stoI8(str_nR, &nR); cBYE(status);
  if ( nR <= 0 ) { go_BYE(-1); } 
  //------------------------------------------------------
  status = chk_tbl_name(tbl); cBYE(status);
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id >= 0 ) { /* Delete the table */
    status = del_tbl(NULL, tbl_id); cBYE(status);
  }
  else {
    status = get_empty_tbl(tbl, g_tbls, g_n_tbl, &tbl_id); cBYE(status);
  }
  strcpy(g_tbls[tbl_id].name, tbl);
  g_tbls[tbl_id].nR = nR;
  g_tbls[tbl_id].tbltype = regular;
  *ptr_tbl_id = tbl_id;
  *ptr_tbl_rec = g_tbls[tbl_id];
 BYE:
  return status ;
}
