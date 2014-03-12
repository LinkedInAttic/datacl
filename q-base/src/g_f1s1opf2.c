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
g_f1s1opf2(
    char *f1,
    char *str_scalar,
    char *str_op,
    char *f2
    )
// STOP FUNC DECL
{
  int status = 0;
  int f1_id = -1; GPU_REG_TYPE f1_rec;
  int f2_id = -1; GPU_REG_TYPE f2_rec;
  char *curl_response = NULL;
  // GPU meta data stuff
  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 
  // START: for scalars
  long long svals_I8[MAX_NUM_SCALARS]; 
  int       svals_I4[MAX_NUM_SCALARS]; 
  short     svals_I2[MAX_NUM_SCALARS]; 
  char      svals_I1[MAX_NUM_SCALARS]; 
  float     svals_F4[MAX_NUM_SCALARS]; 
  double    svals_F8[MAX_NUM_SCALARS]; 

  long long sval_I8 = LLONG_MAX;
  int       sval_I4 = INT_MAX;
  short     sval_I2 = SHRT_MAX;
  char      sval_I1 = SCHAR_MAX;
  float     sval_F4 = FLT_MAX;
  double    sval_F8 = DBL_MAX;

  int num_scalar_vals = -1;
  char      *hashes_I1 = NULL;
  short     *hashes_I2 = NULL;
  int       *hashes_I4 = NULL;
  long long *hashes_I8 = NULL;
  unsigned int n = 0; // size of hash table 
  unsigned int a = 0, b = 0; // coefficients for hash function
  // STOP: for scalars
  FLD_TYPE f2_fldtype;

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
      &gpu_reg, &n_gpu_reg);

  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op == NULL ) || ( *str_op == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, f1, &f1_id, &f1_rec); cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(f1_rec.fldtype) == false ) { go_BYE(-1); }

  status = g_is_fld(gpu_reg, n_gpu_reg, f2, &f2_id, &f2_rec); cBYE(status);
  if ( f2_id < 0 ) { go_BYE(-1); }
  if ( is_fldtype_goodfor_gpu(f2_rec.fldtype) == false ) { go_BYE(-1); }

  if ( f1_rec.nR      != f2_rec.nR       ) { go_BYE(-1); }
  if ( f1_rec.filesz  == 0               ) { go_BYE(-1); }
  if ( f1_rec.nR      == 0               ) { go_BYE(-1); }
  status = get_type_op_fld("f1s1opf2", str_op, f1_rec.fldtype, 
      f1_rec.fldtype, "", &f2_fldtype);

  if ( f2_fldtype != f2_rec.fldtype ) { go_BYE(-1); }

  status = g_chk_f1s1opf2(f1_rec, f2_rec, str_op);  cBYE(status);
  status =  break_into_scalars(str_scalar, f1_rec.fldtype, &num_scalar_vals,
	&sval_I8, &sval_I4, &sval_I2, &sval_I1, &sval_F4, &sval_F8,
	svals_I8, svals_I4, svals_I2, svals_I1, svals_F4, svals_F8,
	&hashes_I1, &hashes_I2, &hashes_I4, &hashes_I8, &n, &a, &b);
  cBYE(status);
  //----------------------------------------------------------
  char url[2048];     zero_string(url, 2048);
  char command[1024]; zero_string(command, 1024);
  sprintf(command, "f1s1opf2 d_fld1=%s scalar=%s op=%s d_fld2=%s",
        f1, str_scalar, str_op, f2); 
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
