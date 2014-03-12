#include "qtypes.h"
#include "mmap.h"
#include "mk_file.h"
#include "aux_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "meta_globals.h"

#define G_HT_TBL_SIZE 1609

#define G_HT_FLD_SIZE 4097

#define G_FLD_INFO_SIZE 4096

#define G_GPU_REG_SIZE 32

extern char g_cwd[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int
init_ddirs(
	   char *dsk_data_dir,
	   char *ram_data_dir
	   )
// STOP FUNC DECL
{
  int status = 0;

  /*-----------------------------------------------------------*/
  char buffer[MAX_LEN_DIR_NAME+1];
  if ( ( dsk_data_dir == NULL ) || ( *dsk_data_dir == '\0' ) ) { go_BYE(-1); }
  if ( is_absolute_path(dsk_data_dir) == false ) { go_BYE(-1); }
  if ( strlen(dsk_data_dir) >= MAX_LEN_DIR_NAME ) { go_BYE(-1); }

  /* 0th position reserved for writable disk access */
  /* 1th position reserved for writable ram  access */
  /* all others reseved for readable access  */
  status = strip_trailing_slash(dsk_data_dir, buffer, MAX_LEN_DIR_NAME+1);
  cBYE(status);
  strcpy(g_ddirs[0].name, buffer); 
  g_ddirs[0].is_writable = true;
  g_ddirs[0].is_ram      = false;

  if ( ( ram_data_dir != NULL ) && ( *ram_data_dir != '\0' ) ) { 
    if ( strcmp(ram_data_dir, dsk_data_dir) == 0 ) { go_BYE(-1); }
    if ( is_absolute_path(ram_data_dir) == false ) { go_BYE(-1); }
    if ( strlen(ram_data_dir) >= MAX_LEN_DIR_NAME ) { go_BYE(-1); }
    status = strip_trailing_slash(ram_data_dir, buffer, MAX_LEN_DIR_NAME+1);
    cBYE(status);
    strcpy(g_ddirs[1].name, buffer); 
    g_ddirs[1].is_writable = true;
    g_ddirs[1].is_ram      = true;
  }
 BYE:
  return(status);
}
/*----------------------------------------------------------*/
// START FUNC DECL
int
mmap_meta_data(
	       char *docroot,

	       char **ptr_x_tbl_X,
	       size_t *ptr_x_tbl_nX,
	       TBL_REC_TYPE **ptr_x_tbl,
	       int *ptr_x_n_tbl,

	       char **ptr_x_ht_tbl_X,
	       size_t *ptr_x_ht_tbl_nX,
	       META_KEY_VAL_TYPE **ptr_x_ht_tbl, 
	       int *ptr_x_n_ht_tbl, 

	       char **ptr_x_fld_X,
	       size_t *ptr_x_fld_nX,
	       FLD_REC_TYPE **ptr_x_fld,
	       int *ptr_x_n_fld,

	       char **ptr_x_ht_fld_X,
	       size_t *ptr_x_ht_fld_nX,
	       META_KEY_VAL_TYPE **ptr_x_ht_fld, 
	       int *ptr_x_n_ht_fld,

	       char **ptr_x_ddir_X,
	       size_t *ptr_x_ddir_nX,
	       DDIR_REC_TYPE **ptr_x_ddir,
	       int *ptr_x_n_ddir,

	       char **ptr_x_fld_info_X,
	       size_t *ptr_x_fld_info_nX,
	       FLD_INFO_TYPE **ptr_x_fld_info,
	       int *ptr_x_n_fld_info,

	       char  **ptr_x_aux_X,
	       size_t *ptr_x_aux_nX,

	       int *ptr_dsk_ddir_id,
	       int *ptr_ram_ddir_id

	       )
// STOP FUNC DECL
{
  int status = 0;

  char *l_ddir_X = NULL;
  size_t l_ddir_nX = 0;
  DDIR_REC_TYPE *l_ddir = NULL;
  int l_n_ddir = 0;

  char *l_tbl_X = NULL;
  size_t l_tbl_nX = 0;
  TBL_REC_TYPE *l_tbl = NULL;
  int l_n_tbl = 0;

  char *l_ht_tbl_X = NULL;
  size_t l_ht_tbl_nX = 0;
  META_KEY_VAL_TYPE *l_ht_tbl = NULL; 
  int l_n_ht_tbl = 0 ;

  char *l_fld_X = NULL;
  size_t l_fld_nX = 0;
  FLD_REC_TYPE *l_fld = NULL;
  int l_n_fld = 0;

  char  *l_ht_fld_X = NULL;
  size_t l_ht_fld_nX = 0;
  META_KEY_VAL_TYPE *l_ht_fld = NULL; 
  int l_n_ht_fld = 0;

  char  *l_fld_info_X = NULL;
  size_t l_fld_info_nX = 0;
  FLD_INFO_TYPE *l_fld_info = NULL;
  int l_n_fld_info = 0;
  
  char  *l_aux_X = NULL;
  size_t l_aux_nX = 0;

  if ( ( docroot == NULL ) || ( *docroot == '\0' ) ) { go_BYE(-1); }
  if ( strlen(docroot) > MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  status = chdir(docroot); cBYE(status);
  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.ht_tbl", &l_ht_tbl_X, &l_ht_tbl_nX, 1); cBYE(status);
  if ( l_ht_tbl_nX == 0 ) { go_BYE(-1); }
  l_n_ht_tbl = l_ht_tbl_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (l_ht_tbl_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  l_ht_tbl = (META_KEY_VAL_TYPE *)l_ht_tbl_X;
  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.ht_fld", &l_ht_fld_X, &l_ht_fld_nX, 1); cBYE(status);
  if ( l_ht_fld_nX == 0 ) { go_BYE(-1); }
  l_n_ht_fld = l_ht_fld_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (l_ht_fld_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  l_ht_fld = (META_KEY_VAL_TYPE *)l_ht_fld_X;
  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.ddirs", &l_ddir_X, &l_ddir_nX, 1); cBYE(status);
  if ( l_ddir_nX == 0 ) { go_BYE(-1); }
  l_n_ddir = l_ddir_nX / sizeof(DDIR_REC_TYPE);
  if (  (l_ddir_nX % sizeof(DDIR_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  l_ddir = (DDIR_REC_TYPE *)l_ddir_X;
  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.tbls", &l_tbl_X, &l_tbl_nX, 1); cBYE(status);
  if ( l_tbl_nX == 0 ) { go_BYE(-1); }
  l_n_tbl = l_tbl_nX / sizeof(TBL_REC_TYPE);
  if (  (l_tbl_nX % sizeof(TBL_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  l_tbl = (TBL_REC_TYPE *)l_tbl_X;
  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.flds", &l_fld_X, &l_fld_nX, 1); cBYE(status);
  if ( l_fld_nX == 0 ) { go_BYE(-1); }
  l_n_fld = l_fld_nX / sizeof(FLD_REC_TYPE);
  if (  (l_fld_nX % sizeof(FLD_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  l_fld = (FLD_REC_TYPE *)l_fld_X;
  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.fld_info", &l_fld_info_X, &l_fld_info_nX, 1); cBYE(status);
  if ( l_fld_info_nX == 0 ) { go_BYE(-1); }
  l_n_fld_info = l_fld_info_nX / sizeof(FLD_INFO_TYPE);
  if (  (l_fld_info_nX % sizeof(FLD_INFO_TYPE) ) != 0 ) { go_BYE(-1); }
  l_fld_info = (FLD_INFO_TYPE *)l_fld_info_X;
  /*----------------------------------------------------------*/
  status = rs_mmap("docroot.aux", &l_aux_X, &l_aux_nX, 1); cBYE(status);
  if ( l_aux_nX == 0 ) { go_BYE(-1); }
  /*----------------------------------------------------------*/

  *ptr_x_ddir_X = l_ddir_X;
  *ptr_x_ddir_nX = l_ddir_nX;
  *ptr_x_ddir = l_ddir;
  *ptr_x_n_ddir = l_n_ddir;

  *ptr_x_tbl_X = l_tbl_X;
  *ptr_x_tbl_nX = l_tbl_nX;
  *ptr_x_tbl = l_tbl;
  *ptr_x_n_tbl = l_n_tbl;

  *ptr_x_ht_tbl_X = l_ht_tbl_X;
  *ptr_x_ht_tbl_nX = l_ht_tbl_nX;
  *ptr_x_ht_tbl = l_ht_tbl;
  *ptr_x_n_ht_tbl = l_n_ht_tbl;

  *ptr_x_fld_X = l_fld_X;
  *ptr_x_fld_nX = l_fld_nX;
  *ptr_x_fld = l_fld;
  *ptr_x_n_fld = l_n_fld;

  *ptr_x_ht_fld_X = l_ht_fld_X;
  *ptr_x_ht_fld_nX = l_ht_fld_nX;
  *ptr_x_ht_fld = l_ht_fld;
  *ptr_x_n_ht_fld = l_n_ht_fld;

  *ptr_x_fld_info_X = l_fld_info_X;
  *ptr_x_fld_info_nX = l_fld_info_nX;
  *ptr_x_fld_info = l_fld_info;
  *ptr_x_n_fld_info = l_n_fld_info;
  
  *ptr_x_aux_X  = l_aux_X;
  *ptr_x_aux_nX = l_aux_nX;

  *ptr_ram_ddir_id = -1;
  *ptr_dsk_ddir_id = -1;
  *ptr_dsk_ddir_id = 0;
  /* make some checks */
  if( chk_dsk_ddir_id(l_ddir) == false ) { go_BYE(-1); }

  if ( ( l_ddir[1].name[0] != '\0' ) && 
       ( l_ddir[1].is_writable == true ) && 
       ( l_ddir[1].is_ram      == true ) ) {
    *ptr_ram_ddir_id = 1;
  }

  /*----------------------------------------------------------*/
 BYE:
  status = chdir(g_cwd); 
  return(status);
}
// START FUNC DECL
int
unmap_meta_data(
		char  *x_tbl_X,
		size_t x_tbl_nX,

		char  *x_ht_tbl_X,
		size_t x_ht_tbl_nX,

		char  *x_fld_X,
		size_t x_fld_nX,

		char  *x_ht_fld_X,
		size_t x_ht_fld_nX,

		char  *x_ddir_X,
		size_t x_ddir_nX,

		char  *x_fld_info_X,
		size_t x_fld_info_nX,

		char  *x_aux_X,
		size_t x_aux_nX

		)
// STOP FUNC DECL
{
  int status = 0;
  rs_munmap(x_tbl_X,     x_tbl_nX);
  rs_munmap(x_ht_tbl_X,  x_ht_tbl_nX);
  rs_munmap(x_fld_X,     x_fld_nX);
  rs_munmap(x_ht_fld_X,  x_ht_fld_nX);
  rs_munmap(x_ddir_X,    x_ddir_nX);
  rs_munmap(x_fld_info_X, x_fld_info_nX);
  rs_munmap(x_aux_X,     x_aux_nX);
  return(status);
}
// START FUNC DECL
int
init_meta_data(
	       char *docroot,
	       char *dsk_data_dir,
	       char *ram_data_dir
	       )
// STOP FUNC DECL
{
  int status = 0;
  size_t filesz;
  
  if ( ( docroot == NULL ) || ( *docroot == '\0' ) ) { go_BYE(-1); }
  if ( strlen(docroot) > MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  /*----------------------------------------------------------*/
  filesz = G_HT_TBL_SIZE * sizeof(META_KEY_VAL_TYPE);
  status = mk_file(g_cwd, docroot, "docroot.ht_tbl", filesz); cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.ht_tbl", &g_ht_tbl_X, &g_ht_tbl_nX, 1); 
  cBYE(status);
  status = chdir(g_cwd); cBYE(status);
  if ( g_ht_tbl_nX == 0 ) { go_BYE(-1); }
  g_n_ht_tbl = g_ht_tbl_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (g_ht_tbl_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  g_ht_tbl = (META_KEY_VAL_TYPE *)g_ht_tbl_X;
  /*----------------------------------------------------------*/
  filesz = G_HT_FLD_SIZE * sizeof(META_KEY_VAL_TYPE);
  status = mk_file(g_cwd, docroot, "docroot.ht_fld", filesz); cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.ht_fld", &g_ht_fld_X, &g_ht_fld_nX, 1); 
  cBYE(status);
  status = chdir(g_cwd); cBYE(status);
  if ( g_ht_fld_nX == 0 ) { go_BYE(-1); }
  g_n_ht_fld = g_ht_fld_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (g_ht_fld_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  g_ht_fld = (META_KEY_VAL_TYPE *)g_ht_fld_X;
  /*----------------------------------------------------------*/
  filesz = MAX_NUM_TBLS * sizeof(TBL_REC_TYPE);
  status = mk_file(g_cwd, docroot, "docroot.tbls", filesz); cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.tbls", &g_tbl_X, &g_tbl_nX, 1); cBYE(status);
  status = chdir(g_cwd); cBYE(status);
  if ( g_tbl_nX == 0 ) { go_BYE(-1); }
  g_n_tbl = g_tbl_nX / sizeof(TBL_REC_TYPE);
  if (  (g_tbl_nX % sizeof(TBL_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  g_tbls = (TBL_REC_TYPE *)g_tbl_X;
  /*----------------------------------------------------------*/
  filesz = MAX_NUM_FLDS * sizeof(FLD_REC_TYPE);
  status = mk_file(g_cwd, docroot, "docroot.flds", filesz);
  cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.flds", &g_fld_X, &g_fld_nX, 1); cBYE(status);
  status = chdir(g_cwd); cBYE(status);
  if ( g_fld_nX == 0 ) { go_BYE(-1); }
  g_n_fld = g_fld_nX / sizeof(FLD_REC_TYPE);
  if (  (g_fld_nX % sizeof(FLD_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  g_flds = (FLD_REC_TYPE *)g_fld_X;
  /*----------------------------------------------------------*/
  filesz = MAX_NUM_DDIRS * sizeof(DDIR_REC_TYPE);
  status = mk_file(g_cwd, docroot, "docroot.ddirs", filesz); cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.ddirs", &g_ddir_X, &g_ddir_nX, 1); cBYE(status);
  status = chdir(g_cwd); cBYE(status);
  if ( g_ddir_nX == 0 ) { go_BYE(-1); }
  g_n_ddir = g_ddir_nX / sizeof(DDIR_REC_TYPE);
  if (  (g_ddir_nX % sizeof(DDIR_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  g_ddirs = (DDIR_REC_TYPE *)g_ddir_X;


  filesz = G_FLD_INFO_SIZE * sizeof(FLD_INFO_TYPE);
  status = mk_file(g_cwd, docroot, "docroot.fld_info", filesz); cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.fld_info", &g_fld_info_X, &g_fld_info_nX, 1); 
  status = chdir(g_cwd); cBYE(status);
  cBYE(status);
  if ( g_fld_info_nX == 0 ) { go_BYE(-1); }
  g_n_fld_info = g_fld_info_nX / sizeof(FLD_INFO_TYPE);
  if (  (g_fld_info_nX % sizeof(FLD_INFO_TYPE) ) != 0 ) { go_BYE(-1); }
  g_fld_info = (FLD_INFO_TYPE *)g_fld_info_X;
  /*----------------------------------------------------------*/
  filesz = 2 * sizeof(int);
  status = mk_file(g_cwd, docroot, "docroot.aux", filesz); cBYE(status);
  status = chdir(docroot); cBYE(status);
  status = rs_mmap("docroot.aux", &g_aux_X, &g_aux_nX, 1); cBYE(status);
  status = chdir(g_cwd); cBYE(status);
  /*----------------------------------------------------------*/
  if ( g_ddir_nX == 0 ) { go_BYE(-1); }
  for ( int i = 0; i < g_n_ht_tbl; i++ ) { 
    g_ht_tbl[i].key = 0;
    g_ht_tbl[i].val = -1;
  }
  for ( int i = 0; i < g_n_tbl; i++ ) { 
    zero_tbl_rec(&(g_tbls[i]));
  }
  for ( int i = 0; i < g_n_ht_fld; i++ ) { 
    g_ht_fld[i].key = 0;
    g_ht_fld[i].val = -1;
  }
  for ( int i = 0; i < g_n_fld; i++ ) { 
    zero_fld_rec(&(g_flds[i]));
  }
  for ( int i = 0; i < g_n_ddir; i++ ) { 
    zero_ddir_rec(&(g_ddirs[i]));
  }
  for ( int i = 0; i < g_n_fld_info; i++ ) { 
    zero_fld_info_rec(&(g_fld_info[i]));
  }

  int *I4ptr = (int *)g_aux_X;
  *I4ptr++ = 1; // Indicating that 1 is the first file number to be used
  *I4ptr++ = 0; // Indicating that we are NOT within compound expression

  status = init_ddirs(dsk_data_dir, ram_data_dir); cBYE(status);
  /*----------------------------------------------------------*/
 BYE:
  chdir(g_cwd);
  return(status);
}
  
void
set_default_meta_data(
		      char *tbl_X,
		      size_t tbl_nX,
		      TBL_REC_TYPE *tbls,
		      int n_tbl,

		      char *ht_tbl_X,
		      size_t ht_tbl_nX,
		      META_KEY_VAL_TYPE *ht_tbl, 
		      int n_ht_tbl, 

		      char *fld_X,
		      size_t fld_nX,
		      FLD_REC_TYPE *flds,
		      int n_fld,

		      char *ht_fld_X,
		      size_t ht_fld_nX,
		      META_KEY_VAL_TYPE *ht_fld, 
		      int n_ht_fld,

		      char *ddir_X,
		      size_t ddir_nX,
		      DDIR_REC_TYPE *ddirs,
		      int n_ddir,

		      char *fld_info_X,
		      size_t fld_info_nX,
		      FLD_INFO_TYPE *fld_info, 
		      int n_fld_info

		      )
{

  g_tbl_X = tbl_X;
  g_tbl_nX = tbl_nX;
  g_tbls = tbls;
  g_n_tbl = n_tbl;

  g_ht_tbl_X = ht_tbl_X;
  g_ht_tbl_nX = ht_tbl_nX;
  g_ht_tbl = ht_tbl;
  g_n_ht_tbl = n_ht_tbl;

  g_fld_X = fld_X;
  g_fld_nX = fld_nX;
  g_flds = flds;
  g_n_fld = n_fld;

  g_ht_fld_X = ht_fld_X;
  g_ht_fld_nX = ht_fld_nX;
  g_ht_fld = ht_fld;
  g_n_ht_fld = n_ht_fld;

  g_ddir_X = ddir_X;
  g_ddir_nX = ddir_nX;
  g_ddirs = ddirs;
  g_n_ddir = n_ddir;

  g_fld_info_X = fld_info_X;
  g_fld_info_nX = fld_info_nX;
  g_fld_info = fld_info;
  g_n_fld_info = n_fld_info;

}
//
// START FUNC DECL
int
bootstrap_meta_data(
		    char *g_docroot,
		    char *bs_docroot,
		    char *dsk_data_dir,
		    char *ram_data_dir
		    )
// STOP FUNC DECL
{
  int status = 0;
  char buffer[MAX_LEN_DIR_NAME+MAX_LEN_FILE_NAME];
  
  if ( ( g_docroot  == NULL ) || ( *g_docroot  == '\0' ) ) { go_BYE(-1); }
  if ( ( bs_docroot == NULL ) || ( *bs_docroot == '\0' ) ) { go_BYE(-1); }
  if ( strlen(g_docroot) > MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  if ( strlen(bs_docroot) > MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  if ( strcmp(g_docroot, bs_docroot) == 0 ) { go_BYE(-1); }
  zero_string(buffer, (MAX_LEN_DIR_NAME+MAX_LEN_FILE_NAME));
  /*----------------------------------------------------------*/
  strcpy(buffer, g_docroot); strcat(buffer, "/docroot.ht_tbl");
  status = copy_file(bs_docroot, "docroot.ht_tbl", g_docroot);
  cBYE(status);
  /*----------------------------------------------------------*/
  strcpy(buffer, g_docroot); strcat(buffer, "/docroot.ht_fld");
  status = copy_file(bs_docroot, "docroot.ht_fld", g_docroot);
  cBYE(status);
  /*----------------------------------------------------------*/
  strcpy(buffer, g_docroot); strcat(buffer, "/docroot.tbls");
  status = copy_file(bs_docroot, "docroot.tbls", g_docroot);
  cBYE(status);
  /*----------------------------------------------------------*/
  strcpy(buffer, g_docroot); strcat(buffer, "/docroot.flds");
  status = copy_file(bs_docroot, "docroot.flds", g_docroot);
  cBYE(status);
  /*----------------------------------------------------------*/
  strcpy(buffer, g_docroot); strcat(buffer, "/docroot.ddirs");
  status = copy_file(bs_docroot, "docroot.ddirs", g_docroot);
  cBYE(status);
  /*----------------------------------------------------------*/
  strcpy(buffer, g_docroot); strcat(buffer, "/docroot.fld_info");
  status = copy_file(bs_docroot, "docroot.fld_info", g_docroot);
  cBYE(status);
  /*----------------------------------------------------------*/
  strcpy(buffer, g_docroot); strcat(buffer, "/docroot.aux");
  status = copy_file(bs_docroot, "docroot.aux", g_docroot);
  cBYE(status);
  /*----------------------------------------------------------*/
  /* TODO: P1. Think. Need to check that all files in the docroot from
   * which we are bootstrapping are in the main data directory */
  /* Mark all fields as external */
  status = mmap_meta_data(g_docroot, 
			  &g_tbl_X, &g_tbl_nX, &g_tbls, &g_n_tbl, 
			  &g_ht_tbl_X, &g_ht_tbl_nX, &g_ht_tbl, &g_n_ht_tbl, 
			  &g_fld_X, &g_fld_nX, &g_flds, &g_n_fld, 
			  &g_ht_fld_X, &g_ht_fld_nX, &g_ht_fld, &g_n_ht_fld, 
			  &g_ddir_X, &g_ddir_nX, &g_ddirs, &g_n_ddir, 
			  &g_fld_info_X, &g_fld_info_nX, &g_fld_info, &g_n_fld_info,
			  &g_aux_X, &g_aux_nX, &g_dsk_ddir_id, &g_ram_ddir_id);
  cBYE(status);
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].fileno > 0 ) { 
      g_flds[i].is_external = true;
    }
  }
  /* make sure that you have space for 2 more in ddir */
  int num_ddirs = 0;
  for ( int i = 0; i < g_n_ddir; i++ ) { 
    if ( g_ddirs[i].name[0] != '\0' ) { num_ddirs++; }
  }
  if ( ( num_ddirs < 1 ) || ( num_ddirs > (g_n_ddir-2) ) ) { go_BYE(-1); }
  /* additional safety check */
  for ( int i = 1; i < g_n_ddir; i++ ) { 
    if ( ( g_ddirs[i].name[0] != '\0' ) && ( g_ddirs[i-1].name[0] == '\0' )  ) {
      go_BYE(-1);
    }
  }
  /* shift everybody by 2 */
  for ( int i = 2; i < g_n_ddir; i++ ) { 
    g_ddirs[i] = g_ddirs[i-2];
  }
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].tbl_id >= 0 ) { 
      g_flds[i].ddir_id += 2;
    }
  }
  /*---------------------------------------*/

  // initialize first 2 spots
  zero_ddir_rec(&(g_ddirs[0]));
  zero_ddir_rec(&(g_ddirs[1]));
  // fill first 2 spots
  status = init_ddirs(dsk_data_dir, ram_data_dir); cBYE(status);
 BYE:
  return(status);
}
