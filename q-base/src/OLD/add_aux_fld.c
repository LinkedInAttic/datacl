#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "dbauxil.h"
#include "del_fld.h"
#include "add_fld.h"
#include "aux_fld_meta.h"
#include "open_temp_file.h"
#include "mk_file.h"
#include "meta_globals.h"

// START FUNC DECL
int
add_aux_fld(
	char *tbl,
	char *fld,
	char *filename,
	char *str_auxtype, /* "nn" or "sz" */
	int *ptr_aux_fld_id
	)
// STOP FUNC DECL
{
  int status = 0;
  int auxtype;
  int tbl_id, fld_id, b_fld_id, aux_fld_id;
  char *aux_fld = NULL;
  char str_fldtype[16]; int n_sizeof;
  char *cptr = NULL;
  char *temp_file_name = NULL; FILE *ofp = NULL;
  long long nR;
  char *X = NULL; size_t nX = 0;
  char *Y = NULL; size_t nY = 0;

  //------------------------------------------------
  status = mk_int_auxtype(str_auxtype, &auxtype); cBYE(status);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( filename == NULL ) || ( *filename == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id);
  chk_range(fld_id, 0, g_n_fld);
  nR = g_tbl[tbl_id].nR;
  //--------------------------------------------
  /* Check if filename is really filename or name of boolean field */
  if ( ( auxtype == AUX_FLD_NN ) && ( strncmp(filename, "fld:", 4) == 0) ) { 
    cptr = filename + 4; /* now we point to the field */
    status = is_fld(NULL, tbl_id, cptr, &b_fld_id);
    chk_range(b_fld_id, 0, g_n_fld);
    status = rs_mmap(g_fld[b_fld_id].filename, &X, &nX, 0); cBYE(status);
    status = open_temp_file(&ofp, &temp_file_name, nX); cBYE(status);
    fclose_if_non_null(ofp);
    status = mk_file(temp_file_name, nX); cBYE(status);
    status = rs_mmap(temp_file_name, &Y, &nY, 1);
    memcpy(Y, X, nX);
    rs_munmap(X, nX);
    rs_munmap(Y, nY);
    filename = temp_file_name;
  }
  //--------------------------------------------
  switch ( auxtype ) { 
    case AUX_FLD_NN : 
    aux_fld_id = g_fld[fld_id].nn_fld_id;
    n_sizeof = sizeof(char);
    strcpy(str_fldtype, "bool");
    break;
    case AUX_FLD_SZ : 
    aux_fld_id = g_fld[fld_id].sz_fld_id;
    n_sizeof = sizeof(int);
    strcpy(str_fldtype, "int");
    break;
    default:
    go_BYE(-1);
    break;
  }
  status = mk_name_aux_fld(fld, str_auxtype, &aux_fld); cBYE(status);
  if ( aux_fld_id > 0 ) { 
    status = del_aux_fld(fld_id, str_auxtype); cBYE(status);
  }
  else {
    status = get_empty_fld(&aux_fld_id); cBYE(status);
  }

  /* Point from primary to auxiliary */
  switch ( auxtype ) {
    case AUX_FLD_NN  : g_fld[fld_id].nn_fld_id = aux_fld_id; break;
    case AUX_FLD_SZ  : g_fld[fld_id].sz_fld_id = aux_fld_id; break;
    default : go_BYE(-1); break;
  }
  /* Point from auxiliary to primary */
  g_fld[aux_fld_id].parent_id  = fld_id;

  g_fld[aux_fld_id].tbl_id     = tbl_id;
  /* externality of aux field same as that of parent field */
  g_fld[aux_fld_id].is_external = g_fld[fld_id].is_external;
  g_fld[aux_fld_id].n_sizeof   = n_sizeof;
  g_fld[aux_fld_id].auxtype    = auxtype;
  strcpy(g_fld[aux_fld_id].fldtype, str_fldtype); 
  strcpy(g_fld[aux_fld_id].name, aux_fld);
  strcpy(g_fld[aux_fld_id].filename, filename);

  *ptr_aux_fld_id = aux_fld_id;
 BYE:
  free_if_non_null(temp_file_name);
  free_if_non_null(aux_fld);
  return(status);
}
