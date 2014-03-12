#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "meta_globals.h"

char g_cwd[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int chk_fld_info(
		 FLD_INFO_TYPE X
		 )
// STOP FUNC DECL
{
  int status = 0;
  short fld_id = -1; 
  short len_fld_id = -1, off_fld_id = -1, dict_fld_id = -1;
  char name1[MAX_LEN_FLD_NAME+1];

  zero_string(name1, MAX_LEN_FLD_NAME+1);
  fld_id = X.fld_id;
  status = chk_fld_meta(fld_id, -1); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  switch ( X.mode ) { 
  case fk_fld_txt_lkp : 
    dict_fld_id = X.xxx.dict_fld_id;
    chk_range(dict_fld_id, 0, g_n_fld);
    if ( is_zero_fld_rec(g_flds[dict_fld_id]) ) { go_BYE(-1); }
    // TODO P2 THINK if ( g_tbls[dict_tbl_id].tbltype != txt_lkp ) { go_BYE(-1); }
    /* I thought you could not be self-referential but this is okay.
     * Consider the following case. You have a lookup table called
     * TCountry. You add a field called xidx by duplicating idx (the
     * index field). Now you sort xidx and you want to print it out.
     * xidx will have to point to this table itself 
    if ( g_flds[fld_id].tbl_id == dict_tbl_id ) { go_BYE(-1); }
    */
    break;
  case fk_fld_len : 
    len_fld_id = X.xxx.len_fld_id;
    chk_range(len_fld_id, 0, g_n_fld);
    if ( is_zero_fld_rec(g_flds[len_fld_id]) ) { go_BYE(-1); }
    if ( g_flds[len_fld_id].fldtype != I2 ) { go_BYE(-1); }
    strcpy(name1, ".len."); strcat(name1, g_flds[fld_id].name);
    if ( strcmp(name1, g_flds[len_fld_id].name) != 0 ) { go_BYE(-1); }
    if ( g_flds[fld_id].tbl_id != g_flds[len_fld_id].tbl_id ) { go_BYE(-1); }
    break;
  case fk_fld_off : 
    off_fld_id = X.xxx.off_fld_id;
    chk_range(off_fld_id, 0, g_n_fld);
    if ( is_zero_fld_rec(g_flds[off_fld_id]) ) { go_BYE(-1); }
    if ( g_flds[off_fld_id].fldtype != I8 ) { go_BYE(-1); }
    strcpy(name1, ".off."); strcat(name1, g_flds[fld_id].name);
    if ( strcmp(name1, g_flds[off_fld_id].name) != 0 ) { go_BYE(-1); }
    if ( g_flds[fld_id].tbl_id != g_flds[off_fld_id].tbl_id ) { go_BYE(-1); }
    break;
  default: 
    go_BYE(-1);
    break;
  }
 BYE:
  return status;
}


// START FUNC DECL
int
chk_tbl_meta(
	     int tbl_id
	     )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(tbl_id, 0, g_n_tbl); 
  TBL_REC_TYPE X = g_tbls[tbl_id];
  status = chk_tbl_name(X.name); cBYE(status);
  if ( X.nR <= 0 ) { go_BYE(-1); }
  status = chk_tbltype(X.tbltype); cBYE(status);
  if ( X.tbltype == txt_lkp ) { 
    int fld_id = -1, nn_fld_id = -1;
    FLD_REC_TYPE fld_rec, nn_fld_rec;
    status = is_fld(NULL, tbl_id, "txt", &fld_id, &fld_rec, 
		    &nn_fld_id, &nn_fld_rec); 
    cBYE(status);
    if ( fld_id < 0 ) { go_BYE(-1); }
    if ( g_flds[fld_id].fldtype != SV ) { go_BYE(-1); }
  }

 BYE:
  return status;
}

