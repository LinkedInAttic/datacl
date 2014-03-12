#include "qtypes.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mmap.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "auxil.h"
#include "meta_globals.h"
#include "pr_fld.h"
#include "pr_fld_SC.h"
#include "pr_fld_SV.h"
#include "fld_meta.h"
#include "./AUTOGEN/GENFILES/extract_I8.h"
#include "./AUTOGEN/GENFILES/pr_fld_I1.h"
#include "./AUTOGEN/GENFILES/pr_fld_I2.h"
#include "./AUTOGEN/GENFILES/pr_fld_I4.h"
#include "./AUTOGEN/GENFILES/pr_fld_I8.h"
#include "./AUTOGEN/GENFILES/pr_fld_F4.h"
#include "./AUTOGEN/GENFILES/pr_fld_F8.h"

#define MAX_NUM_FLDS_TO_PR 16

int from_int_to_str(
		    int ival, 
		    int fld_len,
		    char *txtptr,
		    short *lenI2ptr,
		    long long *offI8ptr,
		    char *buffer, 
		    int buflen
		    )
{
  int status = 0;
  zero_string_to_nullc(buffer);

  if ( ( lenI2ptr == NULL) && ( offI8ptr != NULL ) ) { go_BYE(-1); }
  if ( ( lenI2ptr != NULL) && ( offI8ptr == NULL ) ) { go_BYE(-1); }
  if ( lenI2ptr == NULL ) { 
    if ( fld_len <= 0 ) { go_BYE(-1); }
    long long offset = (long long)ival * (long long)(fld_len+1);
    if ( fld_len >= buflen ) { go_BYE(-1); }
    memcpy(buffer, txtptr + offset, fld_len);
    buffer[fld_len] = '\0';
  }
  else {
    int this_len = lenI2ptr[ival];
    if ( this_len >= buflen ) { go_BYE(-1); }
    memcpy(buffer, txtptr + offI8ptr[ival], this_len);
    buffer[this_len] = '\0';
  }
 BYE:
  return(status);
}
//
//---------------------------------------------------------
int 
setup_str(
	  int fld_id,
	  char **ptr_Ztxt,
	  size_t *ptr_nZtxt,
	  char **ptr_Zlen,
	  size_t *ptr_nZlen,
	  char **ptr_Zoff,
	  size_t *ptr_nZoff
	  )
{
  int status = 0;
  int len_fld_id, off_fld_id;
  *ptr_Ztxt  = NULL;
  *ptr_nZtxt = 0;
  *ptr_Zlen  = NULL;
  *ptr_nZlen = 0;
  *ptr_Zoff  = NULL;
  *ptr_nZoff = 0;

  chk_range(fld_id, 0, g_n_fld);
  FLD_REC_TYPE fld_rec = g_flds[fld_id];
  status = get_data(fld_rec, ptr_Ztxt, ptr_nZtxt, 0); cBYE(status);
  if ( ( fld_rec.fldtype != SV )  &&( fld_rec.fldtype != SC ) ) {
    go_BYE(-1);
  }

  if ( fld_rec.fldtype == SV ) {
    status = get_fld_info(fk_fld_len, fld_id, &len_fld_id); cBYE(status);
    FLD_REC_TYPE len_rec = g_flds[len_fld_id];
    status = get_data(len_rec, ptr_Zlen, ptr_nZlen, 0); cBYE(status);

    status = get_fld_info(fk_fld_off, fld_id, &off_fld_id); cBYE(status);
    FLD_REC_TYPE off_rec = g_flds[off_fld_id];
    status = get_data(off_rec, ptr_Zoff, ptr_nZoff, 0); cBYE(status);
  }

 BYE:
  return(status);
}

//-----------------------------------------------------------------

int 
pr_escaped(
	   char *buffer, 
	   FILE *ofp
	   )
{ 
  int status = 0;
  fprintf(ofp, "\"");
  for ( char *cptr = buffer; *cptr != '\0'; cptr++ ) { 
    if ( ( *cptr == '\\' ) || ( *cptr == '"' )  ) {
      fprintf(ofp, "\\");
    }
    fprintf(ofp, "%c", *cptr);
  }
  fprintf(ofp, "\"");
  return(status);
}
// START FUNC DECL
int
core_pr_flds(
	     int tbl_id,
	     const char *str_flds,
	     char *cfld_X,
	     long long lb,
	     long long ub,
	     long long nR,
	     FILE *ofp
	     )
