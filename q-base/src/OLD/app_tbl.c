#include <stdio.h>
#include <unistd.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "tbl_meta.h"
#include "app_tbl.h"
#include "app_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// Appends table T2 to table T1
// START FUNC DECL
int 
app_tbl(
       char *t1,
       char *t2
       )
// STOP FUNC DECL
{
  int status = 0;
  int num_flds1 = 0; int *fld_ids1 = NULL;
  int num_flds2 = 0; int *fld_ids2 = NULL;
  bool is_present;
  long long nR1, nR2; int t1_id, t2_id;
  char str_new_nR[32];
  //----------------------------------------------------------------
  zero_string(str_new_nR, 32);
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  /* Check if tables exist */
  status = is_tbl(t1, &t1_id); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  status = is_tbl(t2, &t2_id); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  // Get primary fields in Table t1 
  status = flds_in_tbl(t1_id, false, &num_flds1, &fld_ids1);
  cBYE(status);
  status = flds_in_tbl(t2_id, false, &num_flds2, &fld_ids2);
  cBYE(status);
  if ( num_flds1 != num_flds2 ) { 
    fprintf(stderr, "Table %s has %d fields. Table %s has %d fields\n",
	t1, num_flds1, t2, num_flds2);
    go_BYE(-1);
  }
  //------------------------------------------------
  // Check that all fields in Table T1 are present in T2
  for ( int i = 0; i < num_flds1; i++ ) { 
    is_present = false;
    int f1_id = fld_ids1[i];
    chk_range(f1_id, 0, g_n_fld);
    char *f1 = NULL, *f2 = NULL;
    f1 = g_fld[f1_id].name;
    for ( int j = 0; j < num_flds2; j++ ) { 
      int f2_id = fld_ids2[j];
      chk_range(f2_id, 0, g_n_fld);
      f2 = g_fld[f2_id].name;
      if ( strcmp(f1, f2) == 0 ) {
	is_present = true;
	break;
      }
    }
    if ( !is_present ) {
      fprintf(stderr, "Field %s in Table %s not present in Table %s \n",
	  f1, t1, t2);
      go_BYE(-1);
    }
  }
  //------------------------------------------------
  /* Convince yourself that there is no need to check that all fields in
   * T2 are not in T1 */
  for ( int i = 0; i < num_flds1; i++ ) { 
    int f1_id = fld_ids1[i];
    char *f1 = g_fld[f1_id].name;
    status = app_fld(t1, f1, t2, f1);
    cBYE(status);
  }
  //--------------------------------------------------------
  // Update nR for T1
  nR1 = g_tbl[t1_id].nR;
  nR2 = g_tbl[t2_id].nR;
  g_tbl[t1_id].nR = nR1 + nR2;
  //--------------------------------------------------------
BYE:
  free_if_non_null(fld_ids1);
  free_if_non_null(fld_ids2);
  return(status);
}
