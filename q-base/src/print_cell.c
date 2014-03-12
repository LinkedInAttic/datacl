#include "qtypes.h"
#include "auxil.h"
#include "print_cell.h" 
#include "mk_dict.h" 

// following macro for help debugging errors
#define pr_row_fld() {						\
    fprintf(stderr, "ERROR: Row %lld, field %s \n", nR, fld);	\
    fprintf(stderr, "field = [%s] \n", g_dld_buf);		\
  }

extern char *g_dld_buf;
extern int   g_dld_buflen;
// last review 11/5/2013
// START FUNC DECL
int
print_cell(
	   long long nR, /* current row */
	   char *fld, /* current field */
	   char *X,  
	   long long lb, /* inclusive */ 
	   long long ub, /* exclusive */
	   FLD_TYPE fldtype,
	   FLD_PROPS_TYPE *ptr_fld_prop,
	   FILE *fp,
	   FILE *nn_fp, 
	   FILE *len_fp, 
	   int n_keys,
	   char *key_X,
	   char *txt_X,
	   char *off_X,
	   char *len_X,
	   HT_REC_TYPE *ht,
	   int sz_ht,
	   int *ptr_n_ht
	   )
// STOP FUNC DECL
{
  int status = 0;
  int valI4; 
  char valI1; 
  short valI2; 
  long long valI8; 
  unsigned long long uvalI8; 
  float valF4; 
  double valF8;
  char c_nn;
  int n_ht = *ptr_n_ht;
  char nullc = '\0';

  if ( ptr_fld_prop->is_all_def ) { if ( nn_fp != NULL ) { go_BYE(-1); } }
  if ( fldtype == SV ) { if ( len_fp == NULL ) { go_BYE(-1); } }
  if ( fldtype != SV ) { if ( len_fp != NULL ) { go_BYE(-1); } }
  if ( X == NULL ) { go_BYE(-1); }
  if ( lb > ub ) { go_BYE(-1); }
  if ( fp  == NULL ) { go_BYE(-1); }

  if ( ( ub - lb ) >= g_dld_buflen ) {
    g_dld_buflen = 2*(g_dld_buflen+1) - 1;
    g_dld_buf = (char *)realloc(g_dld_buf, g_dld_buflen+1);
    zero_string(g_dld_buf, g_dld_buflen+1);
  }
  // copy from X into g_dld_buf, while dealing with escape characters
  int cell_len = 0;
  for ( long long i = lb; i < ub; i++ ) {
    /* If you see a bslash, next char must be dquote or bslash. 
     * Skip over the bslash and keep the second character. */
    if ( X[i] == '\\' ) { 
      if ( i == (ub-1) ) { go_BYE(-1); }
      if ( ( X[i+1] != '\\' ) && ( X[i+1] != '"' ) ) { go_BYE(-1); }
      i++;
    }
    /*--------------------------------------------------- */
    g_dld_buf[cell_len++] = X[i];
  }
  g_dld_buf[cell_len] = '\0';  // null terminate
  // Note that cell_len can be zero if field allows nul values 
  if ( ptr_fld_prop->is_lkp ) { 
    if ( cell_len >= MAX_LEN_STR ) { 
      fprintf(stderr, "Row %lld, field length (%d) too high\n", nR, cell_len); 
      go_BYE(-1);
    }

  }
  else {
  switch ( fldtype ) {
  case I1 : case I2 : case I4 : case I8 : case F4 : case F8 : 
    if ( cell_len > 32 ) { 
      fprintf(stderr, "Row %lld, field %s \n", nR, fld);
      fprintf(stderr, "Field length = %lu too long \n", strlen(g_dld_buf));
      fprintf(stderr, "Printing first 64 characters \n\t");
      for ( int i = 0; i < strlen(g_dld_buf); i++ ) { 
	fprintf(stderr, "%c", g_dld_buf[i]);
      }
      fprintf(stderr, "\n");
      go_BYE(-1); 
    }
    for ( int i = 0; i < cell_len; i++ ) { 
      char c = g_dld_buf[i];
      if ( ( ( c < '0' ) && ( c > '9' ) ) && ( c != '.' ) && ( c != '-' ) ) {
	fprintf(stderr, "Row %lld. Cannot convert [%s] to numeric\n",
		nR, g_dld_buf);
	go_BYE(-1); 
      }
    }
    break;
  case SC : case SV : 
    if ( cell_len >= MAX_LEN_STR ) { 
      fprintf(stderr, "Row %lld, field length (%d) too high\n", nR, cell_len); 
      go_BYE(-1);
    }
    break;
  default : 
    fprintf(stderr, "fldtype = %d \n", fldtype);
    go_BYE(-1); 
    break;
  }
  }

  /*--- START: Determine out nn value */
  if ( cell_len == 0 ) { /* null value */
    if ( ptr_fld_prop->is_all_def ) { 
      fprintf(stderr, "Null value not allowed for field %s ", fld);
      fprintf(stderr, "but observed in Row %lld\n", nR);
      go_BYE(-1); 
    }
    c_nn = FALSE;
    ptr_fld_prop->is_any_null = true;
  }
  else if ( ub < lb ) {
    go_BYE(-1);
  }
  else {
    c_nn = TRUE;
  }
  /*--- STOP : Determine out nn value */

  valI1 = valI2 = valI4 = valI8 = 0;
  valF4 = valF8 = 0.0;

  if ( ptr_fld_prop->is_lkp ) {
    if ( c_nn == TRUE ) {
      if ( key_X != NULL ) { /* reading from existing dictionary */
	status = get_from_dict(g_dld_buf, (long long *)key_X, n_keys, txt_X, 
			       (int *)off_X, (int *)len_X, &valI4);
	cBYE(status);
	if ( valI4 < 0 ) {
	  if ( ptr_fld_prop->is_null_if_missing ) {
	    c_nn = FALSE;
	    valI4 = 0; // null values are set to 0
	  }
	  else {
	    pr_row_fld(); 
	    fprintf(stderr, "Could not find %s in dictionary \n", g_dld_buf);
	    go_BYE(-1);
	  }
	}
	else {
	  c_nn = TRUE;
	}
      }
      else {
	status = add_to_dict(g_dld_buf, ht, sz_ht, &n_ht, &uvalI8); cBYE(status);
	c_nn = TRUE;
      }
    }
    else {
      uvalI8 = 0;
    }
    if ( key_X != NULL ) { /* reading from existing dictionary */
      fwrite(&valI4, sizeof(int), 1, fp);
    }
    else {
      fwrite(&uvalI8, sizeof(unsigned long long), 1, fp);
    }
  }
  else {
    switch ( fldtype ) {
    case I1 : 
    case I2 : 
    case I4 : 
    case I8 : 
      if ( c_nn == TRUE ) { 
	status = stoI8(g_dld_buf, &valI8);  
	if ( status < 0 ) { pr_row_fld(); go_BYE(-1); }
      }
      else {
	valI8 = 0;
	ptr_fld_prop->is_any_null = true;
      }
      switch ( fldtype ) { 
      case I1 : 
	if ( valI8 > SCHAR_MAX ) { pr_row_fld(); go_BYE(-1); }
	if ( valI8 < SCHAR_MIN ) { pr_row_fld(); go_BYE(-1); }
	valI1 = (char)valI8;
	fwrite(&valI1, sizeof(char), 1, fp);
	break;
      case I2 : 
	if ( valI8 > SHRT_MAX ) { pr_row_fld(); go_BYE(-1); }
	if ( valI8 < SHRT_MIN ) { pr_row_fld(); go_BYE(-1); }
	valI2 = (short)valI8;
	fwrite(&valI2, sizeof(short), 1, fp);
	break;
      case I4 : 
	if ( valI8 > INT_MAX ) { pr_row_fld(); go_BYE(-1); }
	if ( valI8 < INT_MIN ) { pr_row_fld(); go_BYE(-1); }
	valI4 = (int)valI8;
	fwrite(&valI4, sizeof(int), 1, fp);
	break;
      case I8 : 
	fwrite(&valI8, sizeof(long long), 1, fp);
	break;
      default :
	go_BYE(-1);
	break;
      }
      break;
    case F4 : 
      if ( c_nn == TRUE ) { 
	status = stoF4(g_dld_buf, &valF4); 
	if ( status < 0 ) { break; }
      }
      else {
	valF4 = 0;
      }
      fwrite(&valF4, sizeof(float), 1, fp);
      break;
    case F8 : 
      if ( c_nn == TRUE ) { 
	status = stoF8(g_dld_buf, &valF8); 
	if ( status < 0 ) { break; }
      }
      else {
	valF8 = 0;
      }
      fwrite(&valF8, sizeof(double), 1, fp);
      break;
    case SC : 
      if ( ptr_fld_prop->is_trunc ) { 
        cell_len = ptr_fld_prop->maxlen;
      }
      if ( cell_len > ptr_fld_prop->maxlen ) {
	fprintf(stderr, "Error for fld %s on row %lld \n", fld, nR);
	fprintf(stderr, "max    length of string is %d \n", 
	    ptr_fld_prop->maxlen);
	fprintf(stderr, "actual length of string is %d \n", cell_len);
	go_BYE(-1);
      }
      fwrite(g_dld_buf, cell_len, 1, fp);
      int padding = ptr_fld_prop->maxlen - cell_len + 1;
      for ( int i = 0; i < padding; i++ ) { 
        fwrite(&nullc,               1, 1, fp);
      }
      if ( *g_dld_buf == '\0' ) { c_nn = FALSE; } else { c_nn = TRUE; } 
      break;
    case SV : 
      if ( ptr_fld_prop->maxlen > 0 ) {
      if ( cell_len > ptr_fld_prop->maxlen ) {
	fprintf(stderr, "Error for fld %s on row %lld \n", fld, nR);
	fprintf(stderr, "max    length of string is %d \n", 
	    ptr_fld_prop->maxlen);
	fprintf(stderr, "actual length of string is %d \n", cell_len);
	go_BYE(-1);
      }
      }
      if ( len_fp == NULL ) { go_BYE(-1); }
      if ( cell_len > 32767 ) { go_BYE(-1); }
      fwrite(&cell_len, sizeof(short), 1, len_fp);
      if ( cell_len > 0 ) { 
        fwrite(g_dld_buf, cell_len, 1, fp);
      }
      fwrite(&nullc,               1, 1, fp);
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  if ( status < 0 ) { pr_row_fld(); go_BYE(-1); }
  // fprintf(stderr, "[%s]:%d\n", g_dld_buf, c_nn);
  if ( ptr_fld_prop->is_all_def == false ) { 
    fwrite(&c_nn, sizeof(char), 1, nn_fp);
  }
  zero_string_to_nullc(g_dld_buf);
  *ptr_n_ht = n_ht;
 BYE:
  return(status);
}
