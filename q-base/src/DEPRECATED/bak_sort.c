#include <stdlib.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sort.h"

int sort_float_a_compare(
		       const void *ii, 
		       const void *jj
		       )
{ 
  int i, j;
  float *fptr1, *fptr2;
  fptr1 = (float *)ii;
  fptr2 = (float *)jj;
  i = *fptr1;
  j = *fptr2;

  /* Output in ascending order */
  if ( i > j )  {
    return (1);
  }
  else if ( i < j ) {
    return (-1);
  }
  else {
    return(0);
  }
}

int sort_int_a_compare(
		       const void *ii, 
		       const void *jj
		       )
{ 
  int i, j;
  int *iptr1, *iptr2;
  iptr1 = (int *)ii;
  iptr2 = (int *)jj;
  i = *iptr1;
  j = *iptr2;

  /* Output in ascending order */
  if ( i > j )  {
    return (1);
  }
  else if ( i < j ) {
    return (-1);
  }
  else {
    return(0);
  }
}

int sort_int_d_compare(
		       const void *ii, 
		       const void *jj
		       )
{ 
  int i, j;
  int *iptr1, *iptr2;
  iptr1 = (int *)ii;
  iptr2 = (int *)jj;
  i = *iptr1;
  j = *iptr2;

  /* Output in descending order */
  if ( i < j )  {
    return (1);
  }
  else if ( i > j ) {
    return (-1);
  }
  else {
    return(0);
  }
}

int sort_ii_dd_compare(
		       const void *in1, 
		       const void *in2
		       )
{ 
  int *iptr1, *iptr2;
  iptr1 = (int *)in1;
  iptr2 = (int *)in2;
  int x1, y1, x2, y2;

  x1 = *iptr1++;
  y1 = *iptr1++;
  x2 = *iptr2++;
  y2 = *iptr2++;

  /* Output in descending order */
  if ( x1 < x2 )  {
    return (1);
  }
  else if ( x1 > x2 ) {
    return (-1);
  }
  else {
    if ( y1 < y2 )  {
      return (1);
    }
    else if ( y1 > y2 ) {
      return (-1);
    }
    else {
      return(0);
    }
  }
}

