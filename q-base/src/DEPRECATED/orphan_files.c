#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "sqlite3.h"
#include "orphan_files.h"
#include "auxil.h"
#include "dbauxil.h"

// START FUNC DECL
int
orphan_files(
	 char *docroot,
	 sqlite3 *in_db,
	 char *indir
	 )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char **files = NULL; int num_files;
  char cwd[MAX_LEN_CWD]; char *dir = NULL;
  int len;
  //------------------------------------------------
  zero_string(cwd, MAX_LEN_CWD);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //------------------------------------------------
  status = files_in_docroot(docroot, db, &files, &num_files);
  cBYE(status);
  //------------------------------------------------
  if ( ( indir == NULL ) || ( *indir == '\0' ) ) {
    if ( getcwd(cwd, MAX_LEN_CWD) == NULL ) { go_BYE(-1); }
    len = strlen(cwd) + 4;
    dir = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(dir);
    zero_string(dir, len);
    strcpy(dir, cwd);
    len = strlen(dir);
    dir[len] = '/'; /* append a slash */
  }
  else {
    if ( indir[0] != '/' ) { go_BYE(-1); }
    len = strlen(indir) + 4;
    dir = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(dir);
    zero_string(dir, len);
    strcpy(dir, indir);
    len = strlen(dir);
    if ( dir[len-1] != '/' ) {
      dir[len] = '/'; /* append a slash */
    }
  }
    
 BYE:
 free_if_non_null(dir);
  if ( files != NULL ) { 
    for ( int i = 0; i < num_files; i++ ) { 
      free_if_non_null(files[i]);
    }
    free_if_non_null(files);
  }
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}

// START FUNC DECL
int
files_in_docroot(
	    char *docroot,
	    sqlite3 *db,
	    char ***ptr_files,
	    int *ptr_num_files
	    )
// STOP FUNC DECL
{
  int status = 0;
  char **files = NULL; int num_files = 0; int *fld_ids = NULL;
  char qstr[4096];

  zero_string(qstr, 4096);
  *ptr_files = NULL; 
  *ptr_num_files = 0;
  //--------------------------------------------------
  // Find number of fields in this docroot 
  sprintf(qstr, "select id from fld where id > 0 ");
  status = db_get_mult_ival(db, qstr, &fld_ids, &num_files);
  cBYE(status);
  if ( num_files == 0 ) { goto BYE; }
  //------------------------------------------------
  // Allocate space for files 
  files = (char **)malloc(num_files * sizeof(char *));
  return_if_malloc_failed(files);
  //------------------------------------------------
  for ( int i = 0; i < num_files; i++ ) { 
    zero_string_to_nullc(qstr);
    sprintf(qstr, "select filename from fld where id = %d ", fld_ids[i]);
    status = db_get_sval(db, qstr, files+i);
    cBYE(status);
  }
  //------------------------------------------------
  *ptr_files = files;
  *ptr_num_files = num_files;
BYE:
  free_if_non_null(fld_ids);
  return(status);
}
