// START FUNC DECL
void
nn_f_to_s_sum___XTYPE__(
    __ITYPE__ *X,
    char *nn_X,
    long long nX,
    long long *ptr_cnt,
    __RTYPE__ *ptr_rslt
    )
// STOP FUNC DECL
{
  long long cnt = *ptr_cnt;
    __RTYPE__ rslt = *ptr_rslt;
#pragma simd reduction(+:rslt)
  for ( long long i = 0; i < nX; i++ ) { 
    if ( nn_X[i] == 0 ) { continue; }
    rslt += X[i];
    cnt++;
  }
  *ptr_cnt = cnt;
  *ptr_rslt = rslt;
}
