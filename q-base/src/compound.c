#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "extract_S.h"
#include "get_type_op_fld.h"
#include "vec_f1opf2.h"
#include "vec_f1f2opf3.h"
#include "vec_f1s1opf2.h"
#include "vec_f_to_s.h"
#include "f1f2_to_s.h"

#include "par_count_I1.h"
#include "par_count_I2.h"
#include "par_count_cfld_I1.h"
#include "par_count_cfld_I2.h"
#include "count_I1.h"
#include "count_I2.h"
#include "count_I4.h"
#include "count_I8.h"
#include "count_nn_I1.h"
#include "count_nn_I2.h"
#include "count_nn_I4.h"
#include "count_nn_I8.h"

#include "par_countf_I1.h"
#include "par_countf_I2.h"
#include "par_countf_cfld_I1.h"
#include "par_countf_cfld_I2.h"
#include "countf_I1.h"
#include "countf_I2.h"
#include "countf_I4.h"
#include "countf_I8.h"
#include "countf_nn_I1.h"
#include "countf_nn_I2.h"
#include "countf_nn_I4.h"
#include "countf_nn_I8.h"

#include "assign_I8.h"
#include "compound.h"

#define MAX_OP_LEN 15
#define BUFSZ 255

extern int g_num_cores;

// START FUNC DECL
int proc_op_spec(
		 char *op_spec, 
		 int lno, 
		 COMP_EXPR_TYPE *comp_expr,
		 char *pure_op
		 )
// STOP FUNC DECL
{
  int status = 0;
  if  ( ( op_spec == NULL ) || ( *op_spec == '\0' ) ) { go_BYE(-1); }
  int len = strlen(op_spec); 
  if ( len > MAX_LEN_SCALAR_EXPRESSION ) { go_BYE(-1); }
  strcpy(comp_expr[lno].op_spec, op_spec); 
  zero_string(pure_op, MAX_OP_LEN);
  bool is_null;
  status = extract_S(op_spec, "op=[", "]", pure_op, MAX_OP_LEN-1, &is_null); 
  if ( is_null ) { 
    strcpy(pure_op, op_spec);
  }
 BYE:
  return status ;
}

// START FUNC DECL
int proc_qfn(
	     const char *op, 
	     int lno, 
	     COMP_EXPR_TYPE *comp_expr
	     )
// STOP FUNC DECL
{
  int status = 0;
  COMP_OP_TYPE xop;
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  xop = undef_comp_expr;
  if ( strcmp(op, "f1s1opf2"  ) == 0 ) { xop = op_f1s1opf2; }
  if ( strcmp(op, "f1f2opf3"  ) == 0 ) { xop = op_f1f2opf3; }
  if ( strcmp(op, "f1opf2f3"  ) == 0 ) { xop = op_f1opf2f3; }
  if ( strcmp(op, "f1opf2"    ) == 0 ) { xop = op_f1opf2; }
  if ( strcmp(op, "f_to_s"    ) == 0 ) { xop = op_f_to_s; }
  if ( strcmp(op, "f1f2_to_s" ) == 0 ) { xop = op_f1f2_to_s; }
  if ( strcmp(op, "count"     ) == 0 ) { xop = op_count; }
  if ( strcmp(op, "countf"    ) == 0 ) { xop = op_countf; }
  if ( xop == undef_comp_expr ) { 
    fprintf(stderr, "operator [%s] not ok in compound expression\n", op);
    fprintf(stderr, "Line %d\n", lno+1);
    go_BYE(-1);
  }
  comp_expr[lno].op = xop;
 BYE:
  return status ;
}

// START FUNC DECL
int
is_created_prior(
		 char *fld,
		 COMP_EXPR_TYPE *comp_expr,
		 int n_comp_expr,
		 int is_temp,
		 int *ptr_expr_idx,
		 int *ptr_fld_idx
		 )
// STOP  FUNC DECL
{
  int status = 0;
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  *ptr_expr_idx = -1;
  *ptr_fld_idx  = -1;
  for ( int i = 0; i < n_comp_expr; i++ ) {
    for ( int j = 0; j < 3; j++ ) {
      if ( ( strcmp(comp_expr[i].fld[j].name, fld) == 0 ) && 
           ( comp_expr[i].fld[j].is_temp == is_temp ) && 
           ( comp_expr[i].fld[j].is_write == 1 )  ) {
	*ptr_expr_idx = i;
	*ptr_fld_idx  = j;
      }
    }
  }
 BYE:
  return status ;
}


// START FUNC DECL
int proc_scalar(
		char *scalar, 
		int lno, 
		COMP_EXPR_TYPE *comp_expr
		)
// STOP FUNC DECL
{
  int status = 0;
  if ( ( scalar == NULL ) || ( *scalar == '\0' ) ) { go_BYE(-1); }
  int len = strlen(scalar);
  // trim trailing eoln if necessary
  if ( scalar[len-1] == '\n' ) { scalar[len-1] = '\0'; len--; }

  if ( ( len > MAX_LEN_SCALAR_EXPRESSION ) ) { go_BYE(-1); }
  strcpy(comp_expr[lno].scalar,  scalar);
 BYE:
  return status ;
}
//
// START FUNC DECL
int proc_env_var(
		 char *env_var, 
		 int lno, 
		 COMP_EXPR_TYPE *comp_expr
		 )
// STOP FUNC DECL
{
  int status = 0;
  int len = strlen(env_var); if ( len == 0 ) { go_BYE(-1); }
  if ( env_var[len-1] == '\n' ) { env_var[len-1] = '\0'; len--; }
  if ( is_legal_env_var(env_var) == false ) { go_BYE(-1); }
  strcpy(comp_expr[lno].env_var,  env_var);
 BYE:
  return status ;
}


// START FUNC DECL
int
proc_fld(
	 int tbl_id,
	 long long nR,
	 char *fld, 
	 FLD_TYPE dst_fldtype,
	 bool dst_has_null,
	 int expr_idx,
	 int fld_idx,
	 int is_write, 
	 COMP_EXPR_TYPE *comp_expr,
	 FLD_TYPE *ptr_fldtype,
	 bool *ptr_has_null
	 )
