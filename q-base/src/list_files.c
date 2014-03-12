#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "auxil.h"
#include "meta_globals.h"
#include "list_files.h"

extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
list_files(
    char *in_data_dir
	 )
// STOP FUNC DECL
{
  int status = 0;
  struct stat st;
  FILE *ofp = NULL;
  int ddir_id = INT_MAX;
  char data_dir[MAX_LEN_DIR_NAME+1];
  char filename[32];

  if ( ( in_data_dir == NULL ) || ( *in_data_dir == '\0' ) )  {
    ddir_id = 0; 
    status = chdir(g_ddirs[0].name); cBYE(status);
  }
  else {
    status = strip_trailing_slash(in_data_dir, data_dir, MAX_LEN_DIR_NAME+1);
    cBYE(status);
    status = get_ddir_id(data_dir, g_ddirs, g_n_ddir, false, &ddir_id);
    cBYE(status);
    if ( ddir_id < 0 ) { go_BYE(-1); }
    status = chdir(in_data_dir); cBYE(status);
  }

  ofp = stdout;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].name[0] == '\0' ) { continue; } /* null entry */
    if ( ddir_id != g_flds[i].ddir_id ) { continue; }
    mk_file_name(filename, g_flds[i].fileno);
    status = stat(filename, &st); cBYE(status);
    unsigned long long size = st.st_size;
    if ( size <= 0 ) { go_BYE(-1); }
    fprintf(ofp, "%d,%llu,_%d\n", g_flds[i].is_external,size,g_flds[i].fileno);
  }
BYE:
  status = chdir(g_cwd);
  return(status);
}
