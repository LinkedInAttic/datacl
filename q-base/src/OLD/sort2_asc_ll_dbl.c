// START FUNC DECL
int sort2_asc_ll_dbl(
		       const void *ii, 
		       const void *jj
		       )
// STOP FUNC DECL
{ 
  long long val11, val21;
  double val12, val22;
  char *ptr1, *ptr2;
  long long *llptr; double *dptr;

  ptr1 = (char *)ii;
  ptr2 = (char *)jj;

  llptr = (long long *)ptr1;
  val11 = *llptr;
  ptr1 += sizeof(long long);
  dptr = (double *)ptr1;
  val12 = *dptr;

  llptr = (long long *)ptr2;
  val21 = *llptr;
  ptr2 += sizeof(long long);
  dptr = (double *)ptr2;
  val22 = *dptr;

  /* Output in asc order */
  if ( val11 > val21 )  {
    return (1);
  }
  else if ( val11 < val21 ) {
    return (-1);
  }
  else {
    if ( val12 > val22 )  {
      return (1);
    }
    else if ( val12 < val22 ) {
      return (-1);
    }
    else {
      return(0);
    }
  }
}
