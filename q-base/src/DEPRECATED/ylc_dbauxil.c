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
#include "f1f2opf3.h"
#include "is_fld.h"
#include "ext_f_to_s.h"
#include "aux_meta.h"
#include "del_fld.h"
#include "tbl_meta.h"
#include "fld_meta.h"

// START FUNC DECL
int
set_fld_id_cnt(
		   int list_id, 
		   int pos, 
		   char *mode, 
		   int fld_id, 
		   char *fld,
		   long long cnt
		   )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024);
  int rc; char *zErrMsg = NULL;

  if ( mode == NULL ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  chk_mode(mode);
  sprintf(command, "update flds_x_list set %sfld_id = %d, %sfld = '%s', %sfld_cnt = %lld where list_id = %d and position = %d ", 
	mode, fld_id, mode, fld, mode, cnt, list_id, pos);
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ) { handle_sqlite_error(); }
 BYE:
  return(status);
}

// START FUNC DECL
int
get_fld_cnt(
	    int list_id, 
	    int position, 
	    char *mode,
	    long long *ptr_cnt
	    )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024);

  chk_mode(mode);
  sprintf(command, "select %sfld_cnt from  flds_x_list where list_id = %d and position = %d ", mode, list_id, position);
  status = db_get_lval(g_db, command, ptr_cnt); cBYE(status);
 BYE:
  return(status);
}

// START FUNC DECL
int
get_fld_id(
	   int list_id, 
	   int position, 
	   char *mode,
	   int *ptr_fld_id
	   )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024);

  chk_mode(mode);
  sprintf(command, "select %sfld_id from  flds_x_list where list_id = %d and position = %d ", mode, list_id, position);
  status = db_get_ival(g_db, command, ptr_fld_id); cBYE(status);
 BYE:
  return(status);
}

// START FUNC DECL
int
get_fld_name(
	     int list_id, 
	     int position, 
	     char *mode,
	     char **ptr_fld
	     )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024);

  chk_mode(mode);
  sprintf(command, "select %sfld from  flds_x_list where list_id = %d and position = %d ", mode, list_id, position);
  status = db_get_sval(g_db, command, ptr_fld); cBYE(status);
  if ( strcmp(*ptr_fld, "(null)") == 0 ) {
    free_if_non_null(*ptr_fld);
  }
 BYE:
  return(status);
}

// START FUNC DECL
int
get_sum_fld_cnt(
		    int list_id,
		    int num_in_list,
		    char *mode,
		    long long *ptr_sum_cnt
		    )
// STOP FUNC DECL
{
  int status = 0;
  long long cnt;

  *ptr_sum_cnt = 0;
  for ( int pos = 1; pos <= num_in_list; pos++ ) {
    status = get_fld_cnt(list_id, pos, mode, &cnt); cBYE(status);
    *ptr_sum_cnt += cnt;
  }
 BYE:
  return(status);
}
//
// START FUNC DECL
int
is_fld_in_list(
		   int list_id,
		   int fld_id,
		   bool *ptr_b_is_fld_in_list
		   )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024]; int n;

  zero_string(command, 1024);
  if ( list_id <= 0 ) { go_BYE(-1); }
  if ( fld_id  <  0 ) { go_BYE(-1); }
  sprintf(command, "select count(*) from flds_x_list where list_id = %d and fld_id = %d \n", list_id, fld_id);
  status = db_get_num_rslts(g_db, command, &n);
  if ( n == 0 ) { 
    *ptr_b_is_fld_in_list = false; 
  } 
  else { 
    *ptr_b_is_fld_in_list = true; 
  } 
 BYE:
  return(status);
}

// START FUNC DECL
int
is_list(
	    char *list, 
	    bool *ptr_b_is_list
	    )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024]; int n;

  *ptr_b_is_list = false;
  zero_string(command, 1024);
  if ( ( list == NULL ) || ( *list == '\0' ) )  { go_BYE(-1); }
  sprintf(command, "select count(*) from list where name = '%s'\n", list);
  status = db_get_num_rslts(g_db, command, &n);
  if ( n == 0 ) { *ptr_b_is_list = false; } else { *ptr_b_is_list = true; } 
 BYE:
  return(status);
}

