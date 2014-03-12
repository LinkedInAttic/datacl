// START FUNC DECL
void
f_to_s_min_I2(
    short *X,
    long long nX,
    short *ptr_rslt
    )
// STOP FUNC DECL
{
  short rslt;
  rslt = *ptr_rslt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] < rslt ) {
      rslt = X[i];
    }
  }
  *ptr_rslt = rslt;
}
    
