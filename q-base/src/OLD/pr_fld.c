#include "qtypes.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mmap.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "auxil.h"
#include "./AUTOGEN/GENFILES/extract_I8.h"
#include "./AUTOGEN/GENFILES/pr_fld_I1.h"
#include "./AUTOGEN/GENFILES/pr_fld_I2.h"
#include "./AUTOGEN/GENFILES/pr_fld_I4.h"
#include "./AUTOGEN/GENFILES/pr_fld_I8.h"

#define MAX_NUM_FLDS_TO_PR 16
int
core_pr_flds(
	     int tbl_id,
	     const char *str_flds,
	     const char *cfld_X,
	     long long lb,
	     long long ub,
	     FILE *ofp
	     )
{
  int status = 0;

  FLD_REC_TYPE fld_rec[MAX_NUM_FLDS_TO_PR]; 
  int fld_id[MAX_NUM_FLDS_TO_PR];

  FLD_REC_TYPE nn_fld_rec[MAX_NUM_FLDS_TO_PR]; 
  int nn_fld_id[MAX_NUM_FLDS_TO_PR];

  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd = false;

  char *X[MAX_NUM_FLDS_TO_PR]; 
  size_t nX[MAX_NUM_FLDS_TO_PR] ;

  char *nn_X[MAX_NUM_FLDS_TO_PR];
  size_t nn_nX[MAX_NUM_FLDS_TO_PR] ;

  char **flds = NULL; int n_flds = 0;

  if ( ofp == NULL ) { go_BYE(-1); }

  status = explode(str_flds, ':', &flds, &n_flds);
  cBYE(status);
  if ( n_flds == 0 ) { go_BYE(-1); }
  if ( n_flds > MAX_NUM_FLDS_TO_PR ) { go_BYE(-1); }

  for ( int i = 0; i < n_flds; i++ ) { 
    //--------------------------------------------------------
    status = is_fld(NULL, tbl_id, flds[i], &(fld_id[i]), &(fld_rec[i]), 
		    &(nn_fld_id[i]), &(nn_fld_rec[i]));
    if ( fld_id[i] < 0 ) { 
      fprintf(stderr, "Field [%s] not found \n", flds[i]); go_BYE(-1);
    }
    mcr_cd;
    status = rs_mmap(fld_rec[i].filename, &(X[i]), &(nX[i]), 0); cBYE(status);
    mcr_uncd;
    if ( nn_fld_id[i] >= 0 ) { 
      mcr_cd;
      status = rs_mmap(nn_fld_rec[i].filename, &(nn_X[i]), &(nn_nX[i]), 0); cBYE(status);
      mcr_uncd;
    }
  }
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      continue;
    }
    for ( int j = 0; j < n_flds; j++ ) {
    if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
      // do nothing
      continue;
    }
    char *Y;
    char ival1; short ival2; int ival4; long long ival8;
    float fval4; double fval8;
      switch ( fld_rec[j].fldtype ) { 
	case I1 : 
	  Y = X[j] + 1*i;
	  ival1 = *(char *)Y;
	  fprintf(ofp, "%d", ival1);
	  break;
	case I2 : 
	  Y = X[j] + 2*i;
	  ival2 = *((short *)Y);
	  fprintf(ofp, "%d", ival2);
	  break;
	case I4 : 
	  Y = X[j] + 4*i;
	  ival4 = *((int *)Y);
	  fprintf(ofp, "%d", ival4);
	  break;
	case I8 : 
	  Y = X[j] + 8*i;
	  ival8 = *((long long *)Y);
	  fprintf(ofp, "%lld", ival8);
	  break;
	case F4 : 
	  fval4 = *((float *)Y);
	  Y = X[j] + 4*i;
	  fprintf(ofp, "%f", fval4);
	  break;
	case F8 : 
	  fval8 = *((double *)Y);
	  Y = X[j] + 8*i;
	  fprintf(ofp, "%lf", fval8);
	  break;
	default : 
	  go_BYE(-1);
	  break;
      }
      if ( j < (n_flds-1) ) { // if not last field, then print comma
	fprintf(ofp, ",");
      }
      else { // if last field, then print eoln
        fprintf(ofp,"\n");
      }
    }
  }
BYE:
  if ( flds != NULL ) { 
    for ( int i = 0; i < n_flds; i++ ) { 
      free_if_non_null(flds[i]);
      rs_munmap(X[i], nX[i]);
      rs_munmap(nn_X[i], nn_nX[i]);
    }
    free_if_non_null(flds);
  }
  return(status);
}

