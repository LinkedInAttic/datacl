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
#include <stdio.h>
#include "qtypes.h"
#include "open_temp_file.h"
#include "mmap.h"
#include "auxil.h"
#include "mk_file.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

extern char *g_data_dir;
extern int g_alt_ddir_id;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
mk_temp_file(
    char *opfile,
    size_t filesz,
    int *ptr_ddir_id
    )
// STOP FUNC DECL
{
  int status = 0;
  char *dir = NULL;

  if ( g_data_dir == NULL ) { go_BYE(-1); }
  if ( g_alt_ddir_id < 0 ) { 
    dir = g_data_dir; 
    *ptr_ddir_id = -1;
  }
  else {
    dir = g_ddirs[g_alt_ddir_id].name;
    if ( filesz > 0 ) {
      unsigned long long free_space;
      status = get_disk_space(dir, &free_space, "free_space"); cBYE(status);
      if ( free_space < filesz ) {
	*ptr_ddir_id = -1;
      }
      else {
        *ptr_ddir_id = g_alt_ddir_id;
      }
    }
    else {
     *ptr_ddir_id = g_alt_ddir_id;
    }
  }
  status = open_temp_file(g_cwd, dir, opfile, filesz); cBYE(status);
  status = mk_file(g_cwd, dir, opfile, filesz); cBYE(status);
BYE:
  return(status);
}
