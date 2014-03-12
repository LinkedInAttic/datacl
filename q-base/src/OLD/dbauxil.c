#include "qtypes.h"
#include "mmap.h"
#include "aux_meta.h"

//-------------------------------------------------------------
// START FUNC DECL
int
mk_offset(
	  int *szptr,
	  long long nR,
	  long long **ptr_offset
	  )
// STOP FUNC DECL
{
  int status = 0;
  long long *offset = NULL;
  offset = (long long *)malloc(nR * sizeof(long long));
  return_if_malloc_failed(offset);
  offset[0] = 0;
  for ( long long i = 1; i < nR; i++ ) { 
    offset[i] = offset[i-1] + szptr[i-1];
  }
  *ptr_offset = offset;
 BYE:
  return(status);
}

//--------------------------------------------------------
// START FUNC DECL
int
mk_mjoin_op(
	    char *str_mjoin_op,
	    int *ptr_mjoin_op
	    )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_mjoin_op == NULL ) { go_BYE(-1); }
  if ( strcmp(str_mjoin_op, "sum") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_SUM;
  }
  else if ( strcmp(str_mjoin_op, "min") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_MIN;
  }
  else if ( strcmp(str_mjoin_op, "max") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_MAX;
  }
  else if ( strcmp(str_mjoin_op, "reg") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_REG;
  }
  else if ( strcmp(str_mjoin_op, "or") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_OR;
  }
  else if ( strcmp(str_mjoin_op, "and") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_AND;
  }
  else if ( strcmp(str_mjoin_op, "cnt") == 0 ) {
    *ptr_mjoin_op = MJOIN_OP_CNT;
  }
  else { *ptr_mjoin_op = -1; go_BYE(-1); }
 BYE:
  return(status);
}
//--------------------------------------------------------
// START FUNC DECL
int
mk_iop(
       char *str_op,
       int *ptr_iop
       )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_op == NULL ) { go_BYE(-1); }
  if ( strcmp(str_op, "+") == 0 ) { *ptr_iop = IOP_ADD; }
  else if ( strcmp(str_op, "-") == 0 ) { *ptr_iop = IOP_SUB; }
  else if ( strcmp(str_op, "*") == 0 ) { *ptr_iop = IOP_MUL; }
  else if ( strcmp(str_op, "/") == 0 ) { *ptr_iop = IOP_DIV; }
  else if ( strcmp(str_op, ">") == 0 ) { *ptr_iop = IOP_GT; }
  else if ( strcmp(str_op, "<") == 0 ) { *ptr_iop = IOP_LT; }
  else if ( strcmp(str_op, ">=") == 0 ) { *ptr_iop = IOP_GEQ; }
  else if ( strcmp(str_op, "<=") == 0 ) { *ptr_iop = IOP_LEQ; }
  else if ( strcmp(str_op, "!=") == 0 ) { *ptr_iop = IOP_NEQ; }
  else if ( strcmp(str_op, "==") == 0 ) { *ptr_iop = IOP_EQ; }
  else if ( strcmp(str_op, "&&") == 0 ) { *ptr_iop = BOP_AND; }
  else if ( strcmp(str_op, "||") == 0 ) { *ptr_iop = BOP_OR; }
  else { *ptr_iop = -1; go_BYE(-1); }

 BYE:
  return(status);
}
//--------------------------------------------------------
// START FUNC DECL
void
pr_disp_name(
	     FILE *ofp,
	     char *disp_name
	     )
