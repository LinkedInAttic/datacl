
// START FUNC DECL
void
saturate_I1(
    char *X,
    long long n,
    char maxval
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    if ( X[i] > maxval ) { X[i] = maxval; }
  }
}
