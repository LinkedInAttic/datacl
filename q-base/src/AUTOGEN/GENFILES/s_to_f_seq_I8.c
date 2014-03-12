// START FUNC DECL
void
s_to_f_seq_I8(
    long long *X,
    const long long nX,
    const long long start,
    const long long incr
    )
// STOP FUNC DECL
{
  long long val = start;
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
    val += incr;
  }
}
