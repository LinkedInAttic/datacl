#include <stdbool.h>
// START FUNC DECL
void
assign_F8(
    double *out,
    double * restrict in,
    long long n
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    double val;
    val = *in;
    *out = val;
    in++;
    out++;
  }
}


// START FUNC DECL
void
assign_const_F8(
    double *out,
    long long n,
    double val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    *out = val;
    out++;
  }
}

