#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "dbauxil.h"
#include "tbl_meta.h"
#include "aux_fld_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "init.h"
#include "meta_globals.h"
#include "from_to_meta_globals.h"
//---------------------------------------------------------------
// START FUNC DECL
int
import(
       char *src_docroot,
       char *src_tbl,
       char *dst_docroot, // SPECIFIED AS ENV VAR 
       char *dst_tbl
       )
// STOP FUNC DECL
{
  int status = 0;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  long long nR;
  int num_flds, *from_fld_ids = NULL;
  char src_tbl_dispname[MAX_LEN_TBL_NAME+1];
  char str_nR[32];
  //------------------------------------------------
  if ( ( src_docroot == NULL ) || ( *src_docroot == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_docroot, dst_docroot) == 0 ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------
  // Set up meta data for both FROM and TO 
  status = init(src_docroot, false, 
	&from_tbl_X, &from_tbl_nX, &from_tbl, &from_n_tbl, 
	&from_ht_tbl_X, &from_ht_tbl_nX, &from_ht_tbl, &from_n_ht_tbl, 
	&from_fld_X, &from_fld_nX, &from_fld, &from_n_fld, 
	&from_ht_fld_X, &from_ht_fld_nX, &from_ht_fld, &from_n_ht_fld);
  cBYE(status);
  status = init(dst_docroot, false, 
	&to_tbl_X, &to_tbl_nX, &to_tbl, &to_n_tbl, 
	&to_ht_tbl_X, &to_ht_tbl_nX, &to_ht_tbl, &to_n_ht_tbl, 
	&to_fld_X, &to_fld_nX, &to_fld, &to_n_fld, 
	&to_ht_fld_X, &to_ht_fld_nX, &to_ht_fld, &to_n_ht_fld);
  cBYE(status);

  //--------------------------------------------
  /* Use FROM meta data */
  set_default_meta_data(src_docroot, 
	from_tbl_X, from_tbl_nX, from_tbl, from_n_tbl, 
	from_ht_tbl_X, from_ht_tbl_nX, from_ht_tbl, from_n_ht_tbl, 
	from_fld_X, from_fld_nX, from_fld, from_n_fld, 
	from_ht_fld_X, from_ht_fld_nX, from_ht_fld, from_n_ht_fld);
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, from_n_tbl);
  nR = g_tbl[src_tbl_id].nR;
  /* TODO: Document the fact that is_map meta data is lost on import */
  /* TODO: Document the fact that is_lkp meta data is lost on import */
  zero_string(src_tbl_dispname, MAX_LEN_TBL_NAME+1);
  strcpy(src_tbl_dispname, from_tbl[src_tbl_id].dispname);
  /* Find primary fields in src_tbl */
  status = flds_in_tbl(src_tbl_id, false, &num_flds, &from_fld_ids);
  cBYE(status);
  //--------------------------------------------
  /* Use TO meta data */
  set_default_meta_data(dst_docroot, 
	to_tbl_X, to_tbl_nX, to_tbl, to_n_tbl, 
	to_ht_tbl_X, to_ht_tbl_nX, to_ht_tbl, to_n_ht_tbl, 
	to_fld_X, to_fld_nX, to_fld, to_n_fld, 
	to_ht_fld_X, to_ht_fld_nX, to_ht_fld, to_n_ht_fld);
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  if ( dst_tbl_id >= 0 ) { 
    status = del_tbl(NULL, dst_tbl_id);
  }
  // Create entry in dst_tbl
  zero_string(str_nR, 32);
  sprintf(str_nR, "%lld", nR);
  status = add_tbl(dst_tbl, str_nR, &dst_tbl_id);
  strcpy(to_tbl[dst_tbl_id].dispname, src_tbl_dispname);
  //--------------------------------------------------------
  for ( int i = 0; i < num_flds; i++ ) { 
    int from_fld_id = INT_MIN, to_fld_id = INT_MIN;
    int nn_from_fld_id = INT_MIN, sz_from_fld_id = INT_MIN;
    int nn_to_fld_id = INT_MIN,   sz_to_fld_id = INT_MIN;
    FLD_TYPE *from_fld_meta = NULL;
    FLD_TYPE *nn_from_fld_meta = NULL;
    FLD_TYPE *sz_from_fld_meta = NULL;

    from_fld_id = from_fld_ids[i];
    from_fld_meta = &(from_fld[from_fld_id]);
    status = get_empty_fld(&to_fld_id); cBYE(status);
    copy_fld_meta(&(g_fld[to_fld_id]), from_fld_meta);
    /* over-ride specific fields */
    to_fld[to_fld_id].tbl_id = dst_tbl_id;
    to_fld[to_fld_id].is_external = true;
    to_fld[to_fld_id].is_lkp_id = false;
    to_fld[to_fld_id].is_lkp_val = false;
    to_fld[to_fld_id].nn_fld_id = INT_MIN; // Recomputed below
    to_fld[to_fld_id].sz_fld_id = INT_MIN; // Recomputed below
    //--------------------------------------------------
    nn_from_fld_id = from_fld_meta->nn_fld_id;
    if ( nn_from_fld_id >= 0 ) {
      nn_from_fld_meta = &(from_fld[nn_from_fld_id]);
      status = add_aux_fld(dst_tbl, from_fld_meta->name,
	  nn_from_fld_meta->filename, "nn", &nn_to_fld_id);
      cBYE(status);
    }
    sz_from_fld_id = from_fld_meta->sz_fld_id;
    if ( sz_from_fld_id >= 0 ) {
      sz_from_fld_meta = &(from_fld[sz_from_fld_id]);
      status = add_aux_fld(dst_tbl, from_fld_meta->name,
	  sz_from_fld_meta->filename, "sz", &sz_to_fld_id);
      cBYE(status);
    }
  }
BYE:
  teardown(from_tbl_X, from_tbl_nX, &from_tbl, &from_n_tbl, 
	from_ht_tbl_X, from_ht_tbl_nX, &from_ht_tbl, &from_n_ht_tbl, 
	from_fld_X, from_fld_nX, &from_fld, &from_n_fld, 
	from_ht_fld_X, from_ht_fld_nX, &from_ht_fld, &from_n_ht_fld);
  teardown(to_tbl_X, to_tbl_nX, &to_tbl, &to_n_tbl, 
	to_ht_tbl_X, to_ht_tbl_nX, &to_ht_tbl, &to_n_ht_tbl, 
	to_fld_X, to_fld_nX, &to_fld, &to_n_fld, 
	to_ht_fld_X, to_ht_fld_nX, &to_ht_fld, &to_n_ht_fld);
  free_if_non_null(from_fld_ids);
  return(status);
}
