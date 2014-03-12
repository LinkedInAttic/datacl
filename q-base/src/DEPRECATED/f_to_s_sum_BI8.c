// START FUNC DECL
void
f_to_s_sum_BI8(
    unsigned long long *X,
    long long lb,
    long long ub,
    unsigned long long *ptr_rslt
    )
// STOP FUNC DECL
{
  for ( long long i = lb; i < ub; i++ ) { 
    unsigned long long lval = X[i];
    unsigned long long count = 0;
    for ( int i = 0; i < 64; i++ ) {  
      count += (lval & 1);
      lval = lval >> 1;
    }
    *ptr_rslt += count;
  }
}
    
