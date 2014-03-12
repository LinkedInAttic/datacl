// START FUNC DECL
void
f_to_s_min_I8(
    long long *X,
    long long nX,
    long long *ptr_rslt
    )
// STOP FUNC DECL
{
  long long rslt;
  rslt = *ptr_rslt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] < rslt ) {
      rslt = X[i];
    }
  }
  *ptr_rslt = rslt;
}
    
