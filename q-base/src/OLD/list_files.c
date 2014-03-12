#include <dirent.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "sort.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "lock_stuff.h"
#include "meta_data.h"

extern char *g_docroot;
// START FUNC DECL
int
list_files(
	 )
// STOP FUNC DECL
{
  int status = 0;
  struct stat st;
  size_t size;
  FILE *ofp = NULL;
#include "meta_map.h"

  ofp = stdout;
  status = mmap_meta_data(g_docroot, 
	&tbl_X, &tbl_nX, &tbls, &n_tbl, 
	&ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
	&fld_X, &fld_nX, &flds, &n_fld, 
	&ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
    cBYE(status);
  status = get_rd_lock(); cBYE(status);
  for ( int i = 0; i < n_fld; i++ ) { 
    if ( flds[i].name[0] != '\0' ) { /* entry in use */
      stat(flds[i].filename, &st);
      size = st.st_size;
      fprintf(ofp, "%d,%lld,%s\n", 
	  flds[i].is_external, 
	  (long long)size, 
	  flds[i].filename);
    }
  }
BYE:
  release_rd_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
	fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  return(status);
}

// START FUNC DECL
int
orphan_files(
    )
// STOP FUNC DECL
{
  int status = 0;
  DIR *dp = NULL; FILE *ofp = NULL;
  struct dirent *ep = NULL;     
  char Xopfile[MAX_LEN_FILE_NAME+1];
  char Yopfile[MAX_LEN_FILE_NAME+1];
  char *X = NULL; size_t nX = 0;
  char *Y = NULL; size_t nY = 0;
  char nullc = '\0';
  int Xnum_files = 0, Ynum_files = 0;
  char cwd[MAX_LEN_DIR_NAME+1];

  zero_string(Xopfile, MAX_LEN_FILE_NAME+1);
  zero_string(Yopfile, MAX_LEN_FILE_NAME+1);
#include "meta_map.h"

  /*
  zero_string(cwd, MAX_LEN_DIR_NAME+1);
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  if ( ( g_docroot == NULL ) || ( *g_docroot == '\0' ) )  {go_BYE(-1); }
  status = chdir(g_docroot); cBYE(status);
  */
  status = mmap_meta_data(g_docroot, 
	&tbl_X, &tbl_nX, &tbls, &n_tbl, 
	&ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
	&fld_X, &fld_nX, &flds, &n_fld, 
	&ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
    cBYE(status);

  /* START: Create X, which consists of filenames sorted in ascending order */
  status = open_temp_file(Xopfile, 0); cBYE(status);
  ofp = fopen(Xopfile, "w"); 
  return_if_fopen_failed(ofp, Xopfile, "w");
  for ( int i = 0; i < n_fld; i++ ) { 
    if ( flds[i].name[0] != '\0' ) { /* entry in use */
      Xnum_files++;
      fwrite(flds[i].filename, sizeof(char), MAX_LEN_FILE_NAME+1, ofp);
    }
  }
  fclose_if_non_null(ofp);
  if ( Xnum_files > 0 ) { 
    status = rs_mmap(Xopfile, &X, &nX, 1);
    qsort(X, Xnum_files, MAX_LEN_FILE_NAME+1, str_compare);
  }
  /* STOP ---- */

  status = open_temp_file(Yopfile, 0); cBYE(status);
  ofp = fopen(Yopfile, "w"); 
  return_if_fopen_failed(ofp, Xopfile, "w");
  if ( ( g_docroot == NULL ) || ( *g_docroot == '\0' ) )  { go_BYE(-1); }
  dp = opendir(g_docroot);
  if ( dp == NULL) { go_BYE(-1); }
  for ( ; ; ) { 
    ep = readdir (dp);
    if ( ep == NULL ) { break; }
    char *filename = ep->d_name;
    if ( ( filename == NULL ) || ( *filename == '\0' ) )  { break; }
    if ( strncmp(filename, "docroot", 7) == 0  ) { continue; }
    if ( strcmp(filename, "..") == 0  ) { continue; }
    int len = strlen(filename);
    if ( len > MAX_LEN_FILE_NAME ) {
      // This is clearly no good 
      fprintf(stdout, "%s\n", filename);
    }
    else {
      Ynum_files++;
      fwrite(filename, sizeof(char), len, ofp);
      for ( int i = len; i < MAX_LEN_FILE_NAME+1; i++ ) { 
        fwrite(&nullc, sizeof(char), 1, ofp);
      }
    }
  }
  fclose_if_non_null(ofp);
  (void) closedir (dp);
  if ( Ynum_files > 0 ) { 
    status = rs_mmap(Yopfile, &Y, &nY, 1);
    qsort(Y, Ynum_files, MAX_LEN_FILE_NAME+1, str_compare);
  }
  /* Now compare X against Y */
  int xidx = 0, yidx = 0;
  char *xptr = X;
  char *yptr = Y;
  int seenX = 0, seenY = 0;
  for ( ; ( ( seenX < nX ) && ( seenY < nY ) ) ; ) {
    int cmpval = str_compare(X, Y);
    switch ( cmpval ) { 
      case 0 : 
      /* File in meta data exists on disk. This is good */
      xptr += MAX_LEN_FILE_NAME;
      yptr += MAX_LEN_FILE_NAME;
      seenX++;
      seenY++;
      break;
      case 1 : 

      break;
      case -1 : 
      break;
      default : 
      go_BYE(-1);
      break;
    }
  }


BYE:
  fclose_if_non_null(ofp);
  unlink_if_non_null(Xopfile); 
  unlink_if_non_null(Yopfile);
  return(status);
}