int sort_ii_aa_compare(
		       const void *in1, 
		       const void *in2
		       )
{ 
  int *iptr1, *iptr2;
  iptr1 = (int *)in1;
  iptr2 = (int *)in2;
  int x1, y1, x2, y2;

  x1 = *iptr1++;
  y1 = *iptr1++;
  x2 = *iptr2++;
  y2 = *iptr2++;

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

int sort_iL_aa_compare(
		       const void *in1, 
		       const void *in2
		       )
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

int sort_Li_aa_compare(
		       const void *in1, 
		       const void *in2
		       )
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


int sort_LL_aa_compare(
		       const void *in1, 
		       const void *in2
		       )
{ 
  long long *llptr1, *llptr2;
  long long v11, v12, v21, v22;

  llptr1 = (long long *)in1;
  llptr2 = (long long *)in2;

  v11 = llptr1[0];
  v12 = llptr1[1];
  v21 = llptr2[0];
  v22 = llptr2[1];

  /* Output in ascending order */
  if ( v11 > v21 )  {
    return (1);
  }
  else if ( v11 < v21 ) {
    return (-1);
  }
  else {
    if ( v12 > v22 )  {
      return (1);
    }
    else if ( v12 < v22 ) {
      return (-1);
    }
    else {
      return(0);
    }
  }
}

int sort_LL_dd_compare(
		       const void *in1, 
		       const void *in2
		       )
{ 
  long long *llptr1, *llptr2;
  long long v11, v12, v21, v22;

  llptr1 = (long long *)in1;
  llptr2 = (long long *)in2;

  v11 = llptr1[0];
  v12 = llptr1[1];
  v21 = llptr2[0];
  v22 = llptr2[1];

  /* Output in descending order */
  if ( v11 < v21 )  {
    return (1);
  }
  else if ( v11 > v21 ) {
    return (-1);
  }
  else {
    if ( v12 < v22 )  {
      return (1);
    }
    else if ( v12 > v22 ) {
      return (-1);
    }
    else {
      return(0);
    }
  }
}

int sort_LLL_aaa_compare(
			 const void *in1, 
			 const void *in2
			 )
{ 
  long long *llptr1, *llptr2;
  long long v11, v12, v13, v21, v22, v23;

  llptr1 = (long long *)in1;
  llptr2 = (long long *)in2;

  v11 = llptr1[0];
  v12 = llptr1[1];
  v13 = llptr1[2];
  v21 = llptr2[0];
  v22 = llptr2[1];
  v23 = llptr2[2];

  /* Output in ascending order */
  if ( v11 > v21 )  {
    return (1);
  }
  else if ( v11 < v21 ) {
    return (-1);
  }
  else {
    if ( v12 > v22 )  {
      return (1);
    }
    else if ( v12 < v22 ) {
      return (-1);
    }
    else {
      if ( v13 > v23 )  {
	return (1);
      }
      else if ( v13 < v23 ) {
	return (-1);
      }
      else {
	return(0);
      }
    }
  }
}

int sort_LLL_ddd_compare(
			 const void *in1, 
			 const void *in2
			 )
{ 
  long long *llptr1, *llptr2;
  long long v11, v12, v13, v21, v22, v23;

  llptr1 = (long long *)in1;
  llptr2 = (long long *)in2;

  v11 = llptr1[0];
  v12 = llptr1[1];
  v13 = llptr1[2];
  v21 = llptr2[0];
  v22 = llptr2[1];
  v23 = llptr2[2];

  /* Output in descending order */
  if ( v11 < v21 )  {
    return (1);
  }
  else if ( v11 > v21 ) {
    return (-1);
  }
  else {
    if ( v12 < v22 )  {
      return (1);
    }
    else if ( v12 > v22 ) {
      return (-1);
    }
    else {
      if ( v13 < v23 )  {
	return (1);
      }
      else if ( v13 > v23 ) {
	return (-1);
      }
      else {
	return(0);
      }
    }
  }
}

int sort_iii_aaa_compare(
			 const void *in1, 
			 const void *in2
			 )
{
  int *iptr1, *iptr2;
  int v11, v12, v13, v21, v22, v23;

  iptr1 = (int *)in1;
  iptr2 = (int *)in2;

  v11 = iptr1[0];
  v12 = iptr1[1];
  v13 = iptr1[2];
  v21 = iptr2[0];
  v22 = iptr2[1];
  v23 = iptr2[2];

  /* Output in ascending order */
  if ( v11 > v21 )  {
    return (1);
  }
  else if ( v11 < v21 ) {
    return (-1);
  }
  else {
    if ( v12 > v22 )  {
      return (1);
    }
    else if ( v12 < v22 ) {
      return (-1);
    }
    else {
      if ( v13 > v23 )  {
	return (1);
      }
      else if ( v13 < v23 ) {
	return (-1);
      }
      else {
	return(0);
      }
    }
  }
}

int sort_iii_ddd_compare(
			 const void *in1, 
			 const void *in2
			 )
{
  int *iptr1, *iptr2;
  int v11, v12, v13, v21, v22, v23;

  iptr1 = (int *)in1;
  iptr2 = (int *)in2;

  v11 = iptr1[0];
  v12 = iptr1[1];
  v13 = iptr1[2];
  v21 = iptr2[0];
  v22 = iptr2[1];
  v23 = iptr2[2];

  /* Output in descending order */
  if ( v11 < v21 )  {
    return (1);
  }
  else if ( v11 > v21 ) {
    return (-1);
  }
  else {
    if ( v12 < v22 )  {
      return (1);
    }
    else if ( v12 > v22 ) {
      return (-1);
    }
    else {
      if ( v13 < v23 )  {
	return (1);
      }
      else if ( v13 > v23 ) {
	return (-1);
      }
      else {
	return(0);
      }
    }
  }
}