// STOP FUNC DECL
{
  int status = 0;
  int fld_id, nn_fld_id; FLD_REC_TYPE fld_rec, nn_fld_rec;
  char *X = NULL;    size_t nX = 0; 
  char *nn_X = NULL; size_t nn_nX = 0; 
  int ddir_id = -1, fileno = 0, fldsz = 0; size_t filesz = 0;
  int nn_ddir_id = -1, nn_fileno = 0;
  int prior_expr_idx = -1, prior_fld_idx = -1, is_temp;
  int is_first;

  *ptr_fldtype  = undef_fldtype;
  *ptr_has_null = true;
  if ( ( is_write != 0 ) && ( is_write != 1 ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( fld_idx < 0 ) || ( fld_idx > 3 ) ) { go_BYE(-1); }
  int len = strlen(fld);
  // strip trailing slash if any 
  if ( fld[len-1] == '\n' ) { fld[len-1] = '\0'; len--; }
  if  ( ( len <= 0 ) || ( len > MAX_LEN_FLD_NAME ) ) { go_BYE(-1); }

  fld_id   = INT_MIN; // bogus value to make sure we set this
  is_temp  = INT_MIN; // bogus value to make sure we set this
  //  is_write = INT_MIN; // This is input Do not mess with it 
  is_first = INT_MIN; // bogus value to make sure we set this
  *ptr_fldtype = undef_fldtype; // bogus value to make sure we set this

  // If name of field ends wih single quote, it is a temporay field 
  if ( fld[len-1] == '\'' ) { /* temporary field */
    is_temp = 1;
    fld_id = -1; // also indicates temporary field 
    comp_expr[expr_idx].fld[fld_idx].is_temp = 1;
    /* is this new field or old one */
    int prior_expr_idx = -1, prior_fld_idx = -1, is_temp = 1;
    status = is_created_prior(fld, comp_expr, expr_idx+1, is_temp,
			      &prior_expr_idx, &prior_fld_idx); cBYE(status);
    if ( prior_expr_idx >= 0 ) { /* was mentioned in earlier statement */
      is_first = 0;
      if ( is_write == 1 ) { 
	fprintf(stderr, "Cannot write to field created earlier\n");
	go_BYE(-1);
      }

      comp_expr[expr_idx].fld[fld_idx] = 
	comp_expr[prior_expr_idx].fld[prior_fld_idx];
      *ptr_fldtype = comp_expr[expr_idx].fld[fld_idx].fldtype;
      if ( comp_expr[expr_idx].fld[fld_idx].nn_X == NULL ) {
	*ptr_has_null = false;
      }
      else {
	*ptr_has_null = true;
      }
    }
    else {
      is_first = 1;
      if ( is_write == 0 ) {
	fprintf(stderr, "Cannot read from field not created yet\n");
	go_BYE(-1);
      }
      // create storage. The multiplier of 2 is vital. This is because
      // last block can be bigger than others. 
      if ( dst_fldtype == B ) { 
        nX = ( 2 * COMP_EXPR_BLOCK_SIZE / 8 ) + 1024; // TODO P2 Check this
      }
      else {
        status = get_fld_sz(dst_fldtype, &fldsz); cBYE(status);
        nX = 2 * COMP_EXPR_BLOCK_SIZE * fldsz;
      }
      X  = malloc(nX);
      return_if_malloc_failed(X);

      if ( ( dst_fldtype == B ) && ( dst_has_null == true ) ) { go_BYE(-1); }
      if ( dst_has_null == true ) { 
	status = get_fld_sz(I1, &fldsz); cBYE(status);
	nn_nX = 2 * COMP_EXPR_BLOCK_SIZE * fldsz;
	nn_X  = malloc(nn_nX);
	return_if_malloc_failed(nn_X);
      }
      else {
	nn_nX = 0; nn_X = NULL;
      }

      comp_expr[expr_idx].fld[fld_idx].X       = X;
      comp_expr[expr_idx].fld[fld_idx].nX      = nX;
      comp_expr[expr_idx].fld[fld_idx].nn_X    = nn_X;
      comp_expr[expr_idx].fld[fld_idx].nn_nX   = nn_nX;
      comp_expr[expr_idx].fld[fld_idx].fldtype = dst_fldtype;

      fld_id = -1;
    }
  }
  else {
    is_temp = 0;
    status = is_created_prior(fld, comp_expr, expr_idx+1, is_temp,
			      &prior_expr_idx, &prior_fld_idx); cBYE(status);
    if ( is_write == 0 ) {  /* reading the field */
      if ( prior_expr_idx >= 0 ) {
	is_first = 0;
	comp_expr[expr_idx].fld[fld_idx] = 
	  comp_expr[prior_expr_idx].fld[prior_fld_idx];
	*ptr_fldtype = comp_expr[prior_expr_idx].fld[prior_fld_idx].fldtype;
	fld_id = comp_expr[prior_expr_idx].fld[prior_fld_idx].fld_id;
	if ( comp_expr[prior_expr_idx].fld[prior_fld_idx].nn_X == NULL ) {
	  *ptr_has_null = false;
	}
	else {
	  *ptr_has_null = true;
	}
      }
      else {
	is_first = 1;
	status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, 
			&nn_fld_id, &nn_fld_rec);
	cBYE(status);
	if ( fld_id < 0 ) { 
	  fprintf(stderr, "Line %d. Field = [%s] not found \n", expr_idx, fld);
	}
	status = get_data(fld_rec, &X, &nX, 0); cBYE(status); 
	if ( nn_fld_id >= 0 ) { 
	  status = get_data(nn_fld_rec, &nn_X, &nn_nX, 0); cBYE(status); 
	  *ptr_has_null = true;
	}
	else {
	  *ptr_has_null = false;
	}
	if ( ( fld_rec.fldtype == SC ) || ( fld_rec.fldtype == SV ) ) { 
	  // Not ready to handle compound expressions for these types
	  go_BYE(-1); 
	}
	*ptr_fldtype = fld_rec.fldtype;
	comp_expr[expr_idx].fld[fld_idx].fldtype  = fld_rec.fldtype;
	comp_expr[expr_idx].fld[fld_idx].X       = X;
	comp_expr[expr_idx].fld[fld_idx].nX      = nX;
	comp_expr[expr_idx].fld[fld_idx].nn_X    = nn_X;
	comp_expr[expr_idx].fld[fld_idx].nn_nX   = nn_nX;
      }
    }
    else {
      is_first = 1;
      if ( ( prior_expr_idx >= 0 ) && 
	   ( comp_expr[prior_expr_idx].fld[prior_fld_idx].is_write == true ) ) {
	fprintf(stderr, "Field [%s] created twice \n", fld);
	go_BYE(-1);
      }
      if ( dst_fldtype == B ) { 
        filesz = ( nR / 8 )  + 1024; // Think about this. 
      }
      else {
        status = get_fld_sz(dst_fldtype, &fldsz); cBYE(status);
        filesz = nR * fldsz;
      }
      status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
      cBYE(status);
      zero_fld_rec(&fld_rec); fld_rec.fldtype = dst_fldtype; 
      status = add_fld(tbl_id, fld, ddir_id, fileno, &fld_id, &fld_rec);

      if ( ( dst_fldtype == B ) && ( dst_has_null == true ) ) { go_BYE(-1); }
      if ( dst_has_null ) { 
	status = get_fld_sz(I1, &fldsz); cBYE(status);
	filesz = nR * fldsz;
	status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno); cBYE(status);
	cBYE(status);
	zero_fld_rec(&nn_fld_rec); nn_fld_rec.fldtype = I1; 
	status = add_aux_fld(NULL, tbl_id, NULL, fld_id, nn_ddir_id, 
			     nn_fileno, nn, &nn_fld_id, &nn_fld_rec);
	status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, 
			&nn_fld_id, &nn_fld_rec);
	status = get_data(nn_fld_rec, &nn_X, &nn_nX, 1); cBYE(status); 
      }

      status = get_data(fld_rec, &X, &nX, 1); cBYE(status); 
      comp_expr[expr_idx].fld[fld_idx].fldtype  = dst_fldtype;
      comp_expr[expr_idx].fld[fld_idx].X       = X;
      comp_expr[expr_idx].fld[fld_idx].nX      = nX;
      comp_expr[expr_idx].fld[fld_idx].nn_X    = nn_X;
      comp_expr[expr_idx].fld[fld_idx].nn_nX   = nn_nX;
    }
  }
  // Make sure all values are set
  if ( fld_id   == INT_MIN ) { go_BYE(-1); }
  if ( is_temp  == INT_MIN ) { go_BYE(-1); }
  if ( is_write == INT_MIN ) { go_BYE(-1); }
  if ( is_first == INT_MIN ) { go_BYE(-1); } 

  comp_expr[expr_idx].fld[fld_idx].fld_id   = fld_id;
  comp_expr[expr_idx].fld[fld_idx].is_temp  = is_temp;
  comp_expr[expr_idx].fld[fld_idx].is_write = is_write;
  comp_expr[expr_idx].fld[fld_idx].is_first = is_first;
  comp_expr[expr_idx].fld[fld_idx].nR       = nR;
  comp_expr[expr_idx].fld[fld_idx].tbl_id   = tbl_id;

  strcpy(comp_expr[expr_idx].fld[fld_idx].name, fld);
 BYE:
  return status ;
}

