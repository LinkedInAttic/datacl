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
break_str(
    char *X,
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
