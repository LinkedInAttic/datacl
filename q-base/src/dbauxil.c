#include <limits.h>
#include <unistd.h>
#include "qtypes.h"
#include "mmap.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "assign_I1.h"
#include "auxil.h"
#include "dbauxil.h"
#include "mk_hashes_I1.h"
#include "mk_hashes_I2.h"
#include "mk_hashes_I4.h"
#include "mk_hashes_I8.h"
#include "qsort_dsc_I8.h"
#include "qsort_asc_I8.h"

extern char g_cwd[MAX_LEN_DIR_NAME+1];
extern char g_docroot[MAX_LEN_DIR_NAME+1];
//--------------------------------------------------------
// START FUNC DECL
int
mk_join_op(
	   char *str_join_op,
	   int *ptr_join_op
	   )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_join_op == NULL ) { go_BYE(-1); }
  if ( ptr_join_op == NULL ) { go_BYE(-1); }
  if ( strcmp(str_join_op, "sum") == 0 ) {
    *ptr_join_op = join_sum;
  }
  else if ( strcmp(str_join_op, "min") == 0 ) {
    *ptr_join_op = join_min;
  }
  else if ( strcmp(str_join_op, "max") == 0 ) {
    *ptr_join_op = join_max;
  }
  else if ( strcmp(str_join_op, "reg") == 0 ) {
    *ptr_join_op = join_reg;
  }
  else if ( strcmp(str_join_op, "or") == 0 ) {
    *ptr_join_op = join_or;
  }
  else if ( strcmp(str_join_op, "and") == 0 ) {
    *ptr_join_op = join_and;
  }
  else if ( strcmp(str_join_op, "cnt") == 0 ) {
    *ptr_join_op = join_cnt;
  }
  else if ( strcmp(str_join_op, "minidx") == 0 ) {
    *ptr_join_op = join_minidx;
  }
  else if ( strcmp(str_join_op, "exists") == 0 ) {
    *ptr_join_op = join_exists;
  }
  else if ( strcmp(str_join_op, "maxidx") == 0 ) {
    *ptr_join_op = join_maxidx;
  }
  else { 
    fprintf(stderr, "Unknown join operation [%s] \n", str_join_op);
    *ptr_join_op = join_undef;
    go_BYE(-1); 
  }
 BYE:
  return status ;
}
//--------------------------------------------------------
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
  return status ;
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
  return status ;
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
  char filename[32];  zero_string(filename, 32);

  chk_range(primary_fld_id, 0, g_n_fld);
  switch ( auxtype ) { 
  case  nn : 
    aux_fld_id = g_flds[primary_fld_id].nn_fld_id; 
    break; 
  case len : 
    status = get_fld_info(fk_fld_len, primary_fld_id, &aux_fld_id); cBYE(status); 
    break; 
  case off : 
    status = get_fld_info(fk_fld_off, primary_fld_id, &aux_fld_id); cBYE(status); 
    break; 
  default : go_BYE(-1); break;
  }
  if ( aux_fld_id < 0 ) { return status ; }
  g_flds[primary_fld_id].nn_fld_id = -1;

  if ( aux_fld_id >= 0 ) { 
    if ( g_flds[primary_fld_id].is_external == false ) { 
      if ( g_flds[aux_fld_id].is_external != false ) {  go_BYE(-1); }
      status = switch_dirs(g_flds[aux_fld_id].ddir_id); cBYE(status);
      mk_file_name(filename, g_flds[aux_fld_id].fileno); 
      unlink(filename);
      status = chdir(g_cwd); cBYE(status);
    }
    zero_fld_rec(&(g_flds[aux_fld_id]));
  }
 BYE:
  chdir(g_cwd); 
  return status ;
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
  if ( aux_fld == NULL ) { go_BYE(-1); }
  status = chk_fld_name(fld); cBYE(status);
  int fldlen = strlen(fld);
  char str_auxtype[8];
  status = mk_str_auxtype(auxtype, str_auxtype);
  switch ( auxtype ) { 
  case nn : 
  case len : 
  case off : 
    fldlen += (2 + strlen(str_auxtype));
    if ( fldlen > MAX_LEN_FLD_NAME ) { go_BYE(-1); }
    strcpy(aux_fld, ".");
    strcat(aux_fld, str_auxtype);
    strcat(aux_fld, ".");
    strcat(aux_fld, fld);
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
chk_file_size(
	      long long nR,
	      FLD_REC_TYPE fld_rec,
	      size_t *ptr_filesz
	      )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  int fldsz = -1; size_t filesz = -1;
  char filename[32]; zero_string(filename, 32);

  FLD_TYPE fldtype = fld_rec.fldtype;
  int ddir_id = fld_rec.ddir_id;

  if ( nR <= 0 ) { go_BYE(-1); }
  mk_file_name(filename, fld_rec.fileno);
  status = switch_dirs(ddir_id); cBYE(status);
  status = rs_mmap(filename, &X, &nX, 0); cBYE(status);
  *ptr_filesz = nX;
  if ( ( X == NULL ) || ( nX == 0 ) ) { go_BYE(-1); }
  status = chdir(g_cwd); cBYE(status);
    
  switch ( fldtype ) {
  case SV : 
    // Not checking. Fix this. TODO P3
    break;
  case SC : 
    // Note that len includes space for terminating null character
    if ( fld_rec.len <= 1 ) { go_BYE(-1); }
    // +1 for null character that separates strings
    if ( ( (fld_rec.len+1) * nR ) != nX ) { go_BYE(-1); }
    break;
  case B : 
    status = get_file_size_B(nR, &filesz);
    if ( nX != filesz ) { go_BYE(-1); }
    break;
  case I1 : 
  case I2 : 
  case I4 : 
  case I8 : 
  case F4 : 
  case F8 : 
    status = get_fld_sz(fldtype, &fldsz);
    if ( ( fldsz * nR ) != nX ) { go_BYE(-1); }
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  chdir(g_cwd);
  rs_munmap(X, nX);
  return status ;
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
  // TODO: P2 Read this code again
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
  return status ;
}

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
  char filename[32];  zero_string(filename, 32);

  mk_file_name(filename, fld_rec.fileno);
  status = switch_dirs(fld_rec.ddir_id); cBYE(status);
  status = rs_mmap(filename, ptr_X, ptr_nX, is_write); cBYE(status);
  if ( ( *ptr_X == NULL ) || ( *ptr_nX == 0 ) ) { go_BYE(-1); }
  status = chdir(g_cwd);  cBYE(status);
 BYE:
  return status ;
}
// START FUNC DECL
int 
q_mmap(
       int ddir_id,
       int fileno,
       char **ptr_X,
       size_t *ptr_nX,
       bool  is_write
       )