// START FUNC DECL
int 
chk_comp_expr(
	      char *filename,
	      COMP_EXPR_TYPE **ptr_comp_expr,
	      int *ptr_n_comp_expr,
	      int *ptr_tbl_id,
	      TBL_REC_TYPE *ptr_tbl_rec

	      )
// STOP FUNC DECL
{
  int status = 0;
  char tbl[MAX_LEN_TBL_NAME+1]; int tbl_id = -1; TBL_REC_TYPE tbl_rec; 

  char otbl[MAX_LEN_TBL_NAME+1];  int otbl_id = -1; TBL_REC_TYPE otbl_rec;
  char fld1[MAX_LEN_FLD_NAME+1];
  char scalar[MAX_LEN_SCALAR_EXPRESSION+1];
  char fld2[MAX_LEN_FLD_NAME+1];
  char fld3[MAX_LEN_FLD_NAME+1];
  char fld4[MAX_LEN_FLD_NAME+1];
  char qfn[MAX_OP_LEN+1]; // Q function to be performed
  char pure_op[MAX_OP_LEN+1]; 
  char env_var[BUFSZ+1]; 
  char op_spec[BUFSZ+1]; 

  long long nR = LLONG_MAX;
  size_t len_line = 1024;
  char *line = NULL;
  COMP_EXPR_TYPE *comp_expr = NULL;
  int n_comp_expr = 0;
  FILE *cfp = NULL;
  char *delim = "__@@__";
  FLD_TYPE f1_fldtype, f2_fldtype, f3_fldtype, jnk_fldtype;
  bool f1_has_null, f2_has_null, f3_has_null;
  bool jnk_bool;

  if ( ( filename == NULL ) || ( *filename == '\0' ) ) { go_BYE(-1); }
  status = num_lines(filename, &n_comp_expr); cBYE(status);
  if ( n_comp_expr <= 1 ) { go_BYE(-1); }
  if ( n_comp_expr > MAX_STATEMENTS_IN_COMP_EXPR ) { go_BYE(-1); }
  comp_expr = malloc(n_comp_expr * sizeof(COMP_EXPR_TYPE));
  return_if_malloc_failed(comp_expr);
  zero_comp_expr(comp_expr, n_comp_expr); 
  line = malloc(len_line * sizeof(char));
  return_if_malloc_failed(line);

  cfp = fopen(filename, "r");
  return_if_fopen_failed( cfp,  filename, "r");
  //----------------------------------------------
  zero_string(otbl,    MAX_LEN_TBL_NAME+1);
  zero_string(tbl,     MAX_LEN_TBL_NAME+1);
  zero_string(fld1,    MAX_LEN_FLD_NAME+1);
  zero_string(scalar,  MAX_LEN_SCALAR_EXPRESSION+1);
  zero_string(fld2,    MAX_LEN_FLD_NAME+1);
  zero_string(fld3,    MAX_LEN_FLD_NAME+1);
  zero_string(fld4,    MAX_LEN_FLD_NAME+1);
  zero_string(op_spec, BUFSZ+1);
  zero_string(env_var, BUFSZ+1);
  zero_string(qfn,     MAX_OP_LEN+1);
  zero_string(pure_op, MAX_OP_LEN+1);
  //----------------------------------------------
  for ( int lno = 0; lno < n_comp_expr ; lno++ ) {
    /* read each line of the file */
    ssize_t n = getline(&line, &len_line, cfp);
    if ( n <= 0 ) { break; }
    /* break line into parts */
    int colidx = 0;
    // Determine the Q operation to be performed
    status = read_nth_val(line, delim, colidx++, qfn, MAX_OP_LEN);
    status = proc_qfn(qfn, lno, comp_expr); cBYE(status);
    // Determine the table on which operation is to be performed
    /* tbl_id set first time and checked subsequently */
    if ( lno == 0 ) { 
      status = read_nth_val(line, delim, colidx++, tbl, MAX_LEN_TBL_NAME);
      status = is_tbl(tbl, &tbl_id, &tbl_rec); 
      if ( tbl_id < 0 ) { 
	fprintf(stderr, "Line %d. Table = [%s] not found \n", lno+1, tbl); 
      }
      nR = tbl_rec.nR;
    }
    else {
      int this_tbl_id = INT_MAX; TBL_REC_TYPE this_tbl_rec;
      char thistbl[MAX_LEN_TBL_NAME+1]; 
      zero_string(thistbl, MAX_LEN_TBL_NAME+1);
      status = read_nth_val(line, delim, colidx++, thistbl, MAX_LEN_TBL_NAME);
      status = is_tbl(thistbl, &this_tbl_id, &this_tbl_rec); 
      if ( this_tbl_id != tbl_id ) { 
	fprintf(stderr, "Line %d. Old table = %s, new table = %s \n",
		lno+1, tbl, thistbl);
	go_BYE(-1); 
      }
    }
    //----------------------------------------------
    if ( strcmp(qfn, "f1opf2") == 0 ) {
      status = read_nth_val(line, delim, colidx++, fld1, BUFSZ);
      status = proc_fld(tbl_id, nR, fld1, jnk_fldtype, jnk_bool, 
			lno, 0, 0, comp_expr, &f1_fldtype, &f1_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, op_spec, BUFSZ);
      status = proc_op_spec(op_spec, lno, comp_expr, pure_op); cBYE(status);
      //---------------------------------------------------
      status = get_type_op_fld(qfn, pure_op, f1_fldtype, undef_fldtype, 
			       op_spec, &f2_fldtype);
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld2, BUFSZ);
      // Important: f1 has    null values => f2 has    null values
      // Important: f1 has no null values => f2 has no null values
      f2_has_null = f1_has_null;
      status = proc_fld(tbl_id, nR, fld2, f2_fldtype, f2_has_null, 
			lno, 1, 1, comp_expr, &jnk_fldtype, &jnk_bool); 
      cBYE(status);
      //---------------------------------------------------
    }
    else if ( strcmp(qfn, "count") == 0 ) {
      status = read_nth_val(line, delim, colidx++, fld1, BUFSZ);
      status = proc_fld(tbl_id, nR, fld1, jnk_fldtype, jnk_bool, 
			lno, 0, 0, comp_expr, &f1_fldtype, &f1_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld2, BUFSZ);
      if ( *fld2 != '\0' ) { /* cfld can be empty */
	status = proc_fld(tbl_id, nR, fld2, jnk_fldtype, jnk_bool,
			  lno, 1, 0, comp_expr, &f2_fldtype, &f2_has_null);
	cBYE(status);
	if ( f2_fldtype != I1    ) { go_BYE(-1); }
	if ( f2_has_null == true ) { go_BYE(-1); }
      }
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, otbl, BUFSZ);
      status = is_tbl(otbl, &otbl_id, &otbl_rec); 
      if ( otbl_id < 0 ) { go_BYE(-1); }
      if ( otbl_id == tbl_id ) { go_BYE(-1); }
      long long onR =  otbl_rec.nR;
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld3, BUFSZ);
      status = proc_fld(otbl_id, onR, fld3, I8, false, 
			lno, 2, 1, comp_expr, &jnk_fldtype, &jnk_bool);
      cBYE(status);
      //---------------------------------------------------
    }
    else if ( strcmp(qfn, "countf") == 0 ) {
      status = read_nth_val(line, delim, colidx++, fld1, BUFSZ);
      status = proc_fld(tbl_id, nR, fld1, jnk_fldtype, jnk_bool, 
			lno, 0, 0, comp_expr, &f1_fldtype, &f1_has_null); 
      cBYE(status);
      if ( ( f1_fldtype != I4 ) && ( f1_fldtype != I8 ) ) { go_BYE(-1); }
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld2, BUFSZ);
      status = proc_fld(tbl_id, nR, fld2, jnk_fldtype, jnk_bool, 
			lno, 1, 0, comp_expr, &f2_fldtype, &f2_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld3, BUFSZ);
      if ( *fld3 != '\0' ) { /* cfld can be empty */
	status = proc_fld(tbl_id, nR, fld3, jnk_fldtype, jnk_bool,
			  lno, 2, 0, comp_expr, &f3_fldtype, &f3_has_null);
	cBYE(status);
	if ( f3_fldtype != I1    ) { go_BYE(-1); }
	if ( f3_has_null == true ) { go_BYE(-1); }
      }
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, otbl, BUFSZ);
      status = is_tbl(otbl, &otbl_id, &otbl_rec); 
      if ( otbl_id < 0 ) { go_BYE(-1); }
      if ( otbl_id == tbl_id ) { go_BYE(-1); }
      long long onR =  otbl_rec.nR;
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld4, BUFSZ);
      status = proc_fld(otbl_id, onR, fld4, I8, false, 
			lno, 3, 1, comp_expr, &jnk_fldtype, &jnk_bool);
      cBYE(status);
      //---------------------------------------------------
    }
    else if ( strcmp(qfn, "f1s1opf2") == 0 ) {
      status = read_nth_val(line, delim, colidx++, fld1, BUFSZ);
      status = proc_fld(tbl_id, nR, fld1, jnk_fldtype, jnk_bool, 
			lno, 0, 0, comp_expr, &f1_fldtype, &f1_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, scalar, BUFSZ);
      if ( *scalar == '\0' ) { go_BYE(-1); }
      status = proc_scalar(scalar, lno, comp_expr); cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, op_spec, BUFSZ);
      status = proc_op_spec(op_spec, lno, comp_expr, pure_op); cBYE(status);
      //---------------------------------------------------
      status = get_type_op_fld(qfn, pure_op, f1_fldtype, f1_fldtype, 
			       op_spec, &f2_fldtype); cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld2, BUFSZ);
      // Important: f1 has    null values => f2 has    null values
      // Important: f1 has no null values => f2 has no null values
      f2_has_null = f1_has_null;
      status = proc_fld(tbl_id, nR, fld2, f2_fldtype, f2_has_null, 
			lno, 1, 1, comp_expr, &jnk_fldtype, &jnk_bool); 
      if ( status < 0 ) { 
	fprintf(stderr, "fld1 = %s \n", fld1);
	fprintf(stderr, "fld2 = %s \n", fld2);
      }
      cBYE(status);
      //---------------------------------------------------
    }
    else if ( strcmp(qfn, "f1f2opf3") == 0 ) {
      status = read_nth_val(line, delim, colidx++, fld1, BUFSZ);
      status = proc_fld(tbl_id, nR, fld1, jnk_fldtype, jnk_bool, 
			lno, 0, 0, comp_expr, &f1_fldtype, &f1_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld2, BUFSZ);
      status = proc_fld(tbl_id, nR, fld2, jnk_fldtype, jnk_bool, 
			lno, 1, 0, comp_expr, &f2_fldtype, &f2_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, op_spec, BUFSZ);
      status = proc_op_spec(op_spec, lno, comp_expr, pure_op); cBYE(status);
      //---------------------------------------------------
      status = get_type_op_fld(qfn, pure_op, f1_fldtype, f2_fldtype, 
			       op_spec, &f3_fldtype);
      cBYE(status);
      if ( ( f1_has_null == true ) || ( f2_has_null == true ) )  {
	f3_has_null = true;
      }
      else {
	f3_has_null = false;
      }
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld3, BUFSZ);
      status = proc_fld(tbl_id, nR, fld3, f3_fldtype, f3_has_null,
			lno, 2, 1, comp_expr, &jnk_fldtype, &jnk_bool); 
      cBYE(status);
      //---------------------------------------------------
    }
    else if ( strcmp(qfn, "f1f2_to_s") == 0 ) {
      status = read_nth_val(line, delim, colidx++, fld1, BUFSZ);
      status = proc_fld(tbl_id, nR, fld1, jnk_fldtype, jnk_bool, lno, 
			0, 0, comp_expr, &f1_fldtype, &f1_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, fld2, BUFSZ);
      status = proc_fld(tbl_id, nR, fld2, jnk_fldtype, jnk_bool, lno, 
			1, 0, comp_expr, &f2_fldtype, &f2_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, op_spec, BUFSZ);
      status = proc_op_spec(op_spec, lno, comp_expr, pure_op); cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, env_var, BUFSZ);
      status = proc_env_var(env_var, lno, comp_expr); cBYE(status);
      //---------------------------------------------------
    }
    else if ( strcmp(qfn, "f_to_s") == 0 ) {
      status = read_nth_val(line, delim, colidx++, fld1, BUFSZ);
      status = proc_fld(tbl_id, nR, fld1, jnk_fldtype, jnk_bool, lno, 
			0, 0, comp_expr, &f1_fldtype, &f1_has_null); 
      cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, op_spec, BUFSZ);
      status = proc_op_spec(op_spec, lno, comp_expr, pure_op); cBYE(status);
      //---------------------------------------------------
      status = read_nth_val(line, delim, colidx++, env_var, BUFSZ);
      status = proc_env_var(env_var, lno, comp_expr); cBYE(status);
      //---------------------------------------------------
    }
    else {
      go_BYE(-1);
    }
  }
  if ( n_comp_expr <= 1 ) { go_BYE(-1); }
  *ptr_comp_expr   =  comp_expr;
  *ptr_n_comp_expr =  n_comp_expr;
  *ptr_tbl_id      = tbl_id;
  *ptr_tbl_rec     = tbl_rec;
 BYE:
  free_if_non_null(line);
  fclose_if_non_null(cfp);
  return status ;
}
// START FUNC DECL
void 
zero_comp_expr(
	       COMP_EXPR_TYPE *comp_expr, 
	       int n_comp_expr
	       )