// START FUNC DECL
int
get_list_id(
	    char *list,
	    int *ptr_list_id,
	    int *ptr_tbl_id,
	    int *ptr_xlist_id
	    )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_list = false;
  char command[1024];

  if ( ( list == NULL ) || ( *list == '\0' ) )  { go_BYE(-1); }

  *ptr_list_id  = -1; 
  *ptr_tbl_id   = -1; 
  *ptr_xlist_id = -1; 

  status = is_list(list, &b_is_list); cBYE(status);
  if ( b_is_list == false ) { go_BYE(-1); }

  zero_string(command, 1024);
  sprintf(command, "select id from list where name = '%s'\n", list);
  status = db_get_ival(g_db, command, ptr_list_id); cBYE(status);
  if ( *ptr_list_id <= 0 ) { /* Nothing more to do */
    goto BYE;
  }
  sprintf(command, "select tbl_id from list where id = %d\n", *ptr_list_id);
  status = db_get_ival(g_db, command, ptr_tbl_id); cBYE(status);
  sprintf(command, "select xlist_id from list where id = %d\n", *ptr_list_id);
  status = db_get_ival(g_db, command, ptr_xlist_id); cBYE(status);
  // Determine whether there really is an xlist for this list
  int num_rslts;
  zero_string(command, 1024); 
  sprintf(command, "select count(*) from list where xlist_id is not NULL and id = %d ", *ptr_list_id);
  status = db_get_num_rslts(g_db, command, &num_rslts); cBYE(status);
  if ( num_rslts == 0 ) {
    *ptr_xlist_id = -1;
  }

 BYE:
  return(status);
}
// -------------------------------------------------------------------
// START FUNC DECL
int
get_fld_in_list(
		int list_id,
		char *mode,
		int position,
		int *ptr_fld_id
		)
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];

  *ptr_fld_id = -1; 

  zero_string(command, 1024);
  sprintf(command, "select %sfld_id from flds_x_list where list_id = %d and position = %d \n", mode, list_id, position);
  status = db_get_ival(g_db, command, ptr_fld_id); cBYE(status);

 BYE:
  return(status);
}
// -------------------------------------------------------------------
// START FUNC DECL
int
get_num_in_list(
		int list_id,
		int *ptr_n
		)
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024);
  if ( list_id <= 0 ) { go_BYE(-1); }
  *ptr_n = -1;
  sprintf(command, "select count(*) from flds_x_list where list_id = %d\n", list_id);
  status = db_get_num_rslts(g_db, command, ptr_n); cBYE(status);
  if ( *ptr_n < 0 ) { go_BYE(-1); }
 BYE:
  return(status);
}

// START FUNC DECL
int
is_excl_fld_for_list(
		     char *xfld, 
		     int xlist_id, 
		     bool *ptr_brslt
		     )
// STOP FUNC DECL
{
  int status = 0;
  int num_in_xlist; 
  char command[1024]; 
  zero_string(command, 1024); 
  sprintf(command, "select count(*) from flds_x_xlist where fld = '%s' and xlist_id = %d", xfld, xlist_id); 
  status = db_get_num_rslts(g_db, command, &num_in_xlist); cBYE(status);
  switch ( num_in_xlist ) {
  case 0 : *ptr_brslt = false; break; 
  case 1 : *ptr_brslt = true; break; 
  default: go_BYE(-1); break;
  }
 BYE:
  return(status);
}

// START FUNC DECL
int
del_xcl_fld_from_xlist(
		       char *xfld, 
		       int xlist_id
		       )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];  char *zErrMsg = NULL; 
  zero_string(command, 1024); 
  if ( ( xfld == NULL ) || ( *xfld == '\0' ) ) { 
    sprintf(command, "delete from flds_x_xlist where xlist_id = %d", xlist_id);
  }
  else {
    sprintf(command, "delete from flds_x_xlist where fld = '%s' and xlist_id = %d", xfld, xlist_id);
  }
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  // TODO P3 free_if_non_null(zErrMsg);
  return(status);
}

// START FUNC DECL
int
num_exclusions(
	       int xlist_id, 
	       int *ptr_num_in_xlist
	       )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024); 
  sprintf(command, "select count(*) from flds_x_xlist where xlist_id = %d", xlist_id); 
  status = db_get_num_rslts(g_db, command, ptr_num_in_xlist); cBYE(status);
 BYE:
  return(status);
}

// START FUNC DECL
int
set_xlist_info(
		   int xlist_id,
		   char *cum_fld, 
		   int cum_fld_id
		   )
