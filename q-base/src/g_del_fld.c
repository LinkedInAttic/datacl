#undef FAKE
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "add_fld.h"
#include "del_fld.h"
#include "auxil.h"
#include "aux_meta.h"
#include "g_aux.h"
#include "g_meta_data.h"
#include "url.h"
#include "mk_temp_file.h"

extern 	DDIR_REC_TYPE *g_ddirs;
extern char g_docroot[MAX_LEN_DIR_NAME+1];
extern char g_gpu_server[MAX_LEN_SERVER_NAME+1];
extern int  g_gpu_port;

// START FUNC DECL
int
g_del_fld(
    char *d_fld
    )
// STOP FUNC DECL
{
  int status = 0;
  int d_fld_id = -1; GPU_REG_TYPE d_fld_rec;
  char *curl_response = NULL;
  // GPU meta data stuff
  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
      &gpu_reg, &n_gpu_reg);

  if ( ( d_fld == NULL ) || ( *d_fld == '\0' ) ) { go_BYE(-1); }
  status = g_is_fld(gpu_reg, n_gpu_reg, d_fld, &d_fld_id, &d_fld_rec); 
  cBYE(status);
  if ( d_fld_id < 0 ) { /* Nothing to do */ return status ; } 
  //----------------------------------------------------------
  char url[2048];     zero_string(url, 2048);
  char command[512];  zero_string(url, 512);

  sprintf(command, "del_fld d_fld=%s", d_fld); 
  status = g_make_url(command, url); cBYE(status);
  status = g_call_gpu(url, &curl_response); cBYE(status);
  if ( curl_response != NULL ) { 
    fprintf(stderr, "Response from GPU = [%s]\n", curl_response);
  }
BYE:
  free_if_non_null(curl_response);
  g_unmap_meta_data(gpu_reg_X, gpu_reg_nX);
  return status ;
}
