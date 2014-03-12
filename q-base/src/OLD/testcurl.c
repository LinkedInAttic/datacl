#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "constants.h"
#include "macros.h"

int 
main() 
{
  int status = 0;
  CURL *curl = NULL;
  CURLcode curl_res;
  CURLINFO info;
  long http_code;
  double c_length;  
  FILE *fp;
  char *url = "cinco.corp.linkedin.com";

  fp = fopen("_tempf", "w");
  return_if_fopen_failed(fp, "_tempf", "w");

  printf("init curl session\n");
  curl = curl_easy_init();
  printf("set url to download\n");
  curl_easy_setopt(curl, CURLOPT_URL, url);

  printf("set file handler to write\n");
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

  printf("download the file\n");
  curl_res = curl_easy_perform(curl);
  if ( curl_res == 0 ) {
    printf("file downloaded\n");
  } 
  else {
    printf("ERROR in dowloading file\n");
    go_BYE(-1);
  }

  printf("get http return code\n");
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  printf("http code: %lu\n", http_code);

  printf("get size of download page\n");
  curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &c_length);
  printf("length: %g\n", c_length);

  printf("END: close all files and sessions\n");
BYE:
  curl_easy_cleanup(curl);
  fclose_if_non_null(fp);
  return(status);
}
