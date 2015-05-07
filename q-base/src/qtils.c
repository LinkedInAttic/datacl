#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "constants.h"
#include "macros.h"
#include "time1.h"
#include "replace_char.h"
#include "tab_split.h"
#include "qhead.h"
#include "chk_ascii.h"
#include "esc_for_latex.h"
#include "hash_string.h"
#include "asc2bin.h"
#include "bin2asc.h"
#include "txt_sub.h"
#include "url.h"
#include "sortbindmp.h"
#include "auxil.h"
#include "hdfs_get.h"
#include "date_to_I4.h"
#include "mk_ldb_dict.h"
 
char g_docroot[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
qtils(
      int argc,
      char **argv,
      char *rslt_buf,
      int sz_rslt_buf
      )
// STOP FUNC DECL
{
  int status = 0;
  int itemp;
  if ( ( argc <= 1 ) || ( argc >= 14 ) )  {go_BYE(-1); }
  if ( strcmp(argv[1], "time_since_epoch") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    if ( strcmp(argv[2], "usec") == 0 ) { 
      sprintf(rslt_buf, "%llu", get_time_usec());
    }
    else if ( strcmp(argv[2], "sec") == 0 ) { 
      status = time1(&itemp); cBYE(status);
      sprintf(rslt_buf, "%d", itemp);
    }
  }
  else if ( strcmp(argv[1], "hdfs_get") == 0 ) { 
    if ( argc != 11 ) { fprintf(stderr, "argc = %d \n", argc); go_BYE(-1); }
    char url[4096]; zero_string(url, 4096); int port;

    char *username = argv[2]; // 
    char *server   = argv[3]; // 
    char *str_port = argv[4]; // 
    char *from     = argv[5]; // /user/<username>/XXXXX/part-r-00000
    if ( *username == '\0' ) { go_BYE(-1); }
    if ( *server   == '\0' ) { go_BYE(-1); }
    if ( *str_port == '\0' ) { go_BYE(-1); }
    if ( *from     == '\0' ) { go_BYE(-1); }
    //----------------------------------------------------
    if ( *from != '/' ) { go_BYE(-1); }
    status = stoI4(str_port, &port); cBYE(status);
    if ( port < 1024 ) { go_BYE(-1); }
    //----------------------------------------------------
    strcpy(url, "http://");
    strcat(url, server); 
    strcat(url, ":"); 
    strcat(url, str_port);
    strcat(url, "/webhdfs/v1");
    strcat(url, from);
    status = hdfs_get(username, url, argv[6], argv[7], argv[8], argv[9], argv[10]); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "replace_char") == 0 ) { 
    if ( argc != 5 ) { go_BYE(-1); }
    status = replace_char(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "tab_split") == 0 ) {
    if ( argc != 7 ) { go_BYE(-1); }
    status = tab_split(argv[2], argv[3], argv[4], argv[5], argv[6]); cBYE(status);
  }
  else if ( strcmp(argv[1], "qhead") == 0 ) {
    if ( argc != 5 ) { go_BYE(-1); }
    status = qhead(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "chk_ascii") == 0 ) {
    if ( argc != 3 ) { go_BYE(-1); }
    status = chk_ascii(argv[2]); cBYE(status);
  }
  else if ( strcmp(argv[1], "urlencode") == 0 ) {
    if ( argc != 3 ) { go_BYE(-1); }
    status = ext_url_encode(argv[2], rslt_buf, sz_rslt_buf); cBYE(status);
  }
  else if ( strcmp(argv[1], "urldecode") == 0 ) {
    if ( argc != 3 ) { go_BYE(-1); }
    status = ext_url_decode(argv[2], rslt_buf, sz_rslt_buf); cBYE(status);
  }
  else if ( strcmp(argv[1], "esc_for_latex") == 0 ) {
    if ( argc != 2 ) { go_BYE(-1); }
    status = esc_for_latex(); cBYE(status);
  }
  else if ( strcmp(argv[1], "hash_string") == 0 ) {
    if ( argc != 4 ) { go_BYE(-1); }
    status = ext_hash_string(argv[2], argv[3]); cBYE(status);
  }
  else if ( strcmp(argv[1], "txt_sub") == 0 ) {
    if ( argc != 4 ) { go_BYE(-1); }
    status = txt_sub(argv[2], argv[3]); cBYE(status);
  }
  else if ( strcmp(argv[1], "asc2bin") == 0 ) {
    if ( argc != 5 ) { go_BYE(-1); }
    status = asc2bin(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "bin2asc") == 0 ) {
    if ( argc != 6 ) { go_BYE(-1); }
    status = bin2asc(argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "date_to_I4") == 0 ) {
    if ( argc != 6 ) { go_BYE(-1); }
    status = date_to_I4 (argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "sortbindmp") == 0 ) {
    if ( argc != 5 ) { go_BYE(-1); }
    status = sortbindmp(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "mk_ldb_dict") == 0 ) {
    if ( argc != 9 ) { go_BYE(-1); }
    char *q_data_dir = getenv("Q_DATA_DIR");
    if ( q_data_dir == NULL ) { go_BYE(-1); }
    status = mk_ldb_dict(argv[2], argv[3], argv[4], argv[5], argv[6], 
	argv[7], argv[8], q_data_dir); 
    cBYE(status);
  }



  else { go_BYE(-1); }
 BYE:
  return(status);
}
