#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "del_fld.h"
#include "qsort_asc_I8.h"

// START FUNC DECL
int  
del_fld(
	const char *tbl,
	int tbl_id,
	const char *fld,
	int fld_id
	)
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  int nn_fld_id = -1;

  if ( tbl_id < 0 ) {
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  }
  if ( tbl_id < 0 ) { /* nothing to do */ goto BYE; }
  //------------------------------------------------------
  if ( fld_id < 0 ) { 
    if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
    status = is_fld(tbl, tbl_id, fld, &fld_id, &fld_rec, 
		    &nn_fld_id, &nn_fld_rec); 
    cBYE(status);
  }
  else {
    fld_rec = g_flds[fld_id];
    nn_fld_id = g_flds[fld_id].nn_fld_id;
    if ( nn_fld_id >= 0 ) { 
      nn_fld_rec = g_flds[nn_fld_id];
    }
    else {
      zero_fld_rec(&nn_fld_rec);
    }
  }
  if ( fld_id < 0 ) {  /* Nothing to do */ return status ; }
  // ---------------------------------------------------------
  // Make sure that nobody else is using this field
  for ( int i = 0; i < g_n_fld_info; i++ ) { 
    // Following is because all the good entries are at the beginning
    // and the null values come after that
    /* TODO P2: Uncomment. See above for rationale 
     * if ( g_fld_info[i].mode == 0 ) { break; }
     * */
    if ( ( g_fld_info[i].mode == fk_fld_txt_lkp ) && 
         ( g_fld_info[i].xxx.dict_fld_id == fld_id ) ) { 
      int fld_id = g_fld_info[i].fld_id; 
      chk_range(fld_id, 0, g_n_fld);
      int tbl_id = g_flds[fld_id].tbl_id;
      chk_range(tbl_id, 0, g_n_tbl);
      fprintf(stderr, "In use by field %s of table %s \n", 
	      g_flds[fld_id].name, g_tbls[tbl_id].name);
      go_BYE(-1);
    }
  }
  //--------------------------------------------
  // You should not be deleting auxiliary fields directly
  if ( g_flds[fld_id].auxtype != undef_auxtype ) { 
    fprintf(stderr, "Can delete only primary fields\n"); go_BYE(-1);
  }
  /* Unlink storage. Exception when external */
  if ( g_flds[fld_id].is_external == false ) {
    char filename[32]; zero_string(filename, 32);
    status = q_delete(g_flds[fld_id].ddir_id, g_flds[fld_id].fileno);
    cBYE(status);
  }
  zero_fld_rec(&(g_flds[fld_id])); /* Delete entry for this field  */

  // Delete nn field if necessary
  if ( nn_fld_id >= 0 ) {
    if ( ( g_flds[fld_id].is_external ) != (g_flds[nn_fld_id].is_external ) ) {
      go_BYE(-1);
    }
  }
  if ( nn_fld_id >= 0 ) {
    if ( g_flds[nn_fld_id].is_external == false ) { 
      status = q_delete(g_flds[nn_fld_id].ddir_id, g_flds[nn_fld_id].fileno);
    }
    zero_fld_rec(&(g_flds[nn_fld_id])); /* Delete entry for this field  */
  }
  for ( int i = 0; i < g_n_fld_info; i++ ) { 
    int aux_fld_id = -1;  int aux_tbl_id = -1;
    // Following is because all the good entries are at the beginning
    // and the null values come after that
    /* TODO P2: Uncomment. See above for rationale 
     * if ( g_fld_info[i].mode == 0 ) { break; }
     * */
    if ( g_fld_info[i].fld_id != fld_id ) { continue; }
    if ( g_fld_info[i].mode == fk_fld_len ) {
      aux_fld_id = g_fld_info[i].xxx.len_fld_id; 
    }
    else if ( g_fld_info[i].mode == fk_fld_off ) {
      aux_fld_id = g_fld_info[i].xxx.off_fld_id; 
    }
    else if ( g_fld_info[i].mode == fk_fld_txt_lkp ) {
      aux_tbl_id = g_fld_info[i].xxx.dict_fld_id;
    }
    else {
      go_BYE(-1);
    }
    zero_fld_info_rec(&(g_fld_info[i])); 
    // START: Re-sort 
    // Keep stuff sorted for easy access. Unused values at end 
    if ( sizeof(FLD_INFO_TYPE) != sizeof(long long) ) { go_BYE(-1); }
    long long *X = (long long *)g_fld_info;
    qsort_asc_I8(X, g_n_fld_info, sizeof(long long), NULL);
    // TODO: P1 I thought I should be sorting descending. Need to figure
    // STOP: Re-sort 
    if ( aux_fld_id >= 0 ) { 
      if ( g_flds[aux_fld_id].is_external == false ) { 
        status=q_delete(g_flds[aux_fld_id].ddir_id, g_flds[aux_fld_id].fileno);
      }
      int aux_ddir_id = g_flds[aux_fld_id].ddir_id;
      int aux_fileno  = g_flds[aux_fld_id].fileno;
      status = q_delete(aux_ddir_id, aux_fileno); cBYE(status);
      zero_fld_rec(&(g_flds[aux_fld_id])); /* Delete entry for this field  */
    }
  }
 BYE:
  return status ;
}
// START FUNC DECL
int  
iter_del_fld(
	const char *tbl,
	const char *fld
	)
// STOP FUNC DECL
{
  int status = 0;
  char **Y = NULL; int nY = 0; bool is_multiple = false;

  for ( int i = 0; ; i++ ) { 
    if ( fld[i] == '\0' ) { break; }
    if ( fld[i] == ':' ) { is_multiple = true; break; }
  }
  if ( is_multiple ) {
    status = break_str(fld, ":", &Y, &nY); cBYE(status);
    for ( int i = 0; i < nY; i++ ) { 
      status = del_fld(tbl, -1, Y[i], -1); cBYE(status);
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  else {
    status = del_fld(tbl, -1, fld, -1); cBYE(status);
  }
BYE:
  return status ;
}
