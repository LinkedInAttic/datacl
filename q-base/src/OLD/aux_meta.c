#include "qtypes.h"
#include "mmap.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "auxil.h"
#include "dbauxil.h"

/* We need is_partial because there are times when we have added the
 * primary field and have not yet added the auxiliary fields but we want
 * to check some of the attributes */

// START FUNC DECL
int
chk_fld_meta(
    FLD_REC_TYPE M,
    bool is_partial, 
    long long nR
    )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  long long actual_filesize,  exp_filesize;
  int n_sizeof;

  status = chk_fld_name(M.name, 0);
  cBYE(status);
  status = rs_mmap(M.filename, &X, &nX, 0);
  cBYE(status);
  actual_filesize = nX;
  rs_munmap(X, nX);
  if ( M.parent_id >= 0 ) { /* this is an auxilary field */
    switch ( M.fldtype ) { 
     case nn : 
      if ( M.fldtype != I1 ) {
	go_BYE(-1);
      }
      break;
     default :
      go_BYE(-1);
      break;
    }
  }
  switch ( M.sort_type ) { 
    case unknown : break;
    case ascending : break;
    case descending : break;
    case unsorted : break;
    default : go_BYE(-1); break;
  }

  switch ( M.fldtype ) { 
    case I1     : n_sizeof = 1; break; 
    case I2    : n_sizeof = 2; break;
    case I4      : n_sizeof = 4; break;
    case I8 : n_sizeof = 8; break;
    case F4    : n_sizeof = 4; break;
    case F8   : n_sizeof = 8; break;
    case xstring   : n_sizeof = -1; break;
    default : go_BYE(-1); break;
  }
  /* Check on file size */
  if ( M.fldtype != xstring  ) {
    exp_filesize = n_sizeof * nR;
    if ( exp_filesize != actual_filesize ) { go_BYE(-1); }
  }
BYE:
  rs_munmap(X, nX);
  return(status);
}

// START FUNC DECL
void
zero_fld_rec(
    FLD_REC_TYPE *ptr_X
    )
// STOP FUNC DECL
{
  ptr_X->tbl_id = -1;
  ptr_X->is_external = false; // Assume that fields are internal 
  ptr_X->sort_type = unknown;
  ptr_X->auxtype   = undef;
  ptr_X->parent_id = -1;
  ptr_X->nn_fld_id = -1;
  ptr_X->fldtype   = xunknown;

  for ( int i = 0; i < MAX_LEN_FLD_NAME+1; i++ ) { 
    ptr_X->name[i] = '\0';
  }
  for ( int i = 0; i < MAX_LEN_FLD_DISP_NAME+1; i++ ) { 
    ptr_X->dispname[i] = '\0';
  }
  for ( int i = 0; i < MAX_LEN_FILE_NAME+1; i++ ) { 
    ptr_X->filename[i] = '\0';
  }
  ptr_X->magic_val = 0;
  ptr_X->is_dirty  = TRUE;
  ptr_X->num_readers  = 0;
}

// START FUNC DECL
int
pr_fld_meta(
    FLD_REC_TYPE X
    )
// STOP FUNC DECL
{
  int status = 0;
  fprintf(stdout, "tbl_id,%d\n", X.tbl_id);
  fprintf(stdout, "name,%s\n", X.name);
  fprintf(stdout, "dispname,%s\n", X.dispname);
  if ( X.fldtype == xstring ) { 
    fprintf(stdout, "dictionary,%s\n", X.dictionary);
  }
  else {
    fprintf(stdout, "filename,%s\n", X.filename);
  }
  switch ( X.fldtype ) { 
    case I1 : fprintf(stdout, "fldtype,I1\n"); break;
    case I2 : fprintf(stdout, "fldtype,I2\n"); break;
    case I4 : fprintf(stdout, "fldtype,I4\n"); break;
    case I8 : fprintf(stdout, "fldtype,I8\n"); break;
    case F4 : fprintf(stdout, "fldtype,F4\n"); break;
    case F8 : fprintf(stdout, "fldtype,F8\n"); break;
    case xstring   : fprintf(stdout, "fldtype,string\n"); break;
    default : go_BYE(-1); break;
  }
  switch ( X.sort_type ) { 
    case unknown     : fprintf(stdout, "sort,unknown\n"); break;
    case ascending   : fprintf(stdout, "sort,ascending\n"); break;
    case descending  : fprintf(stdout, "sort,descending\n"); break;
    case unsorted    : fprintf(stdout, "sort,unsorted\n"); break;
    default : go_BYE(-1);
  }
  fprintf(stdout, "parent_id,%d\n", X.parent_id);
  fprintf(stdout, "nn_fld_id,%d\n", X.nn_fld_id);
  fprintf(stdout, "is_external,%d\n", X.is_external);
BYE:
  return(status);
}
//---------------------------------------------------------------
// START FUNC DECL
int
chk_fld_name(
    const char *in_X,
    int is_aux /* 0 => false, 1 => true, -1 => dont care */
    )
// STOP FUNC DECL
{
  int status = 0;
  char *X;
  X = (char *)in_X;
  if ( ( X == NULL ) || ( *X == '\0' ) || ( strlen(X) > MAX_LEN_FLD_NAME )
      ) { go_BYE(-1); } 
  for ( ; *X != '\0'; X++ ) {
    if ( ( *X == '_' ) || ( isalnum(*X) ) ) {
      // all is well 
    }
    else { go_BYE(-1); }
  }
  X = (char *)in_X;
  switch ( is_aux ) { 
    case 0 : 
      break;
    case 1 : 
      if ( ( strncmp(X, "_nn_", 4) == 0 ) || 
           ( strncmp(X, "_sz_", 4) == 0 ) ) {
	/* all is well */
      }
      else { go_BYE(-1); }
    break;
    default : 
    go_BYE(-1); 
    break;
  }
BYE:
  return(status);
}

