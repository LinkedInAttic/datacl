#include "qtypes.h"
#include "mmap.h"
#include "mk_file.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "open_temp_file.h"

extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];

// TODO P0 Serious review needed
// START FUNC DECL
int
flush(
      char *from_dir,
      char *tbl,
      char *fld
      )
// STOP FUNC DECL
{
  int status = 0;
  int from_ddir_id = -1;
  char *from_X = NULL; size_t from_nX = 0;
  char *to_X   = NULL; size_t to_nX   = 0;
  char filename[32];
  TBL_REC_TYPE tbl_rec; int tbl_id;
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;


  if ( ( tbl == NULL ) || ( *tbl == '\0' ) )  {
    tbl_id = -1;
    fld_id = -1;
  }
  else {
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
    if ( tbl_id >= 0 ) {
      if ( ( fld == NULL ) || ( *fld == '\0' ) )  {
	fld_id = -1;
      }
      else {
        status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, 
			&nn_fld_rec); 
        cBYE(status);
      }
    }
  }
  if ( ( from_dir == NULL ) || ( *from_dir == '\0' ) ) {
    from_ddir_id = -1; // indicates match all ddir
  }
  else {
    if ( strcmp(from_dir, g_data_dir) == 0 ) { return status ; }
    for ( int i = 0; i < g_n_ddir; i++ ) {
      if ( strcmp(g_ddirs[i].name, from_dir) == 0 ) {
	from_ddir_id = i;
	break;
      }
    }
    if ( from_ddir_id < 0 ) {
      fprintf(stderr, "Unknown directory. Doing nothing\n");
      return status ;
    }
  }
  int x_status = 0;
  // Needs a lot of work TODO P0
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].name[0] == '\0' ) { continue; } // Null entry
    if ( tbl_id >= 0 ) { 
      if ( g_flds[i].tbl_id != tbl_id ) { continue; }
    }
    if ( fld_id >= 0 ) {
      if ( i != fld_id ) { continue; }
    }
    int ddir_id = g_flds[i].ddir_id;
    if ( ddir_id < 0 ) { continue; } // Already in Q_DATA_DIR
    if ( ( from_ddir_id < 0 ) || ( from_ddir_id == ddir_id ) ) {
      /* Move this file to Q_DATA_DIR */
      // Get a handle on the source 
      status = chdir(from_dir); cBYE(status);
      sprintf(filename, "_%d", g_flds[i].fileno);
      status = rs_mmap(filename, &from_X, &from_nX, 0); 
      if ( status < 0 ) { x_status = -1; WHEREAMI; continue; }
      if ( from_nX <= 0 ) { x_status = -1; WHEREAMI; continue; }
      // Now get a handle on the destination 
      status = open_temp_file(g_cwd, g_data_dir, filename, from_nX); 
      if ( status < 0 ) { x_status = -1; WHEREAMI; continue; }
      
      status = mk_file(g_cwd, g_data_dir, filename, from_nX); 
      if ( status < 0 ) { x_status = -1; WHEREAMI; continue; }
      status = chdir(g_data_dir); cBYE(status);
      status = rs_mmap(filename, &to_X, &to_nX, 1); 
      if ( status < 0 ) { x_status = -1; WHEREAMI; continue; }
      if ( to_nX != from_nX ) { x_status = -1; WHEREAMI; continue; }
      memcpy(to_X, from_X, from_nX);
      // Delete old file 
      munmap(from_X, from_nX);
      munmap(to_X, to_nX);
      status = chdir(from_dir); cBYE(status);
      unlink(g_flds[i].filename);
      status = chdir(g_cwd); cBYE(status);
      // Update meta data 
      g_flds[i].ddir_id = -1; // indicates that it is in Q_DATA_DIR
      strcpy(g_flds[i].filename, filename);
    }
  }
  if ( x_status < 0 ) { go_BYE(-1); }
 BYE:
  return status ;
}
