/* Yoni List Creator */
#include <sqlite3.h>
#include "constants.h"
#include "macros.h"
#include "md5global.h"
#include "mddriver.h"
#include "qtypes.h"
#include "auxil.h"
#include "mmap.h"
#include "meta_data.h"
#include "aux_meta.h"
#include "fld_meta.h"
#include "tbl_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f1f2opf3.h"
#include "ext_f_to_s.h"
#include "dup_fld.h"
#include "del_fld.h"
#include "fop.h"

#include "meta_globals.h"

#define chk_mode(x) { if ( ( strcmp(x, "") != 0 ) && ( strcmp(x, "new_") != 0 ) && ( strcmp(x, "lmt_") != 0 ) && ( strcmp(x, "xcl_") != 0 ) ) { go_BYE(-1); } }

#define handle_sqlite_error() {				\
    fprintf(stderr, "SQLite error: %s\n", zErrMsg);	\
    sqlite3_free(zErrMsg);				\
    go_BYE(-1);						\
  }

#define MAX_LEN_LIST_NAME 64
#define MAX_NUM_IN_LIST   16
char *g_docroot;
char *g_data_dir;
char *g_sqlite_db;
char *g_sqlite_sql;
sqlite3 *g_db;
char g_cwd[MAX_LEN_DIR_NAME+1];

extern int g_alt_ddir_id; // For Q
// START FUNC DECL
int
callback_pr(
	    void *vptr,
	    int argc,
	    char **argv,
	    char **azColName
	    )
// STOP FUNC DECL
{
  if ( argc < 1 ) { WHEREAMI; return(-1); }
  /* Print column headers */
  /*
    for ( int i = 0; i < argc; i++ ) { 
    if ( i > 0 ) { fprintf(stdout,","); }
    fprintf(stdout,"%s", azColName[i]);
    }
    fprintf(stdout,"\n");
  */
  /*------------------------------------------------*/
  /* Print info */
  for ( int i = 0; i < argc; i++ ) { 
    if ( i > 0 ) { fprintf(stdout,","); }
    fprintf(stdout,"\"%s\"", argv[i]);
  }
  fprintf(stdout,"\n");
  return 0;
}

//---------------------------------------------------------------
// START FUNC DECL
int 
callback_sval(
	      void *vptr,
	      int argc, 
	      char **argv, 
	      char **azColName
	      )
// STOP FUNC DECL
{
  char *sval = NULL;
  int len;
  char **ptr_sval = NULL;

  ptr_sval = (char **)vptr;
  if ( argc > 0 ) { 
    if ( argv[0] == NULL ) { 
      sval = malloc(1);
      sval[0] = '\0';
    }
    else {
      len = strlen(argv[0]) + 1;
      sval = malloc(len * sizeof(char));
      for ( int i = 0; i < len; i++ ) { sval[i] = '\0'; }
      strcpy(sval, argv[0]);
    }
  }
  *ptr_sval = sval;
  return 0;
}
//---------------------------------------------------------------
//----------------------------------------------
// START FUNC DECL
int
callback_ival(
	      void *vptr,
	      int argc,
	      char **argv,
	      char **azColName
	      )
// STOP FUNC DECL
{
  char *endptr;
  int *ptr_ival = (int *)vptr;

  if ( argc != 1 ) { WHEREAMI; return(-1); }
  if ( argv[0] != NULL ) {
    *ptr_ival = strtol(argv[0], &endptr, 10);
  }
  return 0;
}

// START FUNC DECL
int
callback_lval(
	      void *vptr,
	      int argc,
	      char **argv,
	      char **azColName
	      )
// STOP FUNC DECL
{
  char *endptr;
  long long *ptr_lval = (long long  *)vptr;

  if ( argc != 1 ) { WHEREAMI; return(-1); }
  if ( argv[0] != NULL ) {
    *ptr_lval = strtoll(argv[0], &endptr, 10);
  }
  return 0;
}

// START FUNC DECL
int
callback_num_rslts(
		   void *vptr,
		   int argc,
		   char **argv,
		   char **azColName
		   )
// STOP FUNC DECL
{
  char *endptr;
  int *ptr_num_rslts = (int *)vptr;

  if ( argc == 0 ) {
    *ptr_num_rslts =  0;
  }
  else {
    *ptr_num_rslts = strtol(argv[0], &endptr, 10);
  }
  return 0;
}

// START FUNC DECL
int
db_get_sval(
	    sqlite3 *db,
	    char *qstr,
	    char **ptr_sval
	    )
// STOP FUNC DECL
{
  int status = 0;
  int rc; char *zErrMsg = NULL;
  char *sval = NULL;

  // fprintf(stderr, "DBG: query= %s \n", qstr);
  rc = sqlite3_exec(db, qstr, callback_sval, &sval, &zErrMsg);
  if( rc != SQLITE_OK ){ fprintf(stderr, "ERROR:[%s]\n", zErrMsg); go_BYE(-1); }
  // fprintf(stderr, "DBG: sval = %s \n", sval);
  *ptr_sval = sval;
 BYE:
  // TODO P3 free_if_non_null(zErrMsg);
  return(status);
}
// START FUNC DECL
int
db_get_ival(
	    sqlite3 *db,
	    char *qstr,
	    int *ptr_ival
	    )
// STOP FUNC DECL
{
  int status = 0;
  int rc; char *zErrMsg = NULL;
  // fprintf(stderr, "DBG: query= %s \n", qstr);
  rc = sqlite3_exec(db, qstr, callback_ival, ptr_ival, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    go_BYE(-1);
  }
  // fprintf(stderr, "DBG: ival = %d \n", *ptr_ival);
 BYE:
  free_if_non_null(zErrMsg);
  return(status);
}

// START FUNC DECL
int
db_get_lval(
	    sqlite3 *db,
	    char *qstr,
	    long long *ptr_lval
	    )
// STOP FUNC DECL
{
  int status = 0;
  int rc; char *zErrMsg = NULL;
  // fprintf(stderr, "DBG: query= %s \n", qstr);
  rc = sqlite3_exec(db, qstr, callback_lval, ptr_lval, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    go_BYE(-1);
  }
  // fprintf(stderr, "DBG: ival = %d \n", *ptr_ival);
 BYE:
  free_if_non_null(zErrMsg);
  return(status);
}

// START FUNC DECL
int
db_get_num_rslts(
		 sqlite3 *db,
		 char *qstr,
		 int *ptr_num_rslts
		 )
