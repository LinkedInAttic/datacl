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
#ifdef ICC
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "assign_I1.h"
#include "assign_I4.h"
#include "assign_I8.h"

static int
set_init(
	 char *nn_dst_val,
	 int *dst_val,
	 long long lb,
	 long long ub,
	 int ijoin_op
	 )
{
  int status = 0;

  // Set nn value as default = false
#ifdef ICC
  ippsSet_8u(0, nn_dst_val+lb, (ub-lb));
#else
  assign_const_I1(nn_dst_val+lb, FALSE, (ub-lb));
#endif
  //------------------------------------
  if ( dst_val != NULL ) { 
    int *this_dst_val = dst_val;
    this_dst_val += lb;
    switch ( ijoin_op ) {
    case MJOIN_OP_REG : 
    case MJOIN_OP_SUM : 
    case MJOIN_OP_CNT : 
    case MJOIN_OP_OR : 
      assign_const_I4(this_dst_val, FALSE, (ub-lb));
      break;
      /*---------------------------------------------------------*/
      /* TODO: Need to deal with deafult values for AND, MIN, MAX */
    case MJOIN_OP_AND : 
      assign_const_I4(this_dst_val, 0xFFFFFFFF, (ub-lb));
      break;
      /*---------------------------------------------------------*/
    case MJOIN_OP_MIN : 
      assign_const_I4(this_dst_val, INT_MAX, (ub-lb));
      break;
      /*---------------------------------------------------------*/
    case MJOIN_OP_MAX : 
      assign_const_I4(this_dst_val, INT_MIN, (ub-lb));
      break;
      /*---------------------------------------------------------*/
    default : 
      go_BYE(-1);
      break;
    }
  }
 BYE:
  return(status);
}
/* When we do a join we have the following possibilities
   SL : src lnk = I or LL
   SV : src val = I or LL
   DL : dst lnk = I or LL

   SL1 = I or L
   SV1 = I or L
   DL1 = I or L

   SL2 = int or long long
   SV2 = int or long long
   DL2 = int or long long

   SV3 = int or longlong
   SVMAX= INT_MAX or LL_MAX
   SVMIN= INT_MIN or LL_MIN
   SVAND= 0xFFFFFFFF or 0xFFFFFFFFFFFFFFFF
*/

// START FUNC DECL
int
core_srt_join_I_I_L(
				      int *src_lnk,
				      int *src_val,
				      long long src_nR,
				      long long *dst_lnk,
				      int *dst_val,
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
  int v_src_val; 
  int v_src_lnk;
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
    if ( dst_idx > 0 ) {
      if ( v_dst_lnk == dst_lnk[dst_idx-1] ) {
	if ( dst_val != NULL ) { 
	  dst_val[dst_idx] = dst_val[dst_idx-1];
	}
        nn_dst_val[dst_idx] = nn_dst_val[dst_idx-1];
        continue; /* Move on to next value in dst tbl */
      }
    }
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
      nn_dst_val[dst_idx] = TRUE;
      /* In this loop, we consume all the values in the source table
       * where the src_lnk == dst_lnk */
      for ( ; src_idx < src_nR; src_idx++ ) { 
        v_src_lnk = src_lnk[src_idx];
	if ( v_src_lnk != v_dst_lnk ) { // went too far
	  src_idx--; 
	  break;
	}
	if ( src_val != NULL ) { 
	  v_src_val = src_val[src_idx];
	  switch ( ijoin_op ) {
	  case MJOIN_OP_SUM : 
	    dst_val[dst_idx] +=  v_src_val;
	    break;
	  case MJOIN_OP_MAX : 
	    if ( v_src_val > dst_val[dst_idx] ) { 
	      dst_val[dst_idx] = v_src_val;
	    }
	    break;
	  case MJOIN_OP_MIN : 
	    if ( v_src_val < dst_val[dst_idx] ) { 
	      dst_val[dst_idx] = v_src_val;
	    }
	    break;
	  case MJOIN_OP_OR : 
	    dst_val[dst_idx] |=  v_src_val;
	    break;
	  case MJOIN_OP_AND : 
	    dst_val[dst_idx] &=  v_src_val;
	    break;
	  case MJOIN_OP_REG : 
	    dst_val[dst_idx] =   v_src_val;
	    break;
	  default : 
	    go_BYE(-1);
	    break;
	  }
	}
	else {
	  if ( ijoin_op == MJOIN_OP_CNT ) { 
            dst_val[dst_idx] += 1;
	  }
	}
      }
    }
    else {
      // We put the read check to avoid unnecessary writing 
      if ( *ptr_is_any_null == false ) {
        *ptr_is_any_null = true;
      }
      nn_dst_val[dst_idx] = FALSE;
    }
  }
  if ( *ptr_is_any_null ) { /* Set to 0 where undefined */
    for ( long long dst_idx = dst_lb; dst_idx < dst_ub; dst_idx++ ) {
      if ( nn_dst_val[dst_idx] == FALSE ) { 
	dst_val[dst_idx] = 0;
      }
    }
  }
 BYE:
  return(status);
}