// STOP FUNC DECL
{
  int status = 0;

  if ( ( cum_fld == NULL ) || ( *cum_fld == '\0' ) ) { go_BYE(-1); }
  if ( strlen(cum_fld) > MAX_LEN_FLD_NAME ) { go_BYE(-1); }
  if ( cum_fld_id < 0 ) { go_BYE(-1); }
  if ( xlist_id < 0 ) { go_BYE(-1); }

  char command[1024]; char *zErrMsg = NULL;
  zero_string(command, 1024); 
  sprintf(command, "update xlist set cum_fld = '%s', cum_fld_id = %d where id = %d", cum_fld, cum_fld_id, xlist_id);
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  // TODO P3 free_if_non_null(zErrMsg);
  return(status);
}

// START FUNC DECL
int
get_xlist_info(
		   int xlist_id,
		   char **ptr_fld, 
		   int *ptr_fld_id
		   )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024]; 
  if ( xlist_id < 0 ) { go_BYE(-1); }

  zero_string(command, 1024); 
  sprintf(command, "select cum_fld_id from xlist where id = %d", xlist_id);
  status = db_get_ival(g_db, command, ptr_fld_id);

  zero_string(command, 1024); 
  sprintf(command, "select cum_fld from xlist where id = %d", xlist_id);
  status = db_get_sval(g_db, command, ptr_fld);

 BYE:
  // TODO P3 free_if_non_null(zErrMsg);
  return(status);
}

// START FUNC DECL
int
del_xlist(
    int list_id,
	  int xlist_id
	  )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024]; char *zErrMsg = NULL;

  zero_string(command, 1024); 
  sprintf(command, "update list set xlist_id = NULL where id = %d ", list_id);
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }

  zero_string(command, 1024); 
  sprintf(command, "delete from xlist where id = %d ", xlist_id);
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }

BYE:
  // TODO P3 free_if_non_null(zErrMsg);

  return(status);
}

// START FUNC DECL
int
del_aux_fld_if_exists(
		      int tbl_id,
		      int list_id, 
		      int pos, 
		      char *mode
		      )
