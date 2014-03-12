// START FUNC DECL
void
nn_f_to_s_sum_F8(
    double *X,
    char *nn_X,
    long long nX,
    long long *ptr_cnt,
    double *ptr_rslt
    )
// STOP FUNC DECL
{
  long long cnt = *ptr_cnt;
    double rslt = *ptr_rslt;
#pragma simd reduction(+:rslt)
  for ( long long i = 0; i < nX; i++ ) { 
    if ( nn_X[i] == 0 ) { continue; }
    rslt += X[i];
    cnt++;
  }
  *ptr_cnt = cnt;
  *ptr_rslt = rslt;
}
