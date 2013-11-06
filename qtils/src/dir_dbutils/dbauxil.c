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
#include <limits.h>
#include <unistd.h>
#include "qtypes.h"
#include "mmap.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "assign_I1.h"
#include "auxil.h"
#include "dbauxil.h"


//-------------------------------------------------------------
// START FUNC DECL
int
mk_offset(
	  int *szptr,
	  long long nR,
	  long long **ptr_offset
	  )
// STOP FUNC DECL
{
  int status = 0;
  long long *offset = NULL;
  offset = (long long *)malloc(nR * sizeof(long long));
  return_if_malloc_failed(offset);
  offset[0] = 0;
  for ( long long i = 1; i < nR; i++ ) { 
    offset[i] = offset[i-1] + szptr[i-1];
  }
  *ptr_offset = offset;
 BYE:
  return(status);
}

//--------------------------------------------------------
// START FUNC DECL
int
mk_mjoin_op(
	    char *str_mjoin_op,
	    int *ptr_mjoin_op
	    )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_mjoin_op == NULL ) { go_BYE(-1); }
  if ( strcmp(str_mjoin_op, "sum") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_SUM;
  }
  else if ( strcmp(str_mjoin_op, "min") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_MIN;
  }
  else if ( strcmp(str_mjoin_op, "max") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_MAX;
  }
  else if ( strcmp(str_mjoin_op, "reg") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_REG;
  }
  else if ( strcmp(str_mjoin_op, "or") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_OR;
  }
  else if ( strcmp(str_mjoin_op, "and") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_AND;
  }
  else if ( strcmp(str_mjoin_op, "cnt") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_CNT;
  }
  else { 
    fprintf(stderr, "Unknown join operation [%s] \n", str_mjoin_op);
    *ptr_mjoin_op = -1; 
    go_BYE(-1); 
  }
 BYE:
  return(status);
}
//--------------------------------------------------------
// START FUNC DECL
int
mk_iop(
       char *str_op,
       int *ptr_iop
       )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_op == NULL ) { go_BYE(-1); }
  if ( strcmp(str_op, "+") == 0 ) { *ptr_iop = IOP_ADD; }
  else if ( strcmp(str_op, "-") == 0 ) { *ptr_iop = IOP_SUB; }
  else if ( strcmp(str_op, "*") == 0 ) { *ptr_iop = IOP_MUL; }
  else if ( strcmp(str_op, "/") == 0 ) { *ptr_iop = IOP_DIV; }
  else if ( strcmp(str_op, ">") == 0 ) { *ptr_iop = IOP_GT; }
  else if ( strcmp(str_op, "<") == 0 ) { *ptr_iop = IOP_LT; }
  else if ( strcmp(str_op, ">=") == 0 ) { *ptr_iop = IOP_GEQ; }
  else if ( strcmp(str_op, "<=") == 0 ) { *ptr_iop = IOP_LEQ; }
  else if ( strcmp(str_op, "!=") == 0 ) { *ptr_iop = IOP_NEQ; }
  else if ( strcmp(str_op, "==") == 0 ) { *ptr_iop = IOP_EQ; }
  else if ( strcmp(str_op, "&&") == 0 ) { *ptr_iop = BOP_AND; }
  else if ( strcmp(str_op, "||") == 0 ) { *ptr_iop = BOP_OR; }
  else { *ptr_iop = -1; go_BYE(-1); }

 BYE:
  return(status);
}
//--------------------------------------------------------
// START FUNC DECL
void
pr_disp_name(
	     FILE *ofp,
	     char *disp_name
	     )
