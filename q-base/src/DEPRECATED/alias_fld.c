#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "add_fld.h"
#include "is_fld.h"
#include "del_fld.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
alias_fld(
       char *tbl,
       char *f1,
       char *f2
       )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, f1_id, f2_id, nn_f1_id, nn_f2_id;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec, f2_rec, nn_f2_rec;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); } 
  //-------------------------------------------------------- 
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status); 
  chk_range(tbl_id, 0, g_n_tbl); 
  status = is_fld(NULL, tbl_id, f2, &f2_id, &f2_rec, 
      &nn_f2_id, &nn_f2_rec); cBYE(status); 
  if ( f2_id >= 0 ) { 
    status = del_fld(NULL, tbl_id, NULL, f2_id, 1); cBYE(status);
  }
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  cBYE(status); 
  chk_range(f1_id, 0, g_n_fld); 

  // Get empty spot for f2
  status = get_empty_fld(f2, g_flds, g_n_fld, &f2_id); cBYE(status);
  // f2 is same as f1 except for alias_of_fld_id
  // f1 may be an alias of some other field. Follow the alias chain
  // until you get to the primary field 

  int f0_id = f1_id; // root of alias chain. Could be f1 itself
  for ( ; ; ) { 
    if ( g_flds[f0_id].alias_of_fld_id >= 0 ) {
      f0_id = g_flds[f0_id].alias_of_fld_id;
    }
    else {
      break;
    }
  }
  g_flds[f2_id] = g_flds[f0_id];
  g_flds[f2_id].alias_of_fld_id = f0_id;
  // Add to hash table 
  //--------------------------------------------------------
  // Add auxiliary field nn if if it exists
  if ( nn_f1_id >= 0 ) { 
    fprintf(stderr, " TO BE IMPLEMENTED \n"); go_BYE(-1);
  }
BYE:
  return(status);
}