// STOP FUNC DECL
{
  int status = 0;
  char buffer[MAX_LEN_STR+3];

  FLD_REC_TYPE fld_rec[MAX_NUM_FLDS_TO_PR]; 
  int fld_id[MAX_NUM_FLDS_TO_PR];

  FLD_REC_TYPE nn_fld_rec[MAX_NUM_FLDS_TO_PR]; 
  int nn_fld_id[MAX_NUM_FLDS_TO_PR];

  int dict_fld_ids[MAX_NUM_FLDS_TO_PR];

  char *Ztxt[MAX_NUM_FLDS_TO_PR]; 
  char *Zlen[MAX_NUM_FLDS_TO_PR]; 
  char *Zoff[MAX_NUM_FLDS_TO_PR]; 

  size_t nZtxt[MAX_NUM_FLDS_TO_PR] ;
  size_t nZlen[MAX_NUM_FLDS_TO_PR] ;
  size_t nZoff[MAX_NUM_FLDS_TO_PR] ;

  char *X[MAX_NUM_FLDS_TO_PR]; 
  size_t nX[MAX_NUM_FLDS_TO_PR] ;

  char *nn_X[MAX_NUM_FLDS_TO_PR];
  size_t nn_nX[MAX_NUM_FLDS_TO_PR] ;

  char **flds = NULL; int n_flds = 0;

  if ( ofp == NULL ) { go_BYE(-1); }
  int dict_fld_id = -1;

  zero_string(buffer, MAX_LEN_STR+3);
  /*---------------------------------------------------------*/
  for ( int i = 0; i < MAX_NUM_FLDS_TO_PR; i++  ) { 
    X[i]    = NULL; nX[i]    = 0;
    nn_X[i] = NULL; nn_nX[i] = 0;
    dict_fld_ids[i] = -1;
    Ztxt[i] = NULL; nZtxt[i] = 0;
    Zlen[i] = NULL; nZlen[i] = 0;
    Zoff[i] = NULL; nZoff[i] = 0;
    zero_fld_rec(&(fld_rec[i]));
    zero_fld_rec(&(nn_fld_rec[i]));
    fld_id[i] = -1; nn_fld_id[i] = -1;
  }
  status = explode(str_flds, ':', &flds, &n_flds); cBYE(status);
  if ( n_flds == 0 ) { go_BYE(-1); }
  if ( n_flds > MAX_NUM_FLDS_TO_PR ) { go_BYE(-1); }
  
  for ( int i = 0; i < n_flds; i++ ) {
    //--------------------------------------------------------
    status = is_fld(NULL, tbl_id, flds[i], &(fld_id[i]), &(fld_rec[i]), 
		    &(nn_fld_id[i]), &(nn_fld_rec[i]));
    if ( fld_id[i] < 0 ) { 
      fprintf(stderr, "Field [%s] not found \n", flds[i]); go_BYE(-1);
    }
    status = get_data(fld_rec[i], &(X[i]), &(nX[i]), 0); cBYE(status);
    if ( nn_fld_id[i] >= 0 ) { 
      status = get_data(nn_fld_rec[i], &(nn_X[i]), &(nn_nX[i]), 0);cBYE(status);
    }
    /* If we have an integer field that is actually a fk to a txt field,
     * then we need to set up txt pointers for it */
    status = get_fld_info(fk_fld_txt_lkp, fld_id[i], &dict_fld_id);cBYE(status);
    if ( dict_fld_id >= 0 ) {
      status = setup_str(dict_fld_id, 
			 &(Ztxt[i]), &(nZtxt[i]), 
			 &(Zlen[i]), &(nZlen[i]), 
			 &(Zoff[i]), &(nZoff[i]));
    }
    /* If we have a text field, SC or SV, we need to set up pointers for it */
    if ( ( fld_rec[i].fldtype == SV ) || ( fld_rec[i].fldtype == SC ) ) {
      status = setup_str(fld_id[i], &(Ztxt[i]), &(nZtxt[i]), 
			 &(Zlen[i]), &(nZlen[i]), 
			 &(Zoff[i]), &(nZoff[i]));
    }
    dict_fld_ids[i] = dict_fld_id; 
  }
  /*----------------------------------------------------------*/
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) { continue; }
    for ( int j = 0; j < n_flds; j++ ) {
      if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	fprintf(ofp, "\"\"");
      }
      else {
	char *Y;
	char valI1; short valI2; int valI4; long long valI8;
	float valF4; double valF8;
	switch ( fld_rec[j].fldtype ) {
	case undef_fldtype :
	  go_BYE(-1);
	  break;
	case I1 : 
	  Y = X[j] + sizeof(char)*i;
	  valI1 = *(char *)Y;
	  if ( nn_X[j] == NULL ) { 
	    fprintf(ofp, "%d", valI1);
	  }
	  else {
	    fprintf(ofp, "\"%d\"", valI1);
	  }
	  break;
	case I2 : 
	  Y = X[j] + sizeof(short)*i;
	  valI2 = *((short *)Y);
	  if ( nn_X[j] == NULL ) { 
	    fprintf(ofp, "%d", valI2);
	  }
	  else {
	    fprintf(ofp, "\"%d\"", valI2);
	  }
	  break;
	case I4 : 
	  Y = X[j] + sizeof(int)*i;
	  valI4 = *((int *)Y);
	  int dict_fld_id = dict_fld_ids[j];
	  if ( dict_fld_id >= 0 ) {
	    status = from_int_to_str(valI4, g_flds[dict_fld_id].len, Ztxt[j],
		(short*)(Zlen[j]), (long long *)(Zoff[j]), buffer, MAX_LEN_STR); 
	    cBYE(status);
	    status = pr_escaped(buffer, ofp); cBYE(status);
	  }
	  else {
	    if ( nn_X[j] == NULL ) { 
	      fprintf(ofp, "%d", valI4);
	    }
	    else {
	      fprintf(ofp, "\"%d\"", valI4);
	    }
	  }
	  break;
	case I8 : 
	  Y = X[j] + sizeof(long long)*i;
	  valI8 = *((long long *)Y);
	  if ( nn_X[j] == NULL ) { 
	    fprintf(ofp, "%lld", valI8);
	  }
	  else {
	    fprintf(ofp, "\"%lld\"", valI8);
	  }
	  break;
	case F4 : 
	  Y = X[j] + sizeof(float)*i;
	  valF4 = *((float *)Y);
	  if ( nn_X[j] == NULL ) { 
	    fprintf(ofp, "%f", valF4);
	  }
	  else {
	    fprintf(ofp, "\"%f\"", valF4);
	  }
	  break;
	case F8 : 
	  Y = X[j] + sizeof(double)*i;
	  valF8 = *((double *)Y);
	  if ( nn_X[j] == NULL ) { 
	    fprintf(ofp, "%lf", valF8);
	  }
	  else {
	    fprintf(ofp, "\"%lf\"", valF8);
	  }
	  break;
	case SC : 
	  { 
	    int fldlen = fld_rec[j].len;
	    Y = X[j] + i*(fldlen+1);
	    if ( nn_X[j] != NULL ) { go_BYE(-1); }
	    fprintf(ofp, "\"");
	    for ( int i = 0;  ( ( *Y != '\0' ) && ( i < fldlen) ) ; Y++ ) {
	      if ( ( *Y == '\\' ) || ( *Y == '"' ) ) {
		fprintf(ofp, "\\");
	      }
	      fprintf(ofp, "%c", *Y);
	    }
	    fprintf(ofp, "\"");
	  }
	  break;

	case SV : 
	  {
	    short     *lenptr = (short *)Zlen[j]; 
	    short     len = lenptr[i];
	    long long *offptr = (long long *)Zoff[j]; 
	    long long offset = offptr[i];
	    char      *cptr = Ztxt[j]; cptr += offset;
	    fprintf(ofp, "\"");
	    for ( int i = 0; i < len; i++ ) { 
	      if ( ( *cptr == '\\' ) ||  ( *cptr == '"' ) ) { 
		fprintf(ofp, "\\");
	      }
	      fprintf(ofp, "%c", *cptr);
	      cptr++;
	    }
	    fprintf(ofp, "\"");
	  }
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
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
  /* TODO P3: Valgrind claims I am leaking memory on flds. */
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

// START FUNC DECL
int
core_pr_fld(
	    int tbl_id,
	    int fld_id,
	    FLD_REC_TYPE fld_rec,
	    FLD_REC_TYPE nn_fld_rec,
	    char *cfld_X,
	    long long lb,
	    long long ub,
	    long long nR,
	    FILE *ofp
	    )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *Ztxt = NULL; size_t nZtxt = 0; 
  char *Zlen = NULL; size_t nZlen = 0; 
  char *Zoff = NULL; size_t nZoff = 0; 
  int len, dict_fld_id;
     
  //--------------------------------------------------------
  status = get_data(fld_rec, &X, &nX, false); cBYE(status);
  if ( fld_rec.nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, false); cBYE(status);
  }
  len    = fld_rec.len;
  if ( ( len > 0 ) && ( fld_rec.fldtype != SC ) )  { go_BYE(-1); }
  //--------------------------------------------------------
  status = get_fld_info(fk_fld_txt_lkp, fld_id, &dict_fld_id); cBYE(status); 
  if ( dict_fld_id >= 0 ) {
    FLD_REC_TYPE dict_fld_rec; 
    dict_fld_rec = g_flds[dict_fld_id];
    int dict_tbl_id = dict_fld_rec.tbl_id;
    long long dict_nR = g_tbls[dict_tbl_id].nR;
    status = setup_str(dict_fld_id, &Ztxt, &nZtxt, 
		       &Zlen, &nZlen, &Zoff, &nZoff); 
    cBYE(status);
    short     *lenI2ptr = (short *)Zlen;
    long long *offI8ptr = (long long *)Zoff;
    int       *valI4ptr = (int   *)X;
    short     *valI2ptr = (short *)X;
    char      *valI1ptr = (char  *)X;
    for ( long long i = lb; i < ub; i++ ) {
      if ( ( cfld_X != NULL ) && ( cfld_X[i] == FALSE ) ) { continue; }
      if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) { 
	fprintf(ofp, "\"\"\n"); continue;
      }
      int inval;
      switch ( fld_rec.fldtype ) { 
      case I1 : inval = valI1ptr[i]; break;
      case I2 : inval = valI2ptr[i]; break;
      case I4 : inval = valI4ptr[i]; break;
      default : go_BYE(-1); break;
      }
      if ( ( inval < 0 ) || ( inval >= dict_nR ) ) { go_BYE(-1); }
      short     lenI2 = lenI2ptr[inval];
      long long offI8 = offI8ptr[inval];
      char *cptr = Ztxt + offI8;
      /* We need to print lenI2 bytes starting from Ztxt + offI8 */
      fprintf(ofp, "\""); /* start quote */
      for ( int i = 0; i < lenI2; i++ ) {
	if ( *cptr == '\0' ) { go_BYE(-1); }
	if ( ( *cptr == '\\' ) || ( *cptr == '"' ) ) {
	  fprintf(ofp, "\\");
	}
	fprintf(ofp, "%c", *cptr++);
      }
      fprintf(ofp, "\""); /* end   quote */
      fprintf(ofp, "\n"); /* print end of line */
    }
  }
  else {
    switch ( fld_rec.fldtype )  {
    case I1 : 
      pr_fld_I1((char *)X,      lb, ub, nn_X, cfld_X, ofp);
      break;
    case I2 : 
      pr_fld_I2((short *)X,     lb, ub, nn_X, cfld_X, ofp);
      break;
    case I4 : 
      pr_fld_I4((int *)X,       lb,  ub, nn_X, cfld_X, ofp);
      break;
    case I8 : 
      pr_fld_I8((long long *)X, lb, ub, nn_X, cfld_X, ofp);
      break;
    case F4 : 
      pr_fld_F4((float *)X,     lb, ub, nn_X, cfld_X, ofp);
      break;
    case F8 : 
      pr_fld_F8((double *)X,    lb, ub, nn_X, cfld_X, ofp);
      break;
    case SC : 
      status = pr_fld_SC((char *)X, len, lb, ub, cfld_X, ofp);
      cBYE(status);
      break;
    case SV : 
      status = setup_str(fld_id, &Ztxt, &nZtxt, 
			 &Zlen, &nZlen, &Zoff, &nZoff); 
      cBYE(status);
      status = pr_fld_SV((char *)X, (short *)Zlen, (long long*)Zoff, 
	  lb, ub, nn_X, cfld_X, ofp);
      cBYE(status);
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(Ztxt, nZtxt);
  rs_munmap(Zlen, nZlen);
  rs_munmap(Zoff, nZoff);
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

  int fld_id;  FLD_REC_TYPE fld_rec;
  int nn_fld_id;  FLD_REC_TYPE nn_fld_rec;
  int cfld_id;  FLD_REC_TYPE cfld_rec;
  int nn_cfld_id;  FLD_REC_TYPE nn_cfld_rec;
  int tbl_id; TBL_REC_TYPE tbl_rec; 
  char *cfld_X = NULL; size_t cfld_nX = 0;

  long long nR, lb = 0, ub = 0;
  bool is_lb_null = true, is_ub_null = true, is_cfld_null = true;
  //----------------------------------------------------------------
  zero_string(cfld, MAX_LEN_FLD_NAME+1);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ofp == NULL ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  if ( ( filter != NULL ) && ( *filter != '\0' ) ) {
    status = chk_aux_info(filter); cBYE(status);
    status = extract_I8(filter, "lb=[", "]", &lb, &is_lb_null); cBYE(status);
    status = extract_I8(filter, "ub=[", "]", &ub, &is_ub_null); cBYE(status);
    status = extract_S(filter, "cond=[", "]", cfld, MAX_LEN_FLD_NAME, 
		       &is_cfld_null);
    if ( ( is_lb_null) && ( is_ub_null ) && ( is_cfld_null ) ) { go_BYE(-1); }
  }
  // lb and ub must both be set or both be null 
  if ( ( ( is_lb_null ) && ( !is_ub_null ) ) ||
       ( ( is_lb_null ) && ( !is_ub_null ) ) ) {
    go_BYE(-1);
  }
  if ( ( is_lb_null ) || ( is_ub_null ) )  {
    lb = 0;
    ub = nR;
  }
  else {
    if ( lb < 0   ) { go_BYE(-1); }
    if ( ub >= nR ) { ub = nR; }
    if ( lb >= ub ) { go_BYE(-1); }
  }
  if ( is_cfld_null == false ) { 
    status = is_fld(NULL, tbl_id, cfld, &cfld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec);
    if ( cfld_id < 0 ) { 
      fprintf(stderr, "Field [%s] not found \n", cfld); go_BYE(-1);
    }
    if ( cfld_rec.fldtype != I1 ) { go_BYE(-1); }
    status = get_data(cfld_rec, &cfld_X, &cfld_nX, false); cBYE(status);
  }
  int n_flds = 1;
  for ( const char *cptr = fld; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) { n_flds++; }
  }
  if ( n_flds > MAX_NUM_FLDS_TO_PR ) { go_BYE(-1); }

  if ( n_flds == 1 ) {
    status = is_fld(NULL, tbl_id, fld, 
		    &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
    cBYE(status);
    if ( fld_id < 0 ) { go_BYE(-1); }
    status = core_pr_fld(tbl_id, fld_id, fld_rec, nn_fld_rec, 
			 cfld_X, lb, ub, nR, ofp); cBYE(status);
  }
  else {
    status = core_pr_flds(tbl_id, fld, 
			  cfld_X, lb, ub, nR, ofp); cBYE(status);
  }
  cBYE(status);
 BYE:
  rs_munmap(cfld_X, cfld_nX);
  return(status);
}
