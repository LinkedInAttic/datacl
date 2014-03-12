// START FUNC DECL
int sort2_asc_F4(
		       const void *ii, 
		       const void *jj
		       )
// STOP FUNC DECL
{ 
  float val11, val12, val21,val22;
  float *ptr1, *ptr2;
  ptr1 = (float *)ii;
  ptr2 = (float *)jj;

  val11 = *ptr1++; /* first element of first value */
  val12 = *ptr1++; /* second element of first value */
  val21 = *ptr2++; /* first element of second value */
  val22 = *ptr2++; /* second element of second value */

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
