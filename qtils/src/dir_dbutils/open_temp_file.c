/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
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

// START FUNC DECL
int
open_temp_file(
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
  status = chdir(dir);
  if ( status != 0 ) { 
    fprintf(stderr, "Directory [%s] not accessible \n", dir);
    go_BYE(-1);
  }
  // Create temp file 
  strcpy(fname, "_tempf_XXXXXX");
  int fd = mkstemp(fname);
  close(fd);
  // Get back to where you once belonged
  status = chdir(cwd); 
  cBYE(status);
 BYE:
  return(status);
}
