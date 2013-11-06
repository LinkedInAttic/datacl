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
#include "fld_meta.h"
#include "extract_I8.h"
#include "pr_fld_I1.h"
#include "pr_fld_I2.h"
#include "pr_fld_I4.h"
#include "pr_fld_I8.h"
#include "pr_fld_F4.h"
#include "pr_fld_F8.h"
#include "pr_fld_clob.h"
#include "pr_fld_var_clob.h"

extern char *g_data_dir;
extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];

#define MAX_NUM_FLDS_TO_PR 16

int from_int_to_str(
		    int ival, 
		    bool is_null,
		    bool is_fixed_len,
		    int fixed_len,
		    long long n_dict,
		    char *txtptr,
		    int *lenptr,
		    int *offptr,
		    char *buffer, 
		    int buflen
		    )
{
  int status = 0;
  zero_string_to_nullc(buffer);
  int this_len;

  if ( is_null == true ) { 
    if ( is_fixed_len == true ) {
      // TODO replace by assign_I1
      for ( int i = 0; i < fixed_len; i++ ) { buffer[i] = '\0'; }
    }
    else {
      buffer[0] = '\0';
    }
  }
  else {
    if ( is_fixed_len == true ) { 
      this_len = fixed_len;
    }
    else {
      this_len = lenptr[ival];
    }
    if ( this_len >= buflen ) { 
      fprintf(stderr, "this_len = %d \n", this_len);
      fprintf(stderr, "buflen   = %d \n", buflen);
      go_BYE(-1); }
    if ( is_fixed_len == true ) { 
      if ( n_dict <= 0 ) { go_BYE(-1); }
      if ( ( ival < 0 ) || ( ival >= n_dict ) ) { go_BYE(-1); }
      if ( fixed_len < 2 ) { go_BYE(-1); }
      long long offset = (long long)ival * (long long)fixed_len;
      memcpy(buffer, txtptr + offset, this_len);
    }
    else {
      memcpy(buffer, txtptr + offptr[ival], this_len);
    }
    buffer[this_len] = '\0';
  }
 BYE:
  return(status);
}

