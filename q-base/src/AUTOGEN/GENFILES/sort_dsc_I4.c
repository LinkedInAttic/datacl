// START FUNC DECL
int sort_dsc_I4(
		       const void *ii, 
		       const void *jj
		       )
// STOP FUNC DECL
{ 
  int val1, val2;
  int *ptr1, *ptr2;
  ptr1 = (int *)ii;
  ptr2 = (int *)jj;
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
