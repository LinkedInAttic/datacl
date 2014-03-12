// START FUNC DECL
void
f_to_s_min_F8(
    double *X,
    long long nX,
    double *ptr_rslt
    )
// STOP FUNC DECL
{
  double rslt;
  rslt = *ptr_rslt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] < rslt ) {
      rslt = X[i];
    }
  }
  *ptr_rslt = rslt;
}
    
