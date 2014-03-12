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
#include <math.h>
#include <dirent.h>
#include "qtypes.h"
#include "macros.h"
#include "mmap.h"
#include "sort.h"
#include "auxil.h"

// START FUNC DECL
int
stob(
     char *X,
     bool *ptr_Y
     )
// STOP FUNC DECL
{
  int status = 0;
  if ( ( X == NULL ) || ( strlen(X) != 1 ) ) { go_BYE(-1); }
  if ( *X == '0' ) {
    *ptr_Y = false;
  }
  else if ( *X == '1' ) {
    *ptr_Y = true;
  }
  else { go_BYE(-1); }
 BYE:
  return(status);
}

// START FUNC DECL
int
stoi(
     char *X,
     int *ptr_Y
     )
// STOP FUNC DECL
{
  int status = 0;
  char *endptr;
  if ( ( X == NULL ) || ( strlen(X) == 0 ) ) { go_BYE(-1); }
  *ptr_Y = strtol(X, &endptr, 10);
 BYE:
  return(status);
}

// START FUNC DECL
int
read_nth_val( /* n starts from 0 */
    const char *in_str,
    char delim,
    int n,
    char *out_str,
    int len_out_str
    )
// STOP FUNC DECL
{
  int status = 0;
  const char *cptr = in_str;
  int i = 0;

  for ( i = 0; i < n; cptr++ ) {
    if ( *cptr == '\0' ) { break; }
    if ( *cptr == delim ) { i++; }
  }
  if ( i != n ) { go_BYE(-1); }
  if ( *cptr == delim ) { 
    cptr++; // skip over delim  
  }
  for ( int i = 0; ( ( *cptr != delim ) && ( *cptr != '\0' ) ) ; cptr++, i++ ) {
    if ( i == len_out_str ) { go_BYE(-1); }
    out_str[i]= *cptr;
  }

BYE:
  return(status);
}
// START FUNC DECL
int
break_str(
	  const char *X,
	  char *delim,
	  char ***ptr_Y,
	  int *ptr_nY
	  )
// STOP FUNC DECL
{
  int status = 0;
  char **Y = NULL; int nY = 0;
  char *bak_X, *cptr;

  if ( X == NULL ) { go_BYE(-1); }
  if ( *X == '\0' ) { goto BYE; }
  nY = 1;
  bak_X = X;
  for ( ; *bak_X != '\0'; bak_X++ ) { 
    if( *bak_X == ':' ) {
      nY++;
    }
  }
  Y = (char **)malloc(nY * sizeof(char *));
  return_if_malloc_failed(Y);
  for ( int i = 0; i < nY; i++ ) {
    Y[i] = NULL;
  }
  bak_X = X;
  for ( int i = 0; i < nY; i++ ) {
    cptr = strsep(&bak_X, delim);
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
    Y[i] = (char *)malloc(strlen(cptr) + 1);
    return_if_malloc_failed(Y[i]);
    zero_string(Y[i], strlen(cptr) + 1);
    strcpy(Y[i], cptr);
  }
  *ptr_nY = nY;
  *ptr_Y  = Y;
 BYE:
  return(status);
}

// START FUNC DECL
int
num_lines(
	  char *infile,
	  int *ptr_num_lines
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL;  size_t nX = 0;
  int num_lines = 0;

  if ( ( infile == NULL ) || ( *infile == '\0' ) ) { go_BYE(-1); }
  rs_mmap(infile, &X, &nX, 0);
  for ( int i = 0; i < nX; i++ ) { 
    if ( X[i] == '\n' ) { num_lines++; }
  }
  *ptr_num_lines = num_lines;
 BYE:
  rs_munmap(X, nX);
  return(status);
}
//-------------------------------------------------------------
// START FUNC DECL
int
explode(
	const char *in_X,
	const char delim,
	char ***ptr_Y,
	int *ptr_nY
	)
