#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "auxil.h"
#include "aux_meta.h"
#include "g_aux.h"
#include "g_meta_data.h"
#include "url.h"

extern char g_docroot[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int
g_print_reg_data(
    char *d_fld,
    char *str_n,
    char *mode,
    char *rslt_buf,
    int sz_rslt_buf
    )
// STOP FUNC DECL
{
  int status = 0;
  int d_fld_id = -1;  int n;
  char *curl_response = NULL;
  GPU_REG_TYPE d_fld_rec;

  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
      &gpu_reg, &n_gpu_reg);

  if ( ( d_fld == NULL ) || ( *d_fld == '\0' ) ) { go_BYE(-1); }
  if ( rslt_buf == NULL ) { go_BYE(-1); }
  if ( mode == NULL ) { go_BYE(-1); }
  if ( sz_rslt_buf <= 1 ) { go_BYE(-1); }
  if ( ( strcmp(mode, "head") != 0 ) && ( strcmp(mode, "tail") != 0 ) ) {
    go_BYE(-1);
  }

  status  = chk_fld_name(d_fld);  cBYE(status); 
  status = g_is_fld(gpu_reg, n_gpu_reg, d_fld, &d_fld_id, &d_fld_rec); 
  cBYE(status);
  if ( d_fld_id < 0 ) { go_BYE(-1); }
  status = stoI4(str_n, &n); cBYE(status);
  if ( ( n < 1 ) || ( n > 1024 ) ) { go_BYE(-1); }
  n = min(n, d_fld_rec.nR);

  //----------------------------------------------------------
  char url[2048];     zero_string(url, 2048);
  char command[1024]; zero_string(command, 1024);

  sprintf(command, "print_reg_data d_fld=%s n=%s mode=%s", d_fld, str_n, mode); 
  status = g_make_url(command, url); cBYE(status);
  status = g_call_gpu(url, &curl_response); cBYE(status);
  if ( curl_response != NULL ) { 
    strncpy(rslt_buf, curl_response, sz_rslt_buf);
  }
BYE:
  free_if_non_null(curl_response);
  g_unmap_meta_data(gpu_reg_X, gpu_reg_nX);
  return(status);
}
