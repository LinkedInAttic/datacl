#include <dirent.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "auxil.h"
#include "meta_globals.h"
#include "orphan_files.h"

extern char g_cwd[MAX_LEN_DIR_NAME+1];
extern char *g_data_dir;
#define MAX_DIGITS_IN_I4 12
// START FUNC DECL
int
orphan_files(
    char *in_data_dir
    )
// STOP FUNC DECL
{
  int status = 0;
  DIR *dp = NULL; 
  struct dirent *ep = NULL;     
  int ddir_id;
  char data_dir[MAX_LEN_DIR_NAME+1];
  int files_in_meta[MAX_NUM_FLDS]; int num_files_in_meta = 0;
  bool is_seen[MAX_NUM_FLDS];

  zero_string(data_dir, (MAX_LEN_DIR_NAME+1));
  for ( int i = 0; i < MAX_NUM_FLDS; i++ ) { is_seen[i] = 0; } 
  /*------------------------------------------------------------------*/

  if ( ( in_data_dir == NULL ) || ( *in_data_dir == '\0' ) ) {
    if ( strlen(g_data_dir) >= MAX_LEN_DIR_NAME ) { go_BYE(-1); }
    strcpy(data_dir, g_data_dir);
    ddir_id = -1;
    status = chdir(g_data_dir); cBYE(status);
  }
  else {
    status = strip_trailing_slash(in_data_dir, data_dir, MAX_LEN_DIR_NAME+1);
    cBYE(status);
    status = get_ddir_id(data_dir, g_ddirs, g_n_ddir, false, &ddir_id);
    cBYE(status);
    status = chdir(data_dir); cBYE(status);
  }
  /* START: Create files_in_meta =  files listed in the meta data (whose 
   * ddir_id matches that requested) in sorted order */
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].name[0] == '\0' ) { continue; }
    if ( g_flds[i].ddir_id != ddir_id ) { continue; }
    files_in_meta[num_files_in_meta++] = g_flds[i].fileno;
  }
  /* STOP ---- */
  dp = opendir(data_dir);
  if ( dp == NULL) { go_BYE(-1); }
  for ( ; ; ) { 
    ep = readdir (dp);
    if ( ep == NULL ) { break; }
    char *filename = ep->d_name;
    if ( ( filename == NULL ) || ( *filename == '\0' ) )  { break; }
    if ( strncmp(filename, "docroot", 7) == 0  ) { continue; }
    if ( strcmp(filename, "..") == 0  ) { continue; }
    if ( strcmp(filename, ".") == 0  ) { continue; }
    int len = strlen(filename);
    bool is_bad = false;
    if ( len >= MAX_DIGITS_IN_I4 ) { is_bad = true; }
    if ( filename[0] != '_'  ) { is_bad = true; }
    for ( char *cptr = filename+1; *cptr != '\0'; cptr++ ) { 
      if ( !isdigit(*cptr) ) { is_bad = true; break; }
    }
    /* Check if it is in meta data */
    char *endptr;
    int fileno = strtoll(filename+1, &endptr, 10); 
    if ( *endptr != '\0' ) { go_BYE(-1); }
    bool is_in_meta = false;
    for ( int i = 0; i < num_files_in_meta; i++ ) { 
      if ( fileno == files_in_meta[i] ) {
	is_seen[i]= true;
        is_in_meta = true;
	break; 
      }
    }
    if ( is_in_meta == false ) { is_bad = true; }
    if ( is_bad ) { 
      fprintf(stdout, "%s\n", filename);
    }
  }
  /* Now look for files that are not on disk. This is catastrophic */
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].name[0] == '\0' ) { continue; }
    if ( g_flds[i].ddir_id != ddir_id ) { continue; }
    int fileno = g_flds[i].fileno;
    for ( int j = 0; j < num_files_in_meta; j++ ) { 
      if ( files_in_meta[j] != fileno ) { continue; }
      if ( is_seen[j] == false ) { 
	fprintf(stdout,"CATASTROPHIC,%d\n", fileno);
      }
    }
  }
BYE:
  chdir(g_cwd);
  return(status);
}