// START FUNC DECL
int
chk_fld_meta(
	     int fld_id,
	     long long nR
	     )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  int fldsz; char *cptr;
  FLD_REC_TYPE M;

  chk_range(fld_id, 0, g_n_fld);
  M = g_flds[fld_id]; 
  chk_range(M.tbl_id,  0, g_n_tbl); 
  chk_range(M.ddir_id, 0, g_n_ddir); 

  if ( strlen(g_ddirs[M.ddir_id].name) == 0 ) { go_BYE(-1); }
  if ( dir_exists(g_ddirs[M.ddir_id].name) == false ) { go_BYE(-1); }

  // B cannot have an nn field */
  if ( ( M.fldtype == B ) && ( M.nn_fld_id >= 0 ) )  { go_BYE(-1); }
  if ( M.parent_id >= 0 ) { /* this is an auxilary field */
    char prefix[8]; zero_string(prefix, 8);
    switch ( M.auxtype ) { 
    case nn : 
      if ( ( M.fldtype != I1 ) && ( M.fldtype != B ) ) { go_BYE(-1); }
      strcpy(prefix, ".nn.");
      break;
    case len : 
      if ( M.fldtype != I2 ) { go_BYE(-1); }
      strcpy(prefix, ".len.");
      break;
    case off : 
      if ( M.fldtype != I8 ) { go_BYE(-1); }
      strcpy(prefix, ".off.");
      break;
    default : 
      go_BYE(-1);
      break;
    }
    /* name of nn field must start with ".nn." */
    cptr = strstr(M.name, prefix);
    if ( cptr != M.name ) { go_BYE(-1); }
    cptr = M.name + strlen(prefix); 
    /* name of nn field must be ".nn." + name of parent */
    if ( strcmp(cptr,  g_flds[M.parent_id].name) != 0 ) { go_BYE(-1); }
    /* parent of nn field must have nn field */
    if ( M.auxtype == nn ) { 
      if ( g_flds[M.parent_id].nn_fld_id < 0 ) { go_BYE(-1); }
    }
    /* aux field cannot have nn field */
    if ( M.nn_fld_id >= 0 ) { go_BYE(-1); }
  }
  // If a field is a fk to a lookup table, then it can be I1/I2/I4/I8
  // TODO P1
  if ( M.len >= 1 ) { 
    if ( M.fldtype != SC ) { go_BYE(-1); }
    if ( M.len > MAX_LEN_STR ) { go_BYE(-1); }
  }
  else {
    if ( M.fldtype == SC ) { go_BYE(-1); }
    if ( M.len != 0 ) { go_BYE(-1); }
  }
  int len_fld_id, off_fld_id;
  status = get_fld_info(fk_fld_len, fld_id, &len_fld_id); cBYE(status);
  status = get_fld_info(fk_fld_off, fld_id, &off_fld_id); cBYE(status);
  if ( M.fldtype == SV ) { 
    if ( len_fld_id < 0 ) { go_BYE(-1); }
    if ( off_fld_id < 0 ) { go_BYE(-1); }
    if ( g_flds[len_fld_id].fldtype != I2 ) { go_BYE(-1); }
    if ( g_flds[off_fld_id].fldtype != I8 ) { go_BYE(-1); }
  }
  else {
    if ( len_fld_id != -1 ) { go_BYE(-1); }
    if ( off_fld_id != -1 ) { go_BYE(-1); }
  }

  if ( M.fldtype == SC ) { 
    if ( ( M.len < 1 )  || ( M.len  > MAX_LEN_STR ) ) { go_BYE(-1); }
    if ( M.nn_fld_id >= 0 ) { go_BYE(-1); }
  }
  status = chk_srttype(M.srttype); cBYE(status);
  status = chk_fldtype(M.fldtype); cBYE(status);
  status = chk_auxtype(M.auxtype); cBYE(status);

  if ( M.auxtype == undef_auxtype ) {
    status = chk_fld_name(M.name); cBYE(status);
  }
  switch ( M.fldtype ) { 
  case SC : case SV : 
    if ( ( M.is_max_nn == true ) || 
	 ( M.is_min_nn == true ) || 
	 ( M.is_sum_nn == true ) ) {
      go_BYE(-1);
    }
    break;
  case B : 
    if ( ( M.is_max_nn == true ) || ( M.is_min_nn == true ) ) { go_BYE(-1); }
    break;
  default : 
    break;
  }

  if ( ( M.is_min_nn == false ) && ( M.minval.minF8 != 0 ) ) { go_BYE(-1); }
  if ( ( M.is_max_nn == false ) && ( M.maxval.maxF8 != 0 ) ) { go_BYE(-1); }
  if ( ( M.is_sum_nn == false ) && ( M.sumval.sumF8 != 0 ) ) { go_BYE(-1); }
  if ( ( M.is_min_nn == false ) && ( M.minval.minI8 != 0 ) ) { go_BYE(-1); }
  if ( ( M.is_max_nn == false ) && ( M.maxval.maxI8 != 0 ) ) { go_BYE(-1); }
  if ( ( M.is_sum_nn == false ) && ( M.sumval.sumI8 != 0 ) ) { go_BYE(-1); }

  if ( M.fileno <= 0 ) { go_BYE(-1); }
  if ( ( M.is_max_nn == true ) &&  ( M.is_min_nn == true ) ) {
    switch ( M.fldtype) {
    case I1 : case I2: case I4 : case I8 : 
      if ( M.minval.minI8 > M.maxval.maxI8 ) { go_BYE(-1); }
      break;
    case F4 : case F8 : 
      if ( M.minval.minF8 > M.maxval.maxF8 ) { go_BYE(-1); }
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }

  status = q_mmap(M.ddir_id, M.fileno, &X, &nX, 0); cBYE(status);
  if ( nX != M.filesz ) { 
    fprintf(stderr, "fileno = %d \n", M.fileno);
    fprintf(stderr, "nX     = %d \n", (int)nX);
    fprintf(stderr, "filesz = %d \n", (int)(M.filesz));
    go_BYE(-1); 
  }
  // if a field has an nn field, it cannot be sorted
  switch ( M.srttype ) { 
  case unknown : break;
  case ascending  : if ( M.nn_fld_id>= 0 ) { go_BYE(-1); } break;
  case descending : if ( M.nn_fld_id>= 0 ) { go_BYE(-1); } break;
  case unsorted   : if ( M.nn_fld_id>= 0 ) { go_BYE(-1); } break;
  default : go_BYE(-1); break;
  }

  status = get_fld_sz(M.fldtype, &fldsz); cBYE(status);
  /* Check on file size */
  if ( ( M.fldtype != SC ) && ( M.fldtype != SV ) )  { 
    if ( nR > 0 ) { 
      long long exp_filesize = fldsz * nR;
      if ( exp_filesize != nX ) { 
	go_BYE(-1); 
      }
    }
  }
 BYE:
  if ( status < 0 ) { fprintf(stderr, "Error on field %s \n", M.name); }
  rs_munmap(X, nX);
  return status ;
}
// START FUNC DECL
void
zero_fld_info_rec(
		  FLD_INFO_TYPE *ptr_X
		  )
