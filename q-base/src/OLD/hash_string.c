#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "hash_string.h"
#include "mmap.h"
#include "mddriver.h"

extern void srand48 (long int __seedval) __THROW;
extern long int lrand48 (void) __THROW;

// START FUNC DECL
int
ihash(
    int x,
    int y,
    int *ptr_z
    )
// STOP FUNC DECL
{
  int status = 0;
  char xbuf[32], ybuf[32], zbuf[32];

  zero_string(xbuf, 32);
  zero_string(ybuf, 32);
  zero_string(zbuf, 32);
  sprintf(xbuf, "%d", x);
  sprintf(ybuf, "%d", y);
  strcpy(zbuf, xbuf);
  strcat(zbuf, ybuf);
  status = hash_string(zbuf, ptr_z);
  cBYE(status);
BYE:
  return(status);
}

/***************************************************************/
// START FUNC DECL
int
hash_string(
	    const char *original_in_string,
	    int *ptr_hash
	    )
// STOP FUNC DECL
{/*
  static int m1 = 131101;
  static int m2 = 33554467;
  static int m3  = 2147483629;
 */
  char out_string[128];
  char *in_string = NULL;
  int status = 0;
  int i, L, l1, l2;

  if ( original_in_string == NULL ) go_BYE(-1);
  L = strlen(original_in_string);
  if ( L == 0 ) { 
    *ptr_hash = 0;
    return(status);
  }
  zero_string(out_string, 128);
  L += 4;
  in_string = (char *)malloc(L * sizeof(char));
  return_if_malloc_failed(in_string);
  strcpy(in_string, original_in_string);
  my_MDString(in_string, out_string);

  /* START: Convert out_string to a long long */
  i = 0; l2 = 0;
  for ( char *cptr = out_string; *cptr != '\0' ; cptr++ ) { 
    l1 = *cptr;
    l1 = l1 << (i*8);
    l2 = l2 ^ l1;
    i++;
    if ( i == sizeof(unsigned long long) ) { i = 0; }
  }
  *ptr_hash = l2;
  /* OLD *ptr_hash = (int)hash_string_to_long(out_string); */
  /* STOP : Convert out_string to a long long */
/* 
  if ( strncmp(original_in_string, "ability", 7) == 0 ) {   
    printf("HASH:%12s --> %d \n", original_in_string, *ptr_hash); 
  }
*/
  if ( *ptr_hash < 0 ) go_BYE(-1);
 BYE:
  free_if_non_null(in_string);
  return(status);
}

// START FUNC DECL
unsigned long
hash_string_to_long(
		   char *s
		   )
// STOP FUNC DECL
{
#define MAX_STRING_LENGTH 1024
  static int seeded = FALSE;
  static unsigned long a_i[16];
  static unsigned long M;
  unsigned long u1, u2, u3, u4;
  int i, j, l, len;
  unsigned long sum, rval;

  if ( seeded == FALSE ) {
    M = 2147483629;
    srand48(2059879141);
    seeded = TRUE;
    for ( i = 0; i < 16; i++ ) {
      a_i[i] = lrand48();
      a_i[i] = a_i[i] % 257;
    }
  }
  if ( s == NULL ) { return -1; }
  len = strlen(s);
  sum = 0;
  i = 0; l = 0; j = 1;
  for ( ; ; ) {
    u1 = (unsigned int)s[l];
    u2 = a_i[i];
    u3 = u1 * u2;
    u4 = u3 * j;
    /* printf("u3 = %u, u4 = %u, j = %d \n", u3, u4, j); */
    sum +=  u4;
    i++; if ( i >= 16 ) { i = 0; }
    l++; if ( l == len ) break;
    j *= 2; if ( j >= 65536 ) { j = 1; }
  }
  rval = sum % M;
  /* printf("s = %s,  sum = %u. rval = %u \n", s, sum, rval); */
  return (rval);
}

#undef USE
#ifdef USE
  L = strlen(out_string);
  j = 0;
  for ( i = 0; i < L; ) {
    ub = i+3;
    if ( ub > L ) { ub = L ; }
    sum = 0;
    for ( k = i; k < ub; k++ ) {
      uitemp = (unsigned int)out_string[k];
      sum += uitemp;
      sum *= 256; /* should this be sum << 8 instead? */
    }
    /* original j = 131*j + s[i]; */
    j += sum*m1 + m2;

    if ( j < 0 ) { j *= -1; }
    if ( j > m3 ) {
      j = j % m3;
    }
    i = ub;
  }
  if ( j < 0 ) { j *= -1; }
#endif


// START FUNC DECL
int
ll_hash_string(
	    char *in_string,
	    unsigned long long int *ptr_hash
	    )
// STOP FUNC DECL
{
  int status = 0;
  char out_string[128];
  unsigned long long int l1, l2;
  int i;

  if ( in_string == NULL ) go_BYE(-1);
  if ( *in_string == '\0' ) { *ptr_hash = 0; return(status); }

  zero_string(out_string, 128);

  my_MDString(in_string, out_string);

  /* START: Convert out_string to a long long */
  i = 0; l2 = 0;
  for ( char *cptr = out_string; *cptr != '\0' ; cptr++ ) { 
    l1 = *cptr;
    l1 = l1 << (i*8);
    l2 = l2 ^ l1;
    i++;
    if ( i == sizeof(unsigned long long) ) { i = 0; }
  }
  /* STOP : Convert out_string to a long long */
  *ptr_hash = l2;
 BYE:
  return(status);
}
