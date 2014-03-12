#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "open_temp_file.h"

int 
avail_space(
    char *dir,
    size_t *ptr_avail_space
    )
{
  int status = 0;
  struct statvfs sbuf;

  status = statvfs(dir , &sbuf); cBYE(status);
  *ptr_avail_space = sbuf.f_bsize * sbuf.f_bavail;
BYE:
  return(status);
}

/* TODO: Need to use mutex to make sure that when we look for space to
 * write, that two simultaneous writers don't believe that they
 * have enough space and then clobber each other */

extern bool g_write_to_temp_dir;
// START FUNC DECL
int
open_temp_file(
	       char *fname,
	       size_t filesize
	       )
// STOP FUNC DECL
{
  int status = 0;
  char cwd[MAX_LEN_DIR_NAME+1];
  int fd; 
  char *file_dir = NULL;
  size_t space_available = 0;
  char *data_dir = getenv("Q_DATA_DIR");

  if ( data_dir == NULL ) { go_BYE(-1); }
  zero_string(cwd, MAX_LEN_DIR_NAME+1);
  getcwd(cwd, MAX_LEN_DIR_NAME);
  if ( strlen(cwd) == 0 ) { go_BYE(-1); }
  
  if ( ( filesize > 0 ) && ( g_write_to_temp_dir ) ) {
    file_dir = getenv("Q_TEMP_DIR");
    if ( file_dir == NULL ) { 
      file_dir = data_dir;
    }
    else { 
      /* Check that direcory is accessible */
      status = chdir(file_dir);
      if ( status != 0 ) { 
        fprintf(stderr, "temp dir [%s] not accessible \n", file_dir);
        file_dir = data_dir;
      }
      else {
	/* Check that there is space to write */
	char *tempfs_root = getenv("Q_TEMPFS_ROOT");
	if ( tempfs_root == NULL ) { go_BYE(-1); }
	status = avail_space(tempfs_root, &space_available); cBYE(status);
	if ( space_available <= filesize  ) {
	  /* No space on temp dir. Use DATA_DIR instead */
	  file_dir = data_dir;
	}
      }
      status = chdir(cwd);
    }
  }
  else {
    file_dir = data_dir;
  }
  // Some basic checks 
  if ( ( file_dir == NULL ) || ( *file_dir == '\0' ) ) { go_BYE(-1); }
  status = chdir(file_dir);
  if ( status != 0 ) { 
    fprintf(stderr, "Unable to cd to dir [%s] \n", file_dir);
    go_BYE(-1);
  }
  // Create temp file 
  strcat(fname, "_tempf_XXXXXX");
  fd = mkstemp(fname);
  close(fd);
  // Get back to where you once belonged
  status = chdir(cwd); cBYE(status);

 BYE:
  return(status);
}