// STOP FUNC DECL
{

  // IMPORTANT: The initialization to 0 is to be noted. This fact is
  // used when we perform binary search
  ptr_X->mode    = 0; 
  ptr_X->unused1 = 0;
  ptr_X->fld_id  = 0;
  ptr_X->xxx.dict_fld_id = -1;
  ptr_X->xxx.len_fld_id  = -1;
  ptr_X->xxx.off_fld_id  = -1;
  ptr_X->unused2 = 0;
}
//
// START FUNC DECL
bool
is_zero_fld_info_rec(
		     FLD_INFO_TYPE X
		     )
// STOP FUNC DECL
{

  if ( X.mode    != 0 ) { return false; } 
  if ( X.unused1 != 0 ) { return false; }
  if ( X.fld_id  != 0 ) { return false; }
  if ( X.xxx.dict_fld_id != -1 ) { return false; }
  if ( X.xxx.len_fld_id  != -1 ) { return false; }
  if ( X.xxx.off_fld_id  != -1 ) { return false; }
  if ( X.unused2 != 0 ) { return false; }
  return true;
}

// START FUNC DECL
void
zero_ddir_rec(
	      DDIR_REC_TYPE *ptr_X
	      )
// STOP FUNC DECL
{
  ptr_X->is_writable = false;
  zero_string(ptr_X->name, (MAX_LEN_DIR_NAME+1));
}
// START FUNC DECL
void
zero_fld_rec(
	     FLD_REC_TYPE *ptr_X
	     )
// STOP FUNC DECL
{
  ptr_X->tbl_id      = -1;
  ptr_X->ddir_id     = -1;
  ptr_X->parent_id   = -1;
  ptr_X->nn_fld_id   = -1;
  ptr_X->len         =  0;

  ptr_X->filesz      = 0;
  ptr_X->is_external = false; 
  ptr_X->srttype     = unknown;
  ptr_X->auxtype     = undef_auxtype;
  ptr_X->fldtype     = undef_fldtype;

  for ( int i = 0; i < MAX_LEN_FLD_NAME+1; i++ ) { ptr_X->name[i] = '\0'; }
  ptr_X->fileno = 0; 

  ptr_X->sumval.sumI8 = 0;
  ptr_X->minval.minI8 = 0;
  ptr_X->maxval.maxI8 = 0;

  ptr_X->sumval.sumF8 = 0;
  ptr_X->minval.minF8 = 0;
  ptr_X->maxval.maxF8 = 0;

  ptr_X->is_sum_nn = 0;
  ptr_X->is_min_nn = 0;
  ptr_X->is_max_nn = 0;

  ptr_X->num_nn = -1;

}
// START FUNC DECL
bool
is_zero_fld_rec(
		FLD_REC_TYPE X
		)