// STOP FUNC DECL
{
  int status = 0;
  int rc; char *zErrMsg = NULL;
  *ptr_num_rslts = -1;
  // fprintf(stderr, "DBG:A: query= %s \n", qstr);
  rc = sqlite3_exec(db, qstr, callback_num_rslts, ptr_num_rslts,
		    &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    // TODO P3 NOT WORKING sqlite3_free(zErrMsg);
    go_BYE(-1);
  }
  // fprintf(stderr, "DBG:A: num_rslts = %d \n", *ptr_num_rslts);

 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  return(status);
}

int set_fld_id_cnt(
		   int list_id, 
		   int pos, 
		   char *mode, 
		   int fld_id, 
		   char *fld,
		   long long cnt

		   )
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024);
  int rc; char *zErrMsg = NULL;

  chk_mode(mode);
  if ( cnt > 0 ) {
    sprintf(command, "update flds_x_list set %sfld_id = %d, %sfld = '%s', %sfld_cnt = %lld where list_id = %d and position = %d ", mode, fld_id, mode, fld, mode, cnt, list_id, pos);
  }
  else {
    sprintf(command, "update flds_x_list set %sfld_id = %d, %sfld = '%s' where list_id = %d and position = %d ", mode, fld_id, mode, fld, list_id, pos);
  }
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ) { handle_sqlite_error(); }
 BYE:
  return(status);
}

int
get_fld_cnt(
	    int list_id, 
	    int position, 
	    char *mode,
	    long long *ptr_cnt
	    )
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

int
get_fld_id(
	   int list_id, 
	   int position, 
	   char *mode,
	   int *ptr_fld_id
	   )
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

int
get_fld_name(
	     int list_id, 
	     int position, 
	     char *mode,
	     char **ptr_fld
	     )
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024);

  chk_mode(mode);
  sprintf(command, "select %sfld from  flds_x_list where list_id = %d and position = %d ", mode, list_id, position);
  status = db_get_sval(g_db, command, ptr_fld); cBYE(status);
 BYE:
  return(status);
}

int get_sum_fld_cnt(
		    int list_id,
		    int num_in_list,
		    char *mode,
		    long long *ptr_sum_cnt
		    )
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
int is_fld_in_list(
		   int list_id,
		   int fld_id,
		   bool *ptr_b_is_fld_in_list
		   )
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

int is_list(
	    char *list, 
	    bool *ptr_b_is_list
	    )
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

int ylc_list_lists(
	    )
{
  int status = 0;
  int rc; char *zErrMsg = NULL;
  char command[1024];
  zero_string(command, 1024);
  rc = sqlite3_exec(g_db, "select * from list", callback_pr, 0, &zErrMsg);
  if ( rc ) { 
    fprintf(stderr,"ERROR %s Command %s \n", zErrMsg, command);
    go_BYE(-1); 
  }
 BYE:
  free_if_non_null(zErrMsg);
  return(status);
} 

int
is_excl_fld_for_list(
		     char *xfld, 
		     int xlist_id, 
		     bool *ptr_brslt
		     )
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

int
del_excl_fld_from_list(
		       char *xfld, 
		       int xlist_id
		       )
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

int
num_exclusions(
	       int xlist_id, 
	       int *ptr_num_in_xlist
	       )
{
  int status = 0;
  char command[1024];
  zero_string(command, 1024); 
  sprintf(command, "select count(*) from flds_x_xlist where xlist_id = %d", xlist_id); 
  status = db_get_num_rslts(g_db, command, ptr_num_in_xlist); cBYE(status);
 BYE:
  return(status);
}

int set_xlist_info(
		   char *and_fld, 
		   long long and_fld_cnt, 
		   int and_fld_id, 
		   int xlist_id
		   )
{
  int status = 0;

  if ( ( and_fld == NULL ) || ( *and_fld == '\0' ) ) { go_BYE(-1); }
  if ( strlen(and_fld) > MAX_LEN_FLD_NAME ) { go_BYE(-1); }
  if ( and_fld_id < 0 ) { go_BYE(-1); }
  if ( and_fld_cnt < 0 ) { go_BYE(-1); }
  if ( xlist_id < 0 ) { go_BYE(-1); }

  char command[1024]; char *zErrMsg = NULL;
  zero_string(command, 1024); 
  sprintf(command, "update xlist set and_fld = '%s', and_fld_cnt = %lld, and_fld_id = %d where id = %d", and_fld, and_fld_cnt, and_fld_id, xlist_id);
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  // TODO P3 free_if_non_null(zErrMsg);
  return(status);
}

int
del_xlist(
	  int xlist_id
	  )
{
  int status = 0;
  char command[1024]; char *zErrMsg = NULL;

  zero_string(command, 1024); 
  sprintf(command, "delete from xlist where id = %d ", xlist_id);
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  return(status);
}

int 
mk_name_new_fld(
		char *list, 
		char *tbl, 
		char *fld, 
		char *prefix,
		char new_fld[MAX_LEN_FLD_NAME+1]
		)
{
  int status = 0;
  char md5buf[LEN_MD5_DIGEST+2];
  char buffer[1024];
  int pid;
  struct timeval Tps;
  struct timezone Tpf;
  long long t_sec = 0, t_usec = 0, curr_time = 0;

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }

  gettimeofday(&Tps, &Tpf); 
  t_sec  = (long long)Tps.tv_sec;
  t_usec = (long long)Tps.tv_usec;
  curr_time = t_sec * 1000000 + t_usec;
  pid = getpid();
  zero_string(md5buf, LEN_MD5_DIGEST+2);
  zero_string(buffer, 1024);
  sprintf(buffer, "%s_%s_%s_%lld_%d", list, tbl, fld, curr_time, pid);
  my_MDString(buffer, md5buf);
  if ( ( prefix != NULL ) && ( *prefix != '\0' ) ) { 
    int pre_len = strlen(prefix);
    if ( pre_len >= ( MAX_LEN_FLD_NAME - 4 ) ) { go_BYE(-1); }
    strcpy(new_fld, prefix);
    strncat(new_fld, md5buf, MAX_LEN_FLD_NAME-pre_len);
  }
  else {
    strncpy(new_fld, md5buf, MAX_LEN_FLD_NAME);
  }
 BYE:
  return(status);
}