// STOP FUNC DECL
{
  for ( int i = 0; i < n_comp_expr; i++ ) {
    comp_expr[i].op = undef_comp_expr;
    zero_string(comp_expr[i].scalar, MAX_LEN_SCALAR_EXPRESSION+1);
    zero_string(comp_expr[i].op_spec, MAX_LEN_SCALAR_EXPRESSION+1);
    zero_string(comp_expr[i].env_var, MAX_LEN_FLD_NAME+1);
    for ( int j = 0; j < 4; j++ ) { 
      zero_string(comp_expr[i].fld[j].name, MAX_LEN_FLD_NAME+1);
      comp_expr[i].fld[j].fld_id   = -1;
      comp_expr[i].fld[j].tbl_id   = -1;
      comp_expr[i].fld[j].nR       = -1;
      comp_expr[i].fld[j].is_temp  = -1;
      comp_expr[i].fld[j].is_write = -1;
      comp_expr[i].fld[j].is_first = 0;
      comp_expr[i].fld[j].fldtype  = undef_fldtype;
      comp_expr[i].fld[j].X        = NULL;
      comp_expr[i].fld[j].nX       = 0;
      comp_expr[i].fld[j].nn_X     = NULL;
      comp_expr[i].fld[j].nn_nX    = 0;
    }
  }
}