// STOP FUNC DECL
{
  if ( X.tbl_id      != -1 ) { return false; }
  if ( X.ddir_id     != -1 ) { return false; }
  if ( X.parent_id   != -1 ) { return false; }
  if ( X.nn_fld_id   != -1 ) { return false; }
  if ( X.len         !=  0 ) { return false; }

  if ( X.filesz      != 0 ) { return false; }
  if ( X.is_external != false ) { return false; } 
  if ( X.srttype     != unknown ) { return false; }
  if ( X.auxtype     != undef_auxtype ) { return false; }
  if ( X.fldtype     != undef_fldtype ) { return false; }

  for ( int i = 0; i < MAX_LEN_FLD_NAME+1; i++ ) { 
    if ( X.name[i] != '\0' ) { 
      return false; 
    } 
  }
  if ( X.fileno != 0 ) { return false; } 

  if ( X.sumval.sumI8 != 0 ) { return false; }
  if ( X.minval.minI8 != 0 ) { return false; }
  if ( X.maxval.maxI8 != 0 ) { return false; }

  if ( X.sumval.sumF8 != 0 ) { return false; }
  if ( X.minval.minF8 != 0 ) { return false; }
  if ( X.maxval.maxF8 != 0 ) { return false; }

  if ( X.is_sum_nn != 0 ) { return false; }
  if ( X.is_min_nn != 0 ) { return false; }
  if ( X.is_max_nn != 0 ) { return false; }

  return true;
}
//

// START FUNC DECL
int
pr_fld_meta(
	    int fld_id,
	    FLD_REC_TYPE X
	    )
// STOP FUNC DECL
{
  int status = 0;
  int dict_fld_id, len_fld_id, off_fld_id;
  fprintf(stdout, "tbl_id,%d\n", X.tbl_id);
  fprintf(stdout, "name,%s\n", X.name);
  fprintf(stdout, "fileno,%d\n", X.fileno);
  fprintf(stdout, "filesz,%llu\n", (unsigned long long)X.filesz);
  char str_fldtype[32]; zero_string(str_fldtype, 32);
  status = mk_str_fldtype(X.fldtype, str_fldtype); cBYE(status);
  fprintf(stdout, "fldtype,%s\n", str_fldtype); 

  char str_auxtype[32]; zero_string(str_auxtype, 32);
  status = mk_str_auxtype(X.auxtype, str_auxtype); cBYE(status);
  fprintf(stdout, "auxtype,%s\n", str_auxtype); 

  char str_srttype[32]; zero_string(str_srttype, 32);
  status = mk_str_srttype(X.srttype, str_srttype); cBYE(status);
  fprintf(stdout, "srttype,%s\n", str_srttype); 

  status = get_fld_info(fk_fld_txt_lkp, fld_id, &dict_fld_id); cBYE(status);
  fprintf(stdout, "dict_fld_id,%d\n", dict_fld_id);

  fprintf(stdout, "parent_id,%d\n", X.parent_id);
  fprintf(stdout, "nn_fld_id,%d\n", X.nn_fld_id);
  status = get_fld_info(fk_fld_len, fld_id, &len_fld_id); cBYE(status);
  fprintf(stdout, "len_fld_id,%d\n", len_fld_id);
  status = get_fld_info(fk_fld_off, fld_id, &off_fld_id); cBYE(status);
  fprintf(stdout, "off_fld_id,%d\n", off_fld_id);
  fprintf(stdout, "is_external,%d\n", X.is_external);
 BYE:
  return status ;
}
//---------------------------------------------------------------