int
del_aux_fld_if_exists(
		      int tbl_id,
		      int list_id, 
		      int pos, 
		      char *from,
		      char *to
		      )
{
  int status = 0;

  // Following is equivalent to deleting the to_fld if it exists */
  long long from_cnt; int from_fld_id;
  char *from_fld = NULL, *to_fld = NULL; 

  chk_mode(from); 
  chk_mode(to);
  if ( strcmp(from, to) == 0 ) { go_BYE(-1); }

  status = get_fld_id(  list_id, pos, from, &from_fld_id); cBYE(status);
  status = get_fld_cnt( list_id, pos, from, &from_cnt); cBYE(status);
  status = get_fld_name(list_id, pos, from, &from_fld); cBYE(status);
  status = get_fld_name(list_id, pos, to,   &to_fld); cBYE(status);

  status = del_fld(NULL, tbl_id, to_fld, -1, true); cBYE(status);
  status = set_fld_id_cnt(list_id, pos, "to_", from_fld_id, from_fld, from_cnt);
  cBYE(status);

  free_if_non_null(from_fld);
  free_if_non_null(to_fld);
BYE:
  return(status);
}


int
and_not_x(
    char *tbl,
    char *xfld, 
    char *list,
    int list_id

    )
{
  int status = 0;
  char command[1024]; char *zErrMsg = NULL;
  char xcl_fld[MAX_LEN_FLD_NAME+1], new_fld[MAX_LEN_FLD_NAME+1];
  char *fld1 = NULL, *fld2 = NULL;
  int num_in_list;

  status = get_num_in_list(list_id, &num_in_list); cBYE(status);
  for ( int pos = 1; pos <= num_in_list; pos++ ) { 

    status = get_fld_name(list_id, pos, "new_", &fld1); cBYE(status);
    strcpy(new_fld, fld1); free_if_non_null(fld1);

    status = get_fld_name(list_id, pos, "xcl_", &fld2); cBYE(status);
    strcpy(xcl_fld, fld2); free_if_non_null(fld2);

    if ( strcmp(new_fld, xcl_fld) == 0 ) {
      /* Make a new name for xcl_fld */
      status = mk_name_new_fld(list, tbl, xfld, "xcl_", xcl_fld); 
    }
    status = f1f2opf3(tbl, new_fld, xfld, "&&!", xcl_fld); cBYE(status);
    // Determine number of 1's in the xcl_fld 
    FLD_REC_TYPE xcl_fld_rec; int xcl_fld_id;
    FLD_REC_TYPE  nn_fld_rec; int  nn_fld_id;
    status = is_fld(tbl, -1, xcl_fld, &xcl_fld_id, &xcl_fld_rec, 
	&nn_fld_id, &nn_fld_rec); cBYE(status);
    if ( xcl_fld_rec.cnt < 0 ) {
      char buf[64]; int sz_buf = 64;
      status = ext_f_to_s(tbl, xcl_fld, "sum", buf, sz_buf); cBYE(status);
      status = get_fld_meta(xcl_fld_id, &xcl_fld_rec); cBYE(status);
    }
    // update meta data 
    zero_string(command, 1024); 
    sprintf(command, "update flds_x_list set xcl_fld = '%s', xcl_fld_id = %d, xcl_fld_cnt = %lld where list_id = %d and position = %d", 
	xcl_fld, xcl_fld_id, xcl_fld_rec.cnt, list_id, pos);
    int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if ( rc ) { handle_sqlite_error(); }
    //--------------------
    // Delete lmt_ field if it exists 
    //--------------------
    free_if_non_null(fld1); free_if_non_null(fld2);
  }
BYE:
  free_if_non_null(fld1); free_if_non_null(fld2);
  return(status);
}

int
add_xfld_to_xlist(
		  char *and_fld, 
		  long long and_fld_cnt,
		  int and_fld_id
		  )
{
  int status = 0;

  char command[1024]; char *zErrMsg = NULL;
  zero_string(command, 1024); 
  sprintf(command, "insert into xlist (and_fld, and_fld_id, and_fld_cnt) values ('%s', %d, %lld)", and_fld, and_fld_id, and_fld_cnt);
  int rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { handle_sqlite_error(); }
 BYE:
  return(status);
}
int ylc_add(
	    char *list,
	    char *tbl
	    )
{
  int status = 0;
  int rc; char *zErrMsg = NULL; bool b_is_list;
  char command[1024];

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  if ( ( tbl  == NULL ) || ( *tbl  == '\0' ) ) { go_BYE(-1); }
  if ( strlen(list) >= MAX_LEN_LIST_NAME ) { go_BYE(-1); }

  status = is_list(list, &b_is_list);
  if ( b_is_list ) { 
    fprintf(stderr, "List [%s] already exists \n", list); go_BYE(-1);
  }
  int tbl_id; TBL_REC_TYPE tbl_rec;
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) {
    fprintf(stderr, "Table [%s] does not exist \n", tbl); go_BYE(-1); 
  }

  zero_string(command, 1024); 
  sprintf(command, "insert into list (name, tbl, tbl_id) values ('%s', '%s', %d)", 
	  list, tbl, tbl_id);
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if ( rc ) { 
    fprintf(stderr,"ERROR=[%s] Command=[%s] \n", zErrMsg, command);
    go_BYE(-1); 
  }
 BYE:
  free_if_non_null(zErrMsg);
  return(status);
}