// START FUNC DECL
int 
fld_meta_to_str(
    FLD_REC_TYPE fld_meta,
    char *buffer
    )
// STOP FUNC DECL
{
  int status = 0;
  char buf[8];

  zero_string(buf, 8);
  if ( buffer == NULL ) { go_BYE(-1); }
  if ( *buffer != '\0' ) { go_BYE(-1); }

  if (( fld_meta.filename == NULL ) || ( fld_meta.filename[0] == '\0')){ 
    go_BYE(-1);
  }
  strcat(buffer, "filename=");
  strcat(buffer, fld_meta.filename);

  sprintf(buf, "%d", fld_meta.fldtype);
  strcat(buffer, ":fldtype=");
  strcat(buffer, buf);

BYE:
  return(status);
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
    case I1     : strcpy(str_fldtype, "char"); break;
    case I2    : strcpy(str_fldtype, "I2"); break;
    case I4      : strcpy(str_fldtype, "I4"); break;
    case F4    : strcpy(str_fldtype, "float"); break;
    case I8 : strcpy(str_fldtype, "I8"); break;
    case F8   : strcpy(str_fldtype, "double"); break;
    case xstring    : strcpy(str_fldtype, "string"); break;
    default : go_BYE(-1); break;
  }
BYE:
return(status);
}
//
// START FUNC DECL
int
get_fld_sz(
    int fldtype,
    int *ptr_fld_sz
    )
// STOP FUNC DECL
{
  int status = 0;
  switch ( fldtype ) { 
    case I1     : *ptr_fld_sz = 1; break;
    case I2     : *ptr_fld_sz = 2; break;
    case I4     : *ptr_fld_sz = 4; break;
    case F4     : *ptr_fld_sz = 4; break;
    case I8     : *ptr_fld_sz = 8; break;
    case F8   : *ptr_fld_sz = 8; break;

    case xstring   : 
    default : go_BYE(-1); break;
  }
BYE:
return(status);
}
// START FUNC DECL
int
unstr_fldtype(
    char *str_fldtype,
    FLD_TYPE *ptr_fldtype
    )
// STOP FUNC DECL
{
  int status = 0;
  if ( str_fldtype == NULL ) { go_BYE(-1); }

  if ( strcmp(str_fldtype, "I1") == 0 ) { *ptr_fldtype = I1; return(status);}
  if ( strcmp(str_fldtype, "I2") == 0 ) { *ptr_fldtype = I2; return(status); }
  if ( strcmp(str_fldtype, "I4") == 0 ) { *ptr_fldtype = I4; return(status); }
  if ( strcmp(str_fldtype, "F4") == 0 ) { *ptr_fldtype = F4; return(status); }
  if ( strcmp(str_fldtype, "I8") == 0 ) { *ptr_fldtype = I8; return(status); }
  if ( strcmp(str_fldtype, "F8") == 0 ) { *ptr_fldtype = I8; return(status); }
  if ( strcmp(str_fldtype, "string")  == 0 ) { *ptr_fldtype = xstring; return(status); }
  go_BYE(-1); 
BYE:
return(status);
}


// START FUNC DECL
void
zero_tbl_rec(
    TBL_REC_TYPE *ptr_X
    )
// STOP FUNC DECL
{
  ptr_X->nR = 0;
  for ( int i = 0; i < MAX_LEN_TBL_NAME+1; i++ ) { 
    ptr_X->name[i] = '\0';
  }
  for ( int i = 0; i < MAX_LEN_TBL_DISP_NAME+1; i++ ) { 
    ptr_X->dispname[i] = '\0';
  }
  ptr_X->magic_val = 0;
  ptr_X->num_readers  = 0;
}

// START FUNC DECL
int
chk_tbl_name(
    char *X
    )
// STOP FUNC DECL
{
  int status = 0;
  if ( ( X == NULL ) || ( *X == '\0' ) || 
      ( strlen(X) > MAX_LEN_TBL_NAME )) { 
    go_BYE(-1); 
  }
  for ( ; *X != '\0'; X++ ) {
    if ( ( isalnum(*X) ) || ( *X == '_' ) ) { 
      /* all is well */
    }
    else { 
      fprintf(stderr, "[%s] is invalid tbl name \n", X); go_BYE(-1); 
    }
  }
BYE:
  return(status);
}

// START FUNC DECL
void
copy_fld_meta(
  FLD_REC_TYPE *ptr_dst_fld_meta,
  FLD_REC_TYPE *src_fld_meta
  )
// STOP FUNC DECL
{
  ptr_dst_fld_meta->tbl_id      = src_fld_meta->tbl_id;
  ptr_dst_fld_meta->is_external = src_fld_meta->is_external; 
  ptr_dst_fld_meta->sort_type    = src_fld_meta->sort_type;
  ptr_dst_fld_meta->parent_id   = src_fld_meta->parent_id;
  ptr_dst_fld_meta->nn_fld_id   = src_fld_meta->nn_fld_id; 
  ptr_dst_fld_meta->fldtype     = src_fld_meta->fldtype; 
  strcpy(ptr_dst_fld_meta->name, src_fld_meta->name);
  strcpy(ptr_dst_fld_meta->filename, src_fld_meta->filename);
  strcpy(ptr_dst_fld_meta->dispname, src_fld_meta->dispname);

}
