// START FUNC DECL
void
f_to_s_sum___XTYPE__(
    __ITYPE__ *X,
    long long nX,
    __RTYPE__ *ptr_rslt
    )
// STOP FUNC DECL
{
  __RTYPE__ rslt;

  rslt = *ptr_rslt;
#pragma simd reduction(+:rslt)
  for ( long long i = 0; i < nX; i++ ) { 
    rslt = rslt + X[i];
  }
  *ptr_rslt = rslt;
}
    
