#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "dbauxil.h"
#include "tbl_meta.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "meta_data.h"
#include "meta_globals.h"
#include "from_to_meta_globals.h"
extern char g_docroot[MAX_LEN_DIR_NAME+1];
extern char g_data_dir[MAX_LEN_DIR_NAME+1];
//---------------------------------------------------------------
// START FUNC DECL
int
import(
       char *frm_docroot,
       char *frm_data_dir,
       char *frm_tbl,
       char *to_tbl
       )
// STOP FUNC DECL
{
  int status = 0;
  int ddir_id = INT_MIN;
  int frm_tbl_id = INT_MIN, to_tbl_id = INT_MIN; 
  int frm_dict_tbl_id = INT_MIN, to_dict_tbl_id = INT_MIN;
  TBL_REC_TYPE frm_tbl_rec, to_tbl_rec, to_dict_tbl_rec;
  long long nR;
  int num_flds, frm_fld_ids[MAX_NUM_FLDS];
  char frm_tbl_dispname[MAX_LEN_TBL_NAME+1];
  char str_nR[32];
  //------------------------------------------------
  if ( ( frm_docroot == NULL ) || ( *frm_docroot == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(frm_docroot, g_docroot) == 0 ) { go_BYE(-1); }
  if ( ( to_tbl == NULL ) || ( *to_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( frm_data_dir == NULL ) || ( *frm_data_dir == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(frm_data_dir, g_data_dir) == 0 ) { go_BYE(-1); }
  //--------------------------------------------
  // Set up meta data for both FROM and TO 
  status = mmap_meta_data(frm_docroot, 
	&frm_tbl_X, &frm_tbl_nX, &frm_tbls, &frm_n_tbl, 
	&frm_ht_tbl_X, &frm_ht_tbl_nX, &frm_ht_tbl, &frm_n_ht_tbl, 
	&frm_fld_X, &frm_fld_nX, &frm_flds, &frm_n_fld, 
	&frm_ht_fld_X, &frm_ht_fld_nX, &frm_ht_fld, &frm_n_ht_fld,
	&frm_ddir_X, &frm_ddir_nX, &frm_ddir, &frm_n_ddir);
  cBYE(status);
  status = mmap_meta_data(g_docroot, 
	&to_tbl_X, &to_tbl_nX, &to_tbls, &to_n_tbl, 
	&to_ht_tbl_X, &to_ht_tbl_nX, &to_ht_tbl, &to_n_ht_tbl, 
	&to_fld_X, &to_fld_nX, &to_flds, &to_n_fld, 
	&to_ht_fld_X, &to_ht_fld_nX, &to_ht_fld, &to_n_ht_fld,
	&to_ddir_X, &to_ddir_nX, &to_ddir, &to_n_ddir);
  cBYE(status);

  //--------------------------------------------
  // Add the from data directory to the current list of data directories
  status = get_ddir_id(frm_data_dir, to_ddir, to_n_ddir, true, &ddir_id);
  cBYE(status);
  //--------------------------------------------
  /* Use FROM meta data */
  set_default_meta_data(
	frm_tbl_X, frm_tbl_nX, frm_tbls, frm_n_tbl, 
	frm_ht_tbl_X, frm_ht_tbl_nX, frm_ht_tbl, frm_n_ht_tbl, 
	frm_fld_X, frm_fld_nX, frm_flds, frm_n_fld, 
	frm_ht_fld_X, frm_ht_fld_nX, frm_ht_fld, frm_n_ht_fld,
	frm_ddir_X, frm_ddir_nX, frm_ddir, frm_n_ddir);
  status = is_tbl(frm_tbl, &frm_tbl_id, &frm_tbl_rec); cBYE(status);
  chk_range(frm_tbl_id, 0, frm_n_tbl);
  nR = g_tbls[frm_tbl_id].nR;
  zero_string(frm_tbl_dispname, MAX_LEN_TBL_NAME+1);
  strcpy(frm_tbl_dispname, g_tbls[frm_tbl_id].dispname);
  bool frm_is_dict_tbl = g_tbls[frm_tbl_id].is_dict_tbl;
  /* Find primary fields in frm_tbl */
  status = tbl_meta(frm_tbl, false, frm_fld_ids, &num_flds); cBYE(status);
  //--------------------------------------------
  /* Use TO meta data */
  set_default_meta_data(
	to_tbl_X, to_tbl_nX, to_tbls, to_n_tbl, 
	to_ht_tbl_X, to_ht_tbl_nX, to_ht_tbl, to_n_ht_tbl, 
	to_fld_X, to_fld_nX, to_flds, to_n_fld, 
	to_ht_fld_X, to_ht_fld_nX, to_ht_fld, to_n_ht_fld,
	to_ddir_X, to_ddir_nX, to_ddir, to_n_ddir);
  status = is_tbl(to_tbl, &to_tbl_id, &to_tbl_rec); cBYE(status);
  if ( to_tbl_id >= 0 ) { 
    status = del_tbl(NULL, to_tbl_id);
  }
  // Add table in currrent docroot
  zero_string(str_nR, 32);
  sprintf(str_nR, "%lld", nR);
  status = add_tbl(to_tbl, str_nR, &to_tbl_id, &to_tbl_rec);
  strcpy(g_tbls[to_tbl_id].dispname, frm_tbl_dispname);
  g_tbls[to_tbl_id].is_dict_tbl = frm_is_dict_tbl;
  //--------------------------------------------------------
  for ( int i = 0; i < num_flds; i++ ) { /* for each primary field */
    int frm_fld_id = INT_MIN, to_fld_id = INT_MIN;
    int nn_frm_fld_id = INT_MIN, nn_to_fld_id = INT_MIN;
    FLD_REC_TYPE frm_fld_rec;

    frm_fld_id = frm_fld_ids[i];
    frm_fld_rec = frm_flds[frm_fld_id];
    status = get_empty_fld(frm_fld_rec.name, to_flds, to_n_fld, &to_fld_id); 
    cBYE(status);
    copy_fld_meta(&(to_flds[to_fld_id]), frm_fld_rec);

    /* START: over-ride specific fields */
    // handle dict_tbl_id carefully 
    frm_dict_tbl_id = frm_fld_rec.dict_tbl_id;
    if ( frm_dict_tbl_id >= 0 ) { 
      status = is_tbl( frm_tbls[frm_dict_tbl_id].name, &to_dict_tbl_id, 
	  &to_dict_tbl_rec); 
      cBYE(status);
      chk_range(to_dict_tbl_id, 0, g_n_tbl);
      to_flds[to_fld_id].dict_tbl_id      = to_dict_tbl_id;
    }
    to_flds[to_fld_id].tbl_id      = to_tbl_id;
    to_flds[to_fld_id].ddir_id     = ddir_id; 
    to_flds[to_fld_id].is_external = true;
    to_flds[to_fld_id].nn_fld_id   = INT_MIN; // Recomputed below
    /* STOP: over-ride specific fields */
    //--------------------------------------------------
    nn_frm_fld_id = frm_fld_rec.nn_fld_id;
    if ( nn_frm_fld_id >= 0 ) {
      char *nn_fld_name  = frm_flds[nn_frm_fld_id].name;
      status = get_empty_fld(nn_fld_name, to_flds, to_n_fld, &nn_to_fld_id);
      cBYE(status);

      copy_fld_meta(&(to_flds[nn_to_fld_id]), frm_flds[nn_frm_fld_id]);
      /* over-ride specific fields */
      to_flds[nn_to_fld_id].tbl_id      = to_tbl_id;
      to_flds[nn_to_fld_id].ddir_id     = ddir_id; 
      to_flds[nn_to_fld_id].is_external = true;

      /* set back and forward pointers */
      to_flds[nn_to_fld_id].parent_id = to_fld_id;
      to_flds[to_fld_id].nn_fld_id = nn_to_fld_id;
    }
  }
BYE:
  unmap_meta_data(frm_tbl_X, frm_tbl_nX, frm_ht_tbl_X, frm_ht_tbl_nX, 
	frm_fld_X, frm_fld_nX, frm_ht_fld_X, frm_ht_fld_nX,
	frm_ddir_X, frm_ddir_nX);
  unmap_meta_data(to_tbl_X, to_tbl_nX, to_ht_tbl_X, to_ht_tbl_nX, 
	to_fld_X, to_fld_nX, to_ht_fld_X, to_ht_fld_nX,
	to_ddir_X, to_ddir_nX);
  return status ;
}
