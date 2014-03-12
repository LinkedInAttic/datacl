#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "aux_meta.h"
#include "f1s1opf2.h"
#include "f1opf2.h"
#include "mddriver.h"
#include "is_nn_fld.h"
#include "ext_f_to_s.h"
#include "f1f2opf3.h"
#include "del_fld.h"
#include "set_meta.h"
#include "rename_fld.h"

//--- GLOBALS 
#define MAX_TOKEN_LENGTH 1024
#define MAX_LEN_OP  32
#define MAX_LEN_VAL 32
#define BUFSIZE 32
int g_tbl_id;
char g_tbl[MAX_LEN_TBL_NAME+1];
int g_level_num;
long long g_nR;
/*------------------------------------------------------------------*/

void
dbg_msg_2(
	  int g_level_num, 
	  char *query_spec

	  )
{
  for ( int i = 0; i < g_level_num; i++ ) { fprintf(stderr, "\t"); }
  fprintf(stderr, "%s\n", query_spec);
}
void
dbg_msg_1(
	  int level,
	  int fld_id,
	  char *op,
	  char *val
	  )
{
  for ( int j = 0; j < g_level_num; j++ ) { fprintf(stderr, "\t"); }
  fprintf(stderr, "%d %s %s \n", fld_id,  op, val );
}

int core_leaf_work(
		   char *cfld, 
		   char *bfld, 
		   long long *ptr_n_match, 
		   int *ptr_fld_id
		   )
{
  int status = 0;
  char rslt_buf[BUFSIZE];
  char rslt_buf2[BUFSIZE];
  FLD_REC_TYPE  bfld_rec, nn_bfld_rec;
  int           bfld_id,  nn_bfld_id;

  zero_string(rslt_buf, BUFSIZE);
  zero_string(rslt_buf2, BUFSIZE);

  status = f1opf2(g_tbl, cfld, "op=[conv]:newtype=[B]", bfld);
  cBYE(status);
  status = del_fld(NULL, g_tbl_id, cfld, -1, true);
  cBYE(status);
  status = ext_f_to_s(g_tbl, bfld, "sum", rslt_buf,  BUFSIZE);
  cBYE(status);
  status = is_fld(NULL, g_tbl_id, bfld, &bfld_id, &bfld_rec, 
		  &nn_bfld_id, &nn_bfld_rec);
  *ptr_fld_id = bfld_id; // return this 
  zero_string(rslt_buf2, BUFSIZE);
  status = read_nth_val(rslt_buf, ':', 0, rslt_buf2, BUFSIZE); 
  cBYE(status);
  char *endptr;
  *ptr_n_match = strtoll(rslt_buf2, &endptr, 10); 
  if ( *endptr != '\0' ) { go_BYE(-1); }
  status = int_set_meta(g_tbl_id, bfld_id, "cnt", rslt_buf2);
  cBYE(status);
 BYE:
  return(status);
}
int
mk_fld_names(
	     const char *query_spec, 
	     char *cfld, 
	     char *bfld
	     )
{
  int status = 0;
  char md5buf[(LEN_MD5_DIGEST*2)+8];
  if ( query_spec == NULL ) { go_BYE(-1); }
  if ( cfld == NULL ) { go_BYE(-1); }
  if ( bfld == NULL ) { go_BYE(-1); }

  zero_string(cfld, MAX_LEN_FLD_NAME+1);
  zero_string(bfld, MAX_LEN_FLD_NAME+1);
  zero_string(md5buf,(LEN_MD5_DIGEST*2)+8);

  my_MDString(query_spec, md5buf);

  strcpy(cfld, "c_");
  strncat(cfld, md5buf, MAX_LEN_FLD_NAME-2);
  strcpy(bfld, "b_");
  strncat(bfld, md5buf, MAX_LEN_FLD_NAME-2);
 BYE:
  return(status);
}
void
print_balance(
	      char *str
	      )
{

  fprintf(stderr, ">>> ");
  for ( int i = 0; ( ( i < 80 ) && ( *str != '\0' ) ) ; i++ ) {
    if ( isspace(str[i]) ) { 
      if ( isspace(str[i-1]) ) { 
	// skip it 
      }
      else {
	fprintf(stderr, " "); 
      }
    }
    else { 
      fprintf(stderr, "%c", str[i]);
    }
  }
  fprintf(stderr, "\n");
}
int
ro_read_token(
	      char *str,
	      char *token,
	      int token_length
	      )
{
  int status = 0;
  char *cptr = NULL;

  /* consume preceding white space if any */
  for ( cptr = str; *cptr != '\0'; cptr++ ) { 
    if ( !isspace(*cptr) ) { break; }
  }
  for ( int i = 0; *cptr != '\0'; cptr++, i++ ) { 
    if ( isspace(*cptr) ) { break; }
    if ( i >= token_length ) { go_BYE(-1); }
    token[i] = *cptr;
  }
 BYE:
  return(status);
}