// START FUNC DECL
int
mk_str_srttype(
	       int int_srttype,
	       char *str_srttype
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_srttype == NULL ) { go_BYE(-1); }
  switch ( int_srttype ) { 
  case unknown    : strcpy(str_srttype, "unknown"); break;
  case unsorted   : strcpy(str_srttype, "unsorted"); break;
  case ascending  : strcpy(str_srttype, "ascending"); break;
  case descending : strcpy(str_srttype, "descending"); break;
  default : go_BYE(-1); break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
mk_str_tbltype(

	       TBL_TYPE tbltype,
	       char *str_tbltype
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_tbltype == NULL ) { go_BYE(-1); }
  switch ( tbltype ) { 
  case regular    : strcpy(str_tbltype, "regular"); break;
  case txt_lkp  : strcpy(str_tbltype, "txt_lkp"); break;
  case min_max : strcpy(str_tbltype, "min_max"); break;
  case first_last : strcpy(str_tbltype, "first_last"); break;
  case bins : strcpy(str_tbltype, "bins"); break;
  default : go_BYE(-1); break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
chk_tbltype(
	    int tbltype
	    )
// STOP FUNC DECL
{
  int status = 0;
  switch ( tbltype ) { 
    // case undef_tbl : 
  case regular : 
  case txt_lkp : 
  case min_max : 
  case first_last : 
  case bins : 
    break;
  default : fprintf(stderr, "tbltype = %d \n", tbltype); go_BYE(-1); break;
  }
 BYE:
  return status;
}

// START FUNC DECL
int
chk_fldtype(
	    int fldtype
	    )
// STOP FUNC DECL
{
  int status = 0;
  switch ( fldtype ) { 
  case B  : 
  case I1 : 
  case I2 : 
  case I4 : 
  case F4 : 
  case I8 : 
  case F8 : 
  case SC : 
  case SV : 
    break;
  default : fprintf(stderr, "fldtype = %d \n", fldtype); go_BYE(-1); break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
chk_auxtype(
	    int auxtype
	    )
// STOP FUNC DECL
{
  int status = 0;
  switch ( auxtype ) { 
  case nn : 
  case off : 
  case len : 
  case undef_auxtype  : 
    break;
  default : fprintf(stderr, "auxtype = %d \n", auxtype); go_BYE(-1); break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
chk_srttype(
	    int srttype
	    )
// STOP FUNC DECL
{
  int status = 0;
  switch ( srttype ) { 
  case unknown : 
  case ascending  : 
  case descending  : 
  case unsorted  : 
    break;
  default : fprintf(stderr, "srttype = %d \n", srttype); go_BYE(-1); break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
mk_str_fldtype(
	       int fldtype,
	       char *str_fldtype
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_fldtype == NULL ) { go_BYE(-1); }
  switch ( fldtype ) { 
  case B    : strcpy(str_fldtype, "B"); break;
  case I1   : strcpy(str_fldtype, "I1"); break;
  case I2   : strcpy(str_fldtype, "I2"); break;
  case I4   : strcpy(str_fldtype, "I4"); break;
  case F4   : strcpy(str_fldtype, "F4"); break;
  case I8   : strcpy(str_fldtype, "I8"); break;
  case F8   : strcpy(str_fldtype, "F8"); break;
  case SC : strcpy(str_fldtype, "SC"); break;
  case SV : strcpy(str_fldtype, "SV"); break;
  default : fprintf(stderr, "fldtype = %d \n", fldtype); go_BYE(-1); break;
  }
 BYE:
  return status ;
}
// START FUNC DECL
int
mk_str_auxtype(
	       int auxtype,
	       char *str_auxtype
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_auxtype == NULL ) { go_BYE(-1); }
  switch ( auxtype ) { 
  case undef_fldtype : strcpy(str_auxtype, "primary");  break;
  case nn            : strcpy(str_auxtype, "nn"); break;
  case len           : strcpy(str_auxtype, "len"); break;
  case off           : strcpy(str_auxtype, "off"); break;
  default : go_BYE(-1); break;
  }
 BYE:
  return status ;
}
//
// START FUNC DECL
int
get_fld_sz(
	   FLD_TYPE fldtype,
	   int *ptr_fld_sz
	   )
// STOP FUNC DECL
{
  int status = 0;
  *ptr_fld_sz = -1;
  switch ( fldtype ) { 
  case B       : go_BYE(-1); break; // do not use for bit type
  case I1      : *ptr_fld_sz = 1; break;
  case I2      : *ptr_fld_sz = 2; break;
  case I4      : *ptr_fld_sz = 4; break;
  case I8      : *ptr_fld_sz = 8; break;
  case F4      : *ptr_fld_sz = 4; break;
  case F8      : *ptr_fld_sz = 8; break;
  case SC      :                  break; // TODO P3 Why blank?
  case SV      :                  break; // TODO P3 Why blank?
  default : go_BYE(-1); break;
  }
 BYE:
  return status ;
}
// START FUNC DECL
int
unstr_tbltype(
	      const char *str_tbltype,
	      TBL_TYPE *ptr_tbltype
	      )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_tbltype == NULL ) { go_BYE(-1); }

  if ( strcmp(str_tbltype, "regular") == 0 ) {  
    *ptr_tbltype = regular; return status ;
  }
  if ( strcmp(str_tbltype, "txt_lkp") == 0 ) { 
    *ptr_tbltype = txt_lkp; return status ; 
  }
  if ( strcmp(str_tbltype, "min_max") == 0 ) { 
    *ptr_tbltype = min_max; return status ; 
  }
  if ( strcmp(str_tbltype, "first_last") == 0 ) { 
    *ptr_tbltype = first_last; return status ; 
  }
  if ( strcmp(str_tbltype, "bins") == 0 ) { 
    *ptr_tbltype = bins; return status ; 
  }
  fprintf(stderr, "Unknown type %s \n", str_tbltype);
  go_BYE(-1); 
 BYE:
  return status ;
}
// START FUNC DECL
int
unstr_fldtype(
	      const char *str_fldtype,
	      FLD_TYPE *ptr_fldtype
	      )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_fldtype == NULL ) { go_BYE(-1); }

  if ( strcmp(str_fldtype, "B") == 0 ) {  *ptr_fldtype = B; return status ;}
  if ( strcmp(str_fldtype, "I1") == 0 ) { *ptr_fldtype = I1; return status ;}
  if ( strcmp(str_fldtype, "I2") == 0 ) { *ptr_fldtype = I2; return status ; }
  if ( strcmp(str_fldtype, "I4") == 0 ) { *ptr_fldtype = I4; return status ; }
  if ( strcmp(str_fldtype, "F4") == 0 ) { *ptr_fldtype = F4; return status ; }
  if ( strcmp(str_fldtype, "I8") == 0 ) { *ptr_fldtype = I8; return status ; }
  if ( strcmp(str_fldtype, "F8") == 0 ) { *ptr_fldtype = F8; return status ; }
  if ( strcmp(str_fldtype, "SC") == 0 ) { *ptr_fldtype = SC; return status ; }
  if ( strcmp(str_fldtype, "SV") == 0 ) { *ptr_fldtype = SV; return status ; }
#undef XXX
#ifdef XXX
  // Old stuff. Clean this out. TODO P2
  // Note that string is converted to and stored as 8-byte hash
  if ( strcmp(str_fldtype, "str") == 0 ) { *ptr_fldtype = I8; return status ; }
#endif
  fprintf(stderr, "Unknown type %s \n", str_fldtype);
  go_BYE(-1); 
 BYE:
  return status ;
}
//
// START FUNC DECL
int
unstr_auxtype(
	      const char *str_auxtype,
	      AUX_TYPE *ptr_auxtype
	      )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_auxtype == NULL ) { go_BYE(-1); }

  if ( strcmp(str_auxtype, "nn") == 0 ) { *ptr_auxtype = nn; return status ;}
  if ( strcmp(str_auxtype, "len") == 0 ) { *ptr_auxtype = len; return status ;}
  if ( strcmp(str_auxtype, "off") == 0 ) { *ptr_auxtype = off; return status ;}
  if ( strcmp(str_auxtype, "undef") == 0 ) { *ptr_auxtype = undef_auxtype; return status ;}
  go_BYE(-1); 
 BYE:
  return status ;
}

