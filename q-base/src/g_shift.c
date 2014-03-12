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
g_shift(
    char *f1,
    char *f2,
    char *str_shift_amt,
    char *str_newval
    )
// STOP FUNC DECL
{
  int status = 0;
  int f1_id = -1; GPU_REG_TYPE f1_rec;
  int f2_id = -1; GPU_REG_TYPE f2_rec;
  char *curl_response = NULL;
  long long shift_amt = 0, newval = 0;
  // GPU meta data stuff
  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
      &gpu_reg, &n_gpu_reg);

  if ( ( str_shift_amt == NULL ) || ( *str_shift_amt == '\0' ) ) { go_BYE(-1); }
  if ( ( str_newval == NULL ) || ( *str_newval == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, f1, &f1_id, &f1_rec); cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(f1_rec.fldtype) == false ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, f2, &f2_id, &f2_rec); cBYE(status);
  if ( f2_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(f2_rec.fldtype) == false ) { go_BYE(-1); }

  if ( f1_rec.filesz  == 0               ) { go_BYE(-1); }
  if ( f1_rec.nR      == 0               ) { go_BYE(-1); }
  if ( f2_rec.filesz  == 0               ) { go_BYE(-1); }
  if ( f2_rec.nR      == 0               ) { go_BYE(-1); }
	status = stoI8(str_shift_amt, &shift_amt); cBYE(status);
  status = g_chk_shift(f1_rec, f2_rec, shift_amt, newval);  cBYE(status);
  status = stoI8(str_shift_amt, &shift_amt); cBYE(status);
  if ( ( shift_amt < (-1 * MAX_SHIFT) ) || ( shift_amt > MAX_SHIFT ) ) {
    go_BYE(-1);
  }
  status = stoI8(str_newval, &newval); cBYE(status);
  //----------------------------------------------------------
  char url[2048];     zero_string(url, 2048);
  char command[1024]; zero_string(command, 1024);
  sprintf(command, "f1_shift_f2 d_fld1=%s d_fld2=%s shift_amt=%s newval=%s", 
      f1, f2, str_shift_amt, str_newval); 
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
  return(status);
}