int ylc_unexclude(
		  char *list,
		  char *xfld /* field to un-exclude */
		  )
{
  int status = 0;
  int rc; char *zErrMsg = NULL; bool b_is_list, brslt;
  char command[1024]; 
  char *and_fld = NULL, *lst_fld = NULL; long long and_fld_cnt;  
  int and_fld_id; FLD_REC_TYPE and_fld_rec;
  int nn_fld_id;  FLD_REC_TYPE nn_fld_rec;

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  if ( ( xfld == NULL ) || ( *xfld == '\0' ) ) { go_BYE(-1); }
  if ( strlen(list) >= MAX_LEN_LIST_NAME ) { go_BYE(-1); }

  status = is_list(list, &b_is_list); cBYE(status);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }
  int list_id, tbl_id, xlist_id; TBL_REC_TYPE tbl_rec;
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( tbl_id  <  0 ) { go_BYE(-1); }
  if ( list_id <= 0 ) { go_BYE(-1); }
  if ( xlist_id <= 0 ) { go_BYE(-1); }

  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);

  // If field is NOT in exclusion list, give warning and return 
  status = is_excl_fld_for_list(xfld, xlist_id, &brslt); cBYE(status);
  if ( brslt == false ) { 
    fprintf(stderr, "Field [%s] not in exclusion list for [%s]\n", xfld, list); 
    goto BYE;
  }
  // Delete input field (xfld) from exclusion list for this list 
  status = del_excl_fld_from_list(xfld, xlist_id); cBYE(status);
  int num_in_xlist;
  status = num_exclusions(xlist_id, &num_in_xlist); cBYE(status);
  switch ( num_in_xlist ) { 
  case 0 : 
    sprintf(command, "update list set xlist_id = NULL where id = %d", list_id);
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if ( rc ) { handle_sqlite_error(); }
    /*--------------------------------------------------*/
    /* Delete the field in Q */
    sprintf(command, "select and_fld from xlist where id = %d ", xlist_id);
    status = db_get_sval(g_db, command, &and_fld); cBYE(status);
    status = del_fld(NULL, tbl_id, and_fld, -1, true); cBYE(status);
    /*--------------------------------------------------*/
    sprintf(command, "delete from xlist where id = %d", xlist_id);
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if ( rc ) { handle_sqlite_error(); }
    /*--------------------------------------------------*/
    break;
  case 1 : 
    // Get count for last exclusion field for this list 
    zero_string(command, 1024); 
    sprintf(command, "select fld_cnt from flds_x_xlist where xlist_id = %d ", xlist_id);
    status = db_get_lval(g_db, command, &and_fld_cnt);
    sprintf(command, "select fld from flds_x_xlist where xlist_id = %d ", xlist_id);
    status = db_get_sval(g_db, command, &lst_fld);
    // Get name of current and_fld 
    zero_string(command, 1024); 
    sprintf(command, "select and_fld from xlist where id = %d ", xlist_id);
    status = db_get_sval(g_db, command, &and_fld); cBYE(status);
    // To re-create the and_fld, we need to de-dupe
    status = del_fld(NULL, tbl_id, and_fld, -1, true); cBYE(status);
    status = dup_fld(tbl_rec.name, lst_fld, and_fld);
    status = is_fld(NULL, tbl_id, and_fld, &and_fld_id, &and_fld_rec, 
		    &nn_fld_id, &nn_fld_rec); cBYE(status);
    if ( and_fld_rec.cnt != and_fld_cnt ) { go_BYE(-1); }
    // Update meta data 
    status = set_xlist_info(and_fld, and_fld_rec.cnt, and_fld_id, xlist_id);
    cBYE(status);
    break;
  default : 
    /* Now we need to AND all the remaining  */
    break;
  }
  /*--------------------------------------------------*/
 BYE:
  free_if_non_null(and_fld);
  free_if_non_null(lst_fld);
  return(status);
}


int ylc_exclude(
		char *list,
		char *xfld /* field to exclude */
		)
{
  int status = 0;
  int rc; char *zErrMsg = NULL; bool b_is_list;
  char command[1024];
  char *fld1 = NULL, *fld2 = NULL;

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  if ( ( xfld == NULL ) || ( *xfld == '\0' ) ) { go_BYE(-1); }
  if ( strlen(list) >= MAX_LEN_LIST_NAME ) { go_BYE(-1); }

  status = is_list(list, &b_is_list); cBYE(status);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }
  int list_id, tbl_id, xlist_id; TBL_REC_TYPE tbl_rec;
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( tbl_id  <  0 ) { go_BYE(-1); }
  if ( list_id <= 0 ) { go_BYE(-1); }

  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);

  // If no exclusion list exists and you are asked to delete all, return
  if ( ( strcmp(xfld, "__NONE__") == 0 ) && ( xlist_id <= 0 ) ) {
    fprintf(stderr, "No exclusion list for [%s]. Nothing to do\n", list);
    goto BYE;
  }
  if ( strcmp(xfld, "__NONE__") == 0 ) {
    /*--- Indicate that this list no longer has an exclusion list */
    zero_string(command, 1024); 
    sprintf(command, "update list set xlist_id = NULL where id = %d ", list_id);
    rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
    if ( rc ) { handle_sqlite_error(); }
    /*--------------------------------------------------*/
    /* Delete meta-data for all fields that contributed to this * exclusion */
    del_excl_fld_from_list(NULL, xlist_id);
    /* Get name of and_fld and delete it from Q storage */
    int and_fld_id;
    zero_string(command, 1024); 
    sprintf(command, "select and_fld_id from xlist where id = %d ", xlist_id);
    status = db_get_ival(g_db, command, &and_fld_id); cBYE(status);
    status = del_fld(NULL, tbl_id, NULL, and_fld_id, true); cBYE(status);
    /*--------------------------------------------------*/
    /* Delete the exclusion list itself */
    status = del_xlist(xlist_id); cBYE(status);
    /* Delete xcl_fld entries for fields in this list */
    int num_in_list; 
    status = get_num_in_list(list_id, &num_in_list); cBYE(status);
    for ( int pos = 1; pos <= num_in_list; pos++ ) { 
      status = del_aux_fld_if_exists(tbl_id, list_id, pos, "new_", "lmt_");
      cBYE(status);
      status = del_aux_fld_if_exists(tbl_id, list_id, pos, "new_", "xcl_"); 
      cBYE(status);
    }
  }
  else {
    if ( xlist_id <= 0 ) { /* This is the first exclusion field for list */
      // Get field ID 
      int xfld_id, and_fld_id, nn_fld_id;
      FLD_REC_TYPE xfld_rec, and_fld_rec, nn_fld_rec;
      char and_fld[MAX_LEN_FLD_NAME+1];
      zero_string(and_fld, (MAX_LEN_FLD_NAME+1));
      status = is_fld(NULL, tbl_id, xfld, &xfld_id, &xfld_rec, 
		      &nn_fld_id, &nn_fld_rec);  cBYE(status);
      if ( xfld_rec.fldtype != B ) { go_BYE(-1); }
      // Get number of 1's in this field
      if ( xfld_rec.cnt < 0 ) {
	char buf[64]; int sz_buf = 64;
	status = ext_f_to_s(tbl_rec.name, xfld, "sum", buf, sz_buf); cBYE(status);
      }
      // Make a copy of this field 
      status = mk_name_new_fld(list, tbl_rec.name, xfld, "x_", and_fld); 
      cBYE(status);
      status = dup_fld(tbl_rec.name, xfld, and_fld);
      status = is_fld(NULL, tbl_id, and_fld, &and_fld_id, &and_fld_rec, 
		      &nn_fld_id, &nn_fld_rec);  cBYE(status);
      // Insert the copy of field into xlist
      status = add_xfld_to_xlist(and_fld, and_fld_rec.cnt, and_fld_id);
      cBYE(status);
      // Get ID of inserted row 
      xlist_id = sqlite3_last_insert_rowid(g_db);
      // Insert the original field into flds_x_xlist
      zero_string(command, 1024); 
      sprintf(command, "insert into flds_x_xlist (fld, fld_id, fld_cnt, xlist_id) values ('%s', %d, %lld, %d)", 
	      xfld, xfld_id, xfld_rec.cnt, xlist_id);
      rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
      if ( rc ) { handle_sqlite_error(); }
      /*--------------------------------------------------*/
      zero_string(command, 1024); 
      sprintf(command, "update list set xlist_id = %d where id = %d ", 
	      xlist_id, list_id);
      rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
      if ( rc ) { handle_sqlite_error(); }
      /*--------------------------------------------------*/
      /* For each existing field, we do "and not x" with new excl field */
      status = and_not_x(tbl_rec.name, xfld, list, xlist_id); cBYE(status);
    }
    else {
      /* Make sure that this field does not already occur */
      int itemp;
      zero_string(command, 1024); 
      sprintf(command, "select count(*) from flds_x_xlist where fld = '%s' and xlist_id = %d ", xfld, xlist_id);
      status = db_get_ival(g_db, command, &itemp); cBYE(status);
      if ( itemp > 1 ) { go_BYE(-1); }
      if ( itemp == 1 ) {
	fprintf(stderr, "Field [%s] exists in exclusions for List [%s]\n",
		xfld, list); 
	goto BYE;
      }
      /*--------------------------------------------------*/
      /* For each existing field, we do "and not x" with new excl field */
      status = and_not_x(tbl_rec.name, xfld, list, xlist_id); cBYE(status);
      // Get the fld_id and count for the new exclusion field 
      int xfld_id;  FLD_REC_TYPE xfld_rec;
      status = fld_meta(tbl_rec.name, xfld, &xfld_id, false); cBYE(status);
      status = get_fld_meta(xfld_id, &xfld_rec); cBYE(status);
      // Get number of 1's in this field
      if ( xfld_rec.cnt < 0 ) {
	char buf[64]; int sz_buf = 64;
	status = ext_f_to_s(tbl_rec.name, xfld, "sum", buf, sz_buf); cBYE(status);
        status = get_fld_meta(xfld_id, &xfld_rec); cBYE(status);
      }
      // Insert the original field into flds_x_xlist
      zero_string(command, 1024); 
      sprintf(command, "insert into flds_x_xlist (fld, fld_id, fld_cnt, xlist_id) values ('%s', %d, %lld, %d)", 
	      xfld, xfld_id, xfld_rec.cnt, xlist_id);
      rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
      if ( rc ) { handle_sqlite_error(); }
      /*--------------------------------------------------*/
      // Get name and ID of current and_fld in Table xlist 
      char and_fld[MAX_LEN_FLD_NAME+1]; 
      zero_string(and_fld, (MAX_LEN_FLD_NAME+1));
      int and_fld_id; FLD_REC_TYPE and_fld_rec; 
      zero_string(command, 1024); 
      sprintf(command, "select and_fld_id from xlist where id = %d ", xlist_id);
      status = db_get_ival(g_db, command, &and_fld_id); cBYE(status);
      if ( and_fld_id < 0 ) { go_BYE(-1); }
      status = get_fld_meta(and_fld_id, &and_fld_rec); cBYE(status);
      strcpy(and_fld,  and_fld_rec.name);
      /* Update and_fld with new field */
      status = f1f2opf3(tbl_rec.name, xfld, and_fld, "&&", and_fld);
      cBYE(status);
      // update count for new and_fld
      char buf[64]; int sz_buf = 64;
      status = ext_f_to_s(tbl_rec.name, and_fld, "sum", buf, sz_buf); cBYE(status);
      // update meta data 
      FLD_REC_TYPE nn_fld_rec; int nn_fld_id;
      status = is_fld(NULL, tbl_id, and_fld, &and_fld_id, &and_fld_rec, 
		      &nn_fld_id, &nn_fld_rec); cBYE(status);
      status = set_xlist_info(and_fld_rec.name, and_fld_rec.cnt, and_fld_id, xlist_id);
      cBYE(status);
    }
  }
 BYE:
  // TODO P3: free_if_non_null(zErrMsg);
  free_if_non_null(fld1);
  free_if_non_null(fld2);
  return(status);
}

