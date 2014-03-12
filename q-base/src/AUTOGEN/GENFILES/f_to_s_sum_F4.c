// START FUNC DECL
void
f_to_s_sum_F4(
    float *X,
    long long nX,
    double *ptr_rslt
    )
// STOP FUNC DECL
{
  double rslt;

  rslt = *ptr_rslt;
#pragma simd reduction(+:rslt)
  for ( long long i = 0; i < nX; i++ ) { 
    rslt = rslt + X[i];
  }
  *ptr_rslt = rslt;
}
    
