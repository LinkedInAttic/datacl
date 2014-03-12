#undef FAKE
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "add_fld.h"
#include "del_fld.h"
#include "mmap.h"
#include "aux_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "g_aux.h"
#include "g_meta_data.h"
#include "url.h"

extern char g_docroot[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int
g_f_to_s(
    char *f1,
    char *str_op,
    char *rslt_buf,
    int  sz_rslt_buf
    )
// STOP FUNC DECL
{
  int status = 0;
  int f1_id = -1; GPU_REG_TYPE f1_rec;
  char *curl_response = NULL;
  // GPU meta data stuff
  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
      &gpu_reg, &n_gpu_reg);

  if ( ( str_op == NULL ) || ( *str_op == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op == NULL ) || ( *str_op == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, f1, &f1_id, &f1_rec); cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(f1_rec.fldtype) == false ) { go_BYE(-1); }

  if ( f1_rec.filesz  == 0               ) { go_BYE(-1); }
  if ( f1_rec.nR      == 0               ) { go_BYE(-1); }
  status = g_chk_f_to_s(f1_rec, str_op);  cBYE(status);
  //----------------------------------------------------------
  char url[2048];     zero_string(url, 2048);
  char command[1024]; zero_string(command, 1024);
  sprintf(command, "f_to_s d_fld1=%s op=%s ", f1, str_op);
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