// START FUNC DECL
bool
is_zero_tbl_rec(
		TBL_REC_TYPE X
		)
// STOP FUNC DECL
{
  if ( X.nR != 0 ) { return false; }
  if ( X.tbltype != undef_tbl ) { return false; }
  for ( int i = 0; i < MAX_LEN_TBL_NAME+1; i++ ) { 
    if ( X.name[i] != '\0' ) { return false; }
  }
  return true;
}


// START FUNC DECL
void
zero_tbl_rec(
	     TBL_REC_TYPE *ptr_X
	     )
// STOP FUNC DECL
{
  ptr_X->nR = 0;
  ptr_X->tbltype = undef_tbl;
  for ( int i = 0; i < MAX_LEN_TBL_NAME+1; i++ ) { 
    ptr_X->name[i] = '\0';
  }
  ptr_X->is_temp = false;
}

// START FUNC DECL
int
chk_tbl_name(
	     const char *X
	     )
// STOP FUNC DECL
{
  int status = 0;
  if ( ( X == NULL ) || ( *X == '\0' ) || 
       ( strlen(X) > MAX_LEN_TBL_NAME )) { 
    go_BYE(-1); 
  }
  for ( const char *cptr = X; *cptr != '\0'; cptr++ ) {
    if ( ( isalnum(*cptr) ) || ( *cptr == '_' ) ) { 
      /* all is well */
    }
    else { 
      fprintf(stderr, "[%s] is invalid tbl name \n", X); go_BYE(-1); 
    }
  }
 BYE:
  return status ;
}