int
foo(
    int dict_tbl_id,
    char *fld,
    char **ptr_X,
    size_t *ptr_nX,
    bool *ptr_is_fixed_len,
    int *ptr_fixed_len,
    long long *ptr_n_dict
    )
{
  int status = 0;
  int fld_id, nn_fld_id;
  FLD_REC_TYPE fld_rec, nn_fld_rec;

  chk_range(dict_tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, dict_tbl_id, fld, &fld_id, &fld_rec, 
		  &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  *ptr_n_dict = g_tbls[dict_tbl_id].nR;
  if ( *ptr_n_dict < 1 ) { go_BYE(-1); }
  if ( fld_id    <  0 ) { go_BYE(-1); }
  if ( nn_fld_id >= 0 ) { go_BYE(-1); }
  status = get_data(fld_rec, ptr_X, ptr_nX, false); cBYE(status);
  if ( ( fld_rec.fldtype == clob ) && ( fld_rec.cnt > 0 ) ) { 
    // TODO P1 change cnt to len
    *ptr_is_fixed_len = true;
    *ptr_fixed_len = fld_rec.cnt;
  }
  else {
    *ptr_is_fixed_len = false;
    *ptr_fixed_len = INT_MIN;
  }
 BYE:
  return(status);
}

int setup_int_to_string(
			int dict_tbl_id, 
			bool *ptr_is_fixed_len,
			int *ptr_fixed_len,
			long long *ptr_n_dict,
			char **ptr_Ztxt,
			size_t *ptr_nZtxt,
			char **ptr_Zlen,
			size_t *ptr_nZlen,
			char **ptr_Zoff,
			size_t *ptr_nZoff
			)
{
  int status = 0;
  if ( dict_tbl_id < 0 ) { go_BYE(-1); }
  *ptr_Ztxt = NULL; *ptr_nZtxt = 0; 
  *ptr_Zlen = NULL; *ptr_nZlen = 0; 
  *ptr_Zoff = NULL; *ptr_nZoff = 0; 
  bool btemp; int itemp; long long ltemp;
  //---------------------------------------------------
  status = foo(dict_tbl_id, "txt", ptr_Ztxt, ptr_nZtxt, 
	       ptr_is_fixed_len, ptr_fixed_len, ptr_n_dict);
  cBYE(status);
  if ( *ptr_is_fixed_len == true ) { 
    // nothing more to do 
  }
  else {
    status = foo(dict_tbl_id, "len", ptr_Zlen, ptr_nZlen, &btemp, &itemp, 
		 &ltemp); 
    cBYE(status);
    status = foo(dict_tbl_id, "off", ptr_Zoff, ptr_nZoff, &btemp, &itemp, 
		 &ltemp);
    cBYE(status);
  }
  //---------------------------------------------------
 BYE:
  return(status);
}


int pr_escaped(
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
#define BUFLEN 8192
  char buffer[BUFLEN];

  FLD_REC_TYPE fld_rec[MAX_NUM_FLDS_TO_PR]; 
  int fld_id[MAX_NUM_FLDS_TO_PR];

  FLD_REC_TYPE nn_fld_rec[MAX_NUM_FLDS_TO_PR]; 
  int nn_fld_id[MAX_NUM_FLDS_TO_PR];

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

  bool is_fixed_len[MAX_NUM_FLDS_TO_PR];
  long long n_dict[MAX_NUM_FLDS_TO_PR];
  int fixed_len[MAX_NUM_FLDS_TO_PR];
  char **flds = NULL; int n_flds = 0;

  char *text_Y[MAX_NUM_FLDS_TO_PR]; // to print text fields 
  if ( ofp == NULL ) { go_BYE(-1); }

  zero_string(buffer, BUFLEN);
  for ( int i = 0; i < MAX_NUM_FLDS_TO_PR; i++ ) { is_fixed_len[i] = false; }
  for ( int i = 0; i < MAX_NUM_FLDS_TO_PR; i++ ) { fixed_len[i] = INT_MIN; }
  for ( int i = 0; i < MAX_NUM_FLDS_TO_PR; i++ ) { n_dict[i] = INT_MIN; }
  /*---------------------------------------------------------*/
  for ( int i = 0; i < MAX_NUM_FLDS_TO_PR; i++  ) { 
    X[i] = NULL; nX[i] = 0;
    nn_X[i] = NULL; nn_nX[i] = 0;
  }
  status = explode(str_flds, ':', &flds, &n_flds);
  cBYE(status);
  if ( n_flds == 0 ) { go_BYE(-1); }
  if ( n_flds > MAX_NUM_FLDS_TO_PR ) { go_BYE(-1); }
  
  for ( int j = 0; j < n_flds; j++ ) {
    //--------------------------------------------------------
    status = is_fld(NULL, tbl_id, flds[j], 
		    &(fld_id[j]), &(fld_rec[j]), &(nn_fld_id[j]), &(nn_fld_rec[j]));
    if ( fld_id[j] < 0 ) { 
      fprintf(stderr, "Field [%s] not found \n", flds[j]); go_BYE(-1);
    }
    status = get_data(fld_rec[j], &(X[j]), &(nX[j]), 0); cBYE(status);
    if ( nn_fld_id[j] >= 0 ) { 
      status = get_data(nn_fld_rec[j], &(nn_X[j]), &(nn_nX[j]), 0); cBYE(status);
    }
    if ( fld_rec[j].dict_tbl_id >= 0 ) {
      status = setup_int_to_string(fld_rec[j].dict_tbl_id, 
				   &(is_fixed_len[j]),
				   &(fixed_len[j]),
				   &(n_dict[j]),
				   &(Ztxt[j]), &(nZtxt[j]), 
				   &(Zlen[j]), &(nZlen[j]), 
				   &(Zoff[j]), &(nZoff[j]));
    }
  }
  /* For text fields (with variable length), we need to offset the
     pointer based on the range being printed  */
  for ( int j = 0; j < n_flds; j++ ) { 
    if ( fld_rec[j].fldtype == clob ) { 
      char *cptr;
      if ( fld_rec[j].cnt <= 0 ) { // TODO P0 cnt is being used for len
	cptr = X[j];
	for ( long long i = 0; i < lb; ) {
	  if ( *cptr == '\0' ) {
	    i++;
	  }
	}
      }
      else {
	cptr = X[j] + (lb * fld_rec[j].cnt);
      }
      text_Y[j] = cptr;
    }
  }
  /*----------------------------------------------------------*/
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      /* For text fields (with variable length), we need to offset the
	 pointer to skip over this row. Can be done more efficiently using he len field or the offset field that exist in the lkp table. TODO P2 */
      for ( int j = 0; j < n_flds; j++ ) { 
	if ( fld_rec[j].fldtype == clob ) { 
	  char *cptr;
          if ( fld_rec[j].cnt <= 0 ) { // TODO P0 cnt is being used for len
	    cptr = text_Y[j]; 
	    for ( ; *cptr != '\0' ; ) {
	      cptr++;
	    }
	    cptr++; // skip over nullc
	    text_Y[j] = cptr;
	  }
	}
      }
      continue;
    }
    for ( int j = 0; j < n_flds; j++ ) {
      if ( ( fld_rec[j].fldtype == clob )  &&  ( fld_rec[j].cnt > 0 ) ) {
        text_Y[j] = X[j] + (i * fld_rec[j].cnt);
      }
      if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	fprintf(ofp, "\"\"");
      }
      else {
	char *Y;
	char ival1; short ival2; int ival4; 
	long long ival8;
	float fval4; double fval8;
	switch ( fld_rec[j].fldtype ) {
	case xunknown :
	  go_BYE(-1);
	  break;
	case I1 : 
	  Y = X[j] + 1*i;
	  ival1 = *(char *)Y;
          if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	    fprintf(ofp, "%d", ival1);
	  }
	  else {
	    fprintf(ofp, "\"%d\"", ival1);
	  }
	  break;
	case I2 : 
	  Y = X[j] + 2*i;
	  ival2 = *((short *)Y);
          if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	    fprintf(ofp, "%d", ival2);
	  }
	  else {
	    fprintf(ofp, "\"%d\"", ival2);
	  }
	  break;
	case I4 : 
	  if ( fld_rec[j].dict_tbl_id >= 0 ) {
	    Y = X[j] + 4*i;
	    ival4 = *((int *)Y);
	    bool is_null;
            if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	      is_null = true;
	    }
	    else {
	      is_null = false;
	    }
	    status = from_int_to_str(ival4, is_null, is_fixed_len[j], 
				     fixed_len[j], n_dict[j], 
				     Ztxt[j], (int*)(Zlen[j]), (int *)(Zoff[j]), buffer, BUFLEN); 
	    cBYE(status);
	    status = pr_escaped(buffer, ofp); cBYE(status);
	  }
	  else {
	    Y = X[j] + 4*i;
	    ival4 = *((int *)Y);
            if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	      fprintf(ofp, "%d", ival4);
	    }
	    else {
	      fprintf(ofp, "\"%d\"", ival4);
	    }
	  }
	  break;
	case I8 : 
	  Y = X[j] + 8*i;
	  ival8 = *((long long *)Y);
          if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	    fprintf(ofp, "%lld", ival8);
	  }
	  else {
	    fprintf(ofp, "\"%lld\"", ival8);
	  }
	  break;
	case F4 : 
	  Y = X[j] + 4*i;
	  fval4 = *((float *)Y);
          if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	    fprintf(ofp, "%f", fval4);
	  }
	  else {
	    fprintf(ofp, "\"%f\"", fval4);
	  }
	  break;
	case F8 : 
	  Y = X[j] + 8*i;
	  fval8 = *((double *)Y);
          if ( ( nn_X[j] != NULL ) && ( nn_X[j][i] == 0 ) ) {
	    fprintf(ofp, "%lf", fval8);
	  }
	  else {
	    fprintf(ofp, "\"%lf\"", fval8);
	  }
	  break;

	case clob : 
	  {
	    char *cptr;
	    fprintf(ofp, "\"");
	    for ( cptr = text_Y[j]; *cptr != '\0'; cptr++ ) {
	      fprintf(ofp, "%c", *cptr);
	    }
	    fprintf(ofp, "\"");

            if ( fld_rec[j].cnt <= 0 ) { // TODO P0 cnt is being used for len
	      cptr++; // skip over null character
	      text_Y[j] = cptr;
	    }
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
	    const FLD_REC_TYPE fld_rec,
	    const FLD_REC_TYPE nn_fld_rec,
	    int nn_fld_id,
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
  char *buffer = NULL; int buflen = 8192;
  char *Ztxt = NULL; size_t nZtxt = 0;
  char *Zlen = NULL; size_t nZlen = 0;
  char *Zoff = NULL; size_t nZoff = 0;
  char *T = NULL; size_t nT = 0;
  int *iptr = NULL;
  int len;
  bool is_fixed_len = false; int fixed_len = INT_MIN; long long n_dict;
     
  buffer = malloc(buflen * sizeof(char));
  return_if_malloc_failed(buffer);
  zero_string(buffer, buflen);

  //--------------------------------------------------------
  status = get_data(fld_rec, &X, &nX, false); cBYE(status);
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, false); cBYE(status);
  }
  //--------------------------------------------------------
  char *I1ptr      = NULL; 
  short *I2ptr     = NULL;
  int *I4ptr       = NULL;
  long long *I8ptr = NULL;
  switch ( fld_rec.fldtype )  {
  case I1 : 
    I1ptr = (char *)X;
    pr_fld_I1(I1ptr, lb, ub, nn_X, cfld_X, ofp);
    break;
  case I2 : 
    I2ptr = (short *)X;
    pr_fld_I2(I2ptr, lb, ub, nn_X, cfld_X, ofp);
    break;
  case I4 : 
    I4ptr = (int *)X;
    if ( fld_rec.dict_tbl_id >= 0 ) {
      int txt_fld_id, nn_fld_id;
      FLD_REC_TYPE txt_fld_rec, nn_fld_rec;
      status = is_fld(NULL, fld_rec.dict_tbl_id, "txt", &txt_fld_id, 
		      &txt_fld_rec, &nn_fld_id, &nn_fld_rec); 
      cBYE(status);
      // Get number of rows in dict table 
      long long ndict = g_tbls[fld_rec.dict_tbl_id].nR; 
      if ( ndict < 0 ) { go_BYE(-1); }
      if ( txt_fld_id < 0 ) { go_BYE(-1); }
      len = txt_fld_rec.cnt; // TODO: P1: Replace with len 
      if ( len > 0 ) { /* fixed width */
        status = get_data(txt_fld_rec, &T, &nT, false); cBYE(status);
	if ( nT == 0 ) { go_BYE(-1); }
	if ( ( len * ndict ) != nT ) { go_BYE(-1); }
	long long dbg = 0;
	for ( long long i = lb; i < ub; i++ ) {
	  dbg++;
	  if ( ( cfld_X != NULL ) && ( cfld_X[i] == FALSE ) ) { continue; }
	  if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) { 
	    fprintf(ofp, "\"\"\n");
	    continue;
	  }
	  int I4val = I4ptr[i];
	  long long offset = (long long)I4val * (long long)len;
	  unsigned long long l1 = (long long)T; l1 += nT;
	  unsigned long long l2 = (long long)T; l2 += offset;
	  if ( ( I4val < 0 ) || ( I4val >= ndict ) ) { go_BYE(-1); }
	  status = pr_escaped(T+offset, ofp); cBYE(status);
	  fprintf(ofp, "\n"); /* print end of line */

	}
	rs_munmap(T, nT);
      }
      else {
	status = setup_int_to_string(fld_rec.dict_tbl_id, 
				     &is_fixed_len, &fixed_len, &n_dict, &Ztxt, &nZtxt, 
				     &Zlen, &nZlen, &Zoff, &nZoff); 
	cBYE(status);
	iptr   = (int *)X;
	for ( long long i = lb; i < ub; i++ ) {
	  if ( ( cfld_X != NULL ) && ( cfld_X[i] == FALSE ) ) { continue; }
	  if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) { 
	    fprintf(ofp, "\"\"\n");
	    continue;
	  }
	  bool is_null;
	  if ( nn_X == NULL ) {
	    is_null = false;
	  }
	  else {
	    if ( nn_X[i] == 1 ) { 
	      is_null = false;
	    }
	    else if ( nn_X[i] == 0 ) { 
	      is_null = true;
	    }
	    else {
	      go_BYE(-1);
	    }
	  }
	  status = from_int_to_str(iptr[i], is_null, is_fixed_len, fixed_len, 
				   n_dict, Ztxt, (int *)Zlen, (int *)Zoff, buffer, buflen); 
	  cBYE(status);
	  status = pr_escaped(buffer, ofp); cBYE(status);
	  fprintf(ofp, "\n"); /* print end of line */

	}
      }
    }
    else {
      pr_fld_I4(I4ptr, lb, ub, nn_X, cfld_X, ofp);
    }
    break;
  case I8 : 
    I8ptr = (long long *)X;
    pr_fld_I8(I8ptr, lb, ub, nn_X, cfld_X, ofp);
    break;
  case F4 : 
    pr_fld_F4((float *)X, lb, ub, nn_X, cfld_X, ofp);
    break;
  case F8 : 
    pr_fld_F8((double *)X, lb, ub, nn_X, cfld_X, ofp);
    break;
  case clob : 
    if ( fld_rec.cnt < 0 ) { // TODO P0 replace cnt with len 
      pr_fld_var_clob(X, lb, ub, nn_X, cfld_X, ofp);
    }
    else {
      status = pr_fld_clob(X, 32, lb, ub, nn_X, cfld_X, ofp);
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  free_if_non_null(buffer);
  rs_munmap(X, nX);
  rs_munmap(T, nT);
  rs_munmap(Ztxt, nZtxt);
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
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  cBYE(status);
  nR = tbl_rec.nR;
  if ( ( filter != NULL ) && ( *filter != '\0' ) ) {
    // simple check on filter
    if ( ( strncmp(filter, "lb=[", 4) != 0 ) && 
         ( strncmp(filter, "ub=[", 4) != 0 ) && 
         ( strncmp(filter, "cond=[", 6) != 0 ) ) {
      go_BYE(-1);
    }
    // ----
    status = extract_I8(filter, "lb=[", "]", &lb, &is_lb_null); cBYE(status);
    status = extract_I8(filter, "ub=[", "]", &ub, &is_ub_null); cBYE(status);
    status = extract_S(filter, "cond=[", "]", cfld, MAX_LEN_FLD_NAME, 
		       &is_cfld_null);
  }
  if ( ( is_lb_null ) || ( is_ub_null ) )  {
    lb = 0;
    ub = nR;
  }
  else {
    if ( lb < 0   ) { go_BYE(-1); }
    if ( ub >  nR ) { go_BYE(-1); }
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
    if ( fld_id < 0 ) { go_BYE(-1); }
    status = core_pr_fld(fld_rec, nn_fld_rec, nn_fld_id, 
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
