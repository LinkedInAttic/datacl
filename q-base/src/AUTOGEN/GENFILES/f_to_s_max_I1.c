// START FUNC DECL
void
f_to_s_max_I1(
    char *X,
    long long nX,
    char *ptr_rslt
    )
// STOP FUNC DECL
{
  char rslt;
  rslt = *ptr_rslt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] > rslt ) {
      rslt = X[i];
    }
  }
  *ptr_rslt = rslt;
}
    
