#include <stdbool.h>
// START FUNC DECL
void
assign___TYPE__(
    __TYPE2__ *out,
    __TYPE2__ * restrict in,
    long long n
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    __TYPE2__ val;
    val = *in;
    *out = val;
    in++;
    out++;
  }
}


// START FUNC DECL
void
assign_const___TYPE__(
    __TYPE2__ *out,
    long long n,
    __TYPE2__ val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    *out = val;
    out++;
  }
}

