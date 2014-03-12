#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
// last review 9/10/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
get_val(
	char *tbl,
	char *fld,
	char *str_idx,
	char *rslt_buf,
	int sz_rslt_buf
	)
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  char *X    = NULL; size_t nX    = 0;
  char *nn_X = NULL; size_t nn_nX = 0; 
  int tbl_id, fld_id, nn_fld_id;
  long long nR;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_idx == NULL ) || ( *str_idx == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
  if ( fld_id < 0 ) { 
    fprintf(stderr, "Field [%s] not in Table [%s] \n", fld, tbl);
    go_BYE(-1);
  }
  status = get_data(fld_rec, &X, &nX, true); cBYE(status);
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, true); cBYE(status);
  }
  //--------------------------------------------------------
  long long idx;
  status = stoI8(str_idx, &idx); cBYE(status);
  if ( ( idx < 0 ) || ( idx >= nR ) ) { go_BYE(-1); }
  if ( ( nn_X != NULL ) &&  ( nn_X[idx] == 0 ) ) { 
    sprintf(rslt_buf, "\"\"");
  }
  else {
  switch ( fld_rec.fldtype ) { 
    case I1 : sprintf(rslt_buf, "\"%d\"", (int) ((char *)X)[idx]); break;
    case I2 : sprintf(rslt_buf, "\"%d\"", (int) ((short *)X)[idx]); break;
    case I4 : sprintf(rslt_buf, "\"%d\"", (int) ((int  *)X)[idx]); break;
    case I8 : sprintf(rslt_buf, "\"%lld\"", (long long) ((long long  *)X)[idx]); break;
    case F4 : sprintf(rslt_buf, "\"%f\"", (float) ((float  *)X)[idx]); break;
    case F8 : sprintf(rslt_buf, "\"%lf\"", (double) ((double  *)X)[idx]); break;
    default : go_BYE(-1); break;
  }
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  return(status);
}
