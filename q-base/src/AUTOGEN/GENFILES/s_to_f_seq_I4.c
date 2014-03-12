// START FUNC DECL
void
s_to_f_seq_I4(
    int *X,
    const long long nX,
    const int start,
    const int incr
    )
// STOP FUNC DECL
{
  int val = start;
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
    val += incr;
  }
}
