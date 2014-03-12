#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
union_I8(
         long long *f1,
         long long nR1,
         long long *f2,
         long long nR2,
         long long *f3,
         long long *ptr_nR3,
         unsigned long long mask
   )
// STOP FUNC DECL
{
  int status = 0;
           long long f1val,     f2val;
  unsigned long long eff_f1val, eff_f2val;
  unsigned long long cnt1,      cnt2, cnt;
  unsigned long long inv_mask = ~mask;
  unsigned long long uf3val;
  long long nR3 = 0;

  *ptr_nR3 = -1;
  long long *endf1 = f1 + nR1;
  long long *endf2 = f2 + nR2;
 // cilk loop
  cilkfor ( ; ( ( f1 != endf1 ) || ( f2 != endf2 ) ); ) {
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
          *f3 = (long long)uf3val;
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
