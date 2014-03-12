
// START FUNC DECL
void
saturate___TYPEX__(
    __TYPEI__ *X,
    long long n,
    __TYPEI__ maxval
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    if ( X[i] > maxval ) { X[i] = maxval; }
  }
}