int
core_pr_fld(
	    int tbl_id,
	    const char *fld,
	    const char *cfld_X,
	    long long lb,
	    long long ub,
	    FILE *ofp
	    )
{
  int status = 0;
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd = false;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;


     
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  if ( fld_id < 0 ) { 
    fprintf(stderr, "Field [%s] not found \n", fld); go_BYE(-1);
  }
  mcr_cd;
  status = rs_mmap(fld_rec.filename, &X, &nX, 0); cBYE(status);
  mcr_uncd;
  if ( nn_fld_id >= 0 ) { 
    mcr_cd;
    status = rs_mmap(nn_fld_rec.filename, &nn_X, &nn_nX, 0); cBYE(status);
    mcr_uncd;
  }
  //--------------------------------------------------------
  switch ( fld_rec.fldtype )  {
  case I1 : 
    pr_fld_I1((char *)X, lb, ub, nn_X, cfld_X, ofp);
    break;
  case I2 : 
    pr_fld_I2((short *)X, lb, ub, nn_X, cfld_X, ofp);
    break;
  case I4 : 
    pr_fld_I4((int *)X, lb, ub, nn_X, cfld_X, ofp);
    break;
  case I8 : 
    pr_fld_I8((long long *)X, lb, ub, nn_X, cfld_X, ofp);
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  return(status);
}
//---------------------------------------------------------------
// START FUNC DECL
int 
pr_fld(
       const char *tbl,
       const char *fld,
       const char *filter,
       FILE *ofp
       )
// STOP FUNC DECL
{
  int status = 0;
  char cfld[MAX_LEN_FLD_NAME+1];
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd = false;

  int cfld_id;  FLD_REC_TYPE cfld_rec;
  int nn_cfld_id;  FLD_REC_TYPE nn_cfld_rec;
  int tbl_id; TBL_REC_TYPE tbl_rec;
  char *cfld_X = NULL; size_t cfld_nX = 0;

  long long nR, lb = 0, ub = 0;
  bool is_lb_null = true, is_ub_null = true, is_cfld_null = true;
  //----------------------------------------------------------------
  zero_string(cfld, MAX_LEN_FLD_NAME+1);
  zero_string(cwd,  MAX_LEN_DIR_NAME+1);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ofp == NULL ) { go_BYE(-1); }
  //--------------------------------------------------------

  status = is_tbl(tbl, &tbl_id, &tbl_rec);
  nR = tbl_rec.nR;
  if ( ( filter != NULL ) && ( *filter != '\0' ) ) {
    status = extract_I8(cfld, "lb=[", "]", &lb, &is_lb_null); cBYE(status);
    status = extract_I8(cfld, "ub=[", "]", &lb, &is_ub_null); cBYE(status);
    status = extract_S(cfld, "cond=[", "]", cfld, MAX_LEN_FLD_NAME, 
		       &is_cfld_null);
  }
  if ( ( is_lb_null ) || ( is_ub_null ) )  {
    lb = 0;
    ub = nR;
  }
  else {
    if ( lb < 0   ) { go_BYE(-1); }
    if ( ub >= nR ) { go_BYE(-1); }
    if ( lb >= ub ) { go_BYE(-1); }
  }
  if ( is_cfld_null == false ) { 
    status = is_fld(tbl, -1, cfld, &cfld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec);
    if ( cfld_id < 0 ) { 
      fprintf(stderr, "Field [%s] not found \n", cfld); go_BYE(-1);
    }
    mcr_cd;
    status = rs_mmap(cfld_rec.filename, &cfld_X, &cfld_nX, 0); cBYE(status);
    mcr_uncd;
  }
  int n_flds = 1;
  for ( const char *cptr = fld; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) { n_flds++; }
  }
  if ( n_flds > MAX_NUM_FLDS_TO_PR ) { go_BYE(-1); }

  if ( n_flds == 1 ) {
    status = core_pr_fld(tbl_id, fld, cfld_X, lb, ub, ofp); cBYE(status);
  }
  else {
    status = core_pr_flds(tbl_id, fld, cfld_X, lb, ub, ofp); cBYE(status);
  }
  cBYE(status);
 BYE:
  rs_munmap(cfld_X, cfld_nX);
  return(status);
}
