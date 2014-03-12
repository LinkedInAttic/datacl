#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "md5global.h"
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "auxil.h"
#include "ylc_auxil.h"
#include "mddriver.h"
#include "mmap.h"
#include "ext_f_to_s.h"

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
// START FUNC DECL
int 
mk_name_new_fld(
		char *str1, 
		char *str2, 
		char *str3, 
		char *prefix,
		char new_fld[MAX_LEN_FLD_NAME+1]
		)
// STOP FUNC DECL
{
  int status = 0;
  char md5buf[LEN_MD5_DIGEST+2];
  char buffer[1024];
  int pid;
  struct timeval Tps;
  struct timezone Tpf;
  long long t_sec = 0, t_usec = 0, curr_time = 0;

  if ( ( str1 == NULL ) || ( *str1 == '\0' ) ) { go_BYE(-1); }
  if ( ( str2 == NULL ) || ( *str2 == '\0' ) ) { go_BYE(-1); }
  if ( ( str3 == NULL ) || ( *str3 == '\0' ) ) { go_BYE(-1); }

  gettimeofday(&Tps, &Tpf); 
  t_sec  = (long long)Tps.tv_sec;
  t_usec = (long long)Tps.tv_usec;
  curr_time = t_sec * 1000000 + t_usec;
  pid = getpid();
  zero_string(md5buf, LEN_MD5_DIGEST+2);
  zero_string(buffer, 1024);
  sprintf(buffer, "%s_%s_%s_%lld_%d", str1, str2, str3, curr_time, pid);
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
// START FUNC DECL
int
count_ones(
	   char *tbl,
	   char *fld,
	   long long *ptr_fld_cnt
	   )
// STOP FUNC DECL
{
  int status = 0;
  char buf1[64],buf2[64]; char *endptr;
  zero_string(buf1, 64);
  zero_string(buf2, 64);

  status = ext_f_to_s(tbl, fld, "sum", buf1,  64); cBYE(status);
  status = read_nth_val(buf1, ':', 0, buf2, 64); cBYE(status);
  *ptr_fld_cnt = strtoll(buf2, &endptr, 10); 
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( *ptr_fld_cnt == 0 ) {
    fprintf(stderr, "WARNING! Adding fld [%s] => count = 0\n", fld);
  }
 BYE:
  return(status);
}
// START FUNC DECL
int
fetch_rows(
	   sqlite3 *db,
	   char *tbl,
	   char *fld,
	   char *where,
	   char *order_by,
	   void **ptr_X,
	   int *ptr_n
	   )
// STOP FUNC DECL
{
  /* http://wang.yuxuan.org/blog/?itemid=61 */
  int status = 0;
  char command[1024];
  sqlite3_stmt *stmt = NULL;
  const char *cptr;
  long long *lvals = NULL; char **strvals = NULL;
  int fldtype;

  *ptr_X = NULL; 
  *ptr_n = 0;

  zero_string(command, 1024);
  sprintf(command, "select count(*) from %s %s ", tbl, where);
  status = db_get_num_rslts(db, command, ptr_n); cBYE(status);
  if ( *ptr_n == 0 ) { return(status); } 

  zero_string(command, 1024);
  sprintf(command, "select %s from %s %s %s", fld, tbl, where, order_by);

  int rc = sqlite3_prepare(db, command, 1024, &stmt, &cptr);
  if ( rc < 0 ) { sqlite3_errmsg(db); go_BYE(-1); }

  lvals = malloc(*ptr_n * sizeof(long long));
  return_if_malloc_failed(lvals); 
  strvals = malloc(*ptr_n * sizeof(char *));
  return_if_malloc_failed(strvals); 

  //-------------------------------------------------------------
  int num_cols; int idx = 0;
  while ( (rc = sqlite3_step(stmt)) != SQLITE_DONE ) {
    switch ( rc ) { 
    case SQLITE_BUSY : 
      sleep(1);
      break;
    case SQLITE_ERROR : 
      sqlite3_errmsg(db); go_BYE(-1);
      break;
    case SQLITE_ROW : 
      num_cols = sqlite3_column_count(stmt);
      if ( num_cols != 1 ) { go_BYE(-1); }
      for ( int i = 0; i < num_cols; i++ ) { 
	switch ( sqlite3_column_type(stmt, i)) {
	case SQLITE_INTEGER : 
	  fldtype = SQLITE_INTEGER;
	  lvals[idx++] = sqlite3_column_int(stmt, i);
	  break;
	case SQLITE_TEXT : 
	  fldtype = SQLITE_TEXT;
	  const unsigned char *cptr = sqlite3_column_text(stmt, i);
	  int len = strlen((char *)cptr);
	  strvals[idx] = malloc(len+1);
	  strcpy(strvals[idx], (char *)cptr);
	  strvals[idx][len] = '\0';
	  idx++;
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}

      }
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  if ( idx != *ptr_n ) { go_BYE(-1); }
  switch ( fldtype ) { 
    case SQLITE_INTEGER : *ptr_X = lvals;   break; 
    case SQLITE_TEXT :    *ptr_X = strvals; break; 
    default : go_BYE(-1); break; 
  }
 BYE:
  if ( stmt != NULL ) { sqlite3_finalize(stmt); }
  return(status);
}
