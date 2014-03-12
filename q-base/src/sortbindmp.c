#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "sort.h"
#include "sort_asc_I4.h"
#include "sort_dsc_I4.h"

#include "qsort_asc_I4.h"
#include "qsort_dsc_I4.h"

#include "sort2_asc_F4.h"
#include "sort2_dsc_F4.h"

#include "sort2_asc_I4.h"
#include "sort2_dsc_I4.h"

#include "sort2_asc_I8.h"
#include "sort2_dsc_I8.h"

#include "sort2_dsc_I8.h"

#include "sort3_asc_I4.h"
#include "sort3_dsc_I4.h"
#include "sort3_asc_I8.h"
#include "sort3_dsc_I8.h"

#include "sort4_asc_I4.h"
#include "sort4_dsc_I4.h"
#include "sort4_asc_I8.h"
#include "sort4_dsc_I8.h"
#include "sort4_asc_F4.h"
#include "sort4_dsc_F4.h"

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
  if ( strcmp(fld_spec, "I8:I8" ) == 0 ) {
    rec_size = 2 * sizeof(long long);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AA") == 0 ) {
      qsort(X, nR, rec_size, sort2_asc_I8);
    }
    else if ( strcmp(srt_ordr, "DD") == 0 ) {
      qsort(X, nR, rec_size, sort2_dsc_I8);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "I8:I8:I8" ) == 0 ) {
    rec_size = ( 3 * sizeof(long long));
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AAA") == 0 ) {
      qsort(X, nR, rec_size, sort3_asc_I8);
    }
    else if ( strcmp(srt_ordr, "DDD") == 0 ) {
      qsort(X, nR, rec_size, sort3_dsc_I8);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "I8:I8:I8:I8" ) == 0 ) {
    rec_size = ( 4 * sizeof(long long));
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AAAA") == 0 ) {
      qsort(X, nR, rec_size, sort4_asc_I8);
    }
    else if ( strcmp(srt_ordr, "DDDD") == 0 ) {
      qsort(X, nR, rec_size, sort4_dsc_I8);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "I4" ) == 0 ) {
    rec_size = 1 * sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "A") == 0 ) {
      if ( nR < INT_MAX ) { 
#ifdef IPP
        ippsSortAscend_32s_I((int *)X, nR);
#else
        qsort_asc_I4(X, nR, sizeof(int), NULL);
#endif
      }
      else {
        qsort_asc_I4(X, nR, sizeof(int), NULL);
      }
    }
    else if ( strcmp(srt_ordr, "D") == 0 ) {
      if ( nR < INT_MAX ) { 
#ifdef IPP
        ippsSortDescend_32s_I((int *)X, nR);
#else
        qsort_dsc_I4(X, nR, sizeof(int), NULL);
#endif
      }
      else {
        qsort_dsc_I4(X, nR, sizeof(int), NULL);
      }
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "I4:I4" ) == 0 ) {
    rec_size = 2 * sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AA") == 0 ) {
      qsort(X, nR, rec_size, sort2_asc_I4);
    }
    else if ( strcmp(srt_ordr, "DD") == 0 ) {
      qsort(X, nR, rec_size, sort2_dsc_I4);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  /*
  else if ( strcmp(fld_spec, "F4:F4" ) == 0 ) {
    rec_size = 2 * sizeof(float);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AA") == 0 ) {
      qsort(X, nR, rec_size, sort2_asc_F4);
    }
    else if ( strcmp(srt_ordr, "DD") == 0 ) {
      qsort(X, nR, rec_size, sort2_dsc_F4);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  */
  else if ( strcmp(fld_spec, "I4:I4:I4" ) == 0 ) {
    rec_size = 3 * sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AAA") == 0 ) {
      qsort(X, nR, rec_size, sort3_asc_I4);
    }
    else if ( strcmp(srt_ordr, "DDD") == 0 ) {
      qsort(X, nR, rec_size, sort3_dsc_I4);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fld_spec, "I4:I4:I4:I4" ) == 0 ) {
    rec_size = 4 * sizeof(int);
    nR = nX / rec_size;
    if ( nR * rec_size != nX ) { go_BYE(-1); }
    if ( strcmp(srt_ordr, "AAA_") == 0 ) {
      qsort(X, nR, rec_size, sort3_asc_I4);
    }
    else if ( strcmp(srt_ordr, "DDD_") == 0 ) {
      qsort(X, nR, rec_size, sort3_dsc_I4);
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

