// START FUNC DECL
void
nn_f_to_s_min_F8(
    double *X,
    char *nn_X,
    long long nX,
    long long *ptr_cnt,
    double *ptr_rslt
    )
// STOP FUNC DECL
{
  double rslt = *ptr_rslt;
  long long cnt = *ptr_cnt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( nn_X[i] == 0 ) { continue; }
    if ( X[i] < rslt ) {
      rslt = X[i];
    }
    cnt++;
  }
  *ptr_rslt = rslt;
  *ptr_cnt  = cnt;
}
