#include "qtypes.h"
#include "mmap.h"
#include "mk_file.h"
#include "auxil.h"
#include "g_aux.h"

#define G_GPU_REG_SIZE 32

// START FUNC DECL
int
g_mmap_meta_data(
	       char *docroot,

	       char **ptr_x_gpu_reg_X,
	       size_t *ptr_x_gpu_reg_nX,
	       GPU_REG_TYPE **ptr_x_gpu_reg,
	       int *ptr_x_n_gpu_reg

	       )
// STOP FUNC DECL
{
  int status = 0;
  char cwd[MAX_LEN_DIR_NAME+1];

  char *l_gpu_reg_X = NULL;
  size_t l_gpu_reg_nX = 0;
  GPU_REG_TYPE *l_gpu_reg = NULL;
  int l_n_gpu_reg = 0;
  
  zero_string(cwd, MAX_LEN_DIR_NAME+1);
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  if ( ( docroot == NULL ) || ( *docroot == '\0' ) ) { go_BYE(-1); }
  status = chdir(docroot); cBYE(status);

  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.gpu", &l_gpu_reg_X, &l_gpu_reg_nX, 1); cBYE(status);
  if ( l_gpu_reg_nX == 0 ) { go_BYE(-1); }
  l_n_gpu_reg = l_gpu_reg_nX / sizeof(GPU_REG_TYPE);
  if (  (l_gpu_reg_nX % sizeof(GPU_REG_TYPE) ) != 0 ) { go_BYE(-1); }
  l_gpu_reg = (GPU_REG_TYPE *)l_gpu_reg_X;
  status = chdir(cwd); cBYE(status);
  /*----------------------------------------------------------*/
  
  *ptr_x_gpu_reg_X = l_gpu_reg_X;
  *ptr_x_gpu_reg_nX = l_gpu_reg_nX;
  *ptr_x_gpu_reg = l_gpu_reg;
  *ptr_x_n_gpu_reg = l_n_gpu_reg;
  
  /*----------------------------------------------------------*/
 BYE:
  return status ;
}
// START FUNC DECL
void
g_unmap_meta_data(
		char *x_gpu_reg_X,
		size_t x_gpu_reg_nX

		)
// STOP FUNC DECL
{
  if ( x_gpu_reg_X != NULL ) { 
    munmap(x_gpu_reg_X, x_gpu_reg_nX);
  }
}

// START FUNC DECL
int
g_init_meta_data(
	       char *docroot
	       )
// STOP FUNC DECL
{
  int status = 0;
  char cwd[MAX_LEN_DIR_NAME+1];
  char   *gpu_reg_X = NULL;
  size_t gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; /* hash table for flds */
  int n_gpu_reg = 0; /* sizeof above */

  
  if ( ( docroot == NULL ) || ( *docroot == '\0' ) ) { go_BYE(-1); }
  zero_string(cwd, MAX_LEN_DIR_NAME+1);
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }

  /*----------------------------------------------------------*/
  size_t filesz = G_GPU_REG_SIZE * sizeof(GPU_REG_TYPE);
  status = mk_file(cwd, docroot, "docroot.gpu", filesz); cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.gpu", &gpu_reg_X, &gpu_reg_nX, 1); cBYE(status);
  if ( gpu_reg_nX == 0 ) { go_BYE(-1); }
  n_gpu_reg = gpu_reg_nX / sizeof(GPU_REG_TYPE);
  if (  (gpu_reg_nX % sizeof(GPU_REG_TYPE) ) != 0 ) { go_BYE(-1); }
  gpu_reg = (GPU_REG_TYPE *)gpu_reg_X;
  status = chdir(cwd); cBYE(status);
  /*----------------------------------------------------------*/
  for ( int i = 0; i < n_gpu_reg; i++ ) { 
    zero_gpu_reg(&(gpu_reg[i]));
  }
  /*----------------------------------------------------------*/
 BYE:
  return status ;
}