// STOP FUNC DECL
{
  if ( ( disp_name == NULL ) || ( *disp_name == '\0' ) ) { 
    return;
  }
  for ( int j = 0; disp_name[j] != '\0'; j++ ) {
    if ( ( disp_name[j] == '\\' ) || 
	 ( disp_name[j] == '"' ) ) {
      fprintf(stdout, "\\"); 
    }
    fprintf(stdout, "%c", disp_name[j]);
  }
}
// START FUNC DECL
int
mk_int_auxtype(
	       char *str_auxtype, 
	       int *ptr_auxtype
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( ( str_auxtype == NULL ) || ( *str_auxtype == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(str_auxtype, "nn") == 0 ) { 
    *ptr_auxtype = AUX_FLD_NN;
  }
  else if ( strcmp(str_auxtype, "sz") == 0 ) { 
    *ptr_auxtype = AUX_FLD_SZ;
  }
  else { go_BYE(-1); }
 BYE:
  return(status);
}
#undef XXX
#ifdef XXX
// I do not think these are needed any more
// START FUNC DECL
int
del_ht_tbl_entry(
		 int tbl_id
		 )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(tbl_id, 0, g_n_tbl);
  for ( int i = 0; i < g_n_ht_tbl; i++ ) { 
    if ( g_ht_tbl[i].val == tbl_id ) { 
      g_ht_tbl[i].val = -1;
      g_ht_tbl[i].key = 0;
      break;
    }
  }
 BYE:
  return(status);
}
// START FUNC DECL
int
del_ht_fld_entry(
		 int fld_id
		 )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(fld_id, 0, g_n_fld);
  for ( int i = 0; i < g_n_ht_fld; i++ ) { 
    if ( g_ht_fld[i].val == fld_id ) { 
      g_ht_fld[i].val = -1;
      g_ht_fld[i].key = 0;
      break;
    }
  }
 BYE:
  return(status);
}
#endif
// START FUNC DECL
#ifdef XXX
TODO
int
del_aux_fld(
	    int primary_fld_id,
	    char *auxtype
	    )
// STOP FUNC DECL
{
  int status = 0;
  int aux_fld_id = INT_MIN;

  chk_range(primary_fld_id, 0, g_n_fld);
  if ( strcmp(auxtype, "nn") == 0 ) { 
    aux_fld_id = g_fld[primary_fld_id].nn_fld_id;
    g_fld[primary_fld_id].nn_fld_id = -1;
  }
  else if ( strcmp(auxtype, "sz") == 0 ) { 
    aux_fld_id = g_fld[primary_fld_id].sz_fld_id;
    g_fld[primary_fld_id].sz_fld_id = -1;
  }
  if ( aux_fld_id >= 0 ) { 
    if ( g_fld[primary_fld_id].is_external == false ) { 
      unlink(g_fld[aux_fld_id].filename);
    }
    zero_fld_meta(&(g_fld[aux_fld_id]));
  }
 BYE:
  return(status);
}
#endif
// START FUNC DECL
int
mk_name_aux_fld(
		char *fld,
		char *auxtype,
		char **ptr_aux_fld
		)
// STOP FUNC DECL
{
  int status = 0;
  int len; char *aux_fld;
  if ( fld == NULL ) { go_BYE(-1); }
  len = strlen(fld) + strlen(auxtype) + 2 + 1;
  aux_fld = malloc(len * sizeof(char));
  return_if_malloc_failed(aux_fld);
  zero_string(aux_fld, len);
  strcpy(aux_fld, "_");
  strcat(aux_fld, auxtype);
  strcat(aux_fld, "_");
  strcat(aux_fld, fld);
  *ptr_aux_fld = aux_fld;
  status = chk_fld_name(aux_fld, 1); cBYE(status);
 BYE:
  return(status);
}

extern bool g_write_to_temp_dir;
// START FUNC DECL
  int
    chk_if_ephemeral(
	char **ptr_fld
	)
// STOP FUNC DECL
    {
      int status = 0;
      char *fld = *ptr_fld;
      status = chk_fld_name(fld, 0); cBYE(status);
  if ( strncmp(fld, "ephemeral:", 10) == 0 ) {
    g_write_to_temp_dir = true;
    fld += 10;
  }
  *ptr_fld = fld;
BYE:
  return(status);
    }

// START FUNC DECL
int
chk_file_size(
    char *filename,
    long long nR,
    FLD_TYPE fldtype
    )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  int fldsz;
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd = false; 

  mcr_cd;
  status = rs_mmap(filename, &X, &nX, 0); cBYE(status);
  mcr_uncd;
    
  status = get_fld_sz(fldtype, &fldsz);
  if ( ( fldsz * nR ) != nX ) { go_BYE(-1); }
BYE:
  rs_munmap(X, nX);
  return(status);
}