// START FUNC DECL
int
release_comp_expr(
		  COMP_EXPR_TYPE **ptr_comp_expr, 
		  int *ptr_n_comp_expr
		  )
// STOP FUNC DECL
{
  int status = 0;
  COMP_EXPR_TYPE *comp_expr = *ptr_comp_expr;
  int n_comp_expr = *ptr_n_comp_expr;

  for ( int i = 0; i < n_comp_expr; i++ ) {
    for ( int j = 0; j < 3; j++ ) { 
      if ( comp_expr[i].fld[j].is_temp  == 1 ) {
	if ( comp_expr[i].fld[j].is_write  == 0 ) { continue; }
	free_if_non_null(comp_expr[i].fld[j].X); 
	free_if_non_null(comp_expr[i].fld[j].nn_X); 
      }
      else {
	if ( comp_expr[i].fld[j].is_write  == 1 ) { 
	  munmap(comp_expr[i].fld[j].X, comp_expr[i].fld[j].nX); 
	  munmap(comp_expr[i].fld[j].nn_X, comp_expr[i].fld[j].nn_nX); 

	  comp_expr[i].fld[j].X = NULL; 
	  comp_expr[i].fld[j].nX = 0;
	  comp_expr[i].fld[j].nn_X = NULL; 
	  comp_expr[i].fld[j].nn_nX = 0;
	}
	else {
	  if ( comp_expr[i].fld[j].is_first  == 1 ) { 
	    munmap(comp_expr[i].fld[j].X, comp_expr[i].fld[j].nX); 
	    munmap(comp_expr[i].fld[j].nn_X, comp_expr[i].fld[j].nn_nX); 

	    comp_expr[i].fld[j].X = NULL; 
	    comp_expr[i].fld[j].nX = 0;
	    comp_expr[i].fld[j].nn_X = NULL; 
	    comp_expr[i].fld[j].nn_nX = 0;
	  }
	}
      }
    }
  }
  free_if_non_null(comp_expr);
  *ptr_comp_expr = NULL;
  *ptr_n_comp_expr = 0;
  return(status);
}

