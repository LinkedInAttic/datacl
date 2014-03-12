#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "drop_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "del_fld.h"
#include "drop_aux_fld.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "meta_globals.h"

// START FUNC DECL
int
add_aux_fld(
	char *tbl,
	int in_tbl_id,
	char *fld,
	int in_fld_id,
	int ddir_id,
	int fileno,
	AUX_TYPE auxtype,
	int *ptr_aux_fld_id,
	FLD_REC_TYPE *ptr_aux_fld_rec
	)
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  int aux_fld_id; size_t filesz = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id = INT_MIN;
  FLD_REC_TYPE fld_rec; int fld_id = INT_MIN;
  FLD_TYPE fldtype;

  if ( ( auxtype != nn ) && ( auxtype != len ) && ( auxtype != off ) ) { 
    go_BYE(-1);
  }
  switch ( auxtype ) { 
    case nn : 
    fldtype = ptr_aux_fld_rec->fldtype;
    if ( ( ptr_aux_fld_rec->fldtype != I1 ) && 
	( ptr_aux_fld_rec->fldtype != B ) ) {
      go_BYE(-1);
    }
    break;
    case len : fldtype = I2;  break;
    case off : fldtype = I8;  break;
    default : go_BYE(-1); break;
  }
  zero_fld_rec(&fld_rec);
  //------------------------------------------------
  if ( ( ( tbl == NULL ) || ( *tbl == '\0' ) ) && (in_tbl_id < 0 ) ) {
    go_BYE(-1);
  }
  if ( ( ( fld == NULL ) || ( *fld == '\0' ) ) && (in_fld_id < 0 ) ) {
    go_BYE(-1);
  }
  if ( fileno < 0  ) { go_BYE(-1); }
  char filename[32]; zero_string(filename, 32); 
  sprintf(filename, "%d", fileno);
  //--------------------------------------------

  char aux_fld[MAX_LEN_FLD_NAME+1]; 
  zero_string(aux_fld, MAX_LEN_FLD_NAME+1);
  char str_auxtype[16]; 
  status = mk_str_auxtype(auxtype, str_auxtype); cBYE(status);
  if ( ( in_tbl_id >= 0 ) && ( in_fld_id >= 0 ) ) { 
    status = drop_aux_fld(NULL, NULL, str_auxtype, in_fld_id); cBYE(status);
    tbl_id = in_tbl_id;
    fld_id = in_fld_id;
  }
  else if ( ( in_tbl_id < 0 ) || ( in_fld_id < 0 ) ) { 
    FLD_REC_TYPE nn_fld_rec; int nn_fld_id = INT_MIN;
    status = drop_aux_fld(tbl, fld, str_auxtype, -1); cBYE(status);
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
    if ( tbl_id < 0 ) { go_BYE(-1); }
    status = is_fld(tbl, -1, fld, &fld_id, &fld_rec, 
      &nn_fld_id, &nn_fld_rec); cBYE(status);
    if ( fld_id < 0 ) { go_BYE(-1); }
  }
  else { go_BYE(-1); }

  status = q_mmap(ddir_id, fileno, &X, &nX, false); cBYE(status);
  filesz = nX;
  status = mk_name_aux_fld(g_flds[fld_id].name, auxtype, aux_fld); cBYE(status);
  status = get_empty_fld(aux_fld, g_flds, g_n_fld, &aux_fld_id); cBYE(status);
  zero_fld_rec(&(g_flds[aux_fld_id]));

  switch ( auxtype ) { 
    case  nn : 
      g_flds[fld_id].nn_fld_id  = aux_fld_id;  
      break;
    case len : 
      status = add_to_fld_info(fld_id, fk_fld_len, aux_fld_id); cBYE(status);
      break;
    case off : 
      status = add_to_fld_info(fld_id, fk_fld_off, aux_fld_id); cBYE(status);
      break;
    default : go_BYE(-1); break;
  }
  /* Point from auxiliary to primary */
  g_flds[aux_fld_id].parent_id  = fld_id;
  g_flds[aux_fld_id].tbl_id     = tbl_id;
  /*  TODO: P1 I had thought that externality of aux field same 
    as that of parent field. Now, I think not. Look into this */
  g_flds[aux_fld_id].is_external = false; 
  g_flds[aux_fld_id].auxtype    = auxtype;
  g_flds[aux_fld_id].fldtype    = fldtype;
  strcpy(g_flds[aux_fld_id].name, aux_fld);
  g_flds[aux_fld_id].fileno     =  fileno;
  g_flds[aux_fld_id].ddir_id    = ddir_id;
  g_flds[aux_fld_id].filesz     = filesz;

  *ptr_aux_fld_id  = aux_fld_id;
  *ptr_aux_fld_rec = g_flds[aux_fld_id];
 BYE:
  rs_munmap(X, nX);
  return status ;
}