int ylc_descr_item(
		   char *list,
		   char *str_pos,
		   char *mode,
		   char *what,
		   int *ptr_fld_id,
		   char **ptr_fld,
		   long long *ptr_fld_cnt,
		   char *opbuf
		   )
{
  int status = 0;
  bool b_is_list = false;  char *endptr;
  int list_id = -1, xlist_id = -1, tbl_id = -1, num_in_list = -1, pos = -1;

  *ptr_fld_id = -1;
  *ptr_fld_cnt = -1;
  *ptr_fld = NULL;

  if ( ( list == NULL ) || ( *list == '\0' ) )  { go_BYE(-1); }
  if ( ( str_pos  == NULL ) || ( *str_pos  == '\0' ) )  { go_BYE(-1); }
  if ( ( what == NULL ) || ( *what == '\0' ) )  { go_BYE(-1); }
  chk_mode(mode);
  pos = strtol(str_pos, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( pos <= 0 ) { go_BYE(-1); }

  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }
  status = get_num_in_list(list_id, &num_in_list); cBYE(status);
  if ( num_in_list <= 0 ) { go_BYE(-1); }
  if ( pos > num_in_list ) { go_BYE(-1); }

  chk_mode(mode);
  if ( strcmp(what, "id") == 0 ) {
    status = get_fld_id(list_id, pos, mode, ptr_fld_id); cBYE(status);
    sprintf(opbuf, "%d", *ptr_fld_id);
  }
  else if ( strcmp(what, "name") == 0 ) {
    status = get_fld_name(list_id, pos, mode, ptr_fld); cBYE(status);
    sprintf(opbuf, "%s", *ptr_fld);
  }
  else if ( strcmp(what, "cnt") == 0 ) {
    status = get_fld_cnt(list_id, pos, mode, ptr_fld_cnt); cBYE(status);
    sprintf(opbuf, "%lld", *ptr_fld_cnt);
  }
  else {
    go_BYE(-1);
  }
  
 BYE:
  return(status);
}

