#include "qtypes.h"
#include "mmap.h"
#include "mk_file.h"
#include "aux_meta.h"

#define G_HT_TBL_SIZE 1609
#define G_TBL_SIZE      1024

#define G_HT_FLD_SIZE 4097
#define G_FLD_SIZE      4096

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
     int *ptr_x_n_ht_fld
     )
// STOP FUNC DECL
{
  int status = 0;
  char buffer[MAX_LEN_DIR_NAME+MAX_LEN_FILE_NAME];

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

  char *l_ht_fld_X = NULL;
  size_t l_ht_fld_nX = 0;
  META_KEY_VAL_TYPE *l_ht_fld = NULL; 
  int l_n_ht_fld = 0;
  
  if ( ( docroot == NULL ) || ( *docroot == '\0' ) ) { go_BYE(-1); }
  if ( strlen(docroot) > MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  zero_string(buffer, (MAX_LEN_DIR_NAME+MAX_LEN_FILE_NAME));
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ht_tbl");
  status = rs_mmap(buffer, &l_ht_tbl_X, &l_ht_tbl_nX, 1); cBYE(status);
  if ( l_ht_tbl_nX == 0 ) { go_BYE(-1); }
  l_n_ht_tbl = l_ht_tbl_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (l_ht_tbl_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  l_ht_tbl = (META_KEY_VAL_TYPE *)l_ht_tbl_X;
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ht_fld");
  status = rs_mmap(buffer, &l_ht_fld_X, &l_ht_fld_nX, 1); cBYE(status);
  if ( l_ht_fld_nX == 0 ) { go_BYE(-1); }
  l_n_ht_fld = l_ht_fld_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (l_ht_fld_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  l_ht_fld = (META_KEY_VAL_TYPE *)l_ht_fld_X;
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.tbls");
  status = rs_mmap(buffer, &l_tbl_X, &l_tbl_nX, 1); cBYE(status);
  if ( l_tbl_nX == 0 ) { go_BYE(-1); }
  l_n_tbl = l_tbl_nX / sizeof(TBL_REC_TYPE);
  if (  (l_tbl_nX % sizeof(TBL_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  l_tbl = (TBL_REC_TYPE *)l_tbl_X;
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.flds");
  status = rs_mmap(buffer, &l_fld_X, &l_fld_nX, 1); cBYE(status);
  if ( l_fld_nX == 0 ) { go_BYE(-1); }
  l_n_fld = l_fld_nX / sizeof(FLD_REC_TYPE);
  if (  (l_fld_nX % sizeof(FLD_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  l_fld = (FLD_REC_TYPE *)l_fld_X;
  /*----------------------------------------------------------*/

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
  
  /*----------------------------------------------------------*/
 BYE:
  return(status);
}
// START FUNC DECL
int
unmap_meta_data(
     char *x_tbl_X,
     size_t x_tbl_nX,

     char *x_ht_tbl_X,
     size_t x_ht_tbl_nX,

     char *x_fld_X,
     size_t x_fld_nX,

     char *x_ht_fld_X,
     size_t x_ht_fld_nX
     )
// STOP FUNC DECL
{
  int status = 0;
  rs_munmap(x_tbl_X, x_tbl_nX);
  rs_munmap(x_ht_tbl_X, x_ht_tbl_nX);
  rs_munmap(x_fld_X, x_fld_nX);
  rs_munmap(x_ht_fld_X, x_ht_fld_nX);
  return(status);
}
// START FUNC DECL
int
init_meta_data(
    char *docroot
     )
// STOP FUNC DECL
{
  int status = 0;
  char buffer[MAX_LEN_DIR_NAME+MAX_LEN_FILE_NAME];
  
  if ( ( docroot == NULL ) || ( *docroot == '\0' ) ) { go_BYE(-1); }
  if ( strlen(docroot) > MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  zero_string(buffer, (MAX_LEN_DIR_NAME+MAX_LEN_FILE_NAME));

#include "meta_map.h"
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ht_tbl");
  status = mk_file(buffer, G_HT_TBL_SIZE * sizeof(META_KEY_VAL_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &ht_tbl_X, &ht_tbl_nX, 1); cBYE(status);
  if ( ht_tbl_nX == 0 ) { go_BYE(-1); }
  n_ht_tbl = ht_tbl_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (ht_tbl_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  ht_tbl = (META_KEY_VAL_TYPE *)ht_tbl_X;
  // TODO: Do the initialization here
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ht_fld");
  status = mk_file(buffer, G_HT_FLD_SIZE * sizeof(META_KEY_VAL_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &ht_fld_X, &ht_fld_nX, 1); cBYE(status);
  if ( ht_fld_nX == 0 ) { go_BYE(-1); }
  n_ht_fld = ht_fld_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (ht_fld_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  ht_fld = (META_KEY_VAL_TYPE *)ht_fld_X;
  // TODO: Do the initialization here
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.tbls");
  status = mk_file(buffer, G_TBL_SIZE * sizeof(TBL_REC_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &tbl_X, &tbl_nX, 1); cBYE(status);
  if ( tbl_nX == 0 ) { go_BYE(-1); }
  n_tbl = tbl_nX / sizeof(TBL_REC_TYPE);
  if (  (tbl_nX % sizeof(TBL_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  tbls = (TBL_REC_TYPE *)tbl_X;
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.flds");
  status = mk_file(buffer, G_FLD_SIZE * sizeof(FLD_REC_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &fld_X, &fld_nX, 1); cBYE(status);
  if ( fld_nX == 0 ) { go_BYE(-1); }
  n_fld = fld_nX / sizeof(FLD_REC_TYPE);
  if (  (fld_nX % sizeof(FLD_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  flds = (FLD_REC_TYPE *)fld_X;
  /*----------------------------------------------------------*/
  for ( int i = 0; i < n_ht_tbl; i++ ) { 
    ht_tbl[i].key = 0;
    ht_tbl[i].val = -1;
  }
  for ( int i = 0; i < n_tbl; i++ ) { 
    zero_tbl_rec(&(tbls[i]));
  }
  for ( int i = 0; i < n_ht_fld; i++ ) { 
    ht_fld[i].key = 0;
    ht_fld[i].val = -1;
  }
  for ( int i = 0; i < n_fld; i++ ) { 
    zero_fld_rec(&(flds[i]));
  }
  /*----------------------------------------------------------*/
 BYE:
  return(status);
}
  
