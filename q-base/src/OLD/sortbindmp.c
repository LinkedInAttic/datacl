#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "sort.h"
#include "sort_asc_int.h"
#include "sort_desc_int.h"

#include "sort2_asc_int.h"
#include "sort2_desc_int.h"
#include "sort2_asc_longlong.h"

#include "sort2_asc_ll_dbl.h"
#include "sort3_desc_i_f_i.h"

#include "sort2_desc_longlong.h"
#include "sort3_asc_int.h"
#include "sort3_desc_int.h"
#include "sort3_asc_longlong.h"
#include "sort3_desc_longlong.h"
#include "sortbindmp.h"

// START FUNC DECL
int
sortbindmp(
    char *infile, 
    char *fld_spec, 
    char *srt_ordr
    )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  size_t nR;
  int rec_size;

  status = rs_mmap(infile, &X, &nX, 1); // 1 because we are going to modify it in situ
  cBYE(status);
  if ( strcmp(fld_spec, "IFI" ) == 0 ) {
    rec_size = sizeof(int) + sizeof(float) + sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    else if ( strcmp(srt_ordr, "DDD") == 0 ) {
      qsort(X, nR, rec_size, sort3_desc_i_f_i);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "LL" ) == 0 ) {
    rec_size = 2 * sizeof(long long);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AA") == 0 ) {
      qsort(X, nR, rec_size, sort2_asc_longlong);
    }
    else if ( strcmp(srt_ordr, "DD") == 0 ) {
      qsort(X, nR, rec_size, sort2_desc_longlong);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "LD" ) == 0 ) {
    rec_size = sizeof(long long) + sizeof(double);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AA") == 0 ) {
      qsort(X, nR, rec_size, sort2_asc_ll_dbl);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "LLL" ) == 0 ) {
    rec_size = ( 3 * sizeof(long long));
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AAA") == 0 ) {
      qsort(X, nR, rec_size, sort3_asc_longlong);
    }
    else if ( strcmp(srt_ordr, "DDD") == 0 ) {
      qsort(X, nR, rec_size, sort3_desc_longlong);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "I" ) == 0 ) {
    rec_size = 1 * sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "A") == 0 ) {
      qsort(X, nR, rec_size, sort_asc_int);
    }
    else if ( strcmp(srt_ordr, "D") == 0 ) {
      qsort(X, nR, rec_size, sort_desc_int);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "II" ) == 0 ) {
    rec_size = 2 * sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AA") == 0 ) {
      qsort(X, nR, rec_size, sort2_asc_int);
    }
    else if ( strcmp(srt_ordr, "DD") == 0 ) {
      qsort(X, nR, rec_size, sort2_desc_int);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "III" ) == 0 ) {
    rec_size = 3 * sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AAA") == 0 ) {
      qsort(X, nR, rec_size, sort3_asc_int);
    }
    else if ( strcmp(srt_ordr, "DDD") == 0 ) {
      qsort(X, nR, rec_size, sort3_desc_int);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else { 
    go_BYE(-1);
  }

BYE:
  rs_munmap(X, nX);
  return(status);
}

