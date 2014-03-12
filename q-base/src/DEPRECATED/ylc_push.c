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
#include "f1f2opf3.h"
#include "is_fld.h"
#include "ext_f_to_s.h"
#include "is_fld.h"
#include "tbl_meta.h"
#include "fld_meta.h"
#include "del_fld.h"

int ylc_push(
	     char *list,
	     char  *fld
	     )
{
  int status = 0;
  int rc,  list_id = -1, xlist_id = -1, num_in_list = -9999; 
  char *zErrMsg = NULL;
  long long fld_cnt = -1, new_fld_cnt = -1, lmt_fld_cnt = -1, xcl_fld_cnt = -1, cum_fld_cnt = -1; 
  TBL_REC_TYPE tbl_rec;     int tbl_id     = -1;
  FLD_REC_TYPE fld_rec;     int fld_id     = -1;
  FLD_REC_TYPE new_fld_rec; int new_fld_id = -1;
  FLD_REC_TYPE xcl_fld_rec; int xcl_fld_id = -1;
  FLD_REC_TYPE cum_fld_rec; int cum_fld_id = -1;
  FLD_REC_TYPE prev_cum_fld_rec; int prev_cum_fld_id = -1;
  // FLD_REC_TYPE cum_xcl_fld_rec;  
  int cum_xcl_fld_id = -1;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id = -1;
  int lmt_fld_id;
  char new_fld[MAX_LEN_FLD_NAME+1];
  char lmt_fld[MAX_LEN_FLD_NAME+1];
  char xcl_fld[MAX_LEN_FLD_NAME+1];
  char cum_fld[MAX_LEN_FLD_NAME+1];
  char *tbl = NULL;
  char *prev_cum_fld = NULL; // long long prev_cum_fld_cnt; 
  char *cum_xcl_fld = NULL; 

  zero_string(new_fld, MAX_LEN_FLD_NAME+1); 
  zero_string(lmt_fld, MAX_LEN_FLD_NAME+1); 
  zero_string(xcl_fld, MAX_LEN_FLD_NAME+1); 
  zero_string(cum_fld, MAX_LEN_FLD_NAME+1); 
  if ( ( list == NULL ) || ( *list == '\0' ) ) {  go_BYE(-1); }
  if ( ( fld  == NULL ) || ( *fld  == '\0' ) ) {  go_BYE(-1); }

  /* Determine list_id and tbl_id */
  bool b_is_list, b_is_fld_in_list;
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }

  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status); 
  if ( list_id < 0 ) { go_BYE(-1); }

  status = get_num_in_list(list_id, &num_in_list); cBYE(status); 
  if ( num_in_list < 0 ) { go_BYE(-1); }
  if ( num_in_list == MAX_NUM_IN_LIST ) { go_BYE(-1); }

  /* Is this field in the table of this list? */
  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);
  tbl = tbl_rec.name;
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  cBYE(status);
  if ( fld_id < 0 ) { fprintf(stderr, "No field [%s] in Table [%s]\n", 
			      fld, tbl); go_BYE(-1); }
  if ( fld_rec.fldtype != B ) { 
    fprintf(stderr, "Field [%s] in Table [%s] has wrong type\n", fld, tbl);
    go_BYE(-1); 
  }
  /* Is this field already a part of the list */
  status = is_fld_in_list(list_id, fld_id, &b_is_fld_in_list);
  if ( b_is_fld_in_list ) {
    fprintf(stderr, "Field [%d] already in List [%s] \n", fld_id, list); 
    go_BYE(-1);
  }
  status = count_ones(tbl, fld, &fld_cnt); cBYE(status);

  /* Is it the first field we are adding to the list */
  if ( num_in_list == 0 ) {
    new_fld_id = fld_id;
    strcpy(new_fld, fld);
    new_fld_cnt = fld_cnt;

    cum_fld_id = fld_id;
    strcpy(cum_fld, fld);
    cum_fld_cnt = fld_cnt;

    xcl_fld_id = new_fld_id;
    strcpy(xcl_fld, new_fld);
    xcl_fld_cnt = new_fld_cnt;
  }
  else {
    status = get_fld_in_list(list_id, "cum_", num_in_list, &prev_cum_fld_id); 
    cBYE(status);
    status = get_fld_meta(prev_cum_fld_id, &prev_cum_fld_rec); cBYE(status);
    char *prev_cum_fld = prev_cum_fld_rec.name;
    status = mk_name_new_fld(list, tbl, fld, "n_", new_fld); cBYE(status);
    status = mk_name_new_fld(list, tbl, fld, "c_", cum_fld); cBYE(status);
    status = f1f2opf3(tbl, fld, prev_cum_fld, "&&!", new_fld); cBYE(status);
    status = f1f2opf3(tbl, fld, prev_cum_fld, "||",  cum_fld); cBYE(status);
    status = count_ones(tbl, new_fld, &new_fld_cnt);
    status = count_ones(tbl, cum_fld, &cum_fld_cnt);
    /* Check if we really need to keep new_fld as a separate field */
    if ( new_fld_cnt == fld_cnt ) { 
      /* new_fld is same as old fld. So no need to keep a copy */
      status = del_fld(NULL, tbl_id, new_fld, -1, true); cBYE(status);
      new_fld_id  = fld_id;
      strcpy(new_fld, fld);
      new_fld_cnt = fld_cnt;
    }
    else {
      status = is_fld(NULL, tbl_id, new_fld, &new_fld_id, &new_fld_rec, 
		    &nn_fld_id, &nn_fld_rec); cBYE(status);
    }
    /* Check if we really need to keep cum_fld as a separate field */
    if ( cum_fld_cnt == fld_cnt ) { 
      /* cum_fld is same as old fld. So no need to keep a copy */
      status = del_fld(NULL, tbl_id, cum_fld, -1, true); cBYE(status);
      cum_fld_id  = fld_id;
      strcpy(cum_fld, fld);
      cum_fld_cnt = fld_cnt;
    }
    else {
      status = is_fld(NULL, tbl_id, cum_fld, &cum_fld_id, &cum_fld_rec, 
		    &nn_fld_id, &nn_fld_rec); cBYE(status);
    }
    /*----------------------------------------------------------------*/
    // Do we have exclusion lists we need to deal with?
    if ( xlist_id > 0 ) {
      status = get_xlist_info(xlist_id, &cum_xcl_fld, &cum_xcl_fld_id);
      cBYE(status);
      status = mk_name_new_fld(list, tbl, fld, "x_", xcl_fld);
      status = f1f2opf3(tbl, new_fld, cum_xcl_fld, "&&!", xcl_fld); cBYE(status);
      status = count_ones(tbl, xcl_fld, &xcl_fld_cnt); cBYE(status);
      if ( xcl_fld_cnt == new_fld_cnt ) { 
        status = del_fld(NULL, tbl_id, xcl_fld, -1, true); cBYE(status);
        xcl_fld_id = new_fld_id;
        strcpy(xcl_fld, new_fld);
        xcl_fld_cnt = new_fld_cnt;
      }
      else {
        status = is_fld(NULL, tbl_id, xcl_fld, &xcl_fld_id, &xcl_fld_rec, 
		    &nn_fld_id, &nn_fld_rec); cBYE(status);
      }
    }
    else {
      xcl_fld_id = new_fld_id;
      strcpy(xcl_fld, new_fld);
      xcl_fld_cnt = new_fld_cnt;
    }
  }
  lmt_fld_id = xcl_fld_id;
  strcpy(lmt_fld, xcl_fld);
  lmt_fld_cnt = xcl_fld_cnt;

  if ( strlen(xcl_fld) == 0 ) { go_BYE(-1); } 
  if ( strlen(cum_fld) == 0 ) { go_BYE(-1); } 
  if ( strlen(new_fld) == 0 ) { go_BYE(-1); } 
  if ( strlen(lmt_fld) == 0 ) { go_BYE(-1); } 


  int position = num_in_list + 1;
  char command[1024]; zero_string(command, 1024);
  char *flds = "new_fld, new_fld_id, new_fld_cnt, fld, fld_id, fld_cnt, cum_fld, cum_fld_id, cum_fld_cnt, lmt_fld, lmt_fld_id, lmt_fld_cnt, xcl_fld, xcl_fld_id, xcl_fld_cnt, list_id, position";
  sprintf(command, "insert into flds_x_list (%s) values ('%s', %d, %lld, '%s', %d, %lld, '%s', %d, %lld, '%s', %d, %lld, '%s', %d, %lld, %d, %d) \n", 
	  flds, new_fld, new_fld_id, new_fld_cnt, fld, fld_id, fld_cnt, cum_fld, cum_fld_id, cum_fld_cnt, lmt_fld, lmt_fld_id, lmt_fld_cnt, xcl_fld, xcl_fld_id, xcl_fld_cnt, list_id, position);

  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }

 BYE:
  free_if_non_null(prev_cum_fld);
  free_if_non_null(cum_xcl_fld);
  // TODO P3 sqlite3_free(zErrMsg);
  return(status);
}

