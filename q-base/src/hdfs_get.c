#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "qtypes.h"
#include "mmap.h"
#include "hdfs_get.h"
#include "aux_hdfs.h"
#include "dbauxil.h"
#include "extract_S.h"

#define BUFSIZE 32765 
static int g_num_cols;
static int g_col_idx;
static int g_buf_idx;
static int g_status;
static char g_buffer[BUFSIZE]; // EXPAND WHEN YOU DEAL WITH STRINGS
static FLD_TYPE *g_fldtypes;
static int *g_fldsz;
static FILE**g_binfps;
static FILE**g_nn_binfps;
static bool *g_has_null_vals;
static char g_fld_sep;

static size_t 
WriteMemoryCallback(
		    void *contents, 
		    size_t size, 
		    size_t nmemb, 
		    void *userp
		    )
{
  size_t realsize = size * nmemb;
  char *inbuf = (char *)contents;
  bool got_cell = false;
  char      valI1;
  short     valI2;
  int       valI4;
  long long valI8;
  float     valF4;
  double    valF8;
  char nn = 1;
  char eoln = '\n';
  for ( int i = 0; i < realsize; i++ ) {
    if ( g_status < 0 ) { break; }
    if ( g_buf_idx >= BUFSIZE ) { 
      WHEREAMI; g_status = -1; break;
    }
    //--------------------------------------------
    if ( ( inbuf[i] == g_fld_sep ) || ( inbuf[i] == '\n' ) ) {
      if ( ( inbuf[i] == '\n' ) && ( g_col_idx != (g_num_cols-1) ) ) {
	fprintf(stderr, "inbuf      = [%s] \n", inbuf);
	fprintf(stderr, "g_col_idx  = [%d] \n", g_col_idx);
	fprintf(stderr, "g_num_cols = [%d] \n", g_num_cols);
	fprintf(stderr, "error in center of following \n");
	for ( int ii = i - 30; ii < i + 30; ii++ ) { 
	  fprintf(stderr, "%c", inbuf[ii]);
	}
	fprintf(stderr, "\n\n\n");

	WHEREAMI; g_status = -1; break;
      }
      if ( ( inbuf[i] == g_fld_sep ) && ( g_col_idx == (g_num_cols-1) ) ) {
	fprintf(stderr, "inbuf      = [%s] \n", inbuf);
	fprintf(stderr, "g_col_idx  = [%d] \n", g_col_idx);
	fprintf(stderr, "g_num_cols = [%d] \n", g_num_cols);
	fprintf(stderr, "error in center of following \n");
	for ( int ii = i - 30; ii < i + 30; ii++ ) { 
	  fprintf(stderr, "%c", inbuf[ii]);
	}
	fprintf(stderr, "\n\n\n");
	WHEREAMI; g_status = -1; break;
      }
      g_buffer[g_buf_idx++] = '\0';
      got_cell = true;
    }
    else {
      g_buffer[g_buf_idx++] = inbuf[i];
    }
    if ( got_cell == true ) { 
      nn = 1;
      switch ( g_fldtypes[g_col_idx] ) { 
      case I1 : 
	if ( *g_buffer == '\0' ) { 
	  valI1 = 0; nn = 0; g_has_null_vals[g_col_idx] = true;
	}
	else {
	  g_status = stoI1(g_buffer, &valI1); 
	  if ( g_status < 0 ) { 
	    fprintf(stderr, "Error on col [%d] = [%s] \n", g_col_idx, g_buffer);
	  }
	}
	fwrite(&valI1, g_fldsz[g_col_idx], 1, g_binfps[g_col_idx]); 
	break;
      case I2 : 
	if ( *g_buffer == '\0' ) { 
	  valI2 = 0; nn = 0; g_has_null_vals[g_col_idx] = true;
	}
	else {
	  g_status = stoI2(g_buffer, &valI2); 
	}
	fwrite(&valI2, g_fldsz[g_col_idx], 1, g_binfps[g_col_idx]); 
	break;
      case I4 : 
	if ( *g_buffer == '\0' ) { 
	  valI4 = 0; nn = 0; g_has_null_vals[g_col_idx] = true;
	}
	else {
	  g_status = stoI4(g_buffer, &valI4); 
	}
	fwrite(&valI4, g_fldsz[g_col_idx], 1, g_binfps[g_col_idx]); 
	break;
      case I8 : 
	if ( *g_buffer == '\0' ) { 
	  valI8 = 0; nn = 0; g_has_null_vals[g_col_idx] = true;
	}
	else {
	  g_status = stoI8(g_buffer, &valI8);
	}
	fwrite(&valI8, g_fldsz[g_col_idx], 1, g_binfps[g_col_idx]); 
	break;
      case F4 : 
	if ( *g_buffer == '\0' ) { 
	  valF4 = 0; nn = 0; g_has_null_vals[g_col_idx] = true;
	}
	else {
	  g_status = stoF4(g_buffer, &valF4);
	}
	fwrite(&valF4, g_fldsz[g_col_idx], 1, g_binfps[g_col_idx]); 
	break;
      case F8 : 
	if ( *g_buffer == '\0' ) { 
	  valF8 = 0; nn = 0; g_has_null_vals[g_col_idx] = true;
	}
	else {
	  g_status = stoF8(g_buffer, &valF8);
	}
	fwrite(&valF8, g_fldsz[g_col_idx], 1, g_binfps[g_col_idx]); 
	break;
      case SV : 
	if ( *g_buffer == '\0' ) { 
	  nn = 0; g_has_null_vals[g_col_idx] = true;
	}
	fwrite(g_buffer, strlen(g_buffer), 1, g_binfps[g_col_idx]); 
	fwrite(&eoln, 1, 1, g_binfps[g_col_idx]); 
	break;
      default : 
	g_status = -1;
	break;
      }
      fwrite(&nn, 1, 1, g_nn_binfps[g_col_idx]); 
      got_cell = false;
      g_buf_idx = 0;
      g_col_idx++;
      nn = 1;
      *g_buffer = '\0'; 
      if ( g_col_idx == g_num_cols ) { g_col_idx = 0; }
    }
  }

  /*
    memcpy(userp, contents, realsize);
    fprintf(stderr, "Copied %d bytes. First 32 bytes = ", (int)realsize);
    for ( int i = 0; i < 64; i++ ) { 
    fprintf(stderr, "%c", ((char *)userp)[i]);
    }
    fprintf(stderr, "\n");
  */
  return realsize;
}


