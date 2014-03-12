
// START FUNC DECL
void
saturate_I2(
    short *X,
    long long n,
    short maxval
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    if ( X[i] > maxval ) { X[i] = maxval; }
  }
}
