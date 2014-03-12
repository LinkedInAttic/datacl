#undef FAKE
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "add_fld.h"
#include "del_fld.h"
#include "auxil.h"
#include "dbauxil.h"
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
g_store(
    char *tbl,
    char *d_fld,
    char *h_fld
    )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = -1; TBL_REC_TYPE tbl_rec;
  int h_fld_id = -1; FLD_REC_TYPE h_fld_rec;
  int d_fld_id = -1; GPU_REG_TYPE d_fld_rec;
  int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;
  int ddir_id = -1, fileno = -1; int fldsz = 0; size_t filesz = 0;
  char *curl_response = NULL;
  // GPU meta data stuff
  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
      &gpu_reg, &n_gpu_reg);

  if ( (   tbl == NULL ) || (   *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( h_fld == NULL ) || ( *h_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( d_fld == NULL ) || ( *d_fld == '\0' ) ) { go_BYE(-1); }

  status  = chk_fld_name(d_fld);  cBYE(status); 
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  long long nR = tbl_rec.nR;
  status = is_fld(NULL, tbl_id, h_fld, &h_fld_id, &h_fld_rec, 
      &nn_fld_id, &nn_fld_rec);
  if ( h_fld_id >= 0 ) { 
    status = del_fld(NULL, tbl_id, NULL, h_fld_id); cBYE(status);
  }
  status = g_is_fld(gpu_reg, n_gpu_reg, d_fld, &d_fld_id, &d_fld_rec); 
  cBYE(status);
  if ( d_fld_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(d_fld_rec.fldtype) == false ) { go_BYE(-1); }
  if ( d_fld_rec.nR != nR ) { go_BYE(-1); }

  //----------------------------------------------------------
  status = get_fld_sz(d_fld_rec.fldtype, &fldsz);
  filesz = fldsz * nR;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  zero_fld_rec(&h_fld_rec);
  h_fld_rec.fldtype = d_fld_rec.fldtype; 
  status = add_fld(tbl_id, h_fld, ddir_id, fileno, &h_fld_id, &h_fld_rec);
  cBYE(status)
  //----------------------------------------------------------
  char filename[MAX_LEN_FILE_NAME+1];  
  zero_string(filename, MAX_LEN_FILE_NAME+1);
  char url[2048];     zero_string(url, 2048);
  char command[1024]; zero_string(command, 1024);

  status = mk_full_filename(ddir_id, fileno, filename, MAX_LEN_FILE_NAME);
  cBYE(status);
  sprintf(command, "store d_fld=%s filename=%s", d_fld, filename); 
  status = g_make_url(command, url); cBYE(status);
  status = g_call_gpu(url, &curl_response); cBYE(status);
  if ( curl_response != NULL ) { 
    fprintf(stderr, "Response from GPU = [%s]\n", curl_response);
  }
BYE:
  free_if_non_null(curl_response);
  g_unmap_meta_data(gpu_reg_X, gpu_reg_nX);
  return(status);
}
