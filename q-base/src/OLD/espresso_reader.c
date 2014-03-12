#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"


#define MAXLINE 8192
int g_ival;
char g_buffer[MAXLINE];

size_t 
extract_ival(
    void *ptr, 
    size_t size, 
    size_t nmemb, 
    void *stream
    )
{
  size_t nbytes = size * nmemb;
  char *endptr;
  char *cptr = (char *)ptr;
  int i = 0, j = 0;
  // fprintf(stderr, "nbytes = %d, cptr = %s  \n", nbytes, cptr);
  g_ival = -1;
  if ( nbytes > 0 ) {
    for ( i = 0; i < nbytes; i++ ) { 
      if ( isdigit(cptr[i]) ) {
        g_buffer[j++] = cptr[i];
      }
    }
    g_ival = strtoll(g_buffer, &endptr, 10); 
    // fprintf(stderr, "g_ival = %d \n", g_ival);
  }
  return(nbytes);
}
#define N 1048576

int 
main() 
{
  int status = 0;
  CURL *curl = NULL;
  CURLcode curl_res;
  // CURLINFO info;
  long http_code;
  char geturl[MAXLINE];
  // START : For timing
  struct timeval Tps;
  struct timezone Tpf;
  long long t_before_sec = 0, t_before_usec = 0, t_before = 0;
  long long t_after_sec, t_after_usec, t_after;
  long long t_delta_usec;
  // STOP : For timing
  FILE *fp = NULL;

  char *baseurl = "http://172.16.23.46:12918/";
  char *db = "RameshDB";
  char *tbl = "Monitor";
  char *logfile = "curl_log_r_0.txt"; 
  // w => write, 0 => thread_id 

  if ( ( logfile == NULL ) || ( *logfile == '\0' ) ) {
    fp = stdout;
  }
  else {
    fp = fopen(logfile, "w");
    return_if_fopen_failed(fp, logfile, "w");
  }

  zero_string(geturl, MAXLINE);
  zero_string(g_buffer, MAXLINE);

  curl = curl_easy_init(); // init curl session 
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, extract_ival);
  for ( int id = 0; id < N; id++ ) {
    zero_string_to_nullc(g_buffer);
    g_ival = -1;

    status= gettimeofday(&Tps, &Tpf); cBYE(status);
    t_before_sec  = (long long)Tps.tv_sec;
    t_before_usec = (long long)Tps.tv_usec;
    t_before = t_before_sec * 1000000 + t_before_usec;
    sprintf(geturl,"%s/%s/%s/%d", baseurl, db, tbl, id);

    // printf("url to download = %s \n", geturl);
    curl_easy_setopt(curl, CURLOPT_URL, geturl);

    // printf("set file handler to write\n");
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    curl_res = curl_easy_perform(curl); // execute curl call 
    if ( curl_res != 0 ) { go_BYE(-1); }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    gettimeofday (&Tps, &Tpf); 
    t_after_sec  = (long long)Tps.tv_sec;
    t_after_usec = (long long)Tps.tv_usec;
    t_after = t_after_sec * 1000000 + t_after_usec;
    t_delta_usec = t_after - t_before;

    fprintf(fp,"%lld,%d,%lld,%d,%d\n", t_before, (int)http_code, t_delta_usec,
	id, g_ival);
    fflush(fp);
    // printf("http code: %lu\n", http_code);

    // printf("get size of download page\n");
    // curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &c_length);
    // printf("length: %g\n", c_length);
  }
 BYE:
  if ( ( logfile == NULL ) || ( *logfile == '\0' ) ) {
    // Nothing to do 
  }
  else {
    fclose_if_non_null(fp);
  }
  curl_easy_cleanup(curl);
  return(status);
}
