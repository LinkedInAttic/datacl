#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "mmap.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "assign_I1.h"
#include "assign_I4.h"

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
      assign_const_I1(nn_dst_val+i, block_size, FALSE);
    }
    else { // last block 
      assign_const_I1(nn_dst_val+i, (n - (i*block_size)), FALSE);
    }
  }
  //------------------------------------
  switch ( ijoin_op ) {
  case MJOIN_OP_REG : 
  case MJOIN_OP_SUM : 
  case MJOIN_OP_OR : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_I4(dst_val+i, block_size, FALSE);
      }
      else { // last block 
	assign_const_I4(dst_val+i, (n - (i*block_size)), FALSE);
      }
    }
    break;
  case MJOIN_OP_MIN : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_I4(dst_val+i, block_size, INT_MAX);
      }
      else { // last block 
	assign_const_I4(dst_val+i, (n - (i*block_size)), INT_MAX);
      }
    }
    break;
  case MJOIN_OP_MAX : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_I4(dst_val+i, block_size, INT_MIN);
      }
      else { // last block 
	assign_const_I4(dst_val+i, (n - (i*block_size)), INT_MIN);
      }
    }
    break;
  case MJOIN_OP_AND : 
    for ( long long i = 0; i < n; i += block_size ) {
      if ( ( (i+1) * block_size ) < n ) {
	assign_const_I4(dst_val+i, block_size, 0xFFFFFFFF);
      }
      else { // last block 
	assign_const_I4(dst_val+i, (n - (i*block_size)), 0xFFFFFFFF);
      }
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
BYE:
  return(status);
}
/* When we do a join we have the following possibilities
   src lnk = I or LL
   src idx = I or LL
   src val = I or LL
   dst lnk = I or LL
   dst idx = I or LL
   dst val = I or LL
*/

// START FUNC DECL
int
core_join_I_I_I_I_I_I(
			  int *src_lnk,
			  int *src_idx,
			  int *src_val,
			  long long src_nR,
			  long long nn_src_nR,
			  int *dst_lnk,
			  int *dst_idx,
			  int *dst_val,
			  char *nn_dst_val,
			  long long dst_nR,
			  long long nn_dst_nR,
			  int ijoin_op,
			  bool *ptr_is_any_null
			  )
// STOP FUNC DECL
{
  int status = 0;
  long long src_marker;
  int v_dst_idx;
  int v_dst_lnk;
  int v_src_idx;
  int v_src_val;
  int v_src_lnk;
  bool found, is_any_null;

  status = set_init(nn_dst_val, dst_val, nn_dst_nR, ijoin_op);
  cBYE(status);

  //-------------------------------------------------
  src_marker = 0;
  for ( long long i = 0; i < nn_dst_nR; i++ ) {
    v_dst_lnk = dst_lnk[i];
    v_dst_idx = dst_idx[i];
    if ( ( v_dst_idx < 0 ) || ( v_dst_idx >= dst_nR ) ) { go_BYE(-1); }
    found = false;
    /* Advance src marker until src_lnk_val >= dst_lnk_val */
    for ( ; src_marker < nn_src_nR; src_marker++ ) {
      v_src_lnk = src_lnk[src_marker];
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
      nn_dst_val[v_dst_idx] = TRUE;
      for ( ; src_marker < src_nR; src_marker++ ) { 
        v_src_lnk = src_lnk[src_marker];
        v_src_idx = src_idx[src_marker];
        if ( ( v_src_idx < 0 ) || ( v_src_idx >= src_nR ) ) { go_BYE(-1); }
	if ( v_src_lnk != v_dst_lnk ) { // went too far
	  src_marker--; 
	  break;
	}
	v_src_val = src_val[v_src_idx];
	switch ( ijoin_op ) { 
	case MJOIN_OP_SUM : 
	  dst_val[v_dst_idx] +=  v_src_val;
	  break;
	case MJOIN_OP_MAX : 
	  if ( v_src_val > dst_val[v_dst_idx] ) { 
	    dst_val[v_dst_idx] = v_src_val;
	  }
	  break;
	case MJOIN_OP_MIN : 
	  if ( v_src_val < dst_val[v_dst_idx] ) { 
	    dst_val[v_dst_idx] = v_src_val;
	  }
	  break;
	case MJOIN_OP_OR : 
          dst_val[v_dst_idx] |=  v_src_val;
	  break;
	case MJOIN_OP_AND : 
          dst_val[v_dst_idx] &=  v_src_val;
	  break;
	case MJOIN_OP_REG : 
          dst_val[v_dst_idx] =   v_src_val;
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
    }
  }
  *ptr_is_any_null = is_any_null;
 BYE:
  return(status);
}
