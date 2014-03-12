/* DO NOT AUTO GENERATE HEADER FILE  */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "constants.h"
#include "macros.h"

static size_t 
WriteMemoryCallback(
    void *contents, 
    size_t size, 
    size_t nmemb, 
    void *userp
    )
{
  size_t realsize = size * nmemb;
  memcpy(userp, contents, realsize);
  fprintf(stderr, "Copied %d bytes. First 32 bytes = ", (int)realsize);
  for ( int i = 0; i < 64; i++ ) { 
    fprintf(stderr, "%c", ((char *)userp)[i]);
  }
  fprintf(stderr, "\n");
  return realsize;
}


int 
main() 
{
  int status = 0;
  CURL *ch = NULL;
  CURLcode curl_res;
  CURLINFO info;
  long http_code;
  double c_length;  
  FILE *fp = NULL;
  // char *url = "http://cinco.corp.linkedin.com";
  // char *url = "http://www.google.com";
  // char *url="http://eat1-magicnn01.grid.linkedin.com:50070/webhdfs/v1/user/rsubramo/edu_export/part-r-00000?op=LISTSTATUS&user.name=rsubramo";
  char *url="http://eat1-magicnn01.grid.linkedin.com:50070/webhdfs/v1/user/rsubramo/edu_export/part-r-00000?op=OPEN&user.name=rsubramo";
  char *username="rsubramo";


  char chunk[16*CURL_MAX_WRITE_SIZE];
  bool write_to_file = false;
  /*
   http://curl.haxx.se/libcurl/c/libcurl-tutorial.html.  When using
  the HTTP protocol, there are many different ways a client can
  provide those credentials to the server and you can control which
  way libcurl will (attempt to) use them. The default HTTP
  authentication method is called 'Basic', which is sending the name
  and password in clear-text in the HTTP request, base64-encoded. This
  is insecure.
  */

  if ( write_to_file == true ) { 
    fp = fopen("_tempf", "w");
    return_if_fopen_failed(fp, "_tempf", "w");
  }

  fprintf(stderr, "url=\"%s\"\n", url);
  printf("init curl session\n");
  ch = curl_easy_init();
  printf("set url to download\n");
  curl_easy_setopt(ch, CURLOPT_URL, url);
   // follow redirects
  curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
  // insecure is okay
  curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 0);
  // authentication stuff
  curl_easy_setopt(ch, CURLOPT_USERNAME, username);
  curl_easy_setopt(ch, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
  // curl_easy_setopt(ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  // curl_easy_setopt(ch, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
  // curl_easy_setopt(ch, CURLOPT_HTTPAUTH, CURLAUTH_GSSNEGOTIATE);

  if ( write_to_file ) { 
    printf("set file handler to write\n");
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, fp);
  }
  else {
    /* send all data to this function  */ 
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    /* we pass our 'chunk' struct to the callback function */ 
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *)&chunk);
  }


  printf("download the file\n");
  curl_res = curl_easy_perform(ch);
  if ( curl_res == 0 ) {
    printf("file downloaded\n");
  } 
  else {
    printf("ERROR in dowloading file\n");
    go_BYE(-1);
  }

  printf("get http return code\n");
  curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &http_code);
  printf("http code: %lu\n", http_code);

  printf("get size of download page\n");
  curl_easy_getinfo(ch, CURLINFO_SIZE_DOWNLOAD, &c_length);
  printf("length: %g\n", c_length);

  printf("END: close all files and sessions\n");
BYE:
  curl_easy_cleanup(ch);
  fclose_if_non_null(fp);
  return(status);
}
