/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#include "qtypes.h"
#include "mmap.h"
#include "mk_file.h"
#include "aux_meta.h"
#include "meta_globals.h"

#define G_HT_TBL_SIZE 1609

#define G_HT_FLD_SIZE 4097


extern char g_cwd[MAX_LEN_DIR_NAME+1];

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
     int *ptr_x_n_ddir

     )
// STOP FUNC DECL
{
  int status = 0;
  char buffer[MAX_LEN_DIR_NAME+MAX_LEN_FILE_NAME];

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
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ddirs");
  status = rs_mmap(buffer, &l_ddir_X, &l_ddir_nX, 1); cBYE(status);
  if ( l_ddir_nX == 0 ) { go_BYE(-1); }
  l_n_ddir = l_ddir_nX / sizeof(DDIR_REC_TYPE);
  if (  (l_ddir_nX % sizeof(DDIR_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  l_ddir = (DDIR_REC_TYPE *)l_ddir_X;
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
     size_t x_ht_fld_nX,

     char *x_ddir_X,
     size_t x_ddir_nX

     )
// STOP FUNC DECL
{
  int status = 0;
  rs_munmap(x_tbl_X, x_tbl_nX);
  rs_munmap(x_ht_tbl_X, x_ht_tbl_nX);
  rs_munmap(x_fld_X, x_fld_nX);
  rs_munmap(x_ht_fld_X, x_ht_fld_nX);
  rs_munmap(x_ddir_X, x_ddir_nX);
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

  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ht_tbl");
  status = mk_file(g_cwd, docroot, buffer, G_HT_TBL_SIZE * sizeof(META_KEY_VAL_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &g_ht_tbl_X, &g_ht_tbl_nX, 1); cBYE(status);
  if ( g_ht_tbl_nX == 0 ) { go_BYE(-1); }
  g_n_ht_tbl = g_ht_tbl_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (g_ht_tbl_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  g_ht_tbl = (META_KEY_VAL_TYPE *)g_ht_tbl_X;
  // TODO: Do the initialization here
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ht_fld");
  status = mk_file(g_cwd, docroot, buffer, G_HT_FLD_SIZE * sizeof(META_KEY_VAL_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &g_ht_fld_X, &g_ht_fld_nX, 1); cBYE(status);
  if ( g_ht_fld_nX == 0 ) { go_BYE(-1); }
  g_n_ht_fld = g_ht_fld_nX / sizeof(META_KEY_VAL_TYPE);
  if (  (g_ht_fld_nX % sizeof(META_KEY_VAL_TYPE) ) != 0 ) { go_BYE(-1); }
  g_ht_fld = (META_KEY_VAL_TYPE *)g_ht_fld_X;
  // TODO: Do the initialization here
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.tbls");
  status = mk_file(g_cwd, docroot, buffer, MAX_NUM_TBLS * sizeof(TBL_REC_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &g_tbl_X, &g_tbl_nX, 1); cBYE(status);
  if ( g_tbl_nX == 0 ) { go_BYE(-1); }
  g_n_tbl = g_tbl_nX / sizeof(TBL_REC_TYPE);
  if (  (g_tbl_nX % sizeof(TBL_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  g_tbls = (TBL_REC_TYPE *)g_tbl_X;
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.flds");
  status = mk_file(g_cwd, docroot, buffer, MAX_NUM_FLDS * sizeof(FLD_REC_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &g_fld_X, &g_fld_nX, 1); cBYE(status);
  if ( g_fld_nX == 0 ) { go_BYE(-1); }
  g_n_fld = g_fld_nX / sizeof(FLD_REC_TYPE);
  if (  (g_fld_nX % sizeof(FLD_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  g_flds = (FLD_REC_TYPE *)g_fld_X;
  /*----------------------------------------------------------*/
  strcpy(buffer, docroot); strcat(buffer, "/docroot.ddirs");
  status = mk_file(g_cwd, docroot, buffer, MAX_NUM_DDIRS * sizeof(DDIR_REC_TYPE));
  cBYE(status);
  status = rs_mmap(buffer, &g_ddir_X, &g_ddir_nX, 1); cBYE(status);
  if ( g_ddir_nX == 0 ) { go_BYE(-1); }
  g_n_ddir = g_ddir_nX / sizeof(DDIR_REC_TYPE);
  if (  (g_ddir_nX % sizeof(DDIR_REC_TYPE) ) != 0 ) { go_BYE(-1); }
  g_ddirs = (DDIR_REC_TYPE *)g_ddir_X;
  /*----------------------------------------------------------*/
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
  /*----------------------------------------------------------*/
 BYE:
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
     int n_ddir
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

}