int ylc_describe_exclusions(
			    char *list
			    )
{
  int status = 0;
  bool b_is_list = false; int list_id = -1, tbl_id = -1, xlist_id = -1;
  int rc; char *zErrMsg = NULL;
  char command[1024];
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); 
    go_BYE(-1);
  }
  else {
    status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
    if ( xlist_id <= 0 ) { 
      fprintf(stderr, "No exclusions for List [%s] \n", list); 
    }
    else {
      char *flds1 = "and_fld,and_fld_id,and_fld_cnt";
      zero_string(command, 1024);
      fprintf(stderr,"%s\n", flds1);
      sprintf(command, "select %s from xlist where id = %d ", 
	      flds1, xlist_id);
      rc = sqlite3_exec(g_db, command, callback_pr, 0, &zErrMsg);
      if ( rc ) { handle_sqlite_error(); }

      char *flds2 = "fld,fld_id,fld_cnt";
      zero_string(command, 1024);
      fprintf(stderr,"%s\n", flds2);
      sprintf(command, "select %s from flds_x_xlist where xlist_id = %d ", 
	      flds2, xlist_id);
      rc = sqlite3_exec(g_db, command, callback_pr, 0, &zErrMsg);
      if ( rc ) { handle_sqlite_error(); }
    }
  }
  
 BYE:
  // TODO P3: sqlite3_free(zErrMsg);
  return(status);
}

int ylc_describe(
		 char *list
		 )
{
  int status = 0;
  bool b_is_list = false; int list_id = -1, tbl_id = -1, xlist_id = -1;
  int rc; char *zErrMsg = NULL;
  char command[1024];
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); 
    go_BYE(-1);
  }
  else {
    status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
    if ( list_id <= 0 ) { go_BYE(-1); }

    char *flds="id,fld,fld_id,fld_cnt,new_fld,new_fld_id,new_fld_cnt,xcl_fld,xcl_fld_id,xcl_fld_cnt,lmt_fld,lmt_fld_id,lmt_fld_cnt,list_id,position";
    zero_string(command, 1024);
    fprintf(stderr,"%s\n", flds);
    sprintf(command, "select %s from flds_x_list where list_id = %d ", 
	    flds, list_id);
    rc = sqlite3_exec(g_db, command, callback_pr, 0, &zErrMsg);
    if ( rc ) { 
      fprintf(stderr,"ERROR %s Command %s \n", zErrMsg, command);
      go_BYE(-1); 
    }
  }
  
 BYE:
  sqlite3_free(zErrMsg);
  return(status);
}

int ylc_num_in_list(
		    char *list,
		    char *mode,
		    long long *ptr_cnt
		    )
{
  int status = 0;
  bool b_is_list = false; int list_id = -1, tbl_id = -1, 
			    xlist_id = -1, num_in_list = -1;

  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }
  status = get_num_in_list(list_id, &num_in_list); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }

  status = get_sum_fld_cnt(list_id, num_in_list, mode, ptr_cnt); cBYE(status);
 BYE:
  return(status);
}

int ylc_del(
	    char *list
	    )
{
  int status = 0;
  bool b_is_list; int rc, list_id = -1, tbl_id = -1, xlist_id = -1;
  char *zErrMsg = NULL;

  if ( ( list == NULL ) || ( *list == '\0' ) ) { go_BYE(-1); }
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); return(status); 
  }
  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }

  char command[1024]; 
  zero_string(command, 1024);
  sprintf(command, "delete  from flds_x_list where list_id = %d \n", list_id); 
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }

  zero_string(command, 1024);
  sprintf(command, "delete  from list where name = '%s' \n", list); 
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }
  
 BYE:
  free_if_non_null(zErrMsg);
  return(status);
}
int ylc_pop(
	    char *list
	    )
{
  int status = 0;
  int rc, n, n_before, n_after, list_id = -1, tbl_id = -1, xlist_id = -1;
  char *zErrMsg = NULL;

  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status);
  if ( list_id <= 0 ) { go_BYE(-1); }

  status = get_num_in_list(list_id, &n_before); cBYE(status);

  if ( n_before == 0 ) { 
    fprintf(stderr, "List [%s] is empty \n", list); return(status);
  }

  char command[1024];
  zero_string(command, 1024);
  sprintf(command, "select count(*) from flds_x_list where list_id = %d and position = %d \n", list_id, n_before);
  status = db_get_num_rslts(g_db, command, &n);
  if ( n != 1 ) { fprintf(stderr, "Data Integrity Violated\n"); go_BYE(-1); }

  zero_string(command, 1024);
  sprintf(command, "delete from flds_x_list where list_id = %d and position = %d \n", list_id, n_before);
  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }

  status = get_num_in_list(list_id, &n_after); cBYE(status);
  if ( n_after != (n_before - 1) ) { go_BYE(-1); }

 BYE:
  free_if_non_null(zErrMsg);
  return(status);
}

int ylc_limit(
	      char *list,
	      char  *str_limits
	      )
{
  int status = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id = -1, xlist_id = -1;
  bool b_is_list;  int list_id = -1, num_in_list = -1;
  char **Y = NULL; int nY = 0;
  long long limit[MAX_NUM_IN_LIST];
  /*-----------------------------------------------------------*/
  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }

  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status); 
  if ( list_id < 0 ) { go_BYE(-1); }

  status = get_num_in_list(list_id, &num_in_list); cBYE(status); 
  if ( num_in_list <= 0 ) { go_BYE(-1); }
  /*-----------------------------------------------------------*/
  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);
  char *tbl = tbl_rec.name;
  /*-----------------------------------------------------------*/
  status = break_str(str_limits, ":", &Y, &nY); cBYE(status);
  if ( nY != num_in_list ) { go_BYE(-1); }
  for ( int i = 0; i < num_in_list; i++ ) { 
    char *endptr;
    limit[i] = strtoll(Y[i], &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    free_if_non_null(Y[i]);
  }
  free_if_non_null(Y);
  /*-----------------------------------------------------------*/
  for ( int i = 0; i < num_in_list; i++ ) { 
    int pos = i+1;
    if ( limit[i] <= 0 ) { /* No limit */
      status = del_aux_fld_if_exists(tbl_id, list_id, pos, "xcl_", "lmt_"); 
    }
    else {
      char str_limit_spec[64];
      char *fld = NULL, *lmt_fld = NULL, *new_fld = NULL;
      int new_fld_id = -1, lmt_fld_id = -1; 
      long long new_cnt;
      FLD_REC_TYPE lmt_fld_rec; long long lmt_fld_cnt;
      zero_string(str_limit_spec, 64);
      /* Get the field and the limit field */
      status = get_fld_id(list_id,   pos, "new_", &new_fld_id);  cBYE(status);
      status = get_fld_cnt( list_id, pos, "new_", &new_cnt); cBYE(status);
      status = get_fld_name(list_id, pos, "new_", &new_fld);     cBYE(status);
      status = get_fld_name(list_id, pos, "lmt_", &lmt_fld); cBYE(status);
      status = get_fld_name(list_id, pos, "new_", &new_fld); cBYE(status);

      if ( strcmp(lmt_fld, new_fld) != 0 ) { 
	/* Delete existing limit field */
	status = del_fld(NULL, tbl_id, lmt_fld, -1, true); cBYE(status);
	status = set_fld_id_cnt(list_id, pos, "lmt_", new_fld_id, new_fld, new_cnt);
	cBYE(status);
      }
      /* Create a new limit field */
      free_if_non_null(lmt_fld);
      lmt_fld = malloc(MAX_LEN_FLD_NAME+1);
      zero_string(lmt_fld, MAX_LEN_FLD_NAME+1);

      status = get_fld_name(list_id, pos, "", &fld);     cBYE(status);

      status = mk_name_new_fld( list, tbl, fld, "b_", lmt_fld); cBYE(status);
      status = dup_fld(tbl, fld, lmt_fld); cBYE(status);
      sprintf(str_limit_spec, "op=[zero_after_n]:limit=[%lld]", limit[i]);

      status = fop(tbl, lmt_fld, str_limit_spec); cBYE(status);
      // get lmt_fld_id 
      status = fld_meta(tbl, lmt_fld, &lmt_fld_id, false); cBYE(status);
      // count number of bit in lmt_fld
      char rslt_buf[64]; 
      status = ext_f_to_s(tbl, lmt_fld, "sum", rslt_buf, 64); cBYE(status);
      status = get_fld_meta(lmt_fld_id, &lmt_fld_rec); cBYE(status);
      lmt_fld_cnt = lmt_fld_rec.cnt;
      //------------------------
      status = set_fld_id_cnt(list_id, pos, "lmt_", lmt_fld_id, lmt_fld, lmt_fld_cnt);
      cBYE(status);
      free_if_non_null(new_fld);
      free_if_non_null(lmt_fld);
      free_if_non_null(fld);
    }
  }
 BYE:
  return(status);
}

