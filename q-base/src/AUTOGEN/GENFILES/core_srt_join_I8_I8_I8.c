#ifdef ICC
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "assign_I1.h"
#include "assign_I4.h"
#include "assign_I8.h"


static int
set_init(
	 char *nn_dst_val,
	 void *in_dst_val,
	 long long lb,
	 long long ub,
	 int ijoin_op
	 )
{
  int status = 0;

  // Set nn value as default = false
  if ( nn_dst_val != NULL ) { 
#ifdef ICC
    ippsZero_8u(nn_dst_val+lb, (ub-lb));
#else
    assign_const_I1(nn_dst_val+lb, (ub-lb), FALSE);
#endif
  }
  //------------------------------------
  long long *dst_val = NULL; 
  long long *dst_val_I8 = NULL;
  char      *dst_val_I1 = NULL;
  if ( in_dst_val != NULL ) { 
    dst_val = (long long *)in_dst_val; dst_val += lb;
    switch ( ijoin_op ) {
    case join_exists : 
      dst_val_I1 = (char *)in_dst_val; dst_val += lb;
      assign_const_I1(dst_val_I1, (ub-lb), 0);
      break;
    case join_minidx : 
    case join_maxidx : 
      dst_val_I8 = (long long *)in_dst_val; dst_val += lb;
      assign_const_I8(dst_val_I8, (ub-lb), -1);
      break;
    case join_sum : 
    case join_cnt : 
      dst_val_I8 = (long long *)in_dst_val; dst_val += lb;
      assign_const_I8(dst_val_I8, (ub-lb), 0);
      break;
    case join_reg : 
    case join_or : 
      assign_const_I8(dst_val, (ub-lb), 0);
      break;
    case join_and : 
      assign_const_I8(dst_val, (ub-lb), 0xFFFFFFFFFFFFFFFF);
      break;
      /*---------------------------------------------------------*/
    case join_min : 
      assign_const_I8(dst_val, (ub-lb), LLONG_MAX);
      break;
      /*---------------------------------------------------------*/
    case join_max : 
      assign_const_I8(dst_val, (ub-lb), LLONG_MIN);
      break;
      /*---------------------------------------------------------*/
    default : 
      go_BYE(-1);
      break;
    }
  }
 BYE:
  return status ;
}
/* When we do a join we have the following 2*2*2 possibilities
   SL : src lnk = I4 or I8
   SV : src val = I4 or I8
   DL : dst lnk = I4 or I8
   DV = same as SV, except it is I8 when op = cnt, sum, minidx, maxidx

   SL1 = I4 or I8, SL2 = int or long long
   SV1 = I4 or I8, SV2 = int or long long
   DL1 = I4 or I8, DL2 = int or long long


   SVMAX= INT_MAX or LL_MAX
   SVMIN= INT_MIN or LL_MIN
   SVAND= 0xFFFFFFFF or 0xFFFFFFFFFFFFFFFF
*/

// START FUNC DECL
int
core_srt_join_I8_I8_I8(
				      long long *src_lnk,
				      long long *src_val,
				      long long src_nR,
				      long long *dst_lnk,
				      long long *dst_val,
				      char *nn_dst_val,
				      long long dst_lb,
				      long long dst_ub,
				      int ijoin_op,
				      bool *ptr_is_any_null
				      )
