#ifdef IPP
#include "ipps.h"
#endif
#include <stdio.h>
#include "constants.h"
#include "macros.h"
extern int g_num_cores; 
// last review 2/5/2014
// START FUNC DECL
int
smear(
      char *in,
      long long nR,
      int plus,
      int minus,
      char *out
      )
// STOP FUNC DECL
{
  int status = 0;

  if ( minus < 0 ) { go_BYE(-1); }
  if ( plus  < 0 ) { go_BYE(-1); }
  if ( ( plus == 0 ) && ( minus == 0 ) )  { go_BYE(-1); }
  int nT = g_num_cores;
  long long block_size = nR / nT;
#pragma omp parallel for 
  for ( int tid = 0; tid < nT; tid++ ) { 
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    for ( long long i = lb; i < ub; i++ ) { 
      if ( in[i] == 1 ) { 
	long long lb = i - minus; if ( lb < 0 ) { lb = 0; } 
	long long ub = i + 1 + plus;  if ( ub >= nR ) { ub = nR; } 
	for ( long long j = lb; j < ub; j++ ) { 
	  out[j] = 1;
	}
      }
    }
  }
 BYE:
  return status ;
}