// STOP FUNC DECL
{
  int status = 0;
  char filename[32]; zero_string(filename, 32); 

  status = switch_dirs(ddir_id); cBYE(status);
  mk_file_name(filename, fileno);
  status = rs_mmap(filename, ptr_X, ptr_nX, is_write); cBYE(status); 
  if ( ( *ptr_X == NULL ) || ( *ptr_nX == 0 ) ) { go_BYE(-1); }
  status = chdir(g_cwd);  cBYE(status);
 BYE:
  return status ;
}
// START FUNC DECL
int 
q_re_mmap(
	  int ddir_id,
	  int fileno,
	  char **ptr_X,
	  size_t *ptr_nX,
	  size_t new_filesize,
	  bool  is_write
	  )
// STOP FUNC DECL
{
  // TODO: P2 Read this code again
  int status = 0;
  FILE  *fp = NULL;
  size_t old_filesize = *ptr_nX;
  char filename[32]; zero_string(filename, 32); 
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
  //--------------------------------------------------
  status = switch_dirs(ddir_id); cBYE(status);
  munmap(*ptr_X, *ptr_nX); *ptr_X = NULL; *ptr_nX = 0;
  // stretch the file to the new file size
  mk_file_name(filename, fileno);
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

  status = chdir(g_cwd);  cBYE(status);
 BYE:
  fclose_if_non_null(fp);
  return status ;
}
// START FUNC DECL
int 
q_delete(
	 int ddir_id,
	 int fileno
	 )
// STOP FUNC DECL
{
  int status = 0;
  char filename[32]; zero_string(filename, 32);

  status = switch_dirs(ddir_id); cBYE(status);
  mk_file_name(filename, fileno);
  unlink(filename);
  status = chdir(g_cwd);  cBYE(status);
 BYE:
  return status ;
}
#include<sys/stat.h>
// START FUNC DECL
int 
q_trunc(
	int ddir_id, 
	int fileno,
	size_t filesz
	)
