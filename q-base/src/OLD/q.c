#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_data.h"
#include "list_files.h"
#include "list_tbls.h"
#include "logger.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "ext_s_to_f.h"
#include "pr_fld.h"
#include "tbl_meta.h"
#include "dump.h"
#include "dld.h"
#include "fld_meta.h"

int g_num_threads;
int g_block_size;
char *g_docroot;
char *g_data_dir;
bool g_write_to_temp_dir = false;
// START FUNC DECL
int
q(
     int argc,
     char **argv,
     char *rslt_buf,
     int sz_rslt_buf
     )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_tbl, b_is_fld;
  int tbl_id, fld_id, nn_fld_id;
  long long log_id;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  char *endptr, *cptr;
  FILE *ofp = NULL;
  //----------------------------------------------
  g_docroot = getenv("Q_DOCROOT");
  if ( g_docroot == NULL ) { go_BYE(-1); }
  g_data_dir = getenv("Q_DATA_DIR");
  if ( g_data_dir == NULL ) { go_BYE(-1); }
  //----------------------------------------------
  cptr = getenv("Q_NUM_THREADS");
  if ( ( cptr == NULL ) || (  *cptr == '\0' ) ) { 
    g_num_threads = 1; // default 
  }
  else {
    g_num_threads = (int)strtoll(cptr, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( g_num_threads < 1 ) { go_BYE(-1); }
    if ( g_num_threads > MAX_NUM_THREADS ) { go_BYE(-1); }
  }
  //----------------------------------------------
  cptr = getenv("Q_BLOCK_SIZE");
  if ( ( cptr == NULL ) || (  *cptr == '\0' ) ) { 
    g_block_size = 1048576;
  }
  else {
    g_block_size = (int)strtoll(cptr, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( g_block_size < 1 ) { go_BYE(-1); }
  }
  //----------------------------------------------
  log_request(argc, argv, &log_id);

  if ( ( argc < 2 ) || ( argc > MAX_NUM_ARGS ) ) { go_BYE(-1); }
  if ( ( g_docroot == NULL ) ||  ( *g_docroot == '\0' ) ) { 
    fprintf(stderr,  "Doc Root not specified\n"); go_BYE(-1);
  }
  if ( strcmp(argv[1], "init") == 0 ) {  // DONE
    if ( argc != 2 ) { go_BYE(-1); }
    status = init_meta_data(g_docroot);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "list_files") == 0 ) {  // DONE
    if ( argc != 2 ) { go_BYE(-1); }
    status = list_files();
    cBYE(status);
  }
  else if ( strcmp(argv[1], "dump") == 0 ) {  // DONE
    if ( argc != 4 ) { go_BYE(-1); }
    status = dump(argv[2], argv[3]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "list_tbls") == 0 ) {  // DONE
    if ( argc != 2 ) { go_BYE(-1); }
    status = list_tbls();
    cBYE(status);
  }
  else if ( strcmp(argv[1], "orphan_files") == 0 ) {  // DONE
    if ( argc != 2 ) { go_BYE(-1); }
    status = orphan_files();
    cBYE(status);
  }
  else if ( strcmp(argv[1], "add_tbl") == 0 ) {  // DONE
    if ( argc != 4 ) { go_BYE(-1); }
    status = add_tbl(argv[2], argv[3], &tbl_id, &tbl_rec);
    cBYE(status);
  }
  /*
  else if ( strcmp(argv[1], "add_fld") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = add_tbl(argv[2], argv[3], &tbl_id, &tbl_rec);
    cBYE(status);
  }
  */
  else if ( strcmp(argv[1], "del_tbl") == 0 ) {  // DONE
    if ( argc != 3 ) { go_BYE(-1); }
    status = del_tbl(argv[2], -1);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "is_tbl") == 0 ) { // DONE 
    if ( argc != 3 ) { go_BYE(-1); }
    status = is_tbl(argv[2], &tbl_id, &tbl_rec);
    cBYE(status);
    if ( tbl_id < 0 ) { b_is_tbl = false; } else { b_is_tbl = true; }
    snprintf(rslt_buf, sz_rslt_buf, "%d,%d\n", b_is_tbl, tbl_id);
  }
  else if ( strcmp(argv[1], "is_fld") == 0 ) {  // DONE 
    if ( argc != 4 ) { go_BYE(-1); }
    status = is_fld( argv[2], -1, argv[3], 
	&fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
    cBYE(status);
    if ( fld_id < 0 ) { b_is_fld = false; } else { b_is_fld = true; }
    snprintf(rslt_buf, sz_rslt_buf, "%d,%d\n", b_is_fld, fld_id);
  }
  else if ( strcmp(argv[1], "describe") == 0 ) {  // DONE
    switch ( argc ) { 
      case 3 : status = tbl_meta(argv[2]);  cBYE(status); break;
      case 4 : status = fld_meta(argv[2], argv[3]);  cBYE(status); break;
      default : go_BYE(-1); break;
    }
  }
  else if ( strcmp(argv[1], "dld") == 0 ) {  // DONE
    if ( argc != 6 ) { go_BYE(-1); }
    status = dld(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "s_to_f") == 0 ) {  // DONE
    if ( argc != 5 ) { go_BYE(-1); }
    status = ext_s_to_f(argv[2], argv[3], argv[4]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "pr_fld") == 0 ) {  // DONE
    switch ( argc ) { 
      case 4 : 
	status = pr_fld(argv[2], argv[3], "", stdout);
	break; 
      case 5 : 
	status = pr_fld(argv[2], argv[3], argv[4], stdout);
	break; 
      case 6 : 
	if ( argv[5][0] == '\0' ) {
	  status = pr_fld(argv[2], argv[3], argv[4], stdout);
	}
	else {
	  ofp = fopen(argv[5], "w");
	  return_if_fopen_failed(ofp,  argv[5], "w");
  	  status = pr_fld(argv[2], argv[3], argv[4], ofp);
	  fclose_if_non_null(ofp);
	}
	break; 
      default : 
	go_BYE(-1);
	break;
    }
  }
  else {
    go_BYE(-1);
  }
 BYE:
  fclose_if_non_null(ofp);
  log_response(status, log_id);
  return(status);
}
