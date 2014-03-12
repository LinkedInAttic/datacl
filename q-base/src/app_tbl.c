#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
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
       char *t1, /* destination */
       char *t2  /* source */
       )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE t1_rec, t2_rec;
  int num_flds_1 = 0;
  long long nR1, nR2; int t1_id, t2_id;
  char str_new_nR[32];
  int flds_1[MAX_NUM_FLDS_IN_TBL], flds_2[MAX_NUM_FLDS_IN_TBL];
  //----------------------------------------------------------------
  zero_string(str_new_nR, 32);
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  for ( int i = 0; i < MAX_NUM_FLDS_IN_TBL; i++ ) {
    flds_1[i] = INT_MIN;
    flds_2[i] = INT_MIN;
  }
  /* Check if tables exist */
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  // Get primary fields in Table t1 
  status = tbl_meta(t1, false, flds_1, &num_flds_1); cBYE(status);
  //------------------------------------------------
  /* Check that all T2 has all the fields in T1 */
  for ( int i = 0; i < num_flds_1; i++ ) { 
    int t2f1_id, nn_t2f1_id;
    FLD_REC_TYPE t2f1_rec, nn_t2f1_rec;
    int t1f1_id = flds_1[i];
    char *t1f1 = g_flds[t1f1_id].name;
    FLD_REC_TYPE t1f1_rec = g_flds[t1f1_id];
    switch ( t1f1_rec.fldtype ) { 
      case SC : case SV : go_BYE(-1); break;
      default : /* ok */ break;
    }
    if ( t1f1_rec.is_external == true ) {
      fprintf(stderr, "Field %s in Table %s is external field \n", t1f1, t1);
      go_BYE(-1);
    }
    status = is_fld(NULL, t2_id, t1f1, &t2f1_id, &t2f1_rec, 
	&nn_t2f1_id, &nn_t2f1_rec);
    cBYE(status);
    if ( t2f1_id < 0 ) { 
      fprintf(stderr, "Field %s in Table %s not present in Table %s \n",
	  t1f1, t1, t2);
      go_BYE(-1); 
    }
    if ( t1f1_rec.auxtype != t2f1_rec.auxtype ) { go_BYE(-1); }
    if ( t1f1_rec.fldtype != t2f1_rec.fldtype ) { go_BYE(-1); }
    flds_2[i] = t2f1_id;
  }
  //------------------------------------------------
  for ( int i = 0; i < num_flds_1; i++ ) { 
    int t1f1_id = flds_1[i];
    int t2f1_id = flds_2[i];
    status = app_fld(t1_id, t1f1_id, t2_id, t2f1_id); cBYE(status);
  }
  //--------------------------------------------------------
  // Update nR for T1
  nR1 = t1_rec.nR;
  nR2 = t2_rec.nR;
  g_tbls[t1_id].nR = nR1 + nR2;
  //--------------------------------------------------------
BYE:
  return status ;
}