static int 
offset_pointers(
    int tbl_id,
	        COMP_EXPR_TYPE *comp_expr, 
		int comp_expr_idx,
		int fldidx, 
		long long lb,
		long long *ptr_nR,
		FLD_TYPE *ptr_ftype,
		char **ptr_X, 
		char **ptr_nn_X
		)
{
  int status = 0;
  COMP_EXPR_FLD_INFO_TYPE f;
  f = comp_expr[comp_expr_idx].fld[fldidx];
  FLD_TYPE ftype  = f.fldtype;
  char *X    = f.X;
  char *nn_X = f.nn_X; 
  int fldsz;

  *ptr_nR = f.nR;
  if ( ( ftype == B ) && ( nn_X != NULL ) ) { go_BYE(-1); }
  if ( ( f.is_temp == 0 ) && ( X != NULL ) ) { 
    if ( ftype == B ) { 
      if (    X != NULL ) { X    += (lb/8); }
    }
    else {
      status = get_fld_sz(ftype, &fldsz);  cBYE(status);
      if ( ( X != NULL ) && ( f.tbl_id == tbl_id ) ) { 
	X    += (fldsz * lb); }
    }
    status = get_fld_sz(I1 , &fldsz);  cBYE(status);
    if ( ( nn_X != NULL ) && ( f.tbl_id == tbl_id ) ) { 
      nn_X += (fldsz * lb); 
    }
  }
  *ptr_X = X;
  *ptr_nn_X = nn_X;
  *ptr_ftype = ftype;
 BYE:
  return(status);
}

// START FUNC DECL
int
exec_comp_expr(
	       int tbl_id,
	       TBL_REC_TYPE tbl_rec,
	       COMP_EXPR_TYPE *comp_expr, 
	       int n_comp_expr
	       )
