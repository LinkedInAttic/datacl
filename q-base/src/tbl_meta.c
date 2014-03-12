#include "qtypes.h"
#include "auxil.h"
#include "is_tbl.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "tbl_meta.h"

extern FILE *g_ofp;

// START FUNC DECL
int
tbl_meta(
	 char *tbl,
	 bool include_aux,
	 int *fld_ids,
	 int *ptr_num_flds
	 )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = -1; TBL_REC_TYPE tbl_rec;
  int num_flds = 0;

  //------------------------------------------------
  char str_fldtype[32]; zero_string(str_fldtype, 32);
  char str_auxtype[32]; zero_string(str_auxtype, 32);
  char str_exttype[32]; zero_string(str_exttype, 32);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  //------------------------------------------------
  bool is_hdr_pr = false;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].tbl_id == tbl_id ) { 
      /* This field belongs to input table */
      if ( fld_ids == NULL ) {  /* print to g_ofp */
	if ( is_hdr_pr == false ) { 
          fprintf(stderr,"fld_id,fileno,name,type,auxtype,is_external\n");
	  is_hdr_pr = true;
	}
	if ( g_flds[i].auxtype != undef_auxtype ) { continue; } // skip aux flds
        status = mk_str_fldtype(g_flds[i].fldtype, str_fldtype);
        status = mk_str_auxtype(g_flds[i].auxtype, str_auxtype);
        status = mk_str_exttype(g_flds[i].is_external, str_exttype);
        fprintf(g_ofp, "%d,%d,%s,%s,%s,%s\n", i, 
	    g_flds[i].fileno, g_flds[i].name,
	    str_fldtype, str_auxtype, str_exttype);
      }
      else {
	if ( g_flds[i].auxtype == undef_auxtype ) {
          fld_ids[num_flds++] = i;
	}
	else {
	  if ( include_aux ) {
            fld_ids[num_flds++] = i;
	  }
	}
      }
    }
  }
  if ( ptr_num_flds != NULL ) { *ptr_num_flds = num_flds; }

  //------------------------------------------------
 BYE:
  return(status);
}
// START FUNC DECL
int
get_tbl_meta(
	     int tbl_id,
	     TBL_REC_TYPE *ptr_tbl_meta
	     )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(tbl_id, 0, g_n_tbl);
  copy_tbl_meta(ptr_tbl_meta, g_tbls[tbl_id]); 
 BYE:
  return(status);
}

