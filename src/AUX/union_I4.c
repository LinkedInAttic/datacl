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
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
union_I4(
         int *f1,
         long long nR1,
         int *f2,
         long long nR2,
         int *f3,
         long long *ptr_nR3,
         unsigned int mask
   )
// STOP FUNC DECL
{
  int status = 0;
           int f1val,     f2val;
  unsigned int eff_f1val, eff_f2val;
  unsigned int cnt1,      cnt2, cnt;
  unsigned int inv_mask = ~mask;
  unsigned int uf3val;
  long long nR3 = 0;

  *ptr_nR3 = -1;
  int *endf1 = f1 + nR1;
  int *endf2 = f2 + nR2;
  for ( ; ( ( f1 != endf1 ) || ( f2 != endf2 ) ); ) {
    if ( f1 == endf1 ) { /* copy items from f2 */
      f2val = *f2;
      *f3 = f2val;
      f3++;
      f2++;
      nR3++;
    }
    else if ( f2 == endf2 ) { /* copy items from f1 */
      f1val = *f1;
      *f3 = f1val;
      f3++;
      f1++;
      nR3++;
    }
    else {
      f1val = *f1;
      f2val = *f2;
      if ( mask != 0 ) {
        eff_f1val = f1val & inv_mask;
        eff_f2val = f2val & inv_mask;
      }
      else {
        eff_f1val = f1val;
        eff_f2val = f2val;
      }
      if ( eff_f1val < eff_f2val ) { 
        *f3 = f1val;
        f1++;
      }
      else if ( eff_f1val > eff_f2val ) { 
        *f3 = f2val;
        f2++;
      }
      else if ( eff_f1val == eff_f2val ) {
        if ( mask == 0 ) { /* plain union */
          *f3 = f1val;
        }
        else {
          /* Here is the pvalcalc specific stuff */
          cnt1 = eff_f1val & mask;
          cnt2 = eff_f2val & mask;
          cnt = cnt1 + cnt2;
          uf3val = eff_f1val | cnt;
          *f3 = (int)uf3val;
        }
        f1++;
        f2++;
      }
      f3++;
      nR3++;
    }
  }
  *ptr_nR3 = nR3;
  return(status);
}
