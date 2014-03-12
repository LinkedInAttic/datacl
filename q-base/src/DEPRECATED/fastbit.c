#include "qtypes.h"
#include "auxil.h"
#include "mmap.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "fastbit.h"
#include "meta_globals.h"
#include "../ext/fb_index.h"

#define MAX_NUM_FLDS_IDX 256
extern char *g_data_dir;
// START FUNC DECL
int
init_fb_idx(
    const char *tbl,
    const char *in_flds
    )
// STOP FUNC DECL
{
  int status = 0;
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd;
  char oldpath[MAX_LEN_DIR_NAME+1]; 
  char newpath[MAX_LEN_DIR_NAME+1]; 
  char fb_dir[MAX_LEN_TBL_NAME+1+16]; 
  int tbl_id; TBL_REC_TYPE tbl_rec;
  char **flds = NULL; int n_flds;
  char str_fldtype[16];
  int fld_id, nn_fld_id; FLD_REC_TYPE fld_rec, nn_fld_rec;
  char buffer[2*MAX_LEN_FLD_NAME];
  char *fldstr = NULL; int fldstrlen = 8192;

  if ( g_data_dir == NULL ) { go_BYE(-1); }
  status = is_tbl(tbl, &tbl_id, &tbl_rec);
  chk_range(tbl_id, 0, g_n_tbl);

  fldstr = malloc(fldstrlen * sizeof(char));
  return_if_malloc_failed(fldstr);
  zero_string(fldstr, fldstrlen);
  // Create directory for this table 
  zero_string(buffer, 2*MAX_LEN_FLD_NAME);
  zero_string(oldpath, MAX_LEN_DIR_NAME+1);
  zero_string(newpath, MAX_LEN_DIR_NAME+1);
  zero_string(fb_dir, MAX_LEN_TBL_NAME+1+16);
  strcpy(fb_dir, "_fb_" );
  strcat(fb_dir, tbl);
  mcr_cd;
  struct stat st;
  if ( stat(fb_dir, &st) == 0 ) {
    fprintf(stderr, "Directory exists. Delete it\n"); go_BYE(-1); 
  }
  status = mkdir(fb_dir, 00777); cBYE(status);
  mcr_uncd;
  //--------------------------------
  zero_string(str_fldtype, 16);
  status = explode(in_flds, ':', &flds, &n_flds); cBYE(status);
  if ( n_flds  > MAX_NUM_FLDS ) { go_BYE(-1); }
  // Create soft links for file of each field 
  for ( int i = 0; i < n_flds; i++ ) { 
    status = is_fld(NULL, tbl_id, flds[i], &fld_id, &fld_rec, 
	&nn_fld_id, &nn_fld_rec);
    cBYE(status);
    chk_range(fld_id, 0, g_n_fld);
    if ( nn_fld_id >= 0 ) { go_BYE(-1); }
    switch ( fld_rec.fldtype ) { 
      case I1 : strcpy(str_fldtype, "byte"); break; 
      case I2 : strcpy(str_fldtype, "short"); break; 
      case I4 : strcpy(str_fldtype, "int"); break; 
      case I8 : strcpy(str_fldtype, "long"); break; 
      case F4 : strcpy(str_fldtype, "float"); break; 
      case F8 : strcpy(str_fldtype, "double"); break; 
      default : go_BYE(-1); break;
    }
    if ( i == 0 ) { 
      sprintf(buffer, "%s:%s", flds[i], str_fldtype);
    }
    else {
      sprintf(buffer, ", %s:%s", flds[i], str_fldtype);
    }
    if ( ( strlen(buffer) + strlen(fldstr) ) >= fldstrlen ) {
      fldstrlen *= 2;
      fldstr = realloc(fldstr, fldstrlen);
    }
    else {
      strcat(fldstr, buffer);
    }
    sprintf(oldpath, "%s/%s", g_data_dir, fld_rec.filename);
    sprintf(newpath, "%s/%s/%s", g_data_dir, fb_dir, fld_rec.name);
    struct stat st;
    if ( stat(oldpath, &st) != 0 ) { go_BYE(-1); }
    status = symlink(oldpath, newpath);
    zero_string_to_nullc(oldpath);
    zero_string_to_nullc(newpath);
    zero_string_to_nullc(buffer);
  }
  sprintf(fb_dir, "%s/_fb_%s", g_data_dir, tbl);
  fprintf(stderr, "fldstr = %s\n", fldstr);
  fprintf(stderr, "fb_dir = %s\n", fb_dir);
  status = fb_init(fb_dir, fldstr); cBYE(status);

BYE:
  free_if_non_null(fldstr);
  return(status);
}

// START FUNC DECL
int
undo_fb_idx(
    char *tbl
    )
// STOP FUNC DECL
{
  int status = 0;
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd;
  char fb_dir[MAX_LEN_TBL_NAME+1+16]; 

  if ( g_data_dir == NULL ) { go_BYE(-1); }
  zero_string(fb_dir, MAX_LEN_TBL_NAME+1+16);
  strcpy(fb_dir, "_fb_" );
  strcat(fb_dir, tbl);
  mcr_cd;
  mcr_uncd;
BYE:
  return(status);
}

