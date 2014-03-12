#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
union___XTYPE__(
         __ITYPE__ *f1,
         long long nR1,
         __ITYPE__ *f2,
         long long nR2,
         __ITYPE__ *f3,
         long long *ptr_nR3,
         unsigned __ITYPE__ mask
   )
// STOP FUNC DECL
{
  int status = 0;
           __ITYPE__ f1val,     f2val;
  unsigned __ITYPE__ eff_f1val, eff_f2val;
  unsigned __ITYPE__ cnt1,      cnt2, cnt;
  unsigned __ITYPE__ inv_mask = ~mask;
  unsigned __ITYPE__ uf3val;
  long long nR3 = 0;

  *ptr_nR3 = -1;
  __ITYPE__ *endf1 = f1 + nR1;
  __ITYPE__ *endf2 = f2 + nR2;
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
          *f3 = (__ITYPE__)uf3val;
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
