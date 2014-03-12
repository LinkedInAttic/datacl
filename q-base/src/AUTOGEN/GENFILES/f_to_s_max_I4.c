// START FUNC DECL
void
f_to_s_max_I4(
    int *X,
    long long nX,
    int *ptr_rslt
    )
// STOP FUNC DECL
{
  int rslt;
  rslt = *ptr_rslt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] > rslt ) {
      rslt = X[i];
    }
  }
  *ptr_rslt = rslt;
}
    
