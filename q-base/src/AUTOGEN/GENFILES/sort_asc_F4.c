// START FUNC DECL
int sort_asc_F4(
		       const void *ii, 
		       const void *jj
		       )
// STOP FUNC DECL
{ 
  float val1, val2;
  float *ptr1, *ptr2;
  ptr1 = (float *)ii;
  ptr2 = (float *)jj;
  val1 = *ptr1;
  val2 = *ptr2;

  /* Output in asc order */
  if ( val1 > val2 )  {
    return (1);
  }
  else if ( val1 < val2 ) {
    return (-1);
  }
  else {
    return(0);
  }
}