// STOP FUNC DECL
{
  int status = 0;

  // Following is equivalent to deleting the to_fld if it exists */
  long long from_cnt; int from_fld_id; char *from_fld = NULL;
  char *base_fld = NULL;
  char *new_fld = NULL;
  char *cum_fld = NULL;
  char *xcl_fld = NULL;
  char *lmt_fld = NULL;

  if ( ( mode == NULL ) || ( *mode == '\0' ) ) { go_BYE(-1); }

  status = get_fld_name(list_id, pos, "", &base_fld); cBYE(status);
  status = get_fld_name(list_id, pos, "cum_", &cum_fld); cBYE(status);
  status = get_fld_name(list_id, pos, "new_", &new_fld); cBYE(status);
  status = get_fld_name(list_id, pos, "xcl_", &xcl_fld); cBYE(status);
  status = get_fld_name(list_id, pos, "lmt_", &lmt_fld); cBYE(status);

  if ( cum_fld == NULL ) { go_BYE(-1); }
  if ( new_fld == NULL ) { go_BYE(-1); }
  if ( xcl_fld == NULL ) { go_BYE(-1); }
  if ( lmt_fld == NULL ) { go_BYE(-1); }

  if ( ( strcmp(mode, "lmt_") == 0 ) || ( strcmp(mode, "xcl_") == 0 ) || 
       ( strcmp(mode, "new_") == 0 ) ) { 
    if ( strcmp(lmt_fld, xcl_fld) != 0 ) { 
      fprintf(stderr, "Deleting %s \n", lmt_fld);
      status = del_fld(NULL, tbl_id, lmt_fld, -1, true); cBYE(status);
    }
    if ( strcmp(mode, "lmt_") == 0 ) { 
      status = get_fld_id(  list_id, pos, "xcl_", &from_fld_id); cBYE(status);
      status = get_fld_cnt( list_id, pos, "xcl_", &from_cnt); cBYE(status);
      status = get_fld_name(list_id, pos, "xcl_", &from_fld); cBYE(status);
    }
  }
  if ( ( strcmp(mode, "xcl_") == 0 ) || ( strcmp(mode, "new_") == 0 ) ) { 
    if ( strcmp(xcl_fld, new_fld) != 0 ) { 
      fprintf(stderr, "Deleting %s \n", xcl_fld);
      status = del_fld(NULL, tbl_id, xcl_fld, -1, true); cBYE(status);
    }
    if ( strcmp(mode, "xcl_") == 0 ) { 
    status = get_fld_id(  list_id, pos, "new_", &from_fld_id); cBYE(status);
    status = get_fld_cnt( list_id, pos, "new_", &from_cnt); cBYE(status);
    status = get_fld_name(list_id, pos, "new_", &from_fld); cBYE(status);
    }
  }
  if ( strcmp(mode, "new_") == 0 ) { 
    if ( strcmp(new_fld, base_fld) != 0 ) { 
      fprintf(stderr, "Deleting %s \n", new_fld);
      status = del_fld(NULL, tbl_id, new_fld, -1, true); cBYE(status);
    }
    if ( strcmp(cum_fld, base_fld) != 0 ) { 
      fprintf(stderr, "Deleting %s \n", cum_fld);
      status = del_fld(NULL, tbl_id, cum_fld, -1, true); cBYE(status);
    }
    if ( strcmp(mode, "new_") == 0 ) { 
    status = get_fld_id(  list_id, pos, "", &from_fld_id); cBYE(status);
    status = get_fld_cnt( list_id, pos, "", &from_cnt); cBYE(status);
    status = get_fld_name(list_id, pos, "", &from_fld); cBYE(status);
    }
  }

  if ( ( strcmp(mode, "lmt_") == 0 ) || ( strcmp(mode, "xcl_") == 0 ) || 
       ( strcmp(mode, "new_") == 0 ) ) { 
    status = set_fld_id_cnt(list_id, pos, "lmt_", 
	  from_fld_id, from_fld, from_cnt); cBYE(status);
  }
  if ( ( strcmp(mode, "xcl_") == 0 ) || ( strcmp(mode, "new_") == 0 ) ) { 
    status = set_fld_id_cnt(list_id, pos, "xcl_", 
	  from_fld_id, from_fld, from_cnt); cBYE(status);
  }
  if ( strcmp(mode, "new_") == 0 ) { 
    status = set_fld_id_cnt(list_id, pos, "new_", 
	  from_fld_id, from_fld, from_cnt); cBYE(status);
    status = set_fld_id_cnt(list_id, pos, "cum_", 
	  from_fld_id, from_fld, from_cnt); cBYE(status);
  }

BYE:
  free_if_non_null(from_fld);
  free_if_non_null(lmt_fld);
  free_if_non_null(xcl_fld);
  free_if_non_null(new_fld);
  free_if_non_null(cum_fld);
  free_if_non_null(base_fld);
  return(status);
}


// START FUNC DECL
int
and_not_excl(
    char *list
    )
// STOP FUNC DECL
{
  int status = 0;
  char xcl_fld[MAX_LEN_FLD_NAME+1];
  char *new_fld = NULL, *cum_xcl_fld = NULL;
  int num_in_list;
  int tbl_id, list_id, xlist_id, cum_xcl_fld_id;
  TBL_REC_TYPE tbl_rec;

  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( xlist_id <= 0 ) { go_BYE(-1); }
  status = get_num_in_list(list_id, &num_in_list); cBYE(status);
  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);
  char *tbl = tbl_rec.name;
  if ( num_in_list == 0 ) { return(status); } // Nothing to do 
  status = get_xlist_info(xlist_id, &cum_xcl_fld, &cum_xcl_fld_id);
  cBYE(status);

  for ( int pos = 1; pos <= num_in_list; pos++ ) { 
    int xcl_fld_id = -1; long long xcl_fld_cnt = -1 ;
    int new_fld_id = -1; long long new_fld_cnt = -1 ;
    status = get_fld_name(list_id, pos, "new_", &new_fld); cBYE(status);
    status = fld_meta(tbl, new_fld, "", &new_fld_id, false); cBYE(status);
    status = del_aux_fld_if_exists(tbl_id, list_id, pos, "xcl_"); cBYE(status);
    status = mk_name_new_fld(list, tbl, new_fld, "xcl_", xcl_fld); cBYE(status);
    status = f1f2opf3(tbl, new_fld, cum_xcl_fld, "&&!", xcl_fld); cBYE(status);
    status = count_ones(tbl, xcl_fld, &xcl_fld_cnt); cBYE(status);
    status = fld_meta(tbl, xcl_fld, "", &xcl_fld_id, false); cBYE(status);
    status = count_ones(tbl, new_fld, &new_fld_cnt); cBYE(status);
    // update meta data 
    if ( new_fld_cnt == xcl_fld_cnt ) { 
      status = set_fld_id_cnt(list_id, pos, "xcl_", new_fld_id, new_fld, new_fld_cnt); 
      cBYE(status);
      status = set_fld_id_cnt(list_id, pos, "lmt_", new_fld_id, new_fld, new_fld_cnt); 
      cBYE(status);
      status = del_fld(NULL, tbl_id, NULL, xcl_fld_id, true); cBYE(status);
    }
    else {
    status = set_fld_id_cnt(list_id, pos, "xcl_", xcl_fld_id, xcl_fld, xcl_fld_cnt); 
    cBYE(status);
    status = set_fld_id_cnt(list_id, pos, "lmt_", xcl_fld_id, xcl_fld, xcl_fld_cnt); 
    cBYE(status);
    }
    free_if_non_null(new_fld); 
  }