int
read_token(
	   char **ptr_str,
	   char *token,
	   int token_length
	   )
{
  int status = 0;
  char *cptr = NULL;
  char *str = *ptr_str;

  /* consume preceding white space if any */
  for ( cptr = str; *cptr != '\0'; cptr++ ) { 
    if ( !isspace(*cptr) ) { break; }
  }
  for ( int i = 0; *cptr != '\0'; cptr++, i++ ) { 
    if ( isspace(*cptr) ) { break; }
    if ( i >= token_length ) { go_BYE(-1); }
    token[i] = *cptr;
  }
  *ptr_str = cptr + 1; // TODO: Do we need the +1 ?
  /*
    for ( int i = 0; ( ( i < 50 ) && ( *cptr != '\0' ) ) ; i++, cptr++ ) {
    fprintf(stderr, "%c", *cptr);
    }
    fprintf(stderr, "\n");
  */
 BYE:
  return(status);
}

int
simple_query_parse(
		   char **ptr_str,
		   long long *ptr_n_match, /* num matched */
		   int *ptr_fld_id, /* B field created. n == 0 => fld_id < 0 */
		   bool *ptr_is_leaf
		   )
{
  int status = 0;
  FLD_REC_TYPE  bfld_rec, nn_bfld_rec, fld_rec, nn_fld_rec;
  int           bfld_id,  nn_bfld_id,  fld_id,  nn_fld_id;
  char op[MAX_LEN_OP];
  char val[MAX_LEN_VAL];
  char val1[MAX_LEN_VAL];
  char val2[MAX_LEN_VAL];
  char nn_fld[MAX_LEN_FLD_NAME+1];
  char  fld[MAX_LEN_FLD_NAME+1];
  char cfld[MAX_LEN_FLD_NAME+1];
  char bfld[MAX_LEN_FLD_NAME+1];
  char str_fldtype[BUFSIZE];
  char rslt_buf[BUFSIZE];
  char rslt_buf2[BUFSIZE];
  char query_spec[MAX_TOKEN_LENGTH];
  char token[MAX_TOKEN_LENGTH];

  zero_string(nn_fld, MAX_LEN_FLD_NAME+1);
  zero_string(fld,  MAX_LEN_FLD_NAME+1);
  zero_string(op, MAX_LEN_OP);
  zero_string(val1, MAX_LEN_VAL);
  zero_string(val2, MAX_LEN_VAL);
  zero_string(val, MAX_LEN_VAL);
  zero_string(token, MAX_TOKEN_LENGTH);
  zero_string(query_spec, MAX_TOKEN_LENGTH);
  zero_string(rslt_buf, BUFSIZE);
  zero_string(rslt_buf2, BUFSIZE);

  *ptr_n_match = -99999;
  *ptr_fld_id  = -99999;
  status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
  if ( strcmp(token, "[") != 0 ) { go_BYE(-1); }

  zero_string(token, MAX_TOKEN_LENGTH);
  status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
  // This token must correspond to a field. Get the field type 
  status = is_fld(NULL, g_tbl_id, token, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  strcpy(fld, token);

  zero_string_to_nullc(token);
  status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
  strcpy(op, token);
  // This token must correspond to a field 
  if ( fld_rec.dict_tbl_id >= 0 ) { /* categorical field */
    if ( ( strcmp(op, "==") == 0 ) || ( strcmp(op, "!=") == 0 ) ) {
      // all is well 
      status = mk_str_fldtype(fld_rec.fldtype, str_fldtype); cBYE(status);
      fprintf(stderr, "fldtype = %s, op =  %s \n", str_fldtype, op);
    }
    else {
      go_BYE(-1);
    }
  }
  else {
    switch ( fld_rec.fldtype ) {
    case B  : 
    case I1 : 
      if ( strcmp(op, "==") == 0 ) {
	// all is well 
        zero_string_to_nullc(token);
        status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
	if ( strlen(token) >= MAX_LEN_VAL ) { go_BYE(-1); }
	strcpy(val, token);
	strcpy(query_spec, fld); strcat(query_spec, " ");
	strcat(query_spec, op); strcat(query_spec, " ");
	strcat(query_spec, val); strcat(query_spec, " ");

	status = mk_fld_names(query_spec, cfld, bfld); cBYE(status);
	status = is_fld(NULL, g_tbl_id, bfld, &bfld_id, &bfld_rec,
			&nn_bfld_id, &nn_bfld_rec); 
	cBYE(status);
	if ( bfld_id >= 0 ) { 
	  /* leaf node has been pre-computed */
	  fprintf(stderr, "Using [%s] for [%s] \n", bfld, query_spec);
	  *ptr_fld_id = bfld_id;
	  *ptr_n_match = bfld_rec.cnt;
	}
	else {
	  if ( ( strcmp(val, "TRUE") == 0 ) || 
	       ( strcmp(val, "FALSE") == 0 ) ||
	       ( strcmp(val, "NULL") == 0 ) ||
	       ( strcmp(val, "NOT_NULL") == 0 ) ) {
	    // all is well 
	    dbg_msg_1(g_level_num, fld_id, "==", token);
	    if ( ( strcasecmp(val, "TRUE") == 0 ) || 
		 ( strcasecmp(val, "FALSE") == 0 ) ) {
	      char str_val_I1[2];
	      if ( strcasecmp(val, "TRUE") == 0 ) {
		strcpy(str_val_I1, "1");
	      }
	      else {
		strcpy(str_val_I1, "0");
	      }
	      dbg_msg_2(g_level_num, query_spec);

	      status = f1s1opf2(g_tbl, fld, str_val_I1, "==", cfld); 
	      cBYE(status);
	      fprintf(stderr, "Processing %s --> %s \n", cfld, bfld);
	      status = core_leaf_work(cfld, bfld, ptr_n_match, ptr_fld_id);
	      cBYE(status);
	      
	    }
	    else if ( strcasecmp(val, "NULL") == 0 ) {
	      fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
	      if ( fld_rec.nn_fld_id < 0 ) { /* no null values */
		// Selects none 
	      }
	      else {
		// count null values 
	      }
	    }
	    else if ( strcasecmp(val, "NOT_NULL") == 0 ) {
	      fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
	      if ( fld_rec.nn_fld_id < 0 ) { /* no null values */
		// Selects all 
	      }
	      else {
		// count non-null values
	      }
	    }
	    else {
	      go_BYE(-1);
	    }
	  }
	  else {
	    go_BYE(-1);
	  }
	}
      }
      else {
	go_BYE(-1);
      }
      break;
    case I4 : 
      if ( ( strcmp(op, "==") == 0 ) || 
           ( strcmp(op, "!=") == 0 ) || 
           ( strcmp(op, "<=") == 0 ) || 
           ( strcmp(op, ">=") == 0 ) || 
           ( strcmp(op, "<")  == 0 ) || 
           ( strcmp(op, ">")  == 0 ) || 
           ( strcmp(op, "<||>") == 0 ) || 
	   ( strcmp(op, "<=||>=") == 0 ) || 
	   ( strcmp(op, ">&&<") == 0 ) || 
	   ( strcmp(op, ">=&&<=") == 0 ) ) {
	// all is well 
        zero_string_to_nullc(token);
        status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
	if ( strlen(token) >= MAX_LEN_VAL ) { go_BYE(-1); }
	if ( *token == '\0' ) { go_BYE(-1); }
	strcpy(val, token);

	strcpy(query_spec, fld); strcat(query_spec, " ");
	strcat(query_spec, op); strcat(query_spec, " ");
	strcat(query_spec, val); strcat(query_spec, " ");
	dbg_msg_2(g_level_num, query_spec);

	status = mk_fld_names(query_spec, cfld, bfld); cBYE(status);

	status = is_fld(NULL, g_tbl_id, bfld, &bfld_id, &bfld_rec,
			&nn_bfld_id, &nn_bfld_rec); 
	cBYE(status);
	if ( bfld_id >= 0 ) { 
	  /* leaf node has been pre-computed */
	  fprintf(stderr, "Using [%s] for [%s] \n", bfld, query_spec);
	  *ptr_fld_id = bfld_id;
	  *ptr_n_match = bfld_rec.cnt;
	}
	else {
	  if ( ( strcmp(val, "NULL") == 0 ) || 
	       ( strcmp(val, "NOT_NULL") == 0 ) ) {
	    dbg_msg_1(g_level_num, fld_id, op, val);
	    status = is_nn_fld(g_tbl, fld, &nn_fld_id, rslt_buf); cBYE(status);
	    if ( nn_fld_id >= 0 ) { 
	      strcpy(nn_fld, "_nn_"); strcat(nn_fld, fld);
	      status = f1opf2(g_tbl, nn_fld, "op=[conv]:newtype=[B]", cfld);
	      cBYE(status);
	      fprintf(stderr, "Processing %s --> %s \n", cfld, bfld);
	      status = core_leaf_work(cfld, bfld, ptr_n_match, ptr_fld_id);
	      cBYE(status);
	    }
	    else {
	      *ptr_fld_id = -1; 
	      if ( strcmp(val, "NULL") == 0 ) {
		*ptr_n_match = 0;
	      }
	      else if ( strcmp(val, "NOT_NULL") == 0 ) {
		*ptr_n_match = g_nR;
	      }
	      else { go_BYE(-1); }
	    }
	  }
	  else {	
	    status = f1s1opf2(g_tbl, fld, val, op, cfld); 
	    cBYE(status);
	    fprintf(stderr, "Processing %s --> %s \n", cfld, bfld);
	    status = core_leaf_work(cfld, bfld, ptr_n_match, ptr_fld_id);
	    cBYE(status);
	  }
	}
      }
      else {
	go_BYE(-1);
      }
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  zero_string_to_nullc(token);
  status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
  if ( strcmp(token, "]") != 0 ) { go_BYE(-1); }



  *ptr_is_leaf = true;
 BYE:
  for ( int i = 0; i < g_level_num; i++ ) { fprintf(stderr, "\t"); }
  fprintf(stderr, " --> simple  returning %lld : %d \n", *ptr_n_match, *ptr_fld_id);
  // print_balance(*ptr_str);
  return(status);
}

int
complex_query_parse(
		    char **ptr_str,
		    long long *ptr_n_match, /* num matched */
		    int *ptr_fld_id, /* B field created. n == 0 => fld_id < 0 */
		    bool *ptr_is_leaf
		    )
{
  int status = 0;
  char token[MAX_TOKEN_LENGTH];
  char query_spec[MAX_TOKEN_LENGTH];
  int match_fld_id; long long n_match; bool is_leaf;
#define MAX_NUM_CLAUSES_IN_COMPLEX_QUERY 16
  int x_fld_id[MAX_NUM_CLAUSES_IN_COMPLEX_QUERY];
  long long x_n_match[MAX_NUM_CLAUSES_IN_COMPLEX_QUERY];
  bool x_is_leaf[MAX_NUM_CLAUSES_IN_COMPLEX_QUERY];
  char bop[BUFSIZE];


  *ptr_n_match = -1; 
  *ptr_fld_id  = -1;
  zero_string(token, MAX_TOKEN_LENGTH);
  zero_string(query_spec, MAX_TOKEN_LENGTH);
  zero_string(bop, BUFSIZE);
  g_level_num++;
  status = ro_read_token(*ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
  strcpy(bop, token);
  if ( (  strcmp(bop, "&&" ) == 0 ) || 
       (  strcmp(bop, "||" ) == 0 ) ) {

    zero_string_to_nullc(token);
    status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
    if ( (  strcmp(token, "&&" ) != 0 ) &&
	 (  strcmp(token, "||" ) != 0 ) ) {
      go_BYE(-1);
    }
    strcpy(bop, token);

    zero_string_to_nullc(token);
    status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
    if ( strcmp(token, "(" ) != 0 ) { go_BYE(-1); }

    int num_complex_queries = 0;
    for ( ; ; ) {

      if ( num_complex_queries == MAX_NUM_CLAUSES_IN_COMPLEX_QUERY ) {
	go_BYE(-1);
      }
      status = complex_query_parse(ptr_str, &(x_n_match[num_complex_queries]),
				   &(x_fld_id[num_complex_queries]),
				   &(x_is_leaf[num_complex_queries])
				   ); cBYE(status);
      num_complex_queries++;

      zero_string_to_nullc(token);
      status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
      if ( strcmp(token, ";" ) != 0 ) { go_BYE(-1); }

      zero_string_to_nullc(token);
      status = ro_read_token(*ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
      if ( strcmp(token, ")") == 0 ) {
	break;
      }
    }
    if ( num_complex_queries < 2 ) { 
      go_BYE(-1);
    }
    char strbuf[BUFSIZE];

    strcpy(query_spec, bop); strcat(query_spec, " ");
    for ( int i = 0; i < num_complex_queries; i++ ) { 
      zero_string(strbuf, BUFSIZE);
      sprintf(strbuf, " fld %d ", x_fld_id[i]);
      strcat(query_spec, strbuf); 
    }
    FLD_REC_TYPE  bfld_rec, nn_bfld_rec;
    int bfld_id, nn_bfld_id;
    char rslt_buf[BUFSIZE];
    char rslt_buf2[BUFSIZE];
    char cfld[MAX_LEN_FLD_NAME+1];
    char bfld[MAX_LEN_FLD_NAME+1];

    zero_string(rslt_buf, BUFSIZE);
    zero_string(strbuf, BUFSIZE);

    status = mk_fld_names(query_spec, cfld, bfld); cBYE(status);
    zero_string(cfld, MAX_LEN_FLD_NAME+1); // since we don't need it 
    FLD_REC_TYPE fld0_rec, fld1_rec;
    status = get_fld_meta(x_fld_id[0], &fld0_rec); 
    cBYE(status);
    status = get_fld_meta(x_fld_id[1], &fld1_rec); cBYE(status);
    status = f1f2opf3(g_tbl, fld0_rec.name, fld1_rec.name, bop, bfld); 
    cBYE(status);
    if ( x_is_leaf[0] == false ) {
      fprintf(stderr, "Complex Deleting %s \n", fld0_rec.name); 
      status = del_fld(NULL, g_tbl_id, NULL, x_fld_id[0], true); cBYE(status);
    }
    if ( x_is_leaf[1] == false ) {
      fprintf(stderr, "Complex Deleting %s \n", fld1_rec.name); 
      status = del_fld(NULL, g_tbl_id, NULL, x_fld_id[1], true); cBYE(status);
    }
    for ( int i = 2; i < num_complex_queries; i++ ) { 
      FLD_REC_TYPE fld_rec;
      status = get_fld_meta(x_fld_id[i], &fld_rec); cBYE(status);
      status = f1f2opf3(g_tbl, bfld, fld_rec.name, bop, bfld); cBYE(status);
      if ( x_is_leaf[i] == false ) {
        fprintf(stderr, "Complex Deleting %s \n", fld_rec.name); 
        status = del_fld(NULL, g_tbl_id, NULL, x_fld_id[i], true); cBYE(status);
      }
    }
    status = ext_f_to_s(g_tbl, bfld, "sum", rslt_buf,  BUFSIZE); cBYE(status);
    status = is_fld(NULL, g_tbl_id, bfld, &bfld_id, &bfld_rec, 
		    &nn_bfld_id, &nn_bfld_rec);
    *ptr_fld_id = bfld_id; // return this 
    zero_string(rslt_buf2, BUFSIZE);
    char *endptr;
    status = read_nth_val(rslt_buf, ':', 0, rslt_buf2, BUFSIZE); 
    cBYE(status);
    *ptr_n_match = strtoll(rslt_buf2, &endptr, 10); 
    if ( *endptr != '\0' ) { go_BYE(-1); }
    *ptr_is_leaf = false;
    // Now that you have processed the bop, you need to process the
    // close paren and the semi-colon
    status = read_token(ptr_str, token, MAX_TOKEN_LENGTH); cBYE(status);
    if ( strcmp(token, ")" ) != 0 ) { go_BYE(-1); }
    fprintf(stderr, "Created %s \n", bfld);
  }
  else if (  strcmp(token, "[" ) == 0 ) { 
    status = simple_query_parse(ptr_str, &n_match, &match_fld_id, &is_leaf); 
    cBYE(status);
    *ptr_n_match = n_match;
    *ptr_fld_id = match_fld_id;
    *ptr_is_leaf = is_leaf;
  }
  else {
    go_BYE(-1);
  }
  for ( int i = 0; i < g_level_num; i++ ) { fprintf(stderr, "\t"); }
  fprintf(stderr, " --> complex returning %lld : %d \n", *ptr_n_match, *ptr_fld_id);
  // print_balance(*ptr_str);

  g_level_num--;
 BYE:
  return(status);
}

int
tesla_parse_query( /* Q tesla parser */
		  const char *tbl,
		  const char *fld,
		  char *qstr /* query string */
		   )
{
  int status = 0;
  char *str = qstr; // TODO THINK ABOUT no const 
  TBL_REC_TYPE tbl_rec; int tbl_id = -1;
  FLD_REC_TYPE fld_rec; int fld_id = -1;
  char *X = NULL; size_t nX = 0;


  if ( ( tbl  == NULL ) || ( *tbl  == '\0' ) )  { go_BYE(-1); }
  if ( ( qstr == NULL ) || ( *qstr == '\0' ) )  { go_BYE(-1); }
  status = rs_mmap(qstr, &X, &nX, 0); cBYE(status);
  if ( ( X == NULL ) || ( nX == 0 ) ) { go_BYE(-1); }
  str = X;

  zero_string(g_tbl, MAX_LEN_TBL_NAME+1);
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  g_tbl_id = tbl_id;
  strcpy(g_tbl, tbl);
  g_nR = tbl_rec.nR;

  g_level_num = -1;
  long long n_matched = -1;
  bool is_leaf;
  status = complex_query_parse(&str, &n_matched, &fld_id, &is_leaf); cBYE(status);
  status = get_fld_meta(fld_id, &fld_rec); cBYE(status);
  if ( ( fld != NULL ) && ( *fld != '\0' ) ) {
    status = rename_fld(tbl, fld_rec.name, fld);
    cBYE(status);
  }
  status = get_fld_meta(fld_id, &fld_rec); cBYE(status);
;
 BYE:
  rs_munmap(X, nX);
  return(status);
}
