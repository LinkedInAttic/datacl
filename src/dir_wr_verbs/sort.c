/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
// START FUNC DECL
int sort_iL_aa_compare(
		       const void *in1, 
		       const void *in2
		       )
// STOP FUNC DECL
{ 
  char *cptr1, *cptr2;
  int *iptr; long long *llptr;
  int x1, x2; long long y1, y2;

  cptr1 = (char *)in1;
  cptr2 = (char *)in2;

  iptr   = (int *)cptr1;      x1 = *iptr;  cptr1 += sizeof(int);
  llptr = (long long *)cptr1; y1 = *llptr; cptr1 += sizeof(long long);

  iptr   = (int *)cptr2;      x2 = *iptr;  cptr2 += sizeof(int);
  llptr = (long long *)cptr2; y2 = *llptr; cptr2 += sizeof(long long);

  /* Output in ascending order */
  if ( x1 > x2 )  {
    return (1);
  }
  else if ( x1 < x2 ) {
    return (-1);
  }
  else {
    if ( y1 > y2 )  {
      return (1);
    }
    else if ( y1 < y2 ) {
      return (-1);
    }
    else {
      return(0);
    }
  }
}

// START FUNC DECL
int sort_Li_aa_compare(
		       const void *in1, 
		       const void *in2
		       )
// STOP FUNC DECL
{
  char *cptr1, *cptr2;
  long long *llptr1, *llptr2;
  int *iptr1, *iptr2;
  long long ll1, ll2; 
  int i1, i2;

  cptr1 = (char *)in1;
  cptr2 = (char *)in2;

  llptr1 = (long long *)cptr1; ll1 = *llptr1; cptr1 += sizeof(long long);
  iptr1 = (int *)cptr1;        i1  = *iptr1;  cptr1 += sizeof(int);

  llptr2 = (long long *)cptr2; ll2 = *llptr2; cptr2 += sizeof(long long);
  iptr2 = (int *)cptr2;        i2  = *iptr2;  cptr2 += sizeof(int);

  /* Output in ascending order */
  if ( ll1 > ll2 )  {
    return (1);
  }
  else if ( ll1 < ll2 ) {
    return (-1);
  }
  else {
    if ( i1 > i2 )  {
      return (1);
    }
    else if ( i1 < i2 ) {
      return (-1);
    }
    else {
      return(0);
    }
  }
}

// START FUNC DECL
int str_compare(
		       const void *in1, 
		       const void *in2
		       )
// STOP FUNC DECL
{
  char *cptr1, *cptr2;

  cptr1 = (char *)in1;
  cptr2 = (char *)in2;

  /* Output in ascending order */
  for ( ; ; cptr1++, cptr2++ ) { 
    if ( ( *cptr1 == '\0' ) && ( *cptr2 == '\0' ) ) {
      return(0);
    }
    if ( *cptr1 == '\0' ) { return(1); }
    if ( *cptr2 == '\0' ) { return(-1); }
    if ( *cptr1 < *cptr2 ) { return(1); }
    if ( *cptr2 < *cptr1 ) { return(-1); }
  }
}