// STOP FUNC DECL
{
  int status = 0;
  char **flds = NULL;
  int len, n_flds;
  char *cptr = NULL; 

  n_flds = 1; cptr = (char *)in_X; len = strlen(in_X) + 1;
  for ( ; *cptr != '\0' ; cptr++ ) {
    if ( *cptr == ':' ) {
      n_flds++;
    }
  }
  flds = (char **)malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(flds);
  for ( int i = 0; i < n_flds; i++ ) { 
    flds[i] = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(flds[i]);
    zero_string(flds[i], len);
  }
  n_flds = 0; cptr = (char *)in_X; 
  for ( int ptr = 0; *cptr != '\0' ; cptr++ ) {
    if ( *cptr == ':' ) {
      flds[n_flds][ptr++] = '\0';
      ptr = 0;
      n_flds++;
    }
    else {
      flds[n_flds][ptr++] = *cptr;
    }
  }
  n_flds++;

  *ptr_nY = n_flds;
  *ptr_Y =  flds;
 BYE:
  return(status);
}
// START FUNC DECL
int
extract_name_value(
		   char *in_str,
		   const char *start,
		   const char *stop,
		   char **ptr_val
		   )
// STOP FUNC DECL
{
  int status = 0;
  char *val = NULL;
  char *cptr1, *cptr2, *cptr3;
  int len, in_str_len;

  if ( in_str == NULL ) { go_BYE(-1); }
  if ( ( start == NULL ) || ( *start == '\0' ) ) { go_BYE(-1); }
  if ( ( stop == NULL )  || ( *stop  == '\0' ) ) { go_BYE(-1); }

  cptr1 = strstr(in_str, start);
  in_str_len = strlen(in_str);
  if ( ( cptr1 == NULL ) || ( *cptr1 == '\0' ) ) { 
    // Not there 
  }
  else { // Now advance to after the start string and look for stop string
    cptr2 = cptr1 + strlen(start);
    cptr3 = strstr(cptr2, stop);
    if ( cptr3 == cptr2 ) { 
      // Not there 
    }
    else {
      if ( cptr3 == NULL ) {
	len = in_str_len - ( cptr2 - in_str );
      }
      else {
        len = cptr3 - cptr2;
      }
      len++;
      val = (char *)malloc(len * sizeof(char));
      return_if_malloc_failed(val);
      zero_string(val, len);
      strncpy(val, cptr2, len-1);
    }
  }
  *ptr_val = val;
 BYE:
  return(status);
}
// START FUNC DECL
int
qd_uq_str( /* quick and dirty unique string */
	  char *str,
	  int len
	   )
// STOP FUNC DECL
{
  int status = 0;
  struct timeval *Tps = NULL; 
  struct timezone *Tpf = NULL;
  void *Tzp = NULL;
  unsigned long long t, x; pid_t p;
  if ( str == NULL ) { go_BYE(-1); }

  if ( len < 24 ) { go_BYE(-1); }
  Tps = (struct timeval*) malloc(sizeof(struct timeval));
  Tpf = (struct timezone*) malloc(sizeof(struct timezone));
  Tzp = 0;

  gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
  t = ( (long long int)Tps->tv_sec * 1000000 ) + (long long int)Tps->tv_usec;
  p = getpid();
  x = ( t << 16 )  | p;
  sprintf(str, "%llu", x);
 BYE:
  free_if_non_null(Tps);
  free_if_non_null(Tpf);
  return(status);
}


long long mk_magic_val(
    )

{
  struct timeval Tps;
  struct timezone Tpf;
  long long t_before_sec = 0, t_before_usec = 0, t_before = 0;

  gettimeofday(&Tps, &Tpf); 
  t_before_sec  = (long long)Tps.tv_sec;
  t_before_usec = (long long)Tps.tv_usec;
  t_before = ( (t_before_sec & 0x000FFFFF ) << 20 ) + t_before_usec;
  return( (t_before << 16) | getpid());
}