// STOP FUNC DECL
{
  int status = 0;
  // Following used to combine f_to_s and f1f2_to_s 
  double l_partsum_0_F8; long long l_partsum_0_I8, l_partsum_1_I8;
  double     partsum_0_F8[MAX_STATEMENTS_IN_COMP_EXPR];
  long long  partsum_0_I8[MAX_STATEMENTS_IN_COMP_EXPR];
  long long  partsum_1_I8[MAX_STATEMENTS_IN_COMP_EXPR];
  long long  partsum_2_I8[MAX_STATEMENTS_IN_COMP_EXPR];
  bool  is_numer_set[MAX_STATEMENTS_IN_COMP_EXPR];
#define BUFLEN 32
#define RSLT_BUF_SIZE 1024
  char op[BUFLEN]; bool is_null;
  char rslt_buf[RSLT_BUF_SIZE];
  char buf0[RSLT_BUF_SIZE];
  char buf1[RSLT_BUF_SIZE];
  char buf2[RSLT_BUF_SIZE];
  zero_string(rslt_buf, RSLT_BUF_SIZE);
  zero_string(buf0, RSLT_BUF_SIZE);
  zero_string(buf1, RSLT_BUF_SIZE);
  zero_string(buf2, RSLT_BUF_SIZE);
  long long time_per_expr[MAX_STATEMENTS_IN_COMP_EXPR];
  long long t_before, t_after;

  for ( int i = 0; i < MAX_STATEMENTS_IN_COMP_EXPR; i++ ) { 
    time_per_expr[i] = 0;
  }

  if ( n_comp_expr <= 1 ) { go_BYE(-1); }
  for ( int i = 0; i < MAX_STATEMENTS_IN_COMP_EXPR; i++ ) { 
    // numerator assigned inside loop
    partsum_0_I8[i] = 0;
    partsum_0_F8[i] = 0;
    partsum_1_I8[i] = 0;
    partsum_2_I8[i] = 0;
    is_numer_set[i] = false;
  }

  // determine number of blocks and block size. Last block may be bigger.
  int block_size = COMP_EXPR_BLOCK_SIZE;
  // block_size = 5; // TODO P0 Just for debugging
  if ( n_comp_expr <= 1 ) { go_BYE(-1); }
  long long nR = tbl_rec.nR;
  int num_blocks = nR / block_size;
  if ( num_blocks == 0 ) { num_blocks = 1; }
  for ( int b = 0; b < num_blocks; b++ ) {
    long long lb = block_size * b;
    long long ub = lb + block_size;
    if ( b == ( num_blocks-1) ) { ub = nR; }
    long long eff_nR = ub - lb;
    for ( int i = 0; i < n_comp_expr; i++ ) { 
      t_before = get_time_usec();
      long long *cntI8 = NULL; long long dst_nR; 
      char *f1_X = NULL, *nn_f1_X = NULL; long long f1nR = 0; FLD_TYPE f1type;
      char *f2_X = NULL, *nn_f2_X = NULL; long long f2nR = 0; FLD_TYPE f2type;
      char *f3_X = NULL, *nn_f3_X = NULL; long long f3nR = 0; FLD_TYPE f3type;
      char *f4_X = NULL, *nn_f4_X = NULL; long long f4nR = 0; FLD_TYPE f4type;
      int *src_fld = NULL;
      zero_string(op, BUFLEN);
      char *str_op_spec = comp_expr[i].op_spec;
      char *str_scalar  = comp_expr[i].scalar;
      // NOTE ASSUMPTION THAT nn is stored as I1 and not as B
      status = offset_pointers(tbl_id, comp_expr, i, 0, lb, &f1nR, &f1type, 
	  &f1_X, &nn_f1_X); 
      cBYE(status);
      status = offset_pointers(tbl_id, comp_expr, i, 1, lb, &f2nR, &f2type, 
	  &f2_X, &nn_f2_X); 
      cBYE(status);
      status = offset_pointers(tbl_id, comp_expr, i, 2, lb, &f3nR, &f3type, 
	  &f3_X, &nn_f3_X); 
      cBYE(status);
      status = offset_pointers(tbl_id, comp_expr, i, 3, lb, &f4nR, &f4type, 
	  &f4_X, &nn_f4_X); 
      cBYE(status);

      /* START: Some additional checking on pointers */
      switch ( comp_expr[i].op ) { 
      case op_count    : 
      case op_countf   : 
      case op_f1f2opf3 : 
      case op_f1opf2f3 : 
	break; 
      default : 
	/* there is no f3 field for these operations */
	if ( f3_X    != NULL ) { go_BYE(-1); }
	if ( nn_f3_X != NULL ) { go_BYE(-1); }
	if ( f3type != undef_fldtype ) { go_BYE(-1); }
	break;
      }

      switch ( comp_expr[i].op ) { 
      case op_countf : 
	break; 
      default : 
	/* there is no f4 field for these operations */
	if ( f4_X    != NULL ) { go_BYE(-1); }
	if ( nn_f4_X != NULL ) { go_BYE(-1); }
	if ( f4type != undef_fldtype ) { go_BYE(-1); }
	break;
      }
      /* STOP: Some additional checking on pointers */


      int nT = 1;
      switch ( comp_expr[i].op ) { 
      case op_f1s1opf2 : 
	status = vec_f1s1opf2(eff_nR, f1type, f1_X, nn_f1_X, 
			      str_scalar, str_op_spec, f2_X, nn_f2_X, f2type);
	break;
      case op_f1opf2f3 : 
	go_BYE(-1); // TODO: P2 To be implemented
	break;
      case op_f1f2opf3 : 
	status = vec_f1f2opf3(eff_nR, f1type, f2type, f1_X, nn_f1_X, 
			      f2_X, nn_f2_X, str_op_spec, f3type, f3_X, nn_f3_X);
	break;
      case op_f1opf2 : 
	status = extract_S(str_op_spec, "op=[", "]", op, BUFLEN, &is_null);
	cBYE(status);
	if ( is_null ) { go_BYE(-1); }
	status = vec_f1opf2(eff_nR, f1type, f1_X, nn_f1_X, op,
			    f2_X, nn_f2_X, f2type);

	break;
      case op_countf : 
	// fld1 is src fld 
	// fld2 is fk to outtbl
	// fld3 is cfld
	// fld4 is cnt in outtbl
	src_fld = (int *)f1_X;
	cntI8 = (long long *)f4_X;
	dst_nR = f4nR;
	if ( b == 0 ) { 
	  assign_const_I8(cntI8, dst_nR, 0);
	}
	if ( f3_X != NULL ) {
	  switch ( f2type ) { 
	  case I1 : 
            status = compute_nT_for_count(eff_nR, dst_nR, &nT); cBYE(status);
	    if ( nT == 1 ) { 
	      status = countf_nn_I1(src_fld, (char *)f1_X, eff_nR, 
		  f3_X, cntI8, dst_nR, false);  cBYE(status);
	    }
	    else {
	      status = par_countf_cfld_I1(src_fld, (char *)f2_X, eff_nR, 
		  f3_X, cntI8, dst_nR, nT);  cBYE(status);
	    }
	    break;
	  case I2 : 
	    if ( nT == 1 ) { 
	      status = countf_nn_I2(src_fld, (short *)f2_X, eff_nR, 
		  f3_X, cntI8, dst_nR, false); cBYE(status);
	    }
	    else {
	      status = par_countf_cfld_I2(src_fld, (short *)f2_X, eff_nR, 
		  f3_X, cntI8, dst_nR, nT);  cBYE(status);
	    }
	    break;
	  case I4 : 
	    status = countf_nn_I4(src_fld, (int *)f2_X,  eff_nR, f3_X, 
		cntI8, dst_nR, false); 
	    break;
	  case I8 : 
	    status = countf_nn_I8(src_fld, (long long *)f2_X, eff_nR, 
		f3_X, cntI8, dst_nR, false); 
	    break;
	  default : 
	    go_BYE(-1);
	    break;
	  }
	}
	else {
	  switch ( f2type ) { 
	  case I1 : 
	    if ( nT == 1 ) { 
	      status = countf_I1(src_fld, (char *)f2_X, eff_nR, 
		  cntI8, dst_nR, false); 
	    }
	    else {
	      status = par_countf_I1(src_fld, (char *)f2_X, eff_nR, 
		  cntI8, dst_nR, nT);  cBYE(status);
	    }
	    break;
	  case I2 : 
	    status = countf_I2(src_fld, (short *)f2_X, eff_nR, cntI8, 
		dst_nR, false); 
	    break;
	  case I4 : 
	    status = countf_I4(src_fld, (int *)f2_X, eff_nR, cntI8, 
		dst_nR, false); 
	    break;
	  case I8 : 
	    status = countf_I8(src_fld, (long long *)f2_X, eff_nR, 
		cntI8, dst_nR, false); 
	    break;
	  default : 
	    go_BYE(-1);
	    break;
	  }
	}
	cBYE(status);
	break;
      case op_count : 
	// fld1 is fk to outtbl
	// fld2 is cfld
	// fld3 is cnt in outtbl
	cntI8 = (long long *)f3_X;
	dst_nR = f3nR;
	if ( b == 0 ) { 
	  assign_const_I8(cntI8, dst_nR, 0);
	}
	if ( f2_X != NULL ) {
	  switch ( f1type ) { 
	  case I1 : 
            status = compute_nT_for_count(eff_nR, dst_nR, &nT); cBYE(status);
	    if ( nT == 1 ) { 
	      status = count_nn_I1((char *)f1_X, eff_nR, f2_X, cntI8, 
				   dst_nR, false);  cBYE(status);
	    }
	    else {
	      status = par_count_cfld_I1((char *)f1_X, eff_nR, f2_X, cntI8, 
					 dst_nR, nT);  cBYE(status);
	    }
	    break;
	  case I2 : 
	    if ( nT == 1 ) { 
	      status = count_nn_I2((short *)f1_X, eff_nR, f2_X, cntI8, 
				   dst_nR, false); cBYE(status);
	    }
	    else {
	      status = par_count_cfld_I2((short *)f1_X, eff_nR, f2_X, cntI8, 
					 dst_nR, nT);  cBYE(status);
	    }
	    break;
	  case I4 : 
	    status = count_nn_I4((int *)f1_X,       eff_nR, f2_X, cntI8, dst_nR, false); 
	    break;
	  case I8 : 
	    status = count_nn_I8((long long *)f1_X, eff_nR, f2_X, cntI8, dst_nR, false); 
	    break;
	  default : 
	    go_BYE(-1);
	    break;
	  }
	}
	else {
	  switch ( f1type ) { 
	  case I1 : 
	    if ( nT == 1 ) { 
	      status = count_I1((char *)f1_X, eff_nR, cntI8, dst_nR, false); 
	    }
	    else {
	      status = par_count_I1((char *)f1_X, eff_nR, cntI8, 
				    dst_nR, nT);  cBYE(status);
	    }
	    break;
	  case I2 : 
	    status = count_I2((short *)f1_X, eff_nR, cntI8, dst_nR, false); 
	    break;
	  case I4 : 
	    status = count_I4((int *)f1_X, eff_nR, cntI8, dst_nR, false); 
	    break;
	  case I8 : 
	    status = count_I8((long long *)f1_X, eff_nR, cntI8, dst_nR, false); 
	    break;
	  default : 
	    go_BYE(-1);
	    break;
	  }
	}
	cBYE(status);
	break;
      case op_f_to_s : 
	status = vec_f_to_s(f1_X, f1type, nn_f1_X, eff_nR, 
			    str_op_spec, rslt_buf, RSLT_BUF_SIZE);
	// Combine buffers across invocations
	status = read_nth_val(rslt_buf, ":", 0, buf0, RSLT_BUF_SIZE);
	cBYE(status);
	status = read_nth_val(rslt_buf, ":", 1, buf1, RSLT_BUF_SIZE);
	cBYE(status);
	status = stoI8(buf1, &l_partsum_1_I8); cBYE(status); 
	if ( l_partsum_1_I8 < 0 ) { go_BYE(-1); }
	if ( l_partsum_1_I8 > 0 ) {
	  partsum_1_I8[i] += l_partsum_1_I8;

	  switch ( f1type ) {
	  case I1 : case I2 : case I4 : case I8 : 
	    status = stoI8(buf0, &l_partsum_0_I8); cBYE(status); 
	    if ( is_numer_set[i] == false ) {
	      is_numer_set[i] = true; 
	      partsum_0_I8[i] = l_partsum_0_I8;
	    }
	    else {
	      if ( strcmp(str_op_spec, "min") == 0 ) { 
		partsum_0_I8[i] = min(partsum_0_I8[i], l_partsum_0_I8);
	      }
	      else if ( strcmp(str_op_spec, "max") == 0 ) { 
		partsum_0_I8[i] = max(partsum_0_I8[i], l_partsum_0_I8);
	      }
	      else if ( strcmp(str_op_spec, "sum") == 0 ) { 
		partsum_0_I8[i] += l_partsum_0_I8;
	      }
	      else {
		go_BYE(-1);
	      }
	    }
	    break;
	  case F4 : case F8 : 
	    status = stoF8(buf0, &l_partsum_0_F8); cBYE(status); 
	    if ( is_numer_set[i] == false ) {
	      is_numer_set[i] = true; 
	      partsum_0_F8[i] = l_partsum_0_F8;
	    }
	    else {
	      if ( strcmp(str_op_spec, "min") == 0 ) { 
		partsum_0_F8[i] = min(partsum_0_F8[i], l_partsum_0_F8);
	      }
	      else if ( strcmp(str_op_spec, "max") == 0 ) { 
		partsum_0_F8[i] = max(partsum_0_F8[i], l_partsum_0_F8);
	      }
	      else if ( strcmp(str_op_spec, "sum") == 0 ) { 
		partsum_0_F8[i] += l_partsum_0_F8;
	      }
	      else {
		go_BYE(-1);
	      }
	    }
	    break;
	  default : 
	    go_BYE(-1);
	    break;
	  }
	}
	break;
      case op_f1f2_to_s : 
	status = core_f1f2_to_s(str_op_spec, f1_X, f1type, f2_X, f2type, 
	    eff_nR, rslt_buf, RSLT_BUF_SIZE);
	cBYE(status);
	// Combine buffers across invocations
	status = read_nth_val(rslt_buf, ":", 0, buf0, RSLT_BUF_SIZE);
	cBYE(status);
	status = read_nth_val(rslt_buf, ":", 1, buf1, RSLT_BUF_SIZE);
	cBYE(status);
	status = read_nth_val(rslt_buf, ":", 2, buf2, RSLT_BUF_SIZE);
	cBYE(status);
	// Note that processing for f1f2_to_s is much simpler than
	// f_to_s because we support much fewer options
	//------------------------------
	long long tempI8;
	status = stoI8(buf0, &tempI8); cBYE(status);
	partsum_0_I8[i] += tempI8; 
	//------------------------------
	status = stoI8(buf1, &tempI8); cBYE(status);
	if ( tempI8 < 0 ) { go_BYE(-1); }
	partsum_1_I8[i] += tempI8; 
	//------------------------------
	status = stoI8(buf2, &tempI8); cBYE(status);
	if ( tempI8 <= 0 ) { go_BYE(-1); }
	partsum_2_I8[i] += tempI8; 
	//------------------------------
      break;
      default : 
	go_BYE(-1);
	break;
      }
      t_after = get_time_usec();
      time_per_expr[i] += (t_after - t_before);
    }
  }
  // Print out scalar variables if any
  for ( int i = 0; i < n_comp_expr; i++ ) { 
    // DBG fprintf(stderr, "time[%d] = %lld \n", i, time_per_expr[i]);
    switch ( comp_expr[i].op ) { 
    case op_f_to_s  : 
      if ( is_numer_set[i] == true ) {
        FLD_TYPE f1type = comp_expr[i].fld[0].fldtype;
	switch ( f1type ) { 
	case I1 : case I2 : case I4 : case I8 : 
	  fprintf(stdout, "%s=%lld:%lld\n", 
	      comp_expr[i].env_var, partsum_0_I8[i], partsum_1_I8[i]);
	  break;
	case F4 : case F8 : 
	  fprintf(stdout, "%s=%lf:%lld\n", 
	      comp_expr[i].env_var, partsum_0_F8[i], partsum_1_I8[i]);
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
      else {
	fprintf(stdout, "%s=0:0\n", comp_expr[i].env_var);
      }
      break;
    case op_f1f2_to_s  : 
      fprintf(stdout, "%s=%lld:%lld:%lld\n", comp_expr[i].env_var, 
	    partsum_0_I8[i], partsum_1_I8[i], partsum_2_I8[i]);
    break;
    default : 
      // nothing to do 
      break;
    }
  }
 BYE:
  return status ;
}
