#include <stdbool.h>
// START FUNC DECL
void
assign_I1(
    char *out,
    char * restrict in,
    long long n
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    char val;
    val = *in;
    *out = val;
    in++;
    out++;
  }
}


// START FUNC DECL
void
assign_const_I1(
    char *out,
    long long n,
    char val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    *out = val;
    out++;
  }
}