// STOP FUNC DECL
{
  if ( ( disp_name == NULL ) || ( *disp_name == '\0' ) ) { 
    return;
  }
  for ( int j = 0; disp_name[j] != '\0'; j++ ) {
    if ( ( disp_name[j] == '\\' ) || 
	 ( disp_name[j] == '"' ) ) {
      fprintf(stdout, "\\"); 
    }
    fprintf(stdout, "%c", disp_name[j]);
  }
}
// START FUNC DECL
int
mk_int_auxtype(
	       char *str_auxtype, 
	       int *ptr_auxtype
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( ( str_auxtype == NULL ) || ( *str_auxtype == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(str_auxtype, "nn") == 0 ) { 
    *ptr_auxtype = AUX_FLD_NN;
  }
  else if ( strcmp(str_auxtype, "sz") == 0 ) { 
    *ptr_auxtype = AUX_FLD_SZ;
  }
  else { go_BYE(-1); }
 BYE:
  return(status);
}
#undef XXX
#ifdef XXX
// I do not think these are needed any more
// START FUNC DECL
int
del_ht_tbl_entry(
		 int tbl_id
		 )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(tbl_id, 0, g_n_tbl);
  for ( int i = 0; i < g_n_ht_tbl; i++ ) { 
    if ( g_ht_tbl[i].val == tbl_id ) { 
      g_ht_tbl[i].val = -1;
      g_ht_tbl[i].key = 0;
      break;
    }
  }
 BYE:
  return(status);
}
// START FUNC DECL
int
del_ht_fld_entry(
		 int fld_id
		 )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(fld_id, 0, g_n_fld);
  for ( int i = 0; i < g_n_ht_fld; i++ ) { 
    if ( g_ht_fld[i].val == fld_id ) { 
      g_ht_fld[i].val = -1;
      g_ht_fld[i].key = 0;
      break;
    }
  }
 BYE:
  return(status);
}
#endif
// START FUNC DECL
int
del_aux_fld(
	    int primary_fld_id,
	    AUX_TYPE auxtype
	    )
// STOP FUNC DECL
{
  int status = 0;
  int aux_fld_id = INT_MIN;

  chk_range(primary_fld_id, 0, g_n_fld);
  if ( auxtype == nn ) { 
    aux_fld_id = g_flds[primary_fld_id].nn_fld_id;
    g_flds[primary_fld_id].nn_fld_id = -1;
  }
  else {
    go_BYE(-1);
  }
  if ( aux_fld_id >= 0 ) { 
    if ( g_flds[primary_fld_id].is_external == false ) { 
      unlink(g_flds[aux_fld_id].filename);
    }
    zero_fld_rec(&(g_flds[aux_fld_id]));
  }
 BYE:
  return(status);
}
// START FUNC DECL
int
mk_name_aux_fld(
		char *fld,
		AUX_TYPE auxtype,
		char *aux_fld
		)
// STOP FUNC DECL
{
  int status = 0;

  if ( fld == NULL ) { go_BYE(-1); }
  int len = strlen(fld);
  switch ( auxtype ) { 
  case nn : 
    len += strlen("_nn_");
    if ( len > MAX_LEN_FLD_NAME ) { go_BYE(-1); }
    strcpy(aux_fld, "_nn_");
    strcat(aux_fld, fld);
    break;
  default : 
    go_BYE(-1);
    break;
  }
  status = chk_fld_name(aux_fld, 1); cBYE(status);
 BYE:
  return(status);
}

extern bool g_write_to_temp_dir;
// START FUNC DECL
int
chk_if_ephemeral(
		 char **ptr_fld
		 )
// STOP FUNC DECL
{
  int status = 0;
  char *fld = *ptr_fld;
  status = chk_fld_name(fld, 0); cBYE(status);
  if ( strncmp(fld, "ephemeral:", 10) == 0 ) {
    g_write_to_temp_dir = true;
    fld += 10;
  }
  *ptr_fld = fld;
 BYE:
  return(status);
}

