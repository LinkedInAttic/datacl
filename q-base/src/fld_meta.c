#include "qtypes.h"
#include "auxil.h"
#include "is_tbl.h"
#include "meta_globals.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "is_fld.h"
#include "fld_meta.h"

extern FILE *g_ofp;

// START FUNC DECL
int
pr_min_max_sum(
	       char *mode,
	       FLD_REC_TYPE fld_rec
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( strcmp(mode, "min") == 0 ) {
    if ( fld_rec.is_min_nn == true ) { 
      switch ( fld_rec.fldtype ) { 
      case I1 : case I2 : case I4 : case I8 : 
	fprintf(g_ofp, "%lld", fld_rec.minval.minI8);
	break;
      case F4 : case F8 : 
	fprintf(g_ofp, "%lf", fld_rec.minval.minF8);
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    else { 
      fprintf(g_ofp, "undef");
    }
  }
  else if ( strcmp(mode, "max") == 0 ) {
    if ( fld_rec.is_max_nn == true ) { 
      switch ( fld_rec.fldtype ) { 
      case I1 : case I2 : case I4 : case I8 : 
	fprintf(g_ofp, "%lld", fld_rec.maxval.maxI8);
	break;
      case F4 : case F8 : 
	fprintf(g_ofp, "%lf", fld_rec.maxval.maxF8);
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    else { 
      fprintf(g_ofp, "undef");
    }
  }
  else if ( strcmp(mode, "sum") == 0 ) {
    if ( fld_rec.is_sum_nn == true ) { 
      switch ( fld_rec.fldtype ) { 
      case I1 : case I2 : case I4 : case I8 : 
	fprintf(g_ofp, "%lld", fld_rec.sumval.sumI8);
	break;
      case F4 : case F8 : 
	fprintf(g_ofp, "%lf", fld_rec.sumval.sumF8);
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    else { 
      fprintf(g_ofp, "undef");
    }
  }
  else {
    go_BYE(-1);
  }

 BYE:
  return status ;
}
//
// START FUNC DECL
int
fld_meta(
	 const char *tbl,
	 const char *fld,
	 const char *what_to_pr,
	 int *ptr_fld_id,
	 bool is_print
	 )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = -1;    TBL_REC_TYPE tbl_rec;
  int fld_id = -1;    FLD_REC_TYPE fld_rec;
  int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;
  char str_fldtype[32]; zero_string(str_fldtype, 32);
  char str_auxtype[32]; zero_string(str_auxtype, 32);
  char str_srttype[32]; zero_string(str_srttype, 32);
  bool all_digits = true;

  //------------------------------------------------
  *ptr_fld_id = -1;
  zero_string(str_fldtype, 32);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }

  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) {
    return status ;
  }
  for ( char *cptr = (char *)fld; *cptr != '\0' ; cptr++ ) { 
    if ( !isdigit(*cptr) ) { all_digits = false; }
  }
  if ( all_digits ) {
    status = stoI4(fld, &fld_id); cBYE(status);
    chk_range(fld_id, 0, g_n_fld);
    if ( g_flds[fld_id].name[0] == '\0' ) {
      return status ;
    }
    fld_rec = g_flds[fld_id];
  }
  else {
    status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, 
		    &nn_fld_rec);
    cBYE(status);
  }
  *ptr_fld_id = fld_id;
  if ( fld_id < 0 ) {
    return status ;
  }
  if ( is_print == false ) {
    return status ;
  }

  if ( strcmp(what_to_pr, "name") == 0 ) { 
    fprintf(g_ofp, "%s", fld_rec.name);
  }
  else if ( strcmp(what_to_pr, "file") == 0 ) { 
    fprintf(g_ofp, "%d", fld_rec.fileno);
  }
  else if ( strcmp(what_to_pr, "filesz") == 0 ) { 
    fprintf(g_ofp, "%llu", (unsigned long long)fld_rec.filesz);
  }
  else if ( strcmp(what_to_pr, "external") == 0 ) { 
    fprintf(g_ofp, "%d", fld_rec.is_external);
  }
  else if ( strcmp(what_to_pr, "min") == 0 ) { 
    status = pr_min_max_sum("min", fld_rec); cBYE(status);
  }
  else if ( strcmp(what_to_pr, "max") == 0 ) { 
    status = pr_min_max_sum("max", fld_rec); cBYE(status);
  }
  else if ( strcmp(what_to_pr, "sum") == 0 ) { 
    status = pr_min_max_sum("sum", fld_rec); cBYE(status);
  }
  else if ( strcmp(what_to_pr, "len") == 0 ) { 
    if ( fld_rec.fldtype == SC ) { 
      fprintf(g_ofp, "%d", fld_rec.len);
    }
    else {
      go_BYE(-1);
    }
  }
  else if ( strcmp(what_to_pr, "fldtype") == 0 ) { 
    status = mk_str_fldtype(fld_rec.fldtype, str_fldtype); cBYE(status);
    fprintf(g_ofp, "%s", str_fldtype);
  }
  else if ( strcmp(what_to_pr, "auxtype") == 0 ) { 
    status = mk_str_auxtype(fld_rec.auxtype, str_auxtype); cBYE(status);
    fprintf(g_ofp, "%s", str_auxtype);
  }
  else if ( strcmp(what_to_pr, "srttype") == 0 ) { 
    status = mk_str_srttype(fld_rec.srttype, str_srttype); cBYE(status);
    fprintf(g_ofp, "%s", str_srttype);
  }
  else {
    fprintf(g_ofp, "id,name,fileno,external,num_nn,min,max,sum,len,filesz,fldtype,auxtype,srttype\n");
    fprintf(g_ofp, "%d,", fld_id);
    fprintf(g_ofp, "%s,", fld_rec.name);
    fprintf(g_ofp, "%d,", fld_rec.fileno);
    fprintf(g_ofp, "%d,", fld_rec.is_external);
    if ( fld_rec.num_nn < 0 ) { 
      fprintf(g_ofp, "undef"); fprintf(g_ofp, ",");
    }
    else {
      fprintf(g_ofp, "%lld", fld_rec.num_nn); fprintf(g_ofp, ",");
    }
    status = pr_min_max_sum("min", fld_rec); cBYE(status); fprintf(g_ofp, ",");
    status = pr_min_max_sum("max", fld_rec); cBYE(status); fprintf(g_ofp, ",");
    status = pr_min_max_sum("sum", fld_rec); cBYE(status); fprintf(g_ofp, ",");
    if ( fld_rec.fldtype == SC ) { 
      fprintf(g_ofp, "%d,", fld_rec.len);
    }
    else {
      fprintf(g_ofp, "0,");
    }
    fprintf(g_ofp, "%llu,", (unsigned long long)fld_rec.filesz);
    status = mk_str_fldtype(fld_rec.fldtype, str_fldtype); cBYE(status);
    fprintf(g_ofp, "%s,", str_fldtype);
    status = mk_str_auxtype(fld_rec.auxtype, str_auxtype); cBYE(status);
    fprintf(g_ofp, "%s,", str_auxtype);
    status = mk_str_srttype(fld_rec.srttype, str_srttype); cBYE(status);
    fprintf(g_ofp, "%s\n", str_srttype);
  }
  //------------------------------------------------
 BYE:
  return status ;
}

// START FUNC DECL
int
int_get_fld_meta(
		 const char *tbl,
		 int in_tbl_id,
		 const char *fld,
		 int in_fld_id,
		 const char *attr,
		 char *value,
		 int sz
		 )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = -1;    TBL_REC_TYPE tbl_rec;
  int fld_id = -1;    FLD_REC_TYPE fld_rec;
  int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;

  //------------------------------------------------
  if ( in_tbl_id < 0 ) { 
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  }
  else {
    tbl_id = in_tbl_id;
  }
  if ( tbl_id < 0 ) { go_BYE(-1); }
  //--------------------------------------------------
  if ( in_fld_id < 0 ) { 
    status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, 
		    &nn_fld_id, &nn_fld_rec);
    cBYE(status);
  }
  else {
    fld_id = in_fld_id;
  }
  if ( fld_id < 0 ) { go_BYE(-1); }
  if ( strcmp(attr, "fileno") == 0 ) {
    sprintf(value, "%d", g_flds[fld_id].fileno);
  }
  else if ( strcmp(attr, "ddir_id") == 0 ) {
    sprintf(value, "%d", g_flds[fld_id].ddir_id);
  }
  else {
    go_BYE(-1);
  }
  //------------------------------------------------
 BYE:
  return status ;
}
// START FUNC DECL
int
get_fld_meta(
	     int fld_id,
	     FLD_REC_TYPE *ptr_fld_meta
	     )
// STOP FUNC DECL
{
  int status = 0;
  zero_fld_rec(ptr_fld_meta);
  chk_range(fld_id, 0, g_n_fld);
  copy_fld_meta(ptr_fld_meta, g_flds[fld_id]); 
 BYE:
  return status ;
}

// START FUNC DECL
int
file_to_fld(
	    char *str_fileno,
	    int *ptr_fld_id,
	    char *rslt_buf,
	    int sz_rslt_buf
	    )
// STOP FUNC DECL
{

  int status = 0;
  int fileno;

  if ( ( str_fileno == NULL ) || ( *str_fileno == '\0' ) ) { 
    go_BYE(-1); 
  }
  status = stoI4(str_fileno, &fileno);
  if ( fileno <= 0 ) { go_BYE(-1); }
  *ptr_fld_id = -1;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].fileno == fileno ) {
      int tbl_id = g_flds[i].tbl_id;
      char *tbl = g_tbls[tbl_id].name;
      snprintf(rslt_buf, sz_rslt_buf, "%s:%s\n", tbl, g_flds[i].name); 
      *ptr_fld_id  = i;
      break;
    }
  }
 BYE:
  return status ;
}
