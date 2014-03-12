
// START FUNC DECL
void
saturate_I8(
    long long *X,
    long long n,
    long long maxval
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    if ( X[i] > maxval ) { X[i] = maxval; }
  }
}
