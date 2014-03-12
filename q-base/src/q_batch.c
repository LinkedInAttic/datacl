#include "qtypes.h"
#include "auxil.h"
#include "q_core.h"
#include "q_init.h"
#include "dbauxil.h"
#include "meta_data.h"
#include "meta_globals.h"

extern char g_docroot[MAX_LEN_DIR_NAME+1];
extern char g_cwd[MAX_LEN_DIR_NAME+1];
extern char g_gpu_server[MAX_LEN_SERVER_NAME+1];
extern int g_gpu_port;
// START FUNC DECL
int
q_batch(
     int argc,
     char **argv,
     char *rslt_buf,
     int sz_rslt_buf,
     FILE *ofp
     )
// STOP FUNC DECL
{
  int status = 0;
  FILE *bfp = NULL;
  char *batfile = argv[2];

#define MAXLINE 1024
  char batline[MAXLINE];
  int largc = 0;
  char **largv = NULL;

  largv = malloc(MAX_NUM_ARGS * sizeof(char *));
  return_if_malloc_failed(largv);
  for ( int i = 0; i < MAX_NUM_ARGS; i++ ) { 
    largv[i] = malloc((MAX_LEN_ARG+1) * sizeof(char));
    return_if_malloc_failed(largv[i]);
    zero_string(largv[i], MAX_LEN_ARG+1);
  }
  status = q_init(argc, ofp); cBYE(status);
  status = chk_env_vars(g_docroot, g_gpu_server, &g_gpu_port); cBYE(status);
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
  bfp = fopen(batfile, "r");
  return_if_fopen_failed(bfp,  batfile, "r");
  for ( ; !feof(bfp); ) { 
    zero_string(batline, MAXLINE);
    char *cptr = fgets(batline, MAXLINE-1, bfp);
    if ( cptr == NULL ) { break; }
    if ( ( *batline == '\0' ) || ( *batline == '#' ) ) { /* comment */
      continue;
    }
    status = str_to_argv(batline, largv, MAX_NUM_ARGS, MAX_LEN_ARG, &largc);
    cBYE(status);
    status = q_core(largc, largv, rslt_buf, sz_rslt_buf); cBYE(status);
    for ( int i = 0; i < largc; i++ ) { 
      zero_string(largv[i], MAX_LEN_ARG+1);
    }
    largc = 0;
  }
  //----------------------------------------------
  unmap_meta_data(g_tbl_X, g_tbl_nX, g_ht_tbl_X, g_ht_tbl_nX, 
		  g_fld_X, g_fld_nX, g_ht_fld_X, g_ht_fld_nX,
		  g_ddir_X, g_ddir_nX, 
		  g_fld_info_X, g_fld_info_nX, 
		  g_aux_X, g_aux_nX);
 BYE:
  if ( largv != NULL ) { 
    for ( int i = 0; i < MAX_NUM_ARGS; i++ ) { 
      free_if_non_null(largv[i]);
    }
    free_if_non_null(largv);
  }
  fclose_if_non_null(bfp);
  return(status);
}