// STOP FUNC DECL
{
  int status = 0;
  struct stat st;

  char filename[32]; zero_string(filename, 32);

  if ( filesz == 0 ) { unlink(filename); goto BYE; }
  status = switch_dirs(ddir_id); cBYE(status);
  mk_file_name(filename, fileno);
  status = stat(filename, &st ); 
  if ( status < 0 ) { fprintf(stderr, "File not found [%s] \n", filename); }
  cBYE(status);
  size_t curr_size = st.st_size;
  if ( filesz > curr_size ) { go_BYE(-1); }
  if ( filesz < curr_size ) { 
    status = truncate(filename, filesz);  cBYE(status);
  }
  status = chdir(g_cwd);  cBYE(status);
 BYE:
  return status ;
}
// START FUNC DECL
int
chk_aux_info(
	     const char *aux_info
	     )
// STOP FUNC DECL
{
  int status = 0;
  if ( aux_info == NULL ) { go_BYE(-1); }
  int len = strlen(aux_info); 
  if ( len == 0 ) { return status ; }  // Nothing to do 
  int n1 = 0; // cnt_open_sq_bracket;
  int n2 = 0; // cnt_close_sq_bracket = 0 ;
  status = count_char(aux_info, '[', &n1); cBYE(status);
  status = count_char(aux_info, ']', &n2); cBYE(status);
  if ( n1 != n2 ) { go_BYE(-1); }
  if ( n1 == 0 ) { go_BYE(-1); }
 BYE:
  return status ;
}
// START FUNC DECL
int
get_file_size_B(
		long long nR, 
		size_t *ptr_filesz
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
  return status ;
}

#include "assign_I1.h"
#include "sort_asc_I8.h"
#include "sort_asc_F8.h"

// START FUNC DECL
int
break_into_scalars(
		   const char *str_scalar,
		   FLD_TYPE fldtype,
		   int *ptr_num_scalars,
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
		   double    svals_F8[MAX_NUM_SCALARS],
		   char      **ptr_hashes_I1,
		   short     **ptr_hashes_I2,
		   int       **ptr_hashes_I4,
		   long long **ptr_hashes_I8,
		   unsigned int *ptr_n,
		   unsigned int *ptr_a,
		   unsigned int *ptr_b
		   )
