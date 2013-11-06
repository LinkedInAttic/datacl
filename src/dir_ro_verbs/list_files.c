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
#include <dirent.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "sort.h"
#include "aux_meta.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "meta_globals.h"

extern char *g_docroot;
extern char *g_data_dir;
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
  size_t size;
  FILE *ofp = NULL;
  int ddir_id = -1;
  char data_dir[MAX_LEN_DIR_NAME+1];

  if ( ( in_data_dir == NULL ) || ( *in_data_dir == '\0' ) )  {
    chdir(g_data_dir);
  }
  else {
    status = strip_trailing_slash(in_data_dir, data_dir, MAX_LEN_DIR_NAME+1);
    cBYE(status);
    status = get_ddir_id(data_dir, g_ddirs, g_n_ddir, false, &ddir_id);
    cBYE(status);
    chdir(in_data_dir);
  }

  ofp = stdout;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].name[0] == '\0' ) { continue; } /* null entry */
    int this_ddir_id = g_flds[i].ddir_id;
    if ( ddir_id >= 0 ) { 
      if ( this_ddir_id != ddir_id ) { continue; }
    }
    if ( this_ddir_id < 0 ) { 
      chdir(g_data_dir);
    }
    else {
      chdir(g_ddirs[this_ddir_id].name);
    }
    status = stat(g_flds[i].filename, &st); cBYE(status);
    size = st.st_size;
    if ( size <= 0 ) { go_BYE(-1); }
    fprintf(ofp, "%d,%d,%lld,%s\n", 
	  g_flds[i].is_external, 
	  g_flds[i].ddir_id, 
	  (long long)size, 
	  g_flds[i].filename);
    chdir(g_cwd);
  }
BYE:
  return(status);
}

// START FUNC DECL
int
orphan_files(
    char *in_data_dir,
    char *action
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
  int ddir_id;
  char data_dir[MAX_LEN_DIR_NAME+1];

  if ( ( in_data_dir == NULL ) || ( *in_data_dir == '\0' ) ) {
    status = chdir(g_data_dir); cBYE(status);
    ddir_id = -1;
    if ( strlen(g_data_dir) >= MAX_LEN_DIR_NAME ) { go_BYE(-1); }
    strcpy(data_dir, g_data_dir);
  }
  else {
    status = strip_trailing_slash(in_data_dir, data_dir, MAX_LEN_DIR_NAME+1);
    cBYE(status);
    status = get_ddir_id(data_dir, g_ddirs, g_n_ddir, false, &ddir_id);
    cBYE(status);
    status = chdir(data_dir); cBYE(status);
  }

  zero_string(Xopfile, MAX_LEN_FILE_NAME+1);
  zero_string(Yopfile, MAX_LEN_FILE_NAME+1);

  /* START: Create X. X = filenames listed in the meta data (whose 
   * ddir_id matches that requested) in sorted order */
  status = open_temp_file(g_cwd, g_cwd, Xopfile, 0); cBYE(status);
  ofp = fopen(Xopfile, "w"); 
  return_if_fopen_failed(ofp, Xopfile, "w");
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].name[0] == '\0' ) { continue; }
    if ( g_flds[i].ddir_id != ddir_id ) { continue; }
    Xnum_files++;
    fwrite(g_flds[i].filename, sizeof(char), MAX_LEN_FILE_NAME+1, ofp);
  }
  fclose_if_non_null(ofp);
  if ( Xnum_files > 0 ) { 
    status = rs_mmap(Xopfile, &X, &nX, 1);
    qsort(X, Xnum_files, MAX_LEN_FILE_NAME+1, str_compare);
  }
  /* STOP ---- */

  status = open_temp_file(g_cwd, g_cwd, Yopfile, 0); cBYE(status);
  ofp = fopen(Yopfile, "w"); 
  return_if_fopen_failed(ofp, Xopfile, "w");
  dp = opendir(data_dir);
  if ( dp == NULL) { go_BYE(-1); }
  for ( ; ; ) { 
    ep = readdir (dp);
    if ( ep == NULL ) { break; }
    char *filename = ep->d_name;
    if ( ( filename == NULL ) || ( *filename == '\0' ) )  { break; }
    if ( strncmp(filename, "docroot", 7) == 0  ) { continue; }
    if ( strncmp(filename, ".LDB_META", 9) == 0  ) { continue; }
    if ( strcmp(filename, "..") == 0  ) { continue; }
    if ( strcmp(filename, ".") == 0  ) { continue; }
    int len = strlen(filename);
    if ( len >= MAX_LEN_FILE_NAME ) {
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
  char *xptr = X;
  char *yptr = Y;
  int seenX = 0, seenY = 0;
  for ( ; ( ( seenX < Xnum_files ) && ( seenY < Ynum_files ) ) ; ) {
    int cmpval = str_compare(xptr, yptr);
    switch ( cmpval ) { 
      case 0 : 
      /* File in meta data exists on disk. This is good */
      xptr += MAX_LEN_FILE_NAME+1;
      yptr += MAX_LEN_FILE_NAME+1;
      seenX++;
      seenY++;
      break;
      case -1 : 
      /* File exists in meta data but not on disk. This is horrendous */
      printf("HORRENDOUS %s\n", yptr);
      xptr += MAX_LEN_FILE_NAME+1;
      seenX++;

      break;
      case 1 : 
      /* File on disk but not in meta data. This is an "orphan" * */
      printf("%s\n", yptr);
      yptr += MAX_LEN_FILE_NAME+1;
      seenY++;
      break;
      default : 
      go_BYE(-1);
      break;
    }
  }
  if ( seenX < Xnum_files ) {
    for ( ; seenX < Xnum_files ; seenX++ ) {
      fprintf(stderr, "MISSING FILE %s\n", xptr);
      xptr += MAX_LEN_FILE_NAME;
      seenX++;
    }
  }
  if ( seenY < Ynum_files ) {
    for ( ; seenY < Ynum_files ; seenY++ ) {
      fprintf(stderr, "%s\n", yptr);
      yptr += MAX_LEN_FILE_NAME;
      seenY++;
    }
  }


BYE:
  fclose_if_non_null(ofp);
  unlink_if_non_null(Xopfile); 
  unlink_if_non_null(Yopfile);
  return(status);
}