int delete_directory(
    char *dir_to_del
    )
{
  int status = 0;
  struct dirent *d = NULL;
  DIR *dir = NULL;
  char buf[256];

  if ( ( dir_to_del == NULL ) || ( *dir_to_del == '\0' ) ) { go_BYE(-1); }
  dir = opendir(dir_to_del);
  if ( dir == NULL ) { return(status); }
  for ( ; ; ) {
    d = readdir(dir);
    if ( d == NULL ) { break; } 
    if ( strcmp(d->d_name, ".") == 0 ) { continue; }
    if ( strcmp(d->d_name, "..") == 0 ) { continue; }
    // printf("Deleting %s\n",d->d_name);
    sprintf(buf, "%s/%s", dir_to_del, d->d_name);
    remove(buf);
  }
  remove(dir_to_del);
BYE:
  return(status);
}
int
setup_for_bvec_unpack(
    long long n,
    char **ptr_out, 
    int *ptr_num_blocks,
    int *ptr_block_size
    )
{
  int status = 0;
  *ptr_out = NULL;
  *ptr_num_blocks = INT_MIN;
  *ptr_block_size = INT_MIN;

  int sz = sizeof(unsigned long long);
  int block_size = 1048576; 
  if ( ( ( block_size / sz )  *sz ) != block_size ) { go_BYE(-1); }
  int num_blocks = n / block_size;
  if ( ( num_blocks * block_size )  != n ) { num_blocks++; }
  // TODO: P2: Try and avoid this malloc
  char *out = malloc(block_size * sizeof(char));
  return_if_malloc_failed(out);
  *ptr_out = out; 
  *ptr_block_size = block_size; 
  *ptr_num_blocks = num_blocks; 
BYE:
  return(status);
}

// START FUNC DECL
bool
is_prime(
    unsigned int n
    )
// STOP FUNC DECL
{
  if ( n == 0 ) { return false; }
  if ( n == 1 ) { return true; }
  if ( n == 2 ) { return true; }
  if ( n == 3 ) { return true; }
  int sqrt_n = (int)(sqrt((double)n));
  for ( int j = 2; j < sqrt_n; j++ ) {
    if ( ( n % j ) == 0 ) {
      return(false);
    }
  }
  return(true);
}

// START FUNC DECL
unsigned int
prime_geq(
    int n
    )
// STOP FUNC DECL
{
  for ( int m = n+1; ; m++ ) {
    if ( is_prime(m) ) {
      return(m);
    }
  }
}





#include<sys/stat.h>

// START FUNC DECL
bool file_exists (
    const char *filename
    )
// STOP FUNC DECL
{
  struct stat buf;
  int status = stat(filename, &buf );
  if ( status == 0 ) { /* File found */
    return true;
  }
  else {
    return false;
  }
}
// START FUNC DECL
    int
      strip_trailing_slash(
	  const char *in_str, 
	  char *out_str,
	  int out_len
	  )
// STOP FUNC DECL
      {
	int status = 0;
	if ( in_str == NULL ) { go_BYE(-1); }
	if ( out_str == NULL ) { go_BYE(-1); }
	if ( out_len <= 0 ) { go_BYE(-1); }

    zero_string(out_str, out_len);
    int in_len = strlen(in_str);
    if ( in_len >= out_len ) { go_BYE(-1); }
    strcpy(out_str, in_str);
    for ( int i = in_len-1; i >= 0 ; i-- ) { 
      if ( out_str[i] == '/' ) { 
	out_str[i] = '\0';  
      }
      else {
	break;
      }
    }
    if ( strlen(out_str) == 0 ) { go_BYE(-1); }
BYE:
    return(status);
      }



#include <sys/statvfs.h>
// START FUNC DECL
int
get_disk_space ( 
    char * dev_path,
    unsigned long long *ptr_nbytes,
    char *mode
    )
// STOP FUNC DECL
{
  int status = 0;
  struct statvfs sfs;
  *ptr_nbytes = -1;
  if ( dev_path == NULL ) { go_BYE(-1); }
  if ( mode == NULL ) { go_BYE(-1); }

  status = statvfs ( dev_path, &sfs); cBYE(status);
  if ( strcmp(mode, "capacity") == 0 ) {
    *ptr_nbytes = (unsigned long long)sfs.f_bsize * sfs.f_blocks;
  }
  else if ( strcmp(mode, "free_space") == 0 ) {
    *ptr_nbytes = (unsigned long long)sfs.f_bsize * sfs.f_bfree;
  }
  else { go_BYE(-1); }
BYE:
  return(status);
}