int ylc_push(
	     char *list,
	     char  *fld
	     )
{
  int status = 0;
  int rc,  list_id = -1, xlist_id = -1, num_in_list = -9999; 
  char *zErrMsg = NULL;
  long long fld_cnt = -1, new_fld_cnt = -1, lmt_fld_cnt = -1, xcl_fld_cnt = -1;
  TBL_REC_TYPE tbl_rec;    int tbl_id    = -1;
  FLD_REC_TYPE fld_rec;    int fld_id    = -1;
  FLD_REC_TYPE new_fld_rec;    int new_fld_id;
  int lmt_fld_id, xcl_fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id = -1;
  char new_fld[MAX_LEN_FLD_NAME+1];
  char lmt_fld[MAX_LEN_FLD_NAME+1];
  char xcl_fld[MAX_LEN_FLD_NAME+1];
  char *tbl = NULL;

  zero_string(new_fld, MAX_LEN_FLD_NAME+1); 
  zero_string(lmt_fld, MAX_LEN_FLD_NAME+1); 
  zero_string(xcl_fld, MAX_LEN_FLD_NAME+1); 
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
  fld_cnt = fld_rec.cnt;
  if ( fld_cnt < 0 ) {  /* if count unknown, then compute it */
    char rslt_buf[64]; 
    status = ext_f_to_s(tbl, fld, "sum", rslt_buf, 64);
    cBYE(status);
    status = get_fld_meta(fld_id, &fld_rec); cBYE(status);
    fld_cnt = fld_rec.cnt;
  }
  if ( fld_cnt == 0 ) { 
    fprintf(stderr, "Field [%s] in Table [%s] = 0\n", fld, tbl); go_BYE(-1); 
  }
  /* Is this field already a part of the list */
  status = is_fld_in_list(list_id, fld_id, &b_is_fld_in_list);
  if ( b_is_fld_in_list ) {
    fprintf(stderr, "Field [%d] already in List [%s] \n", fld_id, list); 
    go_BYE(-1);
  }

  /* Is it the first field we are adding to the list */
  if ( num_in_list == 0 ) {
    new_fld_id = fld_id;
    strcpy(new_fld, fld);
    new_fld_cnt = fld_cnt;
  }
  else {
    FLD_REC_TYPE raw_fld_rec;   int raw_fld_id;
    FLD_REC_TYPE nn_new_fld_rec; int nn_new_fld_id;
    char buf1[64], buf2[64];
    status = get_fld_in_list(list_id, "", num_in_list, &raw_fld_id); cBYE(status);
    status = get_fld_meta(raw_fld_id, &raw_fld_rec); cBYE(status);
    char *raw_fld = raw_fld_rec.name;
    status = mk_name_new_fld( list, tbl, fld, "b_", new_fld);
    cBYE(status);
    status = f1f2opf3(tbl, fld, raw_fld, "&&!", new_fld); cBYE(status);
    status = is_fld(NULL, tbl_id, new_fld, &new_fld_id, &new_fld_rec, 
		    &nn_new_fld_id, &nn_new_fld_rec); cBYE(status);
    /* Compute how many bits are set in new_fld */
    zero_string(buf1, 64);
    status = ext_f_to_s(tbl, new_fld, "sum", buf1,  64); cBYE(status);
    zero_string(buf2, 64);
    char *endptr;
    status = read_nth_val(buf1, ':', 0, buf2, 64); cBYE(status);
    new_fld_cnt = strtoll(buf2, &endptr, 10); 
    lmt_fld_cnt = new_fld_cnt;
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( fld_cnt == 0 ) {
      fprintf(stderr, "Adding this field would cause count to go to 0\n");
      go_BYE(-1); 
    }
  }
  lmt_fld_id = new_fld_id;
  strcpy(lmt_fld, new_fld);
  lmt_fld_cnt = new_fld_cnt;

  xcl_fld_id = new_fld_id;
  strcpy(xcl_fld, new_fld);
  xcl_fld_cnt = new_fld_cnt;


  char command[1024];
  zero_string(command, 1024);
  char *flds = "new_fld, new_fld_id, new_fld_cnt, fld, fld_id, fld_cnt, lmt_fld, lmt_fld_id, lmt_fld_cnt, xcl_fld, xcl_fld_id, xcl_fld_cnt, list_id, position";
  sprintf(command, "insert into flds_x_list (%s) values ('%s', %d, %lld, '%s', %d, %lld, '%s', %d, %lld, '%s', %d, %lld, %d, %d) \n", 
	  flds, new_fld, new_fld_id, new_fld_cnt, fld, fld_id, fld_cnt, lmt_fld, lmt_fld_id, lmt_fld_cnt, xcl_fld, xcl_fld_id, xcl_fld_cnt, list_id, num_in_list+1);

  rc = sqlite3_exec(g_db, command, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ){ handle_sqlite_error(); }


 BYE:
  sqlite3_free(zErrMsg);
  return(status);
}


