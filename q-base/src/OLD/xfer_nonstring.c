#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
xfer_nonstring(
    FLD_TYPE *src_fld_meta,
    long long src_nR,
    char *src_fld_X,
    char *nn_src_fld_X,
    FLD_TYPE *dst_idx_meta,
    long long dst_nR,
    char *dst_idx_X,
    char *nn_dst_idx_X,
      char *dst_tbl,
      char *dst_fld
      )
// STOP FUNC DECL
{
  int status = 0;
  char str_meta_data[256];
  char *nn_dst_fld = NULL;
  bool is_any_null = false; 

  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  FILE *ofp    = NULL; char *opfile    = NULL;

  char nn; long long idx;
  int isrc_fldtype, idst_idx_fldtype; 
  int dst_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN;
  //----------------------------------------------------------------
  //--------------------------------------------------------
  status = open_temp_file(&ofp, &opfile, 0); cBYE(status);
  status = open_temp_file(&nn_ofp, &nn_opfile, 0); cBYE(status);
  //--------------------------------------------------------
  status = mk_ifldtype(dst_idx_meta->fldtype, &idst_idx_fldtype); cBYE(status);
  status = mk_ifldtype(src_fld_meta->fldtype, &isrc_fldtype); cBYE(status);
  for ( long long i = 0; i < dst_nR; i++ ) {
    float ftemp; int itemp; long long lltemp; char btemp;
    if ( ( nn_dst_idx_X != NULL ) && ( nn_dst_idx_X[i] == FALSE ) ) { 
      btemp = itemp = lltemp = ftemp = 0; nn = FALSE;
      switch ( isrc_fldtype ) { 
	case FLDTYPE_FLOAT : 
          fwrite(&ftemp, sizeof(float), 1, ofp);
	  break;
	case FLDTYPE_INT : 
          fwrite(&itemp, sizeof(int), 1, ofp);
	  break;
	case FLDTYPE_LONGLONG : 
          fwrite(&lltemp, sizeof(long long), 1, ofp);
	  break;
	case FLDTYPE_BOOL : 
          fwrite(&btemp, sizeof(char), 1, ofp);
	  break;
	default : 
	  go_BYE(-1);
	  break;
      }
      fwrite(&nn, sizeof(char), 1, nn_ofp); 
      is_any_null = true;
    }
    else {
      nn = TRUE; /* This depends on nn_src_fld_X */
      switch ( idst_idx_fldtype ) { 
	case FLDTYPE_INT : 
          idx = ((int *)dst_idx_X)[i];
	  break;
	case FLDTYPE_LONGLONG : 
          idx = ((long long *)dst_idx_X)[i];
	  break;
	default : 
	  go_BYE(-1);
	  break;
      }
      if ( ( idx < 0 ) || ( idx >= src_nR ) ) { go_BYE(-1); }
      ftemp = itemp = btemp = lltemp = 0;
      switch ( isrc_fldtype ) { 
	case FLDTYPE_FLOAT : 
	  if ( ( nn_src_fld_X == NULL ) || ( nn_src_fld_X[idx] == TRUE ) ) { 
	    ftemp = ((float *)src_fld_X)[idx];
	  }
          fwrite(&ftemp, sizeof(float), 1, ofp);
          fwrite(&nn, sizeof(char), 1, nn_ofp); 
	  break;
	case FLDTYPE_INT : 
	  if ( ( nn_src_fld_X == NULL ) || ( nn_src_fld_X[idx] == TRUE ) ) { 
	    itemp = ((int *)src_fld_X)[idx];
	  }
          fwrite(&itemp, sizeof(int), 1, ofp);
          fwrite(&nn, sizeof(char), 1, nn_ofp); 
	  break;
	case FLDTYPE_BOOL : 
	  if ( ( nn_src_fld_X == NULL ) || ( nn_src_fld_X[idx] == TRUE ) ) { 
	    btemp = ((char *)src_fld_X)[idx];
	  }
          fwrite(&btemp, sizeof(char), 1, ofp);
          fwrite(&nn, sizeof(char), 1, nn_ofp); 
	  break;
	case FLDTYPE_LONGLONG : 
	  if ( ( nn_src_fld_X == NULL ) || ( nn_src_fld_X[idx] == TRUE ) ) { 
	    lltemp = ((long long *)src_fld_X)[idx];
	  }
          fwrite(&lltemp, sizeof(long long), 1, ofp);
          fwrite(&nn, sizeof(char), 1, nn_ofp); 
	  break;
	default:
	  go_BYE(-1);
	  break;
      }
    }
  }
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  // Add output field to meta data 
  zero_string(str_meta_data, 256);
  sprintf(str_meta_data, "fldtype=%s:n_sizeof=%d", src_fld_meta->fldtype,
      src_fld_meta->n_sizeof);
  strcat(str_meta_data, ":filename=");
  strcat(str_meta_data, opfile);
  status = add_fld(dst_tbl, dst_fld, str_meta_data, &dst_fld_id);
  cBYE(status);
  if ( is_any_null == true ) {
    status = add_aux_fld(dst_tbl, dst_fld, nn_opfile, "nn", &nn_dst_fld_id);
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
  }
 BYE:
  free_if_non_null(nn_dst_fld);
  free_if_non_null(nn_opfile);
  return(status);
}
