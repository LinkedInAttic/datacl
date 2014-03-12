// START FUNC DECL
int sort3_desc_i_f_i(
		    const void *ii, 
		    const void *jj
		    )
// STOP FUNC DECL
{ 
  int val11, val21, val13, val23;
  float val12, val22;
  char *ptr1, *ptr2;
  int *iptr;  float *fptr;

  ptr1 = (char *)ii;
  ptr2 = (char *)jj;

  //----------------------------------------------
  // Get values in first record
  iptr = (int *)ptr1;
  val11 = *iptr;
  ptr1 += sizeof(int);
  fptr = (float *)ptr1;
  val12 = *fptr;
  ptr1 += sizeof(float);
  iptr = (int *)ptr1;
  val13 = *iptr;
  //----------------------------------------------
  // Get values in second record
  iptr = (int *)ptr2;
  val21 = *iptr;
  ptr2 += sizeof(int);
  fptr = (float *)ptr2;
  val22 = *fptr;
  ptr2 += sizeof(float);
  iptr = (int *)ptr2;
  val23 = *iptr;

  /* Output in desc order */
  if ( val11 < val21 )  {
    return (1);
  }
  else if ( val11 > val21 ) {
    return (-1);
  }
  else {
    if ( val12 < val22 )  {
      return (1);
    }
    else if ( val12 > val22 ) {
      return (-1);
    }
    else {
      if ( val13 < val23 )  {
        return (1);
      }
      else if ( val13 > val23 ) {
        return (-1);
      }
      else {
        return(0);
      }
    }
  }
}
