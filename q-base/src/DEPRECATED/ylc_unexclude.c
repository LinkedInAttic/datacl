/* Yoni List Creator */
#include <sqlite3.h>
#include "constants.h"
#include "macros.h"
#include "md5global.h"
#include "mddriver.h"
#include "qtypes.h"
#include "auxil.h"
#include "ylc_auxil.h"
#include "ylc_dbauxil.h"
#include "ylc_globals.h"
#include "mmap.h"
#include "del_fld.h"
#include "dup_fld.h"
#include "is_fld.h"
#include "tbl_meta.h"
#include "f1f2opf3.h"
// START FUNC DECL
int ylc_unexclude(
		  char *list,
		  char *xfld /* field to un-exclude */
		  )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_list;
  char *cum_xcl_fld = NULL, *xcl_fld = NULL, *new_fld = NULL; 
  int cum_xcl_fld_id; FLD_REC_TYPE cum_xcl_fld_rec, xcl_fld_rec;
  int nn_fld_id;  FLD_REC_TYPE nn_fld_rec;
  char **xcl_flds = NULL; int n_xcl_flds; char where_clause[64];

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  if ( strlen(list) >= MAX_LEN_LIST_NAME ) { go_BYE(-1); }

  status = is_list(list, &b_is_list); cBYE(status);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); goto BYE;
  }
  int list_id, tbl_id, xlist_id; TBL_REC_TYPE tbl_rec;
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( tbl_id  <  0 ) { go_BYE(-1); }
  if ( list_id <= 0 ) { go_BYE(-1); }

  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);
  char *tbl = tbl_rec.name;

  int num_in_list; 
  status = get_num_in_list(list_id, &num_in_list); cBYE(status);

  if ( ( xfld == NULL ) || ( *xfld == '\0' ) ) { /* Delete exclusions */
    if ( xlist_id <= 0 ) { 
      /* The exclusion lis is empty; Nothing to do */
    }
    else {
      int itemp;
      status = del_all_lmt_flds(list); cBYE(status); 
      /* Delete meta-data for all fields that contributed to this exclusion */
      status = del_xcl_fld_from_xlist(NULL, xlist_id); cBYE(status);
      /* Get name of cum_fld and delete it from Q storage */
      status = get_xlist_info(xlist_id, &cum_xcl_fld, &itemp); cBYE(status);
      status = del_fld(NULL, tbl_id, cum_xcl_fld, -1, true); cBYE(status);
      /*--------------------------------------------------*/
      /* Delete the exclusion list itself (update list and xlist) */
      status = del_xlist(list_id, xlist_id); cBYE(status);
      /* Delete xcl_fld entries for fields in this list */
      for ( int pos = 1; pos <= num_in_list; pos++ ) { 
	status = del_aux_fld_if_exists(tbl_id, list_id, pos, "xcl_");
	cBYE(status);
      }
    }
  }
  else {
    status = del_all_lmt_flds(list); cBYE(status); 
    bool brslt; int itemp; char command[1024]; 
    // If field is NOT in exclusion list, give warning and return 
    status = is_excl_fld_for_list(xfld, xlist_id, &brslt); cBYE(status);
    if ( brslt == false ) { 
      fprintf(stderr, "Field [%s] not in excl list of [%s]\n", xfld, list); 
      goto BYE;
    }
    // Delete input field (xfld) from exclusion list for this list 
    status = del_xcl_fld_from_xlist(xfld, xlist_id); cBYE(status);
    int num_in_xlist;
    status = num_exclusions(xlist_id, &num_in_xlist); cBYE(status);
    // Get name of current cum_xcl_fld 
    status = get_xlist_info(xlist_id, &cum_xcl_fld, &itemp); cBYE(status);
    // Delete cumulative exclusion field. We will re-create it
    status = del_fld(NULL, tbl_id, cum_xcl_fld, -1, true); cBYE(status);
    switch ( num_in_xlist ) { 
    case 0 : 
      /* Delete the exclusion list itself (update list and xlist) */
      status = del_xlist(list_id, xlist_id); cBYE(status);
      break;
    case 1 : 
      // A duplicate of the last xcl field becomes the cum_xcl_fld
      // Get name of last xcl field
      zero_string(command, 1024);
      sprintf(command, "select fld from flds_x_xlist where xlist_id = %d ", xlist_id);
      status = db_get_sval(g_db, command, &xcl_fld);
      status = dup_fld(tbl, xcl_fld, cum_xcl_fld);
      status = is_fld(NULL, tbl_id, cum_xcl_fld, &cum_xcl_fld_id, 
		      &cum_xcl_fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
      // Update meta data 
      status = set_xlist_info(xlist_id, cum_xcl_fld, cum_xcl_fld_id); 
      cBYE(status);
      break;
    default : 
      zero_string(where_clause, 64);
      sprintf(where_clause, " where xlist_id = %d ", xlist_id); 
      status = fetch_rows(g_db, "flds_x_xlist", "fld", where_clause, 
			  "", (void **)&xcl_flds, &n_xcl_flds); 
      status = f1f2opf3(tbl, xcl_flds[0], xcl_flds[1], "||", cum_xcl_fld);
      cBYE(status);
      for ( int i = 2; i < n_xcl_flds; i++ ) { 
        status = f1f2opf3(tbl, xcl_flds[i], cum_xcl_fld, "||", cum_xcl_fld);
        cBYE(status);
      }
      status = is_fld(NULL, tbl_id, cum_xcl_fld, &cum_xcl_fld_id, 
		      &cum_xcl_fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
      // Update meta data 
      status = set_xlist_info(xlist_id, cum_xcl_fld, cum_xcl_fld_id); 
      cBYE(status);
      break;
    }
    if ( num_in_xlist > 0 ) { 
      char xcl_fld[MAX_LEN_FLD_NAME+1];
      /* We need to update the xcl_ fields for all fields in the list */
      for ( int pos = 1; pos <= num_in_list; pos++ ) { 
        long long xcl_fld_cnt; int xcl_fld_id;
	long long new_fld_cnt; int new_fld_id;
	status = del_aux_fld_if_exists(tbl_id, list_id, pos, "xcl_"); cBYE(status);
        status = get_fld_name(list_id, pos, "new_", &new_fld); cBYE(status);
	status = mk_name_new_fld(list, tbl, new_fld, "xcl_", xcl_fld);
        status = get_fld_cnt(list_id, pos, "new_", &new_fld_cnt); cBYE(status);
        status = get_fld_id (list_id, pos, "new_", &new_fld_id); cBYE(status);
        status = f1f2opf3(tbl, new_fld, cum_xcl_fld, "&&!", xcl_fld); cBYE(status);
        status = is_fld(NULL, tbl_id, xcl_fld, &xcl_fld_id, &xcl_fld_rec, 
			&nn_fld_id, &nn_fld_rec); cBYE(status);
	status = count_ones(tbl, xcl_fld, &xcl_fld_cnt); cBYE(status);
        // Update meta data 
	if ( new_fld_cnt == xcl_fld_cnt ) { 
	  status = set_fld_id_cnt(list_id, pos, "xcl_", new_fld_id, 
				  new_fld, new_fld_cnt);
	  cBYE(status);
	  status = set_fld_id_cnt(list_id, pos, "lmt_", new_fld_id, 
				  new_fld, new_fld_cnt);
	  cBYE(status);
	  status = del_fld(NULL, tbl_id, xcl_fld, -1, true); cBYE(status);
	}
	else {
	  status = set_fld_id_cnt(list_id, pos, "xcl_", xcl_fld_id, 
				  xcl_fld, xcl_fld_cnt);
	  cBYE(status);
	  status = set_fld_id_cnt(list_id, pos, "lmt_", xcl_fld_id, 
				  xcl_fld, xcl_fld_cnt);
	  cBYE(status);
	}
	free_if_non_null(new_fld);
      }
    }
  }
  /*--------------------------------------------------*/
 BYE:
  free_if_non_null(cum_xcl_fld);
  free_if_non_null(xcl_fld);
  free_if_non_null(new_fld);
  return(status);
}
