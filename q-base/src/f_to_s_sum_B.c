#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"

int
mask_byte_L(
	  char *ptr_ctemp, 
	  int nbits
	  )
{
  /* TODO P0: need to check the logic of this */
  int status = 0;
  switch ( nbits ) { 
  case 1 : *ptr_ctemp = *ptr_ctemp & 0x80; break;
  case 2 : *ptr_ctemp = *ptr_ctemp & 0xB0; break;
  case 3 : *ptr_ctemp = *ptr_ctemp & 0xD0; break;
  case 4 : *ptr_ctemp = *ptr_ctemp & 0xF0; break;
  case 5 : *ptr_ctemp = *ptr_ctemp & 0xF8; break;
  case 6 : *ptr_ctemp = *ptr_ctemp & 0xFB; break;
  case 7 : *ptr_ctemp = *ptr_ctemp & 0xFD; break;
  default : 
    go_BYE(-1); 
    break
      ;
  }
 BYE:
  return status ;
}
int
mask_byte_R(
	  char *ptr_ctemp, 
	  int nbits
	  )
{
  int status = 0;
  switch ( nbits ) { 
  case 1 : *ptr_ctemp = *ptr_ctemp & 0x01; break;
  case 2 : *ptr_ctemp = *ptr_ctemp & 0x03; break;
  case 3 : *ptr_ctemp = *ptr_ctemp & 0x07; break;
  case 4 : *ptr_ctemp = *ptr_ctemp & 0x0F; break;
  case 5 : *ptr_ctemp = *ptr_ctemp & 0x1F; break;
  case 6 : *ptr_ctemp = *ptr_ctemp & 0x3F; break;
  case 7 : *ptr_ctemp = *ptr_ctemp & 0x7F; break;
  default : 
    go_BYE(-1); 
    break
      ;
  }
 BYE:
  return status ;
}
#define LOCAL_DEBUG
// START FUNC DECL
int
f_to_s_sum_B(
	     char *X,
	     long long lb,
	     long long ub,
	     long long *ptr_rslt
	     )
// STOP FUNC DECL
{
  int status = 0;
  long long rslt = 0;
  long long lb2, ub2, n2_UI8;
  char *X2_I1 = NULL; 
  unsigned long long *X2_UI8 = NULL;

  //  lb2/ub2 denotes boundaries of part handled as I8's 
  lb2 = ( lb / NUM_BITS_IN_I8 ) * NUM_BITS_IN_I8 ;
  if ( lb2 != lb ) { lb2 += NUM_BITS_IN_I8; }
  ub2 = ( ub / NUM_BITS_IN_I8 ) * NUM_BITS_IN_I8 ;

  // handle case until we get to a UI8 boundary
  if ( lb < lb2 ) {
    int n_bits_left = lb2 - lb;
    int n_bytes_left = n_bits_left / NUM_BITS_IN_I1;
    char *X1_I1 = X + (lb2/NUM_BITS_IN_I1) - n_bytes_left;
    register unsigned int part_rslt;
    for ( int i = 0; i < n_bytes_left; i++ ) {
      char ctemp = X1_I1[i];
      if ( ctemp < 0 ) { 
	ctemp = ctemp & 0x7F;
        part_rslt = 1 + __builtin_popcount((unsigned int)ctemp);
      }
      else {
        part_rslt = __builtin_popcount((unsigned int)ctemp);
      }
#ifdef LOCAL_DEBUG
      if ( part_rslt > 8 ) { go_BYE(-1); }
#endif
      rslt += part_rslt;
#ifdef LOCAL_DEBUG
      if ( ( X1_I1[i] == 0 ) && ( part_rslt >  0 ) )  { go_BYE(-1); }
      if ( ( X1_I1[i] != 0 ) && ( part_rslt == 0 ) )  { go_BYE(-1); }
#endif
    }
    n_bits_left -= (n_bytes_left * NUM_BITS_IN_I1);
    if ( n_bits_left > 0 ) {
      char ctemp = *(X1_I1 - 1 + n_bytes_left); // last byte 
      status = mask_byte_L(&ctemp, n_bits_left); cBYE(status);
      if ( ctemp < 0 ) { 
	ctemp = ctemp & 0x7F;
        part_rslt = 1 + __builtin_popcount((unsigned int)ctemp);
      }
      else {
        part_rslt = __builtin_popcount((unsigned int)ctemp);
      }
      rslt += part_rslt;
    }
  }



  // handle part that can be handled as UI8
  X2_I1  = X + lb2/NUM_BITS_IN_I1;
  X2_UI8 = (unsigned long long *)X2_I1;
  n2_UI8 = ( ub2 - lb2 ) / NUM_BITS_IN_I8;
#pragma simd reduction(+:rslt)
  for ( long long i = 0; i < n2_UI8; i++ ) { 
    register int part_rslt;
    part_rslt = __builtin_popcountll(X2_UI8[i]);
    rslt += part_rslt;
#ifdef LOCAL_DEBUG
    if ( ( X2_UI8[i] == 0 ) && ( part_rslt >  0 ) )  { go_BYE(-1); }
    if ( ( X2_UI8[i] != 0 ) && ( part_rslt == 0 ) )  { go_BYE(-1); }
#endif
  }
  // handle stuff after last I8 boundary
  if ( ub > ub2 ) {
    int n_bits_left = ub - ub2;
    int n_bytes_left = n_bits_left / NUM_BITS_IN_I1;
    char *X3_I1 = X + (ub2/NUM_BITS_IN_I1);
    register unsigned int part_rslt;
    for ( int i = 0; i < n_bytes_left; i++ ) {
      char ctemp = X3_I1[i];
      if ( ctemp < 0 ) { 
	ctemp = ctemp & 0x7F;
        part_rslt = 1 + __builtin_popcount((unsigned int)ctemp);
      }
      else {
        part_rslt = __builtin_popcount((unsigned int)ctemp);
      }
#ifdef LOCAL_DEBUG
      if ( part_rslt > 8 ) { go_BYE(-1); }
#endif
      rslt += part_rslt;
#ifdef LOCAL_DEBUG
      if ( ( X3_I1[i] == 0 ) && ( part_rslt >  0 ) )  { go_BYE(-1); }
      if ( ( X3_I1[i] != 0 ) && ( part_rslt == 0 ) )  { go_BYE(-1); }
#endif
    }
    n_bits_left -= (n_bytes_left * NUM_BITS_IN_I1);
    if ( n_bits_left > 0 ) { 
      char ctemp = *(X3_I1 + n_bytes_left); // last byte 
      status = mask_byte_R(&ctemp, n_bits_left); cBYE(status);
      if ( ctemp < 0 ) { 
	go_BYE(-1); 
      }
      part_rslt = __builtin_popcount((unsigned int)ctemp);
      rslt += part_rslt;
    }
  }

  *ptr_rslt += rslt;
 BYE:
  return status ;
}
