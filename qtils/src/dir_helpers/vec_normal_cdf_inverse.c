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
#include <math.h>
// START FUNC DECL
double
rational_approximation(
    double t
    )
// STOP FUNC DECL
{
// Abramowitz and Stegun formula 26.2.23.
//     The absolute value of the error should be less than 4.5e-4.
  double c[] = {2.515517, 0.802853, 0.010328};
  double d[] = {1.432788, 0.189269, 0.001308};
  return t - ((c[2]*t + c[1])*t + c[0]) / 
      (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}
// START FUNC DECL
void
vec_normal_cdf_inverse( 
			    double *in, 
			    int nR, 
			    char *nn_X,
			    double *out
			    )
// STOP FUNC DECL
{
  for ( int i = 0; i < nR; i++ ) { 
    double outval;
    double inval = *in;
    if ( ( inval <= 0 ) || ( inval >= 1 ) ) { 
      outval = 0;
      nn_X[i] = 0;
    }
    else {
      nn_X[i] = 1;
      if ( inval < 0.5 ) { 
	outval = -1 * rational_approximation( sqrt(-2.0 * log(inval) ) );
      }
      else {
	outval =      rational_approximation( sqrt(-2.0 * log((1.0-inval)) ) );
      }
    }
    *out = outval;
    in++;
    out++;
    nn_X++;
  }
}

