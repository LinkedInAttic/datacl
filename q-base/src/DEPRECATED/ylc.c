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
#include "ylc_add.h"
#include "ylc_del.h"
#include "ylc_pop.h"
#include "ylc_push.h"
#include "ylc_exclude.h"
#include "ylc_unexclude.h"
#include "ylc_num_in_list.h"
#include "ylc_list_lists.h"
#include "ylc_describe.h"
#include "ylc_descr_item.h"
#include "ylc_descr_excl.h"
#include "ylc_limit.h"
#include "ylc_check.h"
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
      long long cnt;  int num_in_list;
      if ( argc != 4 ) { go_BYE(-1); }
      status = ylc_num_in_list(argv[2], argv[3], &num_in_list, &cnt);
      fprintf(stdout, "%d:%lld", num_in_list, cnt);
    }
    else if ( strcmp(argv[1], "describe") == 0 ) {
      if ( argc == 3 ) { 
	status = ylc_describe(argv[2]); cBYE(status);
      }
      else if ( argc == 4 ) { 
	if ( strcmp(argv[3], "exclusions") != 0 ) { go_BYE(-1); }
	status = ylc_descr_excl(argv[2]); cBYE(status);
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
    else if ( strcmp(argv[1], "check") == 0 ) {
      if ( argc != 2 ) { go_BYE(-1); }
      status = ylc_check();
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

