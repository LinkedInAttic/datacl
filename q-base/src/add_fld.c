#include "qtypes.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "extract_S.h"
#include "extract_I4.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "auxil.h"
#include "del_fld.h"
#include "add_fld.h"
#include "meta_globals.h"
#include "hash_string.h"
#include "mmap.h"
#include "mk_temp_file.h"

// START FUNC DECL
int
get_empty_fld(
	      const char *fld,
	      FLD_REC_TYPE *flds,
	      int n_fld,
	      int *ptr_fld_id
	      )
// STOP FUNC DECL
{
  int status = 0;
  unsigned int startidx;

  if ( ( fld == NULL ) || ( *fld == '\0' ) )  { go_BYE(-1); }
  // Pick a spot to start searching for an empty spot
  status = hash_string_UI4(fld, &startidx);
  startidx = startidx % n_fld;

  *ptr_fld_id = INT_MIN; /* no empty spot */
  for ( int i = startidx; i < n_fld; i++ ) { 
    if ( flds[i].name[0] == '\0' ) { 
      *ptr_fld_id = i;
      break;
    }
  }
  if ( *ptr_fld_id < 0 ) {
    for ( int i = 0; i < startidx; i++ ) { 
      if ( flds[i].name[0] == '\0' ) { 
	*ptr_fld_id = i;
	break;
      }
    }
  }
  if ( *ptr_fld_id < 0 ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1);
  }

 BYE:
  return status ;
}

// START FUNC DECL
int
add_fld(
	int tbl_id,
	const char *fld,
	int ddir_id,
	int fileno,
	int *ptr_fld_id,
	FLD_REC_TYPE *ptr_fld_rec
	)
