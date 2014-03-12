#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "auxil.h"

// START FUNC DECL
int
open_file_in_dir(
		 char *cwd,
		 char *dir,
		 char *fname,
		 size_t filesize
		 )
// STOP FUNC DECL
{
  int status = 0;

  if ( cwd == NULL ) { go_BYE(-1); }
  if ( dir == NULL ) { go_BYE(-1); }
  if ( fname == NULL ) { go_BYE(-1); }
  FILE *fp = NULL;
  
  if ( filesize > 0 ) {
    /* Check that there is space to write */
    size_t space_available;
    status = avail_space(dir, &space_available); cBYE(status);
    if ( space_available <= filesize  ) {
      fprintf(stderr, "Space (%lld bytes) in directory [%s] not available \n", 
	      (long long)filesize, dir);
      go_BYE(-1);
    }
  }
  /* Check that directory is accessible */
  status = chdir(dir); cBYE(status);
  if ( status != 0 ) { 
    fprintf(stderr, "Directory [%s] not accessible \n", dir);
    go_BYE(-1);
  }
  // Create temp file 
  fp = fopen(fname, "wb"); 
  return_if_fopen_failed(fp,  fname, "wb"); 
  fclose_if_non_null(fp);
  // Get back to where you once belonged
  status = chdir(cwd); cBYE(status);
 BYE:
  return(status);
}