// STOP FUNC DECL
{
  int status = 0;
  int num_scalars = 1;/* Number of multiple scalar values */
  long long sval_I8 = LLONG_MAX;
  int       sval_I4 = INT_MAX;
  short     sval_I2 = SHRT_MAX;
  char      sval_I1 = SCHAR_MAX;
  float     sval_F4 = FLT_MAX;
  double    sval_F8 = DBL_MAX;
  char **Y = NULL; int nY = 0;
  char      *hashes_I1 = NULL;
  short     *hashes_I2 = NULL;
  int       *hashes_I4 = NULL;
  long long *hashes_I8 = NULL;

  unsigned int a = 0, b = 0, n = 0;

  char *l_scalar = NULL;

  l_scalar = strdup(str_scalar); 
  for ( char *cptr = l_scalar ; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) {
      num_scalars++;
    }
  }
  if ( num_scalars > MAX_NUM_SCALARS ) { go_BYE(-1); }
  switch ( fldtype ) { 
  case I1 : 
  case I2 : 
  case I4 : 
  case I8 : 
    if ( num_scalars == 1 ) { 
      status = stoI8(l_scalar, &sval_I8);  cBYE(status);
    }
    else {
      status = break_str(l_scalar, ":", &Y, &nY); cBYE(status);
      if ( nY != num_scalars ) { go_BYE(-1); }
      for ( int i = 0; i < num_scalars; i++ ) { 
        status = stoI8(Y[i], &(svals_I8[i]));  cBYE(status);
	free_if_non_null(Y[i]);
      }
      free_if_non_null(Y);
      qsort(svals_I8, num_scalars, sizeof(long long), sort_asc_I8);
      // no dupes allowed
      for ( int i = 1; i < num_scalars; i++ ) { 
	if ( svals_I8[i] == svals_I8[i-1] ) { go_BYE(-1); }
      }
    }
    break;
  case F4 : 
  case F8 : 
    if ( num_scalars == 1 ) { 
      status = stoF8(l_scalar, &sval_F8); cBYE(status);
    }
    else {
      status = break_str(l_scalar, ":", &Y, &nY); cBYE(status);
      if ( nY != num_scalars ) { go_BYE(-1); }
      for ( int i = 0; i < num_scalars; i++ ) { 
	status = stoF8(Y[i], &(svals_F8[i])); cBYE(status);
	free_if_non_null(Y[i]);
      }
      free_if_non_null(Y);
      qsort(svals_F8, num_scalars, sizeof(long long), sort_asc_F8);
      // no dupes allowed
      for ( int i = 1; i < num_scalars; i++ ) { 
	if ( svals_F8[i] == svals_F8[i-1] ) { go_BYE(-1); }
      }
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
  
  switch ( fldtype ) { 
  case I1 : 
    if ( num_scalars == 1 ) {
      if ( sval_I8 > SCHAR_MAX ) { go_BYE(-1); }
      if ( sval_I8 < SCHAR_MIN ) { go_BYE(-1); }
      sval_I1 = sval_I8;
    }
    else {
      for ( int i = 0; i < num_scalars; i++ ) { 
	if ( svals_I8[i] > SCHAR_MAX ) { go_BYE(-1); }
	if ( svals_I8[i] < SCHAR_MIN ) { go_BYE(-1); }
	svals_I1[i] = svals_I8[i];
      }
    }
    break;
  case I2 : 
    if ( num_scalars == 1 ) {
      if ( sval_I8 > SHRT_MAX ) { go_BYE(-1); }
      if ( sval_I8 < SHRT_MIN ) { go_BYE(-1); }
      sval_I2 = sval_I8;
    }
    else {
      for ( int i = 0; i < num_scalars; i++ ) { 
	if ( svals_I8[i] > SHRT_MAX ) { go_BYE(-1); }
	if ( svals_I8[i] < SHRT_MIN ) { go_BYE(-1); }
	svals_I2[i] = svals_I8[i];
      }
    }
  case I4 : 
    if ( num_scalars == 1 ) {
      if ( sval_I8 > INT_MAX ) { go_BYE(-1); }
      if ( sval_I8 < INT_MIN ) { go_BYE(-1); }
      sval_I4 = sval_I8;
    }
    else {
      for ( int i = 0; i < num_scalars; i++ ) { 
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
    if ( num_scalars == 1 ) {
      if ( sval_F8 > FLT_MAX ) { go_BYE(-1); }
      if ( sval_F8 < FLT_MIN ) { go_BYE(-1); }
      sval_F4 = sval_F8;
    }
    else {
      for ( int i = 0; i < num_scalars; i++ ) { 
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

  if ( ( num_scalars > 1 ) && ( num_scalars > OPT_NUM_SCALARS ) )  {
    switch ( fldtype ) { 
    case I1 : 
      status = mk_hashes_I1(svals_I1, num_scalars, &hashes_I1, &n, &a, &b);
      break;
    case I2 : 
      status = mk_hashes_I2(svals_I2, num_scalars, &hashes_I2, &n, &a, &b);
      break;
    case I4 : 
      status = mk_hashes_I4(svals_I4, num_scalars, &hashes_I4, &n, &a, &b);
      break;
    case I8 : 
      status = mk_hashes_I8(svals_I8, num_scalars, &hashes_I8, &n, &a, &b);
      break;
    default : 
      go_BYE(-1);
      break;
    }
    cBYE(status);
  }
  *ptr_num_scalars = num_scalars;
  *ptr_sval_I8 = sval_I8;
  *ptr_sval_I4 = sval_I4;
  *ptr_sval_I2 = sval_I2;
  *ptr_sval_I1 = sval_I1;
  *ptr_sval_F4 = sval_F4;
  *ptr_sval_F8 = sval_F8;

  *ptr_hashes_I1 = hashes_I1;
  *ptr_hashes_I2 = hashes_I2;
  *ptr_hashes_I4 = hashes_I4;
  *ptr_hashes_I8 = hashes_I8;
  *ptr_n = n;
  *ptr_a = a;
  *ptr_b = b;
 BYE:
  free_if_non_null(l_scalar);
  if ( Y != NULL ) { 
    for ( int i = 0; i < nY; i++ ) { 
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  return status ;
}
// START FUNC DECL
int
switch_dirs(
	    int ddir_id
	    )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(ddir_id, 0, g_n_ddir );
  char *data_dir = g_ddirs[ddir_id].name;
  if ( ( data_dir == NULL ) || ( *data_dir == '\0' ) ) { go_BYE(-1); }
  status = chdir(data_dir);  cBYE(status);
 BYE:
  return status ;
}


// START FUNC DECL
int
chk_env_vars(
	     char *in_docroot, 
	     char *in_gpu_server,
	     int *ptr_gpu_port
	     )
// STOP FUNC DECL
{
  int status = 0; 


  char *docroot = getenv("Q_DOCROOT");
  if ( ( docroot == NULL ) ||  ( *docroot == '\0' ) ) { go_BYE(-1); }
  if ( strlen(docroot) >= MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  for ( char *cptr = docroot; *cptr != '\0'; cptr++ )  {
    if ( *cptr == '.' ) { go_BYE(-1); }
  }
  strcpy(in_docroot,  docroot);

  char *gpu_server = getenv("Q_GPU_SERVER");
  if ( ( gpu_server == NULL ) ||  ( *gpu_server == '\0' ) ) { 
    /* no gpu server in place */
  }
  else {
    if ( strlen(gpu_server) >= MAX_LEN_DIR_NAME ) { go_BYE(-1); }
    strcpy(in_gpu_server, gpu_server);
    char *str_gpu_port = getenv("Q_GPU_PORT");
    if ( ( str_gpu_port == NULL ) || ( *str_gpu_port == '\0' ) ) { go_BYE(-1); }
    status = stoI4(str_gpu_port, ptr_gpu_port); cBYE(status);
    if ( ( *ptr_gpu_port <= 0 ) || ( *ptr_gpu_port > 65536 ) ) { go_BYE(-1); }
  }

 BYE:
  return status ;
}
// START FUNC DECL
int
get_max_fileno()
// STOP FUNC DECL
{
  int *iptr = (int *)g_aux_X;
  int max_fileno = *iptr;
  *iptr += 1;
  return(max_fileno);
}

// START FUNC DECL
int
is_ddir( /* ddir = data directory */
	const char *in_ddir,
	int *ptr_ddir_id
	 )
// STOP FUNC DECL
{
  int status = 0;
  char ddir[MAX_LEN_DIR_NAME+1];
  if ( ( in_ddir == NULL ) || ( *in_ddir == '\0' ) ) { go_BYE(-1); }
  *ptr_ddir_id = -1;
  status = strip_trailing_slash(in_ddir, ddir, MAX_LEN_DIR_NAME); cBYE(status);
  for ( int i = 0; i < g_n_ddir; i++ ) { 
    if ( strcmp(g_ddirs[i].name, ddir) == 0 ) {
      *ptr_ddir_id = i;
      break;
    }
  }
 BYE:
  return status ;
}

// Very dangerous function. Use with care. 
// START FUNC DECL
int
set_ddir( /* ddir = data directory */
	 const char *in_ddir,
	 const char *str_ddir_id
	  )
// STOP FUNC DECL
{
  int status = 0;
  char ddir[MAX_LEN_DIR_NAME+1];
  int ddir_id;

  status = stoI4(str_ddir_id, &ddir_id); cBYE(status); 
  if ( strlen(in_ddir) >= MAX_LEN_DIR_NAME ) { go_BYE(-1); }
  chk_range(ddir_id, 0, g_n_ddir); 
  status = strip_trailing_slash(in_ddir, ddir, MAX_LEN_DIR_NAME); cBYE(status);
  strcpy(g_ddirs[ddir_id].name, ddir); 
 BYE:
  return status ;
}

// START FUNC DECL
int
get_ddir( /* ddir = data directory */
	 const char *str_ddir_id,
	 char *buf,
	 int buflen
	  )
// STOP FUNC DECL
{
  int status = 0;
  int ddir_id = -1;

  zero_string(buf, buflen); 
  status = stoI4(str_ddir_id, &ddir_id); cBYE(status); 
  chk_range(ddir_id, 0, g_n_ddir); 
  strncpy(buf, g_ddirs[ddir_id].name, buflen-1);
 BYE:
  return status ;
}

// START FUNC DECL
int
mk_full_filename(
		 int ddir_id,
		 int fileno,
		 char *filename,
		 int bufsize
		 )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(ddir_id, 0, g_n_ddir);
  if ( fileno < 1 ) { go_BYE(-1); }
  if ( ( strlen(g_ddirs[ddir_id].name) + 16 ) >= bufsize ) { go_BYE(-1); }
  zero_string(filename, bufsize);
  sprintf(filename, "%s/_%d", g_ddirs[ddir_id].name, fileno);
 BYE:
  return status ;
}

// START FUNC DECL
int
add_to_fld_info(
		int fld_id,
		int mode,
		int aux_id
		)
// STOP FUNC DECL
{
  int status = 0;
  bool done = false;
  // Find first unused spot 
  // TODO P2: Make this faster with binary search
  chk_range(fld_id, 0, g_n_fld); 
  for ( int i = 0; i < g_n_fld_info; i++ ) { 
    if ( g_fld_info[i].mode == 0 ) { 
      done = true;
      g_fld_info[i].mode   = mode;
      g_fld_info[i].fld_id = fld_id;
      switch ( mode ) { 
      case fk_fld_txt_lkp : 
	chk_range(aux_id, 0, g_n_fld);
	g_fld_info[i].xxx.dict_fld_id = aux_id;
	break;
      case fk_fld_len : 
	chk_range(aux_id, 0, g_n_fld);
	g_fld_info[i].xxx.len_fld_id = aux_id;
	break;
      case fk_fld_off : 
	chk_range(aux_id, 0, g_n_fld);
	g_fld_info[i].xxx.off_fld_id = aux_id;
	break;
      default : 
	go_BYE(-1); 
	break;
      }
      break;
    }
  }
  if ( done == false ) { go_BYE(-1); }
  // Keep stuff sorted for easy access. Unused values at end 
  if ( sizeof(FLD_INFO_TYPE) != sizeof(long long) ) { go_BYE(-1); }
  long long *X = (long long *)g_fld_info;
  qsort_asc_I8(X, g_n_fld_info, sizeof(long long), NULL);
  // TODO: P1 I thought I should be sorting descending. Need to figure
  // out why asc works and dsc does not

 BYE:
  return(status);
}

// START FUNC DECL
int
get_fld_info(
	     FLD_INFO_MODE_TYPE mode,
	     int fld_id,
	     int *ptr_out_id
	     )
// STOP FUNC DECL
{
  int status = 0;

  // TODO: P2 Can search better than this 
  *ptr_out_id = -1; 
  for ( int i = 0; i < g_n_fld_info; i++ ) { 
    if ( *ptr_out_id >= 0 ) { break; } 
    if ( g_fld_info[i].mode == 0 ) { break; }
    if ( g_fld_info[i].mode != mode ) { continue; }
    if ( g_fld_info[i].fld_id == fld_id ) { 
      switch ( mode ) { 
      case fk_fld_len : 
	*ptr_out_id = g_fld_info[i].xxx.len_fld_id;
	break;
      case fk_fld_off : 
	*ptr_out_id = g_fld_info[i].xxx.off_fld_id;
	break;
      case fk_fld_txt_lkp : 
	*ptr_out_id = g_fld_info[i].xxx.dict_fld_id;
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
  }
 BYE:
  return(status);
}

// START FUNC DECL
int
del_from_fld_info(
		  int fld_id
		  )
// STOP FUNC DECL
{
  int status = 0;
  // TODO Make this faster 
  chk_range(fld_id, 0, g_n_fld); 
  for ( int i = 0; i < g_n_fld_info; i++ ) { 
    if ( g_fld_info[i].fld_id == fld_id ) { 
      zero_fld_info_rec(&(g_fld_info[i]));
    }
  }
  // Keep stuff sorted for easy access. Unused values at end 
  if ( sizeof(FLD_INFO_TYPE) != sizeof(long long) ) { go_BYE(-1); }
  long long *X = (long long *)g_fld_info;
  qsort_asc_I8(X, g_n_fld_info, sizeof(long long), NULL);
  // TODO: P1 I thought I should be sorting descending. Need to figure
  // out why asc works and dsc does not

 BYE:
  return(status);
}
//
// START FUNC DECL
int
del_from_fld_info_1(
		    int fld_id,
		    FLD_INFO_MODE_TYPE mode
		    )
// STOP FUNC DECL
{
  int status = 0;
  // TODO Make this faster 
  chk_range(fld_id, 0, g_n_fld); 
  switch ( mode ) { 
  case fk_fld_txt_lkp : 
  case fk_fld_len : 
  case fk_fld_off : 
    break;
  default : 
    go_BYE(-1); 
    break;
  }
  for ( int i = 0; i < g_n_fld_info; i++ ) { 
    if ( ( g_fld_info[i].fld_id == fld_id )  && 
         ( g_fld_info[i].mode == mode ) ) {
      zero_fld_info_rec(&(g_fld_info[i]));
    }
  }
  // Keep stuff sorted for easy access. Unused values at end 
  if ( sizeof(FLD_INFO_TYPE) != sizeof(long long) ) { go_BYE(-1); }
  long long *X = (long long *)g_fld_info;
  qsort_asc_I8(X, g_n_fld_info, sizeof(long long), NULL);
  // TODO: P1 I thought I should be sorting descending. Need to figure
  // out why asc works and dsc does not
 BYE:
  return(status);
}
// START FUNC DECL
bool
chk_dsk_ddir_id(
		DDIR_REC_TYPE *ddir
		)
// STOP FUNC DECL
{
  if ( ddir == NULL ) { WHEREAMI; return false; } 
  if ( ddir[0].name[0] == '\0' ) { WHEREAMI; return false; } 
  if ( ddir[0].is_writable == false ) { WHEREAMI; return false; } 
  if ( ddir[0].is_ram      == true ) { WHEREAMI; return false; } 
  return true;
}

// START FUNC DECL
int
start_compound_expr(
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *g_aux_X = NULL; size_t g_aux_nX = 0;
  chdir(g_docroot);
  status = rs_mmap("docroot.aux", &g_aux_X, &g_aux_nX, 1); cBYE(status);
  int *I4ptr = (int *)g_aux_X;
  if ( I4ptr[1] != 0 ) { go_BYE(-1); }
  I4ptr[1] = 1; // Indicating that we are starting a compound expression
  rs_munmap(g_aux_X, g_aux_nX);
  status = chdir(g_cwd); cBYE(status);
 BYE:
  return(status);
}
//
// START FUNC DECL
int
stop_compound_expr(
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *g_aux_X = NULL; size_t g_aux_nX = 0;
  chdir(g_docroot);
  status = rs_mmap("docroot.aux", &g_aux_X, &g_aux_nX, 1); cBYE(status);
  int *I4ptr = (int *)g_aux_X;
  if ( I4ptr[1] != 1 ) { go_BYE(-1); }
  I4ptr[1] = 0; // Indicating that we are starting a compound expression
  rs_munmap(g_aux_X, g_aux_nX);
  status = chdir(g_cwd); cBYE(status);
 BYE:
  return(status);
}
// START FUNC DECL
int
is_in_compound(
    int argc,
    char **argv,
    bool *ptr_b_is_in_compound
      )
// STOP FUNC DECL
{
  int status = 0;
  int *I4ptr = NULL;
  FILE *cfp = NULL;

  I4ptr = (int *)g_aux_X;
  if ( I4ptr == NULL ) { go_BYE(-1); }
  switch ( I4ptr[1] ) { 
    case 0 : 
      *ptr_b_is_in_compound = false; 
      break;
    case 1 : 
      *ptr_b_is_in_compound = true; 
      if ( argc < 2 ) { go_BYE(-1); }
      chdir(g_docroot);
      cfp = fopen("compound_expressions.txt", "a");
      return_if_fopen_failed(cfp, "compound_expressions.txt", "a");
      /* copy input arguments to compound expression file */
      fprintf(cfp, "%s", argv[1]);
    for ( int i = 2; i < argc; i++ ) { 
      fprintf(cfp, "__@@__%s", argv[i]);
    }
    fprintf(cfp, "\n");
    fclose_if_non_null(cfp);
    break;
    default : 
    go_BYE(-1);
    break;
  }
BYE:
  chdir(g_cwd);
  fclose_if_non_null(cfp);
  return(status);
}
// START FUNC DECL
int
compute_nT_for_count(
    long long src_nR, 
    long long dst_nR, 
    int *ptr_nT
    )
// STOP FUNC DECL
{ 
  int status = 0;
  int nT = 1;
  long long seq_time = dst_nR * nT; // to accumulate partial results
  long long par_time = src_nR / nT; // to creat partial results
  for ( ; seq_time < (8 * par_time) ; ) { 
    if ( src_nR < 1024 ) { break; }
    if ( nT >= g_num_cores ) { break; }
    nT++;
    seq_time = dst_nR * nT; 
    par_time = src_nR / nT; 
  }
  *ptr_nT = nT;
  // fprintf(stderr, "src_nR = %lld \n", src_nR);
  // fprintf(stderr, "dst_nR = %lld \n", dst_nR);
  // fprintf(stderr, "nT     = %d   \n", nT);
BYE:
  return(status);
}

