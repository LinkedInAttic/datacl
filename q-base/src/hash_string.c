#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "hash_string.h"
#include "mix.h"
#include "mddriver.h"

/***************************************************************/
// START FUNC DECL
int
int_hash_string(
	    const char *original_in_string,
	    unsigned long long *ptr_hash
	    )
// STOP FUNC DECL
{
  int status = 0;
  char out_string[128];
  char *in_string = NULL;
  unsigned long long ull[128/8];
  unsigned long long ulltemp = 0;

  if ( original_in_string == NULL ) go_BYE(-1);
  if ( *original_in_string == '\0' ) { *ptr_hash = 0; return status ; }

  zero_string(out_string, 128);
  int L = strlen(original_in_string);
  L += 4;
  in_string = (char *)malloc(L * sizeof(char));
  return_if_malloc_failed(in_string);
  strcpy(in_string, original_in_string);
  my_MDString(in_string, out_string);

  /* START: Convert out_string to a long long */
  char *cptr = out_string;
  int cnt = 0;
  for (int i = 0; i < 128/8; i++ ) { ull[i] = 0; }

  for (int i = 0; i < 128/8; i++ ) { 
    ulltemp = 0;
    for ( int j = 0; j < 8; j++ ) {
      if ( *cptr == '\0' ) { break; }
      unsigned long long l1 = (unsigned long long)(*cptr++);
      ulltemp = ulltemp  | ( l1 << (j*8) );
      cnt++;
    }
    ull[i] = ulltemp;
  }
  ulltemp = ull[0];
  for (int i = 1; i < 128/8; i++ ) { 
    if ( ull[i] == 0 ) { break; }
    ulltemp = ulltemp ^ mix_I8(ull[i]);
  }
  *ptr_hash = ulltemp;
 BYE:
  free_if_non_null(in_string);
  return status ;
}

// START FUNC DECL
int
hash_string_UI4(
	    const char *str,
	    unsigned int *ptr_hash
	    )
// STOP FUNC DECL
{
  int status = 0;
  unsigned long long ulltemp;
  status = int_hash_string(str, &ulltemp); cBYE(status);
  *ptr_hash = ulltemp & 0xFFFFFFFF;
BYE:
  return status ;
}

// START FUNC DECL
int
hash_string_UI8(
	    const char *str,
	    unsigned long long *ptr_hash
	    )
// STOP FUNC DECL
{
  int status = 0;
  status = int_hash_string(str, ptr_hash); cBYE(status);
BYE:
  return status ;
}

// START FUNC DECL
int
ext_hash_string(
    char *in_string,
    char *hashtype
    )
// STOP FUNC DECL
{
  int status = 0;
  unsigned long long ulltemp;

  if ( strcmp(hashtype, "I4") == 0 )  {
    status = int_hash_string(in_string, &ulltemp); cBYE(status);
    unsigned int uitemp = ulltemp & 0xFFFFFFFF;
    fprintf(stdout, "%u,%s \n", uitemp, in_string);
  }
  else if ( strcmp(hashtype, "I8") == 0 )  {
    status = int_hash_string(in_string, &ulltemp); cBYE(status);
    fprintf(stdout, "%lld,%s \n", ulltemp, in_string);
  }
  else { go_BYE(-1); }
 BYE:
  return status ;
}
