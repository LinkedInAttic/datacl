// START FUNC DECL
int sort_dsc_I8(
		       const void *ii, 
		       const void *jj
		       )
// STOP FUNC DECL
{ 
  long long val1, val2;
  long long *ptr1, *ptr2;
  ptr1 = (long long *)ii;
  ptr2 = (long long *)jj;
  val1 = *ptr1;
  val2 = *ptr2;

  /* Output in dsc order */
  if ( val1 < val2 )  {
    return (1);
  }
  else if ( val1 > val2 ) {
    return (-1);
  }
  else {
    return(0);
  }
}
