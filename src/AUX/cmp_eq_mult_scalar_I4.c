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
#include <stdbool.h>
#include "cmp_eq_mult_scalar_simple_I1.h"
#include "cmp_eq_mult_scalar_simple_I2.h"
#include "cmp_eq_mult_scalar_simple_I4.h"
#include "cmp_eq_mult_scalar_simple_I8.h"
#include "cmp_eq_mult_scalar_log_I1.h"
#include "cmp_eq_mult_scalar_log_I2.h"
#include "cmp_eq_mult_scalar_log_I4.h"
#include "cmp_eq_mult_scalar_log_I8.h"
// START FUNC DECL
void
cmp_eq_mult_scalar_I4( 
			      int *in1, 
			      long long n1, 
			      int *in2, 
			      int n2, 
			      char * restrict out
			       )
// STOP FUNC DECL
{
  if ( n2 <= 8 ) {
    cmp_eq_mult_scalar_simple_I4( in1, n1, in2, n2, out);
  }
  else 
    cmp_eq_mult_scalar_log_I4( in1, n1, in2, n2, out);
}
