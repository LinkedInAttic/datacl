// START FUNC DECL
void
f_to_s_sum_I1(
    char *X,
    long long nX,
    long long *ptr_rslt
    )
// STOP FUNC DECL
{
  long long rslt;

  rslt = *ptr_rslt;
#pragma simd reduction(+:rslt)
  for ( long long i = 0; i < nX; i++ ) { 
    rslt = rslt + X[i];
  }
  *ptr_rslt = rslt;
}
    