// STOP FUNC DECL
{
  int status = 0;
  int fld_id;  long long nR; size_t filesz;
  char *X = NULL; size_t nX = 0;

  //------------------------------------------------
  // Done by caller: zero_fld_rec(ptr_fld_rec);
  if  ( ptr_fld_rec->fldtype == undef_fldtype ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  status = chk_fld_name(fld); cBYE(status);
  if ( fileno <= 0 ) { go_BYE(-1); }
  chk_range(ddir_id, 0, g_n_ddir);
  chk_range(tbl_id, 0, g_n_tbl);
  *ptr_fld_id = -1;
  nR = g_tbls[tbl_id].nR; if ( nR <= 0 ) { go_BYE(-1); }
  //------------------------------------------------
  ptr_fld_rec->fileno  = fileno;
  ptr_fld_rec->ddir_id = ddir_id;
  status = chk_file_size(nR, *ptr_fld_rec, &filesz); 
  if ( status < 0 ) { printf("Error on field %s \n", fld); }
  cBYE(status);
  status = del_fld(NULL, tbl_id, fld, -1); cBYE(status);
  status = get_empty_fld(fld, g_flds, g_n_fld, &fld_id); cBYE(status);

  // Initialize with input. Then, over-write following fields. 
  g_flds[fld_id] = *ptr_fld_rec; 

  g_flds[fld_id].tbl_id  = tbl_id;
  g_flds[fld_id].ddir_id = ddir_id;
  g_flds[fld_id].fileno  = fileno;
  g_flds[fld_id].filesz  = filesz;
  strcpy(g_flds[fld_id].name, fld);

  *ptr_fld_id  = fld_id;
  *ptr_fld_rec = g_flds[fld_id];
 BYE:
  rs_munmap(X, nX);
  return status ;
}

extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
ext_add_fld(
	    const char *tbl,
	    const char *fld,
	    const char *fldspec
	    )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;    int tbl_id = -1; 
  FLD_REC_TYPE fld_rec;    int fld_id = -1; 
  FLD_TYPE fldtype = undef_fldtype;
  bool is_null; int fileno = -1; int ddir_id = -1;
  size_t filesz = 0; int len = 0;
  char *inX = NULL; size_t in_nX = 0;
  char *opX = NULL; size_t op_nX = 0;

  char from_dir[MAX_LEN_DIR_NAME+1]; 
  zero_string(from_dir, MAX_LEN_DIR_NAME+1);
  char ext_filename[MAX_LEN_EXT_FILE_NAME+1];
  zero_string(ext_filename, MAX_LEN_EXT_FILE_NAME+1);
  char str_fldtype[32]; zero_string(str_fldtype, 32);

  status = is_tbl(tbl, &tbl_id, &tbl_rec);
  chk_range(tbl_id, 0, g_n_tbl);
  status = del_fld(NULL, tbl_id, fld, -1); cBYE(status);

  status = chk_aux_info(fldspec); cBYE(status);
  status = extract_S(fldspec, "file=[", "]", ext_filename, 
      MAX_LEN_EXT_FILE_NAME, &is_null);
  if ( is_null ){ go_BYE(-1); }
  // Check that file name is not underscore followed by digits
  bool is_first_char_uscore = false;
  bool is_other_char_digit = true;
  if ( ext_filename[0] == '_' ) { is_first_char_uscore = true; }
  for ( char *cptr = ext_filename + 1; *cptr != '\0'; cptr++ ) { 
    if ( !isdigit(*cptr) ) {
      is_other_char_digit = false; break;
    }
  }
  if ( ( is_first_char_uscore ) && ( is_other_char_digit ) ) {
    fprintf(stderr, "File name is not valid\n"); go_BYE(-1);
  }
  //----------------------------------------------
  status = extract_S(fldspec, "fldtype=[", "]", str_fldtype, 
      32, &is_null);
  if ( is_null ){ go_BYE(-1); }
  status = unstr_fldtype(str_fldtype, &fldtype); cBYE(status);
  if ( fldtype == SC ) { 
    status = extract_I4(fldspec, "len=[", "]", &len, &is_null);
    if ( is_null ){ go_BYE(-1); }
  }
  status = unstr_fldtype(str_fldtype, &fldtype); cBYE(status);
  // Import of field of type SV not implemented
  if ( fldtype == SV ) { go_BYE(-1); }

  status = extract_S(fldspec, "dir=[", "]", from_dir, MAX_LEN_DIR_NAME, 
      &is_null);
  if ( is_null ){ 
    strcpy(from_dir, g_cwd);
  }
  /* determine whether you have to "cp" or "mv" original file */
  char buffer[8]; bool is_keep_original = true;
  status = extract_S(fldspec, "keep_original=[", "]", buffer, 8, 
      &is_null);
  if  ( is_null ) { 
    is_keep_original = true;
  }
  else {
    if ( strcasecmp(buffer, "true") == 0 ) { 
      is_keep_original = true;
    }
    else if ( strcasecmp(buffer, "false") == 0 ) { 
      is_keep_original = false;
    }
    else {
      go_BYE(-1);
    }
  }
  // Move to directory where file exists
  status = chdir(from_dir); 
  if ( status < 0 ) { 
    fprintf(stderr, "Unable to cd to %s \n", from_dir);
  }
  cBYE(status); 
  if ( file_exists(ext_filename) == false ) { 
    fprintf(stderr, "File not found [%s] in [%s] \n", ext_filename, from_dir);
    go_BYE(-1); 
  }
  // "mv" or "cp" input file to data directory and rename
  status = rs_mmap(ext_filename, &inX, &in_nX, 0); cBYE(status);
  status = mk_temp_file(in_nX, &ddir_id, &fileno); cBYE(status); 
  if ( is_keep_original == true ) { 
    status = q_mmap(ddir_id, fileno, &opX, &op_nX, 1); cBYE(status); 
    memcpy(opX, inX, in_nX); 
  }
  else {
    char newfile[MAX_LEN_DIR_NAME+1+MAX_LEN_FILE_NAME+1];
    char *q_data_dir = g_ddirs[ddir_id].name;
    sprintf(newfile, "%s/_%d", q_data_dir, fileno);
    rename(ext_filename, newfile);
  }
  status = chdir(g_cwd);  cBYE(status);
  zero_fld_rec(&fld_rec); 
  fld_rec.fldtype = fldtype;
  fld_rec.fileno  = fileno;
  fld_rec.ddir_id = ddir_id;
  fld_rec.filesz  = filesz;
  fld_rec.len     = len;
  // Check that file exists and is of correct size
  status = chk_file_size(tbl_rec.nR, fld_rec, &filesz); cBYE(status);
  status = add_fld(tbl_id, fld, ddir_id, fileno, &fld_id, &fld_rec);
  cBYE(status);
 BYE:
  rs_munmap(inX, in_nX); 
  rs_munmap(opX, op_nX); 
  chdir(g_cwd); 
  return status ;
}
