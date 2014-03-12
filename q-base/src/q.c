// MAC #include <sys/sysinfo.h>
#ifdef IPP
#include "ippcore.h"
#endif
#include "qtypes.h"
#include "dbauxil.h"
#include "meta_data.h"
#include "orig_meta_globals.h"
#include "q_core.h"
#include "q_init.h"

int  g_num_cores; // number of cores on this machine
char g_gpu_server[MAX_LEN_SERVER_NAME+1];
char g_docroot[MAX_LEN_DIR_NAME+1];
int  g_gpu_port;
char g_cwd[MAX_LEN_DIR_NAME+1];
FILE *g_ofp;
bool g_use_ram_dir;
// START FUNC DECL
int
q(
  int argc,
  char **argv,
  char *rslt_buf,
  int sz_rslt_buf,
  FILE *ofp
  )
// STOP FUNC DECL
{
  int status = 0;

  status = q_init(argc, ofp); cBYE(status);

  if ( ( argc == 6 ) && ( strcmp(argv[1], "bootstrap") == 0 ) ) {  
    char *l_docroot      = argv[2];
    char *bs_docroot     = argv[3];
    char *l_dsk_data_dir = argv[4];
    char *l_ram_data_dir = argv[5];
    status = bootstrap_meta_data(l_docroot, bs_docroot, l_dsk_data_dir, 
	l_ram_data_dir); 
    cBYE(status);
    goto BYE;
  }
  //----------------------------------------------
  // Note that chk_env_vars is not relevant for bootstrap
  status = chk_env_vars(g_docroot, g_gpu_server, &g_gpu_port); cBYE(status);
  //----------------------------------------------
  if ( ( argc == 4 ) && ( strcmp(argv[1], "init") == 0 ) ) {  
    g_dsk_ddir_id = g_ram_ddir_id = -1;
    char *dsk_data_dir = argv[2];
    char *ram_data_dir = argv[3];
    status = init_meta_data(g_docroot, dsk_data_dir, ram_data_dir); 
    cBYE(status);
    // fprintf(stderr, " -- WARNING! WARNING!! SHOULD BE DONE BY GPU\n");
    // status = g_init_meta_data(g_docroot); cBYE(status);
    goto BYE;
  }
  //------------------------------------------------------
  status = mmap_meta_data(g_docroot, 
			  &g_tbl_X, &g_tbl_nX, &g_tbls, &g_n_tbl, 
			  &g_ht_tbl_X, &g_ht_tbl_nX, &g_ht_tbl, &g_n_ht_tbl, 
			  &g_fld_X, &g_fld_nX, &g_flds, &g_n_fld, 
			  &g_ht_fld_X, &g_ht_fld_nX, &g_ht_fld, &g_n_ht_fld, 
			  &g_ddir_X, &g_ddir_nX, &g_ddirs, &g_n_ddir, 
			  &g_fld_info_X, &g_fld_info_nX, &g_fld_info, &g_n_fld_info,
			  &g_aux_X, &g_aux_nX, &g_dsk_ddir_id, &g_ram_ddir_id);
  cBYE(status);
  // safety check
  if ( g_dsk_ddir_id   != 0 ) { go_BYE(-1); }
  if ( ( g_ram_ddir_id > 0 ) && ( g_ram_ddir_id != 1 ) ) { go_BYE(-1); }
  //----------------------------------------------
  if ( ( argc == 2 ) && ( strcmp(argv[1], "reset_compound") == 0 ) ) {
    // THIS SHOULD BE USED WITH GREAT CARE
    status = stop_compound_expr(); cBYE(status);
    goto BYE;
  }
  if ( ( argc == 2 ) && ( strcmp(argv[1], "start_compound") == 0 ) ) {
    status = start_compound_expr(); cBYE(status);
    goto BYE;
  }
  // Determine if we are inside a compound expression
  if ( strcmp(argv[1], "stop_compound") != 0 ) {
    bool b_is_in_compound = false;
    status = is_in_compound(argc, argv, &b_is_in_compound); cBYE(status);
    if ( b_is_in_compound == true ) {
      goto BYE;
    }
  }
  //----------------------------------------------
  status = q_core(argc, argv, rslt_buf, sz_rslt_buf); cBYE(status);
 BYE:
  unmap_meta_data(g_tbl_X, g_tbl_nX, g_ht_tbl_X, g_ht_tbl_nX, 
		  g_fld_X, g_fld_nX, g_ht_fld_X, g_ht_fld_nX,
		  g_ddir_X, g_ddir_nX, 
		  g_fld_info_X, g_fld_info_nX, 
		  g_aux_X, g_aux_nX);
  return(status);
}
