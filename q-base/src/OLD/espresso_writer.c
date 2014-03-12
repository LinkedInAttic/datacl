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
long long g_etag;
char g_buffer[MAXLINE];
char g_out_buffer[MAXLINE];
int g_counter;

/*-------------------------------------------------------------------*/
size_t 
mk_data(
	void *ptr, 
	size_t size, 
	size_t nmemb, 
	void *userdata
	)
{
  /*
  int len = strlen(g_out_buffer);
  int itemp = *((int *)userdata);
  size_t nbytes = len * sizeof(char);
  char *xptr = (char *)userdata; // testing 
  memcpy(ptr, g_out_buffer, (len * sizeof(char)));
  g_counter++;
  return(nbytes);
  */
  size_t retcode = fread(ptr, size, nmemb, userdata);
//  fprintf(stderr, "*** We read %d bytes from file\n", (int)retcode);
  g_counter++;
  return retcode;
}

/*-------------------------------------------------------------------*/
size_t 
extract_etag(
    void *ptr, 
    size_t size, 
    size_t nmemb, 
    void *stream
    )
{
  size_t nbytes = size * nmemb;
  char *endptr;
  char *cptr = (char *)ptr;
  char *needle = "ETag: ";
  int i = 0;

//  fprintf(stderr, "ptr= %s \n", (char *)ptr);
  if ( nbytes > 0 ) {
    for ( i = 0; i < nbytes; i++ ) { 
      g_buffer[i] = cptr[i];
    }
    g_buffer[i] = '\0';
    //fprintf(stderr, "buffer = %s, needle = %s \n", g_buffer, needle);
    cptr = strstr(g_buffer, needle); 
    if ( cptr != NULL ) { 
      cptr += strlen(needle);
      g_etag = strtoll(cptr, &endptr, 10); 
    }
  }
  return(nbytes);
}
/*-------------------------------------------------------------------*/
#define N 1048576

int 
main() 
{
  int status = 0;
  CURL *curl = NULL;
  CURLcode curl_res;
  // CURLINFO info;
  long http_code;
  // double c_length;  
  unsigned int ival;
  char puturl[MAXLINE];
  // START : For timing
  struct timeval Tps;
  struct timezone Tpf;
  long long t_before_sec = 0, t_before_usec = 0, t_before = 0;
  long long t_after_sec, t_after_usec, t_after;
  long long t_delta_usec;
  // STOP : For timing
  FILE *fp = NULL;
  FILE *tfp = NULL;
  int itemp = 12345; // just for testing

  char *baseurl = "http://172.16.23.46:12918";
  char *db = "RameshDB";
  char *tbl = "Monitor";
  char *logfile = "curl_log_w_0.txt"; 
  // w => write, 0 => thread_id 

  if ( ( logfile == NULL ) || ( *logfile == '\0' ) ) {
    fp = stdout;
  }
  else {
    fp = fopen(logfile, "w");
    return_if_fopen_failed(fp, logfile, "w");
  }

  g_counter = 0;
  zero_string(puturl, MAXLINE);
  zero_string(g_buffer, MAXLINE);
  zero_string(g_out_buffer, MAXLINE);

  curl = curl_easy_init(); // init curl session 
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, extract_etag);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  curl_easy_setopt(curl, CURLOPT_READFUNCTION, mk_data);
  curl_easy_setopt(curl, CURLOPT_READDATA, g_out_buffer);
//  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "Transfer-Encoding: chunked");

  status= gettimeofday(&Tps, &Tpf); cBYE(status);
  t_before_sec  = (long long)Tps.tv_sec;
  t_before_usec = (long long)Tps.tv_usec;
  srand((unsigned int)t_before_usec);

  for ( int id = 1; id < N; id++ ) {
    ival = id; // should add thread info to this as well
    zero_string_to_nullc(g_buffer);
    zero_string_to_nullc(g_out_buffer);
    g_etag = -1;
    g_counter = 0;

    sprintf(g_out_buffer, "{\"ival\":%u}", ival);
    status= gettimeofday(&Tps, &Tpf); cBYE(status);
    t_before_sec  = (long long)Tps.tv_sec;
    t_before_usec = (long long)Tps.tv_usec;
    t_before = t_before_sec * 1000000 + t_before_usec;
    sprintf(puturl,"%s/%s/%s/%d", baseurl, db, tbl, id);

    // write to a file instead of to memory
    tfp = fopen("_tempf_curl_xxx", "wb");
    fwrite(g_out_buffer, sizeof(char), strlen(g_out_buffer), tfp);
    fclose(tfp);
    tfp = fopen("_tempf_curl_xxx", "rb");
    curl_easy_setopt(curl, CURLOPT_READDATA, tfp);

    itemp = 1111;
    // printf("url to download = %s \n", puturl);
    curl_easy_setopt(curl, CURLOPT_URL, puturl);

    // printf("set file handler to write\n");
    // curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    curl_res = curl_easy_perform(curl); // execute curl call 
//    printf("g_counter : %d\n", g_counter);
    if ( curl_res != 0 ) { go_BYE(-1); }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    gettimeofday (&Tps, &Tpf); 
    t_after_sec  = (long long)Tps.tv_sec;
    t_after_usec = (long long)Tps.tv_usec;
    t_after = t_after_sec * 1000000 + t_after_usec;
    t_delta_usec = t_after - t_before;

    fprintf(fp,"%lld,%d,%lld,%d,%d,%lld\n", t_before, (int)http_code, 
	t_delta_usec, id, ival, g_etag);
//    printf("http code: %lu\n", http_code);

    // printf("get size of download page\n");
    // curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &c_length);
    // printf("length: %g\n", c_length);
//    fprintf(stderr, " puturl = %s \n", puturl);
    fflush(fp);
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