// START FUNC DECL
void
copy_fld_meta(
	      FLD_REC_TYPE *ptr_dst_fld_meta,
	      FLD_REC_TYPE src_fld_meta
	      )
// STOP FUNC DECL
{
  ptr_dst_fld_meta->tbl_id      = src_fld_meta.tbl_id;
  ptr_dst_fld_meta->ddir_id     = src_fld_meta.ddir_id;
  ptr_dst_fld_meta->parent_id   = src_fld_meta.parent_id;
  ptr_dst_fld_meta->nn_fld_id   = src_fld_meta.nn_fld_id; 
  ptr_dst_fld_meta->len         = src_fld_meta.len;

  ptr_dst_fld_meta->filesz      = src_fld_meta.filesz; 
  ptr_dst_fld_meta->is_external = src_fld_meta.is_external; 
  ptr_dst_fld_meta->srttype     = src_fld_meta.srttype;
  ptr_dst_fld_meta->fldtype     = src_fld_meta.fldtype; 
  ptr_dst_fld_meta->auxtype     = src_fld_meta.auxtype; 

  strcpy(ptr_dst_fld_meta->name, src_fld_meta.name);
  ptr_dst_fld_meta->fileno      = src_fld_meta.fileno;

  ptr_dst_fld_meta->sumval.sumI8 = src_fld_meta.sumval.sumI8;
  ptr_dst_fld_meta->minval.minI8 = src_fld_meta.minval.minI8;
  ptr_dst_fld_meta->maxval.maxI8 = src_fld_meta.maxval.maxI8;

  ptr_dst_fld_meta->sumval.sumF8 = src_fld_meta.sumval.sumF8;
  ptr_dst_fld_meta->minval.minF8 = src_fld_meta.minval.minF8;
  ptr_dst_fld_meta->maxval.maxF8 = src_fld_meta.maxval.maxF8;

  ptr_dst_fld_meta->is_sum_nn = src_fld_meta.is_sum_nn;
  ptr_dst_fld_meta->is_min_nn = src_fld_meta.is_min_nn;
  ptr_dst_fld_meta->is_max_nn = src_fld_meta.is_max_nn;

  ptr_dst_fld_meta->num_nn    = src_fld_meta.num_nn;

}

// START FUNC DECL
int
get_ddir_id(
	    char *in_data_dir,
	    DDIR_REC_TYPE *ddirs,
	    int n_ddirs,
	    bool add_if_missing,
	    int *ptr_ddir_id
	    )
// STOP FUNC DECL
{
  int status = 0;
  char data_dir[MAX_LEN_DIR_NAME+1];

  *ptr_ddir_id = INT_MIN;
  if ( is_absolute_path(in_data_dir) == false ) { go_BYE(-1); }

  status = strip_trailing_slash(in_data_dir, data_dir, MAX_LEN_DIR_NAME+1);
  cBYE(status);
  for ( int i = 0; i < n_ddirs; i++ ) { 
    if ( strcmp(ddirs[i].name, data_dir) == 0 ) {
      *ptr_ddir_id = i;
      break;
    }
  }
  if ( ( add_if_missing ) && ( *ptr_ddir_id < 0 ) )  {
    for ( int i = 0; i < n_ddirs; i++ ) { 
      if ( strlen(ddirs[i].name) == 0 ) { /* spot is empty. Pop it in here */
        strcpy(ddirs[i].name, data_dir);
        *ptr_ddir_id = i;
        break;
      }
    }
  }
  if ( *ptr_ddir_id < 0 ) { go_BYE(-1); } // no space 
 BYE:
  return status ;
}

// START FUNC DECL
int
chk_fld_name(
	     const char *in_X /* not to be used for aux fields */
	     )
