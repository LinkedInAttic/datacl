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
extern int
mmap_meta_data(
     char *dir,

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

     );
extern int
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

     );
extern int
init_meta_data(
    char *docroot
     );
extern void
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
     );