// START FUNC DECL
int 
hdfs_get(
	 char *username,
	 char *in_from,
	 char *whether_source_is_file_or_dir,
	 char *meta_info,
	 char *to,
	 char *whether_dest_is_file_or_dir,
	 char *aux_info
	 )
// STOP FUNC DECL
{
  int status = 0;
  CURL *ch = NULL;
  CURLcode curl_res;
  // CURLINFO info;
  long http_code;
  double c_length;  
  char *from_dir = NULL, *from_file = NULL;
  char *to_dir   = NULL, *to_file   = NULL;
  FILE *fp = NULL;
  char **Y = NULL; int nY = 0;
  char **filenames = NULL;
  char **nn_filenames = NULL;
  char cwd[MAX_LEN_DIR_NAME+1]; zero_string(cwd, MAX_LEN_DIR_NAME+1);
  char *X = NULL; size_t nX = 0;
  int in_num_files = 0;
  char **in_filenames = NULL;
  long long *in_file_lengths = NULL;
  char *from = NULL, *save_from = NULL;

  if ( aux_info != NULL ) { 
    status = chk_aux_info(aux_info); cBYE(status);
    char str_fld_sep[16]; bool is_null;
    zero_string(str_fld_sep, 16);
    status = extract_S(aux_info, "fld_sep=[", "]", str_fld_sep, 16, &is_null); 
    if ( is_null == false ) { 
      if ( strcmp(str_fld_sep, "tab") == 0 ) {
	g_fld_sep = '\t';
      }
      else if ( strcmp(str_fld_sep, "ctrl_z") == 0 ) {
	g_fld_sep = 26;
      }
      else if ( strcmp(str_fld_sep, "soh") == 0 ) {
	g_fld_sep = 1;
      }
      else {
	go_BYE(-1);
      }
    }
    else {
      g_fld_sep = '\t';
    }
  }
  else {
    g_fld_sep = '\t';
  }
  char chunk[16*CURL_MAX_WRITE_SIZE];
  /*
    http://curl.haxx.se/libcurl/c/libcurl-tutorial.html.  When using
    the HTTP protocol, there are many different ways a client can
    provide those credentials to the server and you can control which
    way libcurl will (attempt to) use them. The default HTTP
    authentication method is called 'Basic', which is sending the name
    and password in clear-text in the HTTP request, base64-encoded. This
    is insecure.
  */

  g_has_null_vals = NULL;
  g_binfps = NULL;
  g_nn_binfps = NULL;
  g_fldtypes = NULL;
  g_fldsz = NULL;
  g_num_cols= 0;
  g_col_idx = 0;
  g_buf_idx = 0;
  g_status    = 0;
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); } 
  if ( ( username == NULL ) || ( *username == '\0' ) )  { go_BYE(-1); }
  if ( ( in_from  == NULL ) || ( *in_from  == '\0' ) )  { go_BYE(-1); }
  if ( ( to       == NULL ) || ( *to       == '\0' ) )  { go_BYE(-1); }
  from = malloc((strlen(in_from) + 64) * sizeof(char) );
  strcpy(from, in_from);
  //------------------------------------------------------------
  if ( strcasecmp(whether_source_is_file_or_dir, "FILE") == 0 ) {
    from_file = from;
  }
  else if ( strcasecmp(whether_source_is_file_or_dir, "DIR") == 0 ) {
    from_dir = from;
  }
  else {
    go_BYE(-1);
  }
  //------------------------------------------------------------
  if ( strcasecmp(whether_dest_is_file_or_dir, "FILE") == 0 ) {
    to_file = to;
  }
  else if ( strcasecmp(whether_dest_is_file_or_dir, "DIR") == 0 ) {
    to_dir = to;
  }
  else {
    go_BYE(-1);
  }
  if ( ( meta_info != NULL ) && ( *meta_info != '\0' ) )  {
    status = explode(meta_info, ':', &Y, &nY); cBYE(status);
    if ( nY == 0 ) { go_BYE(-1); }
    g_num_cols = nY;
    //-----------------------------------------------------------
    g_fldtypes = malloc(nY * sizeof(FLD_TYPE));
    return_if_malloc_failed(g_fldtypes); 
    g_fldsz = malloc(nY * sizeof(int));
    return_if_malloc_failed(g_fldsz); 
    g_has_null_vals = malloc(nY * sizeof(bool));
    return_if_malloc_failed(g_has_null_vals); 
    g_binfps = malloc(nY * sizeof(FILE *));
    return_if_malloc_failed(g_binfps); 
    g_nn_binfps = malloc(nY * sizeof(FILE *));
    return_if_malloc_failed(g_nn_binfps); 
    filenames = malloc(nY * sizeof(char **));
    return_if_malloc_failed(filenames); 
    nn_filenames = malloc(nY * sizeof(char **));
    return_if_malloc_failed(nn_filenames); 
    //-----------------------------------------------------------
    for ( int i = 0; i < nY; i++ ) { 
      g_fldtypes[i]      = undef_fldtype;
      g_has_null_vals[i] = false;
      g_fldsz[i]         = INT_MAX;
      g_binfps[i]        = NULL;
      g_nn_binfps[i]     = NULL;
      filenames[i]       = NULL;
    }
    for ( int i = 0; i < nY; i++ ) { 
      filenames[i]    = malloc(32); zero_string(filenames[i], 32);
      nn_filenames[i] = malloc(32); zero_string(nn_filenames[i], 32);
      sprintf(filenames[i],    "_tempf_%s_%d",    Y[i], i);
      sprintf(nn_filenames[i], "_nn_tempf_%s_%d", Y[i], i);
    }
    //-----------------------------------------------------------
    chdir(to_dir); 
    for ( int i = 0; i < nY; i++ ) { 
      if ( strcmp(Y[i], "I1") == 0 ) { 
        g_fldtypes[i] = I1; g_fldsz[i] = sizeof(char); 
      }
      else if ( strcmp(Y[i], "I2") == 0 ) { 
	g_fldtypes[i] = I2; g_fldsz[i] = sizeof(short); 
      }
      else if ( strcmp(Y[i], "I4") == 0 ) { 
	g_fldtypes[i] = I4; g_fldsz[i] = sizeof(int); 
      }
      else if ( strcmp(Y[i], "I1") == 0 ) { 
	g_fldtypes[i] = I8; g_fldsz[i] = sizeof(long long); 
      }
      else if ( strcmp(Y[i], "F4") == 0 ) { 
	g_fldtypes[i] = F4; g_fldsz[i] = sizeof(float); 
      }
      else if ( strcmp(Y[i], "F8") == 0 ) { 
	g_fldtypes[i] = F8; g_fldsz[i] = sizeof(double); 
      }
      else if ( strcmp(Y[i], "SV") == 0 ) { 
	g_fldtypes[i] = SV; g_fldsz[i] = 0; // fldsz will not be used
      }
      else { go_BYE(-1); }
      int fd = mkstemp(filenames[i]);
      close(fd);
      g_binfps[i] = fopen(filenames[i], "wb");
      return_if_fopen_failed(g_binfps[i],  filenames[i], "wb");
      g_nn_binfps[i] = fopen(nn_filenames[i], "wb");
      return_if_fopen_failed(g_nn_binfps[i],  nn_filenames[i], "wb");
    }
    chdir(cwd); 
  }
  //------------------------------------------------------------
  //  START: Common CURL stuff ------------------------------
  ch = curl_easy_init();
  // follow redirects
  curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
  // insecure is okay
  curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 0);
  // authentication stuff
  curl_easy_setopt(ch, CURLOPT_USERNAME, username);
  curl_easy_setopt(ch, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
  // not sure if this is really needed
  char buffer[256]; zero_string(buffer, 256);
  sprintf(buffer, "%s:XXXXXXXX", username); // password is junk
  curl_easy_setopt(ch, CURLOPT_USERPWD, buffer);
  //  STOP: Common CURL stuff ------------------------------

  if ( ( from_file != NULL ) && ( to_file != NULL ) )  {
    // from a file to a file 
    sprintf(buffer, "?op=OPEN&user.name=%s", username);
    strcat(from, buffer);
    curl_easy_setopt(ch, CURLOPT_URL, from_file);
    fp = fopen(to_file, "w");
    return_if_fopen_failed(fp, to_file, "w");
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, fp);
    curl_res = curl_easy_perform(ch);
    if ( curl_res != 0 ) { go_BYE(-1); }
    if ( g_status < 0) { go_BYE(-1); }
  }
  else if ( ( from_file != NULL ) && ( to_dir != NULL ) )  {
    // from a file to a directory (consisting of binary files)
    // META DATA MUST BE PROVIDED
    if ( ( meta_info == NULL ) || ( *meta_info == '\0' ) )  { go_BYE(-1); }
    sprintf(buffer, "?op=OPEN&user.name=%s", username);
    strcat(from, buffer);
    curl_easy_setopt(ch, CURLOPT_URL, from);
    /* send all data to this function  */ 
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    /* we pass our 'chunk' struct to the callback function */ 
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *)&chunk);
    if ( is_directory(to_dir) == false ) { go_BYE(-1); }
    curl_res = curl_easy_perform(ch);
    if ( curl_res != 0 ) { go_BYE(-1); }
    if ( g_status < 0) { go_BYE(-1); }
  }
  else if ( ( from_dir != NULL ) && ( to_dir != NULL ) )  {
    // from a directory to a directory (consisting of binary files)
    // META DATA MUST BE PROVIDED
    if ( ( meta_info == NULL ) || ( *meta_info == '\0' ) )  { go_BYE(-1); }
    /* START: First figure out the files in the directory to be downloaded */
    char buffer[256]; zero_string(buffer, 256);
    save_from = strdup(from);
    sprintf(buffer, "?op=LISTSTATUS&user.name=%s", username);
    strcat(from, buffer);
    fprintf(stderr, "url=\"%s\"\n", from);
    curl_easy_setopt(ch, CURLOPT_URL, from);


    char tempfile[32]; zero_string(tempfile, 32);
    strcpy(tempfile, "/tmp/_tempf_XXXXXX");
    int fd = mkstemp(tempfile);
    close(fd);
    fp = fopen(tempfile, "w");
    return_if_fopen_failed(fp, tempfile, "w");
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, fp);
    curl_res = curl_easy_perform(ch);
    fclose_if_non_null(fp);
    status = rs_mmap(tempfile, &X, &nX, 0); cBYE(status);
    /* Check that file exists */
    {
      char *cptr = strstr(X, "FileNotFoundException");
      if ( cptr != NULL ) { 
	fprintf(stderr, "File not found. Error is [%s]\n", X); 
	unlink(tempfile);
	rs_munmap(X, nX);
	go_BYE(-1);
      }
    }
    status = get_file_names(X, &in_num_files, &in_filenames, &in_file_lengths);
    cBYE(status);
    for ( int i = 0; i < in_num_files; i++ ) { 
      fprintf(stderr, "%d: %lld : %s \n", i, in_file_lengths[i], in_filenames[i]);
    }
    rs_munmap(X, nX);
    if ( *tempfile != '\0' ) { unlink(tempfile); }
    /* STOP: Figure out the files in the directory to be downloaded */
    /* Now switch modes to get the data */
    for ( int i = 0; i < in_num_files; i++ ) { 
      strcpy(from, save_from);
      sprintf(buffer, "/%s?op=OPEN&user.name=%s", in_filenames[i], username);
      strcat(from, buffer);
      curl_easy_setopt(ch, CURLOPT_URL, from);
      fprintf(stderr, "url=\"%s\"\n", from);
      /* send all data to this function  */ 
      curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
      /* we pass our 'chunk' struct to the callback function */ 
      curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *)&chunk);
      // execute the call 
      curl_res = curl_easy_perform(ch);
      zero_string(buffer, 256);
      if ( curl_res != 0 ) { go_BYE(-1); }
      if ( g_status != 0 ) { go_BYE(-1); }
    }
    //----------------------------
  }
  else {
    go_BYE(-1);
  }

  printf("get http return code\n");
  curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &http_code);
  if ( http_code >= 400 ) { 
    fprintf(stderr, "http code: %lu\n", http_code);
    go_BYE(-1);
  }

  printf("get size of download page\n");
  curl_easy_getinfo(ch, CURLINFO_SIZE_DOWNLOAD, &c_length);
  printf("length: %g\n", c_length);

  if ( to_dir != NULL ) { 
    chdir(to_dir);
    for ( int i = 0; i < nY; i++ ) { 
      if ( g_has_null_vals[i] == false ) { 
        unlink(nn_filenames[i]);
      }
    }
    chdir(cwd);
  }

 BYE:
  if ( g_binfps != NULL ) { 
    for ( int i = 0; i < nY; i++ ) {
      fclose_if_non_null(g_binfps[i]);
    }
    free_if_non_null(g_binfps);
  }
  if ( filenames != NULL ) { 
    for ( int i = 0; i < nY; i++ ) {
      free_if_non_null(filenames[i]);
    }
    free_if_non_null(filenames);
  }
  if ( nn_filenames != NULL ) { 
    for ( int i = 0; i < nY; i++ ) {
      free_if_non_null(nn_filenames[i]);
    }
    free_if_non_null(nn_filenames);
  }
  if ( Y != NULL ) { 
    for ( int i = 0; i < nY; i++ ) {
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  free_if_non_null(save_from);
  free_if_non_null(g_fldtypes);
  free_if_non_null(g_fldsz);
  curl_easy_cleanup(ch);
  fclose_if_non_null(fp);
  rs_munmap(X, nX);
  //--------------------------------------------------
  free_if_non_null(in_file_lengths);
  if ( in_filenames != NULL ) { 
    for ( int i = 0; i < in_num_files; i++ ) { 
      free_if_non_null(in_filenames[i]);
    }
    free_if_non_null(in_filenames);
  }
  //--------------------------------------------------
  return(status);
}