// STOP FUNC DECL
{
  int status = 0;
  char *X;
  X = (char *)in_X;
  if ( ( X == NULL ) || 
       ( *X == '\0' ) || 
       ( strlen(X) > MAX_LEN_FLD_NAME )) { 
    go_BYE(-1); 
  }
  for ( ; *X != '\0'; X++ ) {
    if ( ( *X == '_' ) || ( isalnum(*X) ) ) {
      // all is well 
    }
    else { 
      fprintf(stderr, "Field name [%s] is bad. See [%c] \n", in_X, *X);
      go_BYE(-1); 
    }
  }
 BYE:
  return status ;
}

// START FUNC DECL
void
copy_tbl_meta(
	      TBL_REC_TYPE *ptr_dst_tbl_meta,
	      TBL_REC_TYPE src_tbl_meta
	      )
// STOP FUNC DECL
{
  ptr_dst_tbl_meta->is_temp     = src_tbl_meta.is_temp;
  ptr_dst_tbl_meta->nR          = src_tbl_meta.nR;
  ptr_dst_tbl_meta->tbltype     = src_tbl_meta.tbltype;
  strcpy(ptr_dst_tbl_meta->name,  src_tbl_meta.name);
}

// START FUNC DECL
int all_chk_fld_meta(
		     int num_to_check
		     )
// STOP FUNC DECL
{
  int status = 0;
  int num_checked = 0;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].fileno > 0 )   {
      num_checked++; 
      int tbl_id = g_flds[i].tbl_id;
      chk_range(tbl_id, 0, g_n_tbl);
      long long nR = g_tbls[tbl_id].nR;
      if ( nR <= 0 ) { go_BYE(-1); }
      status = chk_fld_meta(i, nR); cBYE(status);
    }
    if ( num_checked >= num_to_check ) { break; }
  }
 BYE:
  return status ;
}
// START FUNC DECL
int
mk_str_fld_info_mode(
		     FLD_INFO_MODE_TYPE mode,
		     char *strbuf
		     )
// STOP FUNC DECL
{
  int status = 0;
  if ( strbuf == NULL ) { go_BYE(-1); }
  switch ( mode ) { 
  case fk_fld_txt_lkp : strcpy(strbuf, "fk_fld_txt_lkp"); break; 
  case fk_fld_len     : strcpy(strbuf, "fk_fld_len");     break; 
  case fk_fld_off     : strcpy(strbuf, "fk_fld_off");     break; 
  default : go_BYE(-1); break; 
  }
 BYE:
  return status ;
}
// START FUNC DECL
int
mk_str_exttype(
	       bool is_external,
	       char *strbuf
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( strbuf == NULL ) { go_BYE(-1); }
  if ( is_external == true ) { 
    strcpy(strbuf, "external"); 
  }
  else if ( is_external == false ) { 
    strcpy(strbuf, "internal"); 
  }
  else { 
    go_BYE(-1);
  }
 BYE:
  return status ;
}
// START FUNC DECL
int
copy_meta(
	  int dst_fld_id,
	  int src_fld_id,
	  int mode
	  )
// STOP FUNC DECL
{
  int status = 0;
  int dict_fld_id, len_fld_id, off_fld_id; 
  if ( dst_fld_id < 0 ) { go_BYE(-1); }
  if ( dst_fld_id == src_fld_id ) { go_BYE(-1); }

  switch ( mode ) { 
  case 1 : 
    g_flds[dst_fld_id].srttype     = g_flds[src_fld_id].srttype;

    status=get_fld_info(fk_fld_txt_lkp, src_fld_id, &dict_fld_id);cBYE(status);
    if ( dict_fld_id >= 0 ) { 
      status = add_to_fld_info(dst_fld_id, fk_fld_txt_lkp, dict_fld_id); 
      cBYE(status);
    }

    status = get_fld_info(fk_fld_len, src_fld_id, &len_fld_id); cBYE(status);
    if ( len_fld_id >= 0 ) { 
      status = add_to_fld_info(dst_fld_id, fk_fld_len, len_fld_id); 
      cBYE(status);
    }

    status = get_fld_info(fk_fld_off, src_fld_id, &off_fld_id); cBYE(status);
    if ( off_fld_id >= 0 ) { 
      status = add_to_fld_info(dst_fld_id, fk_fld_off, off_fld_id); 
      cBYE(status);
    }

    break;
  case 2 : 
    /* TODO: P2: same as above but also copy sum/min/max/... */
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  return status ;
}
