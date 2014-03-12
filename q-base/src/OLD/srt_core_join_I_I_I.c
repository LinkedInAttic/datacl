#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "assign_char.h"
#include "assign_int.h"

static int
    set_init(
	char *nn_dst_val,
	int *dst_val,
	long long n,
	int ijoin_op
	)
    {
      int status = 0;
  int block_size = 65536;

  // Set nn value as default = false
  for ( long long i = 0; i < n; i += block_size ) {
    if ( ( (i+1) * block_size ) < n ) {
      assign_const_char(nn_dst_val+i, FALSE, block_size);
    }
    else { // last block 
      assign_const_char(nn_dst_val+i, FALSE, (n - (i*block_size)));
    }
  }
  //------------------------------------
  switch ( ijoin_op ) {
  case MJOIN_OP_REG : 
  case MJOIN_OP_SUM : 
  case MJOIN_OP_OR : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_int(dst_val+i, FALSE, block_size);
      }
      else { // last block 
	assign_const_int(dst_val+i, FALSE, (n - (i*block_size)));
      }
    }
    break;
    /*---------------------------------------------------------*/
  case MJOIN_OP_AND : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_int(dst_val+i, 0xFFFFFFFF, block_size);
      }
      else { // last block 
	assign_const_int(dst_val+i, 0xFFFFFFFF, (n - (i*block_size)));
      }
    }
    break;
    /*---------------------------------------------------------*/
  case MJOIN_OP_MIN : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_int(dst_val+i, INT_MAX, block_size);
      }
      else { // last block 
	assign_const_int(dst_val+i, INT_MAX, (n - (i*block_size)));
      }
    }
    break;
    /*---------------------------------------------------------*/
  case MJOIN_OP_MAX : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_int(dst_val+i, INT_MIN, block_size);
      }
      else { // last block 
	assign_const_int(dst_val+i, INT_MIN, (n - (i*block_size)));
      }
    }
    break;
    /*---------------------------------------------------------*/
  default : 
    go_BYE(-1);
    break;
  }
BYE:
  return(status);
}
/* When we do a join we have the following possibilities
   src lnk = I or LL
   src val = I or LL
   dst lnk = I or LL
   dst val = I or LL
*/

// START FUNC DECL
int
srt_core_join_I_I_I(
			  int *src_lnk,
			  int *src_val,
			  long long src_nR,
			  int *dst_lnk,
			  int *dst_val,
			  char *nn_dst_val,
			  long long dst_nR,
			  int ijoin_op,
			  bool *ptr_is_any_null
			  )
// STOP FUNC DECL
{
  int status = 0;
  long long src_idx;
  int v_dst_lnk, v_src_val, v_src_lnk;
  bool found, is_any_null;

  status = set_init(nn_dst_val, dst_val, dst_nR, ijoin_op);
  cBYE(status);

  //-------------------------------------------------
  src_idx = 0;
  for ( long long dst_idx = 0; dst_idx < dst_nR; dst_idx++ ) {
    v_dst_lnk = dst_lnk[dst_idx];
    /* If the current value of the dst lnk is the same as the previous
     * value of the dst lnk, then the current value of dst src must be
     * the same as previous value of dst src */
    if ( dst_idx > 0 ) {
      if ( v_dst_lnk == dst_lnk[dst_idx-1] ) {
	dst_val[dst_idx] = dst_val[dst_idx-1];
        nn_dst_val[dst_idx] = nn_dst_val[dst_idx-1];
        continue; /* Move on to next value in dst tbl */
      }
    }
    found = false;
    /* Advance src marker until src_lnk_val >= dst_lnk_val */
    for ( ; src_idx < src_nR; src_idx++ ) {
      v_src_lnk = src_lnk[src_idx];
      if ( v_src_lnk == v_dst_lnk ) {
	found = true;
	break;
      }
      else if ( v_src_lnk > v_dst_lnk ) {
        is_any_null = true;
	found = false;
	break;
      }
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
    }
    else {
      is_any_null = true;
      nn_dst_val[dst_idx] = FALSE;
    }
  }
  *ptr_is_any_null = is_any_null;
 BYE:
  return(status);
}