BYE:
  free_if_non_null(new_fld); 
  return(status);
}

// START FUNC DECL
int
add_to_xlist(
		  char *list,
		  int list_id,
		  char *cum_fld, 
		  int cum_fld_id
		  )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024]; char *zErrMsg = NULL;
  zero_string(command, 1024); 

  if ( list_id <= 0 ) { 
    int tbl_id, xfld_id; 
    status = get_list_id(list, &list_id, &tbl_id, &xfld_id); cBYE(status);
  }
  sprintf(command, "insert into xlist (cum_fld, cum_fld_id, list, list_id) values ('%s', %d, '%s', %d)", cum_fld, cum_fld_id, list, list_id);
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  return(status);
}

// START FUNC DECL
int
update_xlist(
		  char *cum_fld, 
		  char *list,
		  int cum_fld_id
		  )
// STOP FUNC DECL
{
  int status = 0;
  char command[1024]; char *zErrMsg = NULL;
  zero_string(command, 1024); 

  sprintf(command, "update xlist set cum_fld = '%s', cum_fld_id = %d where list = '%s' ", cum_fld, cum_fld_id, list);
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  return(status);
}

// START FUNC DECL
int
del_all_lmt_flds(
    char *list
    )
// STOP FUNC DECL
{
  int status = 0;
  int num_in_list,list_id, tbl_id, xlist_id;
  // OLD char *new_fld = NULL; int new_fld_id; long long new_fld_cnt;
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  status = get_num_in_list(list_id, &num_in_list); cBYE(status);
  for ( int pos = num_in_list; pos > 0; pos-- ) { 
    status = del_aux_fld_if_exists(tbl_id, list_id, pos, "lmt_"); cBYE(status);
    /* OLD
    status = del_aux_fld_if_exists(tbl_id, list_id, pos, "new_", "xcl_"); 
    cBYE(status);
    status = get_fld_name(list_id, pos, "new_", &new_fld); cBYE(status);
    status = get_fld_id  (list_id, pos, "new_", &new_fld_id); cBYE(status);
    status = get_fld_cnt (list_id, pos, "new_", &new_fld_cnt); cBYE(status);
    status = set_fld_id_cnt(list_id, pos, "xcl_", 
	new_fld_id, new_fld, new_fld_cnt); cBYE(status);
    status = set_fld_id_cnt(list_id, pos, "lmt_", 
	new_fld_id, new_fld, new_fld_cnt); cBYE(status);
    free_if_non_null(new_fld);
    */
  }
BYE:
  // OLD free_if_non_null(new_fld);
  return(status);
}
// START FUNC DECL
int
is_fld_in_xlist(
    char *xfld, 
    int xlist_id, 
    bool *ptr_b_exists
    )
// STOP FUNC DECL
{
  int status = 0;
  int itemp; char command[1024]; zero_string(command, 1024); 
  sprintf(command, "select count(*) from flds_x_xlist where fld = '%s' and xlist_id = %d ", xfld, xlist_id);
  status = db_get_ival(g_db, command, &itemp); cBYE(status);
  if ( itemp > 1 ) { go_BYE(-1); }
  if ( itemp == 1 ) { *ptr_b_exists = true; } else { *ptr_b_exists = false; }
BYE:
  return(status);
}