// START FUNC DECL
int
chk_file_size(
	      char *filename,
	      long long nR,
	      FLD_TYPE fldtype
	      )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  int fldsz = -1;

  if ( fldtype == clob ) { return(status); }
  status = rs_mmap(filename, &X, &nX, 0); cBYE(status);
  if ( X == NULL ) { go_BYE(-1); }
    
  if ( fldtype == B ) { 
    long long filesz = -1;
    status = get_file_size_B(nR, &filesz);
    if ( nX != filesz ) { go_BYE(-1); }
  }
  else {
    status = get_fld_sz(fldtype, &fldsz);
    if ( ( fldsz * nR ) != nX ) { go_BYE(-1); }
  }
 BYE:
  rs_munmap(X, nX);
  return(status);
}
extern int g_num_cores;
// START FUNC DECL
int partition(
	      long long num_to_divide,
	      long long min_block_size,
	      int max_num_chunks,
	      long long *ptr_block_size,
	      int *ptr_num_chunks /* this many chunks of work */
	      )
// STOP FUNC DECL
{
  int status = 0;
  int num_chunks;
  long long block_size;

  if ( num_to_divide <= 0 ) { go_BYE(-1); }
  if ( min_block_size <= 0 ) { go_BYE(-1); }
  num_chunks = num_to_divide / min_block_size;
  if ( num_chunks == 0 ) { num_chunks = 1; }
  if ( max_num_chunks > 0 ) {
    if ( num_chunks > max_num_chunks ) {
      num_chunks = max_num_chunks;
    }
  }
  block_size = num_to_divide / num_chunks;
  if ( num_to_divide - (block_size * num_chunks) >= block_size ) {
    go_BYE(-1);
  }
  // fprintf(stderr, "number of chunks = %d \n", num_chunks);
  // fprintf(stderr, "block size       = %lld \n", block_size);
  // fprintf(stderr, "num_to_divide = %lld \n", num_to_divide);

  if ( ( block_size * (num_chunks+1) ) < num_to_divide ) { go_BYE(-1); }
  *ptr_block_size = block_size;
  *ptr_num_chunks = num_chunks;
BYE:
  return(status);
}

extern char *g_data_dir; 
extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
get_data(
	 FLD_REC_TYPE fld_rec,
	 char **ptr_X,
	 size_t *ptr_nX,
	 bool is_write
	 )
