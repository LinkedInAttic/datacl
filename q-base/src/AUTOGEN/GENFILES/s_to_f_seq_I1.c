// START FUNC DECL
void
s_to_f_seq_I1(
    char *X,
    const long long nX,
    const char start,
    const char incr
    )
// STOP FUNC DECL
{
  char val = start;
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
    val += incr;
  }
}