int
ylc(
    int argc,
    char **argv
    )
{
  int status = 0;
  int rc;

  g_docroot = getenv("Q_DOCROOT");
  if ( g_docroot == NULL ) { go_BYE(-1); }
  g_data_dir = getenv("Q_DATA_DIR");
  if ( g_data_dir == NULL ) { go_BYE(-1); }
  g_sqlite_db = getenv("Q_SQLITE_DB");
  if ( g_sqlite_db == NULL ) { go_BYE(-1); }

  zero_string(g_cwd, (MAX_LEN_DIR_NAME+1));
  if ( getcwd(g_cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); } 
  //----------------------------------------------
  if ( ( argc == 2 ) && ( strcmp(argv[1], "init") == 0 ) ) {
    char command[1024];
    zero_string(command, 1024);
    g_sqlite_sql = getenv("Q_SQLITE_SQL");
    if ( file_exists(g_sqlite_sql) == false ) { go_BYE(-1); }
    if ( g_sqlite_sql == NULL ) { go_BYE(-1); }
    char full_g_sqlite_db[1024];
    zero_string(full_g_sqlite_db, 1024);
    //------------------------------------------------
    int len;
    strcpy(full_g_sqlite_db, g_docroot);
    len = strlen(g_docroot);
    if ( g_docroot[len-1] != '/' ) {
      strcat(full_g_sqlite_db, "/");
    }
    strcat(full_g_sqlite_db, g_sqlite_db);
    //------------------------------------------------
    sprintf(command,"sqlite3 %s < %s \n", full_g_sqlite_db, g_sqlite_sql);
    system(command);
    chdir(g_cwd);
  }
  else {
    chdir(g_docroot);
    rc = sqlite3_open(g_sqlite_db, &g_db);
    if ( rc ) { 
      fprintf(stderr, "ERR: %s Cannot open [%s]\n", 
	      sqlite3_errmsg(g_db), g_sqlite_db);
      go_BYE(-1);
    }
    chdir(g_docroot);
    /*---------------------------------------------------*/
    /* Q specific initializations                        */
    char *alt_data_dir;
    status = mmap_meta_data(g_docroot, 
			    &g_tbl_X, &g_tbl_nX, &g_tbls, &g_n_tbl, 
			    &g_ht_tbl_X, &g_ht_tbl_nX, &g_ht_tbl, &g_n_ht_tbl, 
			    &g_fld_X, &g_fld_nX, &g_flds, &g_n_fld, 
			    &g_ht_fld_X, &g_ht_fld_nX, &g_ht_fld, &g_n_ht_fld, 
			    &g_ddir_X, &g_ddir_nX, &g_ddirs, &g_n_ddir);
    cBYE(status);
    g_alt_ddir_id = -1;
    alt_data_dir = getenv("Q_ALT_DATA_DIR");
    if ( alt_data_dir != NULL ) { 
      status = get_ddir_id(alt_data_dir, g_ddirs, g_n_ddir, true, &g_alt_ddir_id);
      cBYE(status);
    }
    /*---------------------------------------------------*/
    if ( ( argc < 2 ) || ( argc > 6 ) ) { go_BYE(-1); }
    if ( strcmp(argv[1], "add") == 0 ) {
      if ( argc != 4 ) { go_BYE(-1); }
      status = ylc_add(argv[2], argv[3]);
    }
    else if ( strcmp(argv[1], "exclude") == 0 ) {
      if ( argc != 4 ) { go_BYE(-1); }
      status = ylc_exclude(argv[2], argv[3]);
    }
    else if ( strcmp(argv[1], "unexclude") == 0 ) {
      if ( argc != 4 ) { go_BYE(-1); }
      status = ylc_unexclude(argv[2], argv[3]);
    }
    else if ( strcmp(argv[1], "list_lists") == 0 ) {
      if ( argc != 2 ) { go_BYE(-1); }
      status = ylc_list_lists();
    }
    else if ( strcmp(argv[1], "num_in_list") == 0 ) {
      long long cnt; 
      if ( argc != 4 ) { go_BYE(-1); }
      status = ylc_num_in_list(argv[2], argv[3], &cnt);
      fprintf(stdout, "%lld", cnt);
    }
    else if ( strcmp(argv[1], "describe") == 0 ) {
      if ( argc == 3 ) { 
	status = ylc_describe(argv[2]); cBYE(status);
      }
      else if ( argc == 4 ) { 
	if ( strcmp(argv[3], "exclusions") != 0 ) { go_BYE(-1); }
	status = ylc_describe_exclusions(argv[2]); cBYE(status);
      }
      else if ( argc == 6 ) { 
	int fld_id; char *fld = NULL; long long fld_cnt;
	char rslt_buf[64];
	status = ylc_descr_item(argv[2], argv[3], argv[4],argv[5], 
				&fld_id, &fld, &fld_cnt, rslt_buf); cBYE(status);
	fprintf(stdout, "%s", rslt_buf);
	free_if_non_null(fld);
      }
      else {
	go_BYE(-1);
      }
    }
    else if ( strcmp(argv[1], "del") == 0 ) {
      if ( argc != 3 ) { go_BYE(-1); }
      status = ylc_del(argv[2]);
    }
    else if ( strcmp(argv[1], "pop") == 0 ) {
      if ( argc != 3 ) { go_BYE(-1); }
      status = ylc_pop(argv[2]);
    }
    else if ( strcmp(argv[1], "push") == 0 ) {
      if ( argc != 4 ) { go_BYE(-1); }
      status = ylc_push(argv[2], argv[3]);
    }
    else if ( strcmp(argv[1], "limit") == 0 ) {
      if ( argc != 4 ) { go_BYE(-1); }
      status = ylc_limit(argv[2], argv[3]);
    }
    else {
      go_BYE(-1);
    }
  }

 BYE:
  unmap_meta_data(g_tbl_X, g_tbl_nX, g_ht_tbl_X, g_ht_tbl_nX, 
		  g_fld_X, g_fld_nX, g_ht_fld_X, g_ht_fld_nX,
		  g_ddir_X, g_ddir_nX);
  return(status);
}


int
main(
     int argc,
     char **argv
     )
{
  int status = 0;

  g_db = NULL;
  status = ylc(argc, argv);
  cBYE(status);
 BYE:
  if ( g_db != NULL ) { sqlite3_close(g_db); }
  return(status);
}

