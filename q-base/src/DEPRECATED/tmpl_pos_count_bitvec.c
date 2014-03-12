#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_count_bitvec___XTYPE1_____XTYPE2__(
    __ITYPE1__ *f1, 
    long long nR1, 
    __ITYPE2__ *f2, 
    long long nR2, 
    int *f3,
    long long nR3
    )
// STOP FUNC DECL
{
  int status = 0;
  for ( long long i = 0; i < nR2; i++ ) {
    __ITYPE2__ f1idx = f2[i];
#ifdef DEBUG
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
#endif
    unsigned __ITYPE1__ bitvec = f1[f1idx];
    for ( int i = 0; i < nR3; i++ ) { 
      if ( bitvec == 0 ) { break; }
      if ( ( bitvec & 1 ) == 1 ) {
	f3[i]++; // TODO P2 speed this up with ptr arith int *tempf3 = f3;
      }
      bitvec = bitvec >> 1; 
    }
  }
BYE:
  return(status);
}