// START FUNC DECL
int
x_mk_fb_idx(
    char *tbl,
    char *fld,
    char *options
    )
// STOP FUNC DECL
{
  int status = 0;
  char fb_dir[MAX_LEN_TBL_NAME+1+16]; 
  struct stat st;

  // check that data exists 
  if ( g_data_dir[strlen(g_data_dir)-1] == '/' ) {
    sprintf(fb_dir, "%s_fb_%s/%s", g_data_dir, tbl, fld);
  }
  else {
    sprintf(fb_dir, "%s/_fb_%s/%s", g_data_dir, tbl, fld);
  }
  if ( stat(fb_dir, &st) != 0 ) { 
    char cwd[MAX_LEN_DIR_NAME+1];
    getcwd(cwd, MAX_LEN_DIR_NAME);
    fprintf(stderr, "Cannot find %s in %s \n", fb_dir, cwd);
    go_BYE(-1); 
  }
  // delete index if it exists
  sprintf(fb_dir, "%s/_fb_%s/%s.idx", g_data_dir, tbl, fld);
  if ( stat(fb_dir, &st) != 0 ) { 
    unlink(fb_dir);
  }
  // -----
  zero_string(fb_dir, MAX_LEN_TBL_NAME+1+16);
  bool is_fake = false;
  if ( is_fake ) { 
    sprintf(fb_dir, "%s/_fb_%s/%s.idx", g_data_dir, tbl, fld);
    FILE *fp = fopen(fb_dir, "w");
    fclose(fp);
  }
  else {
    sprintf(fb_dir, "%s/_fb_%s", g_data_dir, tbl);
    status = mk_fb_idx(fb_dir, fld, options); cBYE(status);
  }
BYE:
  return(status);
}

// START FUNC DECL
int
is_fb_idx(
    char *tbl,
    char *fld,
    int *ptr_is_idx
    )
// STOP FUNC DECL
{
  int status = 0;
  struct stat st;
#define LEN1 (MAX_LEN_DIR_NAME+1+MAX_LEN_TBL_NAME+1+MAX_LEN_FLD_NAME+1+8)
  char fb_dir[LEN1];

  zero_string(fb_dir, LEN1);
  if ( g_data_dir == NULL ) { go_BYE(-1); }

  if ( ( fld == NULL ) || ( *fld == '\0' ) ) {
    sprintf(fb_dir, "%s/_fb_%s", g_data_dir, tbl);
  }
  else { 
    sprintf(fb_dir, "%s/_fb_%s/%s.idx", g_data_dir, tbl, fld);
  }
  if ( stat(fb_dir, &st) == 0 ) { 
    *ptr_is_idx = 1; 
  } 
  else { 
    *ptr_is_idx = 0; 
  }
BYE:
  return(status);
}

// START FUNC DECL
int
x_del_fb_idx(
    char *tbl,
    char *fld
    )
// STOP FUNC DECL
{
  int status = 0;
  char fb_dir[LEN1];
#define LEN1 (MAX_LEN_DIR_NAME+1+MAX_LEN_TBL_NAME+1+MAX_LEN_FLD_NAME+1+8)

  if ( g_data_dir == NULL ) { go_BYE(-1); }
  zero_string(fb_dir, LEN1);
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) {
    sprintf(fb_dir, "%s/_fb_%s", g_data_dir, tbl);
    status = delete_directory(fb_dir); cBYE(status);
  }
  else {
    sprintf(fb_dir, "%s/_fb_%s/%s.idx", g_data_dir, tbl, fld);
    unlink(fb_dir);
  }
BYE:
  return(status);
}
//
// START FUNC DECL
int
x_query_fb_idx(
    char *tbl,
    char *select_fld,
    char *where_clause,
    char *output_format,
    char *rslt_buf,
    int sz_rslt_buf,
    int *ptr_n_hits
    )
// STOP FUNC DECL
{
  int status = 0;
  char fb_dir[MAX_LEN_TBL_NAME+1+16]; 
  struct stat st;
  int tbl_id; TBL_REC_TYPE tbl_rec;
  int fld_id, nn_fld_id; FLD_REC_TYPE fld_rec, nn_fld_rec;

  // check that table exists
  status = is_tbl(tbl, &tbl_id, &tbl_rec);
  chk_range(tbl_id, 0, g_n_tbl);
  // check that select field exists in table
  status = is_fld(NULL, tbl_id, select_fld, &fld_id, &fld_rec, 
	&nn_fld_id, &nn_fld_rec);
  cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  // check that fastbit directory exists 
  sprintf(fb_dir, "%s/_fb_%s", g_data_dir, tbl);
  if ( stat(fb_dir, &st) != 0 ) { go_BYE(-1); }
  // -----
  sprintf(fb_dir, "%s/_fb_%s", g_data_dir, tbl);
  // -----
  int fldsz;
  status = get_fld_sz(fld_rec.fldtype, &fldsz); cBYE(status);
  status = print_query_result(select_fld, fb_dir, where_clause, 
      fld_rec.fldtype, fldsz, output_format, rslt_buf, sz_rslt_buf, ptr_n_hits);
    cBYE(status);
BYE:
  return(status);
}

