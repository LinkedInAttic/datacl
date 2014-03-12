#include "qtypes.h"
#include "mmap.h"
#include "is_tbl.h"
#include "meta_data.h"
#include "lock_stuff.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "is_fld.h"
#include "fld_meta.h"

extern char *g_docroot; 
// START FUNC DECL
int
fld_meta(
	 char *tbl,
	 char *fld
	 )
// STOP FUNC DECL
{
  int status = 0;
  int fld_id = -1;    FLD_REC_TYPE fld_rec;
  int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;
  char str_fldtype[32];

  //------------------------------------------------
  zero_string(str_fldtype, 32);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  get_rd_lock();
  status = is_fld(tbl, -1, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  fprintf(stdout, "%d,%s,\"", fld_id, fld_rec.name);
  pr_disp_name(stdout, fld_rec.filename);
  fprintf(stdout, "\",\"");
  pr_disp_name(stdout, fld_rec.dispname);
  status = mk_str_fldtype(fld_rec.fldtype, str_fldtype);
  fprintf(stdout, "\",%s,", str_fldtype);
  if ( fld_rec.parent_id < 0 ) { 
    fprintf(stdout, "\"\",");
  }
  else {
    fprintf(stdout, "%d,", fld_rec.parent_id);
  }
  if ( fld_rec.nn_fld_id < 0 ) { 
    fprintf(stdout, "\"\",");
  }
  else {
    fprintf(stdout, "%d,", fld_rec.nn_fld_id);
  }
  fprintf(stdout, "\n");

  //------------------------------------------------
 BYE:
  return(status);
}