// STOP FUNC DECL
{
  int status = 0;
  long long src_idx;
  long long v_dst_lnk; 
  long long v_src_val; 
  long long v_src_lnk;
  bool found, prev_val_known = false;

  status = set_init(nn_dst_val, dst_val, dst_lb, dst_ub, ijoin_op);
  cBYE(status);

  //-------------------------------------------------
  src_idx = 0;
  for ( long long dst_idx = dst_lb; dst_idx < dst_ub; dst_idx++ ) {
    v_dst_lnk = dst_lnk[dst_idx];
    /* If the current value of the dst lnk is the same as the previous
     * value of the dst lnk, then the current value of dst src must be
     * the same as previous value of dst src */
    /* if not the first dst value */
    if ( dst_idx > dst_lb ) { 
      /* if current dst value same as previous */
      if ( v_dst_lnk == dst_lnk[dst_idx-1] ) { 
	dst_val[dst_idx] = dst_val[dst_idx-1];
	if ( nn_dst_val != NULL ) { 
          nn_dst_val[dst_idx] = nn_dst_val[dst_idx-1];
	}
        continue; /* Move on to next value in dst tbl */
      }
    }
    /* If you have consumed all src values, get out. Note that dst_val
     * has been initialzied in set_init() earlier */
    if ( src_idx >= src_nR ) { break; }
    /*----------------------------------------------------------*/
    found = false;
    /* Advance src marker until src_lnk_val >= dst_lnk_val */
    for ( ; src_idx < src_nR; ) { 
      v_src_lnk = src_lnk[src_idx];
      if ( v_src_lnk == v_dst_lnk ) {
	found = true;
	prev_val_known = true;
	break;
      }
      else if ( v_src_lnk > v_dst_lnk ) {
	// We put the read check to avoid unnecessary writing 
	if ( *ptr_is_any_null == false ) {
          *ptr_is_any_null = true;
	}
	found = false;
	break;
      }
#define FANCY_JUMPING
#ifdef FANCY_JUMPING
      /* Consider doing some fancy jumping here */
      long long delta = 1;
      long long expt_src_idx, prev_expt_src_idx;
      prev_expt_src_idx = src_idx + delta;
      for ( ; ; ) { 
        expt_src_idx = src_idx + delta;
	if ( expt_src_idx >= src_nR ) {
	  expt_src_idx = prev_expt_src_idx;
	  break;
	}
        v_src_lnk = src_lnk[expt_src_idx];
        if ( v_src_lnk >= v_dst_lnk ) {
	  expt_src_idx = prev_expt_src_idx;
	  break;
        }
	else if ( v_src_lnk < v_dst_lnk ) {
	  delta *= 2;
          prev_expt_src_idx = expt_src_idx;
        }
      }
      src_idx = expt_src_idx;
#else
      src_idx++;
#endif
      /*--------------------------------------------*/
    }
    if ( found == true ) {
      if ( nn_dst_val != NULL ) { 
        nn_dst_val[dst_idx] = TRUE;
      }
      /* In this loop, we consume all the values in the source table
       * where the src_lnk == dst_lnk */
      for ( ; src_idx < src_nR; src_idx++ ) { 
        v_src_lnk = src_lnk[src_idx];
	if ( v_src_lnk != v_dst_lnk ) { // went too far
	  src_idx--; 
	  break;
	}
	v_src_val = src_val[src_idx];
	switch ( ijoin_op ) {
	case join_minidx : 
	  if ( dst_val[dst_idx] < 0 ) { dst_val[dst_idx] = src_idx; }
	  break;
	case join_maxidx : 
	  dst_val[dst_idx] = max(src_idx, dst_val[dst_idx]);
	  break;
	case join_exists : 
	  dst_val[dst_idx]  = 1;
	  break;
	case join_cnt : 
	  dst_val[dst_idx] +=  1;
	  break;
	case join_sum : 
	  dst_val[dst_idx] +=  v_src_val;
	  break;
	case join_max : 
	  dst_val[dst_idx] = max(v_src_val, dst_val[dst_idx]);
	  break;
	case join_min : 
	  dst_val[dst_idx] = min(v_src_val, dst_val[dst_idx]);
	  break;
	case join_or : 
	  dst_val[dst_idx] |=  v_src_val;
	  break;
	case join_and : 
	  dst_val[dst_idx] &=  v_src_val;
	  break;
	case join_reg : 
	  dst_val[dst_idx] =   v_src_val;
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
    }
    else {
      if ( nn_dst_val != NULL ) { 
        nn_dst_val[dst_idx] = FALSE;
        // We put the read check to avoid unnecessary writing 
        if ( *ptr_is_any_null == false ) {
          *ptr_is_any_null = true;
        }
      }
    }
  }
  /* We have a convention that if nn=0, then value must be 0 */
  if ( ( nn_dst_val != NULL ) && ( *ptr_is_any_null ) ) {
    for ( long long dst_idx = dst_lb; dst_idx < dst_ub; dst_idx++ ) {
      if ( nn_dst_val[dst_idx] == FALSE ) { 
        dst_val[dst_idx] = 0;
      }
    }
  }
 BYE:
  return(status);
}
