// START FUNC DECL
void
f_to_s_min_F4(
    float *X,
    long long nX,
    float *ptr_rslt
    )
// STOP FUNC DECL
{
  float rslt;
  rslt = *ptr_rslt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] < rslt ) {
      rslt = X[i];
    }
  }
  *ptr_rslt = rslt;
}
    