// STOP FUNC DECL
{
  int status = 0;

  if (fld_rec.filename[0] == '\0' ) { go_BYE(-1); }
  if ( fld_rec.ddir_id >= 0 ) { 
    if ( fld_rec.ddir_id >= g_n_ddir ) { go_BYE(-1); }
    char *alt_data_dir = g_ddirs[fld_rec.ddir_id].name;
    if ( ( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' ) ) { go_BYE(-1); }
    chdir(alt_data_dir); 
  }
  else {
    if ( ( g_data_dir == NULL ) || ( *g_data_dir == '\0' ) ) { go_BYE(-1); }
    chdir(g_data_dir); 
  }
  status = rs_mmap(fld_rec.filename, ptr_X, ptr_nX, is_write);
  cBYE(status);
  if ( *ptr_nX == 0 ) { go_BYE(-1); }
  chdir(g_cwd); 
 BYE:
  return(status);
}
// START FUNC DECL
int 
q_mmap(
       int ddir_id,
       char *filename,
       char **ptr_X,
       size_t *ptr_nX,
       bool  is_write
       )
// STOP FUNC DECL
{
  int status = 0;
  if ( ddir_id < 0 ) { 
    chdir(g_data_dir); 
  }
  else {
    char *alt_data_dir = g_ddirs[ddir_id].name;
    if ( ( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' ) ) { go_BYE(-1); }
    chdir(alt_data_dir); 
  }
  status = rs_mmap(filename, ptr_X, ptr_nX, is_write); cBYE(status); 
  chdir(g_cwd); 
 BYE:
  return(status);
}
// START FUNC DECL
int 
q_re_mmap(
	  int ddir_id,
	  char *filename,
	  char **ptr_X,
	  size_t *ptr_nX,
	  size_t new_filesize,
	  bool  is_write
	  )
// STOP FUNC DECL
{
  int status = 0;
  FILE  *fp = NULL;
  size_t old_filesize = *ptr_nX;
#define NULL_BUF_SIZE 4096
  char nullbuf[NULL_BUF_SIZE];

#ifdef IPP
  ippsZero_8u(nullbuf, NULL_BUF_SIZE);
#else
  assign_const_I1(nullbuf, NULL_BUF_SIZE, I1);
#endif
  if ( new_filesize <= 0 ) { go_BYE(-1); }
  if ( old_filesize <= 0 ) { go_BYE(-1); }
  if ( new_filesize <= old_filesize ) { go_BYE(-1); }
  if ( ddir_id < 0 ) { 
    chdir(g_data_dir); 
  }
  else {
    char *alt_data_dir = g_ddirs[ddir_id].name;
    if ( ( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' ) ) { go_BYE(-1); }
    chdir(alt_data_dir); 
  }
  munmap(*ptr_X, *ptr_nX); *ptr_X = NULL; *ptr_nX = 0;
  // Stretch the file to the new file size
  fp = fopen(filename, "ab");
  return_if_fopen_failed(fp,  filename, "ab");
  size_t increase_by = new_filesize - old_filesize;
  int num_chunks = increase_by / NULL_BUF_SIZE; 
  int leftover = increase_by - (NULL_BUF_SIZE * num_chunks);
  for ( int i = 0; i < num_chunks; i++ ) { 
    fwrite(nullbuf, sizeof(char), NULL_BUF_SIZE, fp);
  }
  if ( leftover > 0 ) { 
    fwrite(nullbuf, sizeof(char), leftover, fp);
  }
  fclose_if_non_null(fp);
  //-------------------------------------
#ifdef EXPT
  // Following does not do the stretch as expected. TODO P3
  int fd = open(filename, O_RDWR | O_APPEND, (mode_t)0600);
  if ( fd < 0 ) { go_BYE(-1); }
  /* Stretch the file to new_filesize */
  int result = lseek(fd, new_filesize - 1, SEEK_SET);
  if ( result  < 0 ) { 
    close(fd);
    fprintf(stderr, "Error calling lseek() to 'stretch' the file\n");
    go_BYE(-1);
  }
#endif
  status = rs_mmap(filename, ptr_X, ptr_nX, is_write); cBYE(status);
  // fprintf(stderr, "DBG: New file size is %lld \n", (long long)(*ptr_nX));

  chdir(g_cwd); 
 BYE:
  fclose_if_non_null(fp);
  return(status);
}
// START FUNC DECL
int 
q_delete(
	 int ddir_id,
	 char *filename
	 )
// STOP FUNC DECL
{
  int status = 0;
  if ( ddir_id < 0 ) { 
    chdir(g_data_dir); 
  }
  else {
    char *alt_data_dir = g_ddirs[ddir_id].name;
    if ( ( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' ) ) { go_BYE(-1); }
    chdir(alt_data_dir); 
  }
  unlink(filename);
  chdir(g_cwd); 
 BYE:
  return(status);
}
#include<sys/stat.h>
// START FUNC DECL
int 
q_trunc(
	int ddir_id, 
	char *filename,
	size_t filesz
	)
// STOP FUNC DECL
{
  int status = 0;
  struct stat st;

  if ( filesz == 0 ) { unlink(filename); }
  if ( ( filename == NULL )  || ( *filename == '\0' ) ) { go_BYE(-1); }
  if ( ddir_id < 0 ) { 
    chdir(g_data_dir); 
  }
  else {
    char *alt_data_dir = g_ddirs[ddir_id].name;
    if ( ( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' ) ) { go_BYE(-1); }
    chdir(alt_data_dir); 
  }
  status = stat(filename, &st ); 
  if ( status < 0 ) { 
    fprintf(stderr, "Could not find file = [%s] \n", filename);
  }
  cBYE(status);
  size_t curr_size = st.st_size;
  if ( filesz > curr_size ) { go_BYE(-1); }
  if ( filesz < curr_size ) { 
    status = truncate(filename, filesz);  cBYE(status);
  }
  chdir(g_cwd); 
 BYE:
  return(status);
}
// START FUNC DECL
int
chk_aux_info(
	     char *aux_info
	     )
// STOP FUNC DECL
{
  int status = 0;
  int cnt_open_sq_bracket = 0 ;
  int cnt_close_sq_bracket = 0 ;
  for ( char *cptr = aux_info; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == '[' ) { cnt_open_sq_bracket++; } 
    if ( *cptr == ']' ) { cnt_close_sq_bracket++; } 
  }
  if ( ( cnt_open_sq_bracket == 0 ) || 
       ( cnt_open_sq_bracket != cnt_close_sq_bracket ) ) {
    go_BYE(-1);
  }
 BYE:
  return(status);
}
// START FUNC DECL
int
get_file_size_B(
		long long nR, 
		long long *ptr_filesz
		)
// STOP FUNC DECL
{
  int status = 0;

  if ( nR <= 0 ) { go_BYE(-1); }
  long long nbytes, nKB;
  nbytes = nR / 8;
  if ( ( nbytes * 8 ) != nR ) { nbytes++; }
  nKB = nbytes / 1024; 
  if ( ( nKB * 1024 ) != nbytes ) { nKB++; }
  *ptr_filesz = nKB * 1024; 
 BYE:
  return(status);
}

#include "assign_I1.h"
#include "sort_asc_I8.h"
#include "sort_asc_F8.h"

// START FUNC DECL
  int
  break_into_scalars(
      char *str_scalar,
      FLD_TYPE fldtype,
      int *ptr_num_scalar_vals,
  long long *ptr_sval_I8,
  int       *ptr_sval_I4,
  short     *ptr_sval_I2,
  char      *ptr_sval_I1,
  float     *ptr_sval_F4,
  double    *ptr_sval_F8,
  long long svals_I8[MAX_NUM_SCALARS], 
  int       svals_I4[MAX_NUM_SCALARS], 
  short     svals_I2[MAX_NUM_SCALARS], 
  char      svals_I1[MAX_NUM_SCALARS], 
  float     svals_F4[MAX_NUM_SCALARS], 
  double    svals_F8[MAX_NUM_SCALARS]
  )
// STOP FUNC DECL
  {
    int status = 0;
  int num_scalar_vals = 1;/* Number of multiple scalar values */
  long long sval_I8 = LLONG_MAX;
  int       sval_I4 = INT_MAX;
  short     sval_I2 = SHRT_MAX;
  char      sval_I1 = SCHAR_MAX;
  float     sval_F4 = FLT_MAX;
  double    sval_F8 = DBL_MAX;
  char *endptr;
  char **Y = NULL; int nY = 0;

  for ( char *cptr = str_scalar; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) {
      num_scalar_vals++;
    }
  }
  switch ( fldtype ) { 
  case I1 : 
  case I2 : 
  case I4 : 
  case I8 : 
    if ( num_scalar_vals == 1 ) { 
      sval_I8 = strtoll(str_scalar, &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
    }
    else {
      status = break_str(str_scalar, ":", &Y, &nY); cBYE(status);
      if ( nY != num_scalar_vals ) { go_BYE(-1); }
      for ( int i = 0; i < num_scalar_vals; i++ ) { 
	svals_I8[i] = strtoll(Y[i], &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	free_if_non_null(Y[i]);
      }
      free_if_non_null(Y);
      qsort(svals_I8, num_scalar_vals, sizeof(long long), sort_asc_I8);
    }
    break;
  case F4 : 
  case F8 : 
    if ( num_scalar_vals == 1 ) { 
      sval_F8 = strtod(str_scalar, &endptr);
      if ( *endptr != '\0' ) { go_BYE(-1); }
    }
    else {
      status = break_str(str_scalar, ":", &Y, &nY); cBYE(status);
      if ( nY != num_scalar_vals ) { go_BYE(-1); }
      for ( int i = 0; i < num_scalar_vals; i++ ) { 
	svals_F8[i] = strtod(Y[i], &endptr);
	if ( *endptr != '\0' ) { go_BYE(-1); }
	free_if_non_null(Y[i]);
      }
      free_if_non_null(Y);
      qsort(svals_F8, num_scalar_vals, sizeof(long long), sort_asc_F8);
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
  
  switch ( fldtype ) { 
  case I1 : 
    if ( num_scalar_vals == 1 ) {
      if ( sval_I8 > SCHAR_MAX ) { go_BYE(-1); }
      if ( sval_I8 < SCHAR_MIN ) { go_BYE(-1); }
      sval_I1 = sval_I8;
    }
    else {
      for ( int i = 0; i < num_scalar_vals; i++ ) { 
	if ( svals_I8[i] > SCHAR_MAX ) { go_BYE(-1); }
	if ( svals_I8[i] < SCHAR_MIN ) { go_BYE(-1); }
	svals_I1[i] = svals_I8[i];
      }
    }
    break;
  case I2 : 
    if ( num_scalar_vals == 1 ) {
      if ( sval_I8 > SHRT_MAX ) { go_BYE(-1); }
      if ( sval_I8 < SHRT_MIN ) { go_BYE(-1); }
      sval_I2 = sval_I8;
    }
    else {
      for ( int i = 0; i < num_scalar_vals; i++ ) { 
	if ( svals_I8[i] > SHRT_MAX ) { go_BYE(-1); }
	if ( svals_I8[i] < SHRT_MIN ) { go_BYE(-1); }
	svals_I2[i] = svals_I8[i];
      }
    }
  case I4 : 
    if ( num_scalar_vals == 1 ) {
      if ( sval_I8 > INT_MAX ) { go_BYE(-1); }
      if ( sval_I8 < INT_MIN ) { go_BYE(-1); }
      sval_I4 = sval_I8;
    }
    else {
      for ( int i = 0; i < num_scalar_vals; i++ ) { 
	if ( svals_I8[i] > INT_MAX ) { go_BYE(-1); }
	if ( svals_I8[i] < INT_MIN ) { go_BYE(-1); }
	svals_I4[i] = svals_I8[i];
      }
    }
    break;
  case I8 : 
    /* Nothing to do */
    break;
  case F4 : 
    if ( num_scalar_vals == 1 ) {
      if ( sval_F8 > FLT_MAX ) { go_BYE(-1); }
      if ( sval_F8 < FLT_MIN ) { go_BYE(-1); }
      sval_F4 = sval_F8;
    }
    else {
      for ( int i = 0; i < num_scalar_vals; i++ ) { 
	if ( svals_F8[i] > FLT_MAX ) { go_BYE(-1); }
	if ( svals_F8[i] < FLT_MIN ) { go_BYE(-1); }
	svals_F4[i] = svals_F8[i];
      }
    }
    break;
  case F8 : 
    /* Nothing to do */
    break;
  default : 
    go_BYE(-1);
    break;
  }
  *ptr_num_scalar_vals = num_scalar_vals;
  *ptr_sval_I8 = sval_I8;
  *ptr_sval_I4 = sval_I4;
  *ptr_sval_I2 = sval_I2;
  *ptr_sval_I1 = sval_I1;
  *ptr_sval_F4 = sval_F4;
  *ptr_sval_F8 = sval_F8;
BYE:
  if ( Y != NULL ) { 
    for ( int i = 0; i < nY; i++ ) { 
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  return(status);
}
