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
#include "mk_temp_file.h"
#include "get_type_op_fld.h"

extern char g_docroot[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int
g_count(
    char *f1,
    char *cfld,
    char *f2
    )
// STOP FUNC DECL
{
  int status = 0;
  int f1_id = -1; GPU_REG_TYPE f1_rec;
  int cfld_id = -1; GPU_REG_TYPE cfld_rec;
  int f2_id = -1; GPU_REG_TYPE f2_rec;
  char *curl_response = NULL;
  // GPU meta data stuff
  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
      &gpu_reg, &n_gpu_reg);

  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( cfld == NULL ) || ( *cfld == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }
  if ( strcmp(f2, cfld) == 0 ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, f1, &f1_id, &f1_rec); cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(f1_rec.fldtype) == false ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, cfld, &cfld_id, &cfld_rec); cBYE(status);
  if ( cfld_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(cfld_rec.fldtype) == false ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, f2, &f2_id, &f2_rec); cBYE(status);
  if ( f2_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(f2_rec.fldtype) == false ) { go_BYE(-1); }

  status = g_chk_count(f1_rec, cfld_rec, f2_rec);  cBYE(status);
  //----------------------------------------------------------
  char url[2048];     zero_string(url, 2048);
  char command[1024]; zero_string(command, 1024);
  sprintf(command, "f1f2opf3 d_fld1=%s d_cfld=%s d_fld2=%s",
        f1, cfld, f2);
  status = g_make_url(command, url); cBYE(status);
  status = g_call_gpu(url, &curl_response); cBYE(status);
  if ( curl_response != NULL ) { 
    if ( strstr(curl_response, "\"\"") == NULL ) { 
      fprintf(stderr, "Response from GPU = [%s]\n", curl_response);
    }
  }
BYE:
  free_if_non_null(curl_response);
  g_unmap_meta_data(gpu_reg_X, gpu_reg_nX);
  return status ;
}
