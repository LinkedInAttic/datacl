// START FUNC DECL
void
s_to_f_seq_I2(
    short *X,
    const long long nX,
    const short start,
    const short incr
    )
// STOP FUNC DECL
{
  short val = start;
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
    val += incr;
  }
}
