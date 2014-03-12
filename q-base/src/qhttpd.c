/* 
 * Go through http://www.jmarshall.com/easy/http/
 *
 * and make sure you implement its recommendations
 */
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <inttypes.h>
#include <netinet/tcp.h>
#include "constants.h"
#include "macros.h"
#include "qhttpd.h"
#include "auxil.h"
#include "mmap.h"
#include "q.h"
#include "url.h"
#include "extract_I8.h"
#include "extract_S.h"

/* Invocations 
   curl localhost:8000/init?TABLESPACE=test1
   TOKEN=12345
   curl localhost:8000/free?TOKEN=$TOKEN
   instr="q f1s1opf2 t1 f1 f2 '&&' f3"
   instr="date_range=YYYYMMDD:YYYYMMDD pageID=1234 "
   echo $instr
   q f1s1opf2 t1 f1 f2 '&&' f3
   codestr=`qtils urlencode "$instr"`
   echo $codestr
   q+f1s1opf2+t1+f1+f2+%27%26%26%27+f3
   curl --url "localhost:8000/q?TOKEN=$TOKEN&COMMAND=$codestr"

   *
   * 
   *
   */

#define SERVER_STRING "Server: qhttpd/0.1.0\r\n"
#define MAX_LEN_QUERY_STRING 1023
#define ERR_MSG_LEN 1024
#define SERVER_PORT 8000
#define SERVER_REQ_Q 16 // number of concurrent requests 

#define MAX_LEN_LGCL_NAME 31

typedef struct table_space_type {
  unsigned int token; // token > 0 => busy 
  char lgcl_name[MAX_LEN_LGCL_NAME+1];
  char bs_docroot[MAX_LEN_DIR_NAME+1];
  char q_docroot[MAX_LEN_DIR_NAME+1];
  char dsk_data_dir[MAX_LEN_DIR_NAME+1];
  char ram_data_dir[MAX_LEN_DIR_NAME+1];
} TABLE_SPACE_TYPE;

char **qargv;
extern int mkstemp(char *template);

bool g_is_shutdown;
char             g_accept_request_ret_val[ERR_MSG_LEN];
TABLE_SPACE_TYPE *g_tablespace;
int              g_num_tablespace;
pthread_mutex_t  g_tablespace_lock;

int conv_query_string_to_args(
			      char *encoded_query_string, 
			      int *ptr_qargc, 
			      char **qargv

			      )
{
  int status = 0;
  bool is_null = false;
  char encoded_qstr[MAX_LEN_QUERY_STRING+1];
  char arg[MAX_LEN_QUERY_STRING+1];
  char *qstr = NULL;

  *ptr_qargc = 0;
  zero_string(encoded_qstr, MAX_LEN_QUERY_STRING+1);
  zero_string(arg, MAX_LEN_QUERY_STRING+1);
  status = extract_S(encoded_query_string, "COMMAND=", "&", 
		     encoded_qstr, MAX_LEN_QUERY_STRING, &is_null);
  if ( is_null == true ) { go_BYE(-1);}
  for ( int i = 0; i < MAX_NUM_ARGS; i++ ) {
    zero_string(qargv[i], MAX_LEN_ARG+1);
  }
  status = url_decode(encoded_qstr, &qstr); cBYE(status);

  /* Now we have the q command of the form "f1s1opf2 T1 f1 100 + f2 " */
  status = str_to_argv(qstr, qargv, MAX_NUM_ARGS, MAX_LEN_ARG, ptr_qargc); 
  cBYE(status);
  //----------------------------------------------------
 BYE:
  free_if_non_null(qstr);
  return status ;
}

// START FUNC DECL
void split_query_str(
		     char *src, 
		     char *dest
		     )
// STOP FUNC DECL
{
  unsigned int i;
  for ( i = 0; i < strlen(src); i++) {
    if ( src[i] == '&') {
      dest[i] = ' ';
    }
    else {
      dest[i] = src[i];
    }
  }
  dest[i] = '\0';
}

void
nullify(
	char *X,
	int n
	)
{
  for ( int i = 0; i < n; i++ ) { 
    X[i] = '\0';
  }
}

void
zero_tbspc(
	   TABLE_SPACE_TYPE *ptr_tbspc
	   )
{
  ptr_tbspc->token = 0;
  nullify(ptr_tbspc->lgcl_name,  MAX_LEN_LGCL_NAME+1);
  nullify(ptr_tbspc->q_docroot,  MAX_LEN_DIR_NAME+1);
  nullify(ptr_tbspc->dsk_data_dir, MAX_LEN_DIR_NAME+1);
  nullify(ptr_tbspc->ram_data_dir, MAX_LEN_DIR_NAME+1);
  nullify(ptr_tbspc->bs_docroot, MAX_LEN_DIR_NAME+1);
}


// START FUNC DECL
void send_server_info(
		      int client
		      )
// STOP FUNC DECL
{
#ifdef XXX
  char tempfile[L_tmpnam + 16]; //  concatenating thread_id 
  char cmd[L_tmpnam+16+32]; // 32 is for extra chars and command name

  fprintf(stderr, "sending server info\n");

  for ( int i = 0; i < L_tmpnam+16; i++ ) { tempfile[i] = '\0'; }
  for ( int i = 0; i < L_tmpnam+16+32; i++ ) { cmd[i] = '\0'; }
  strcpy(tempfile, "_tempf_XXXXXX");
  int fd = mkstemp(tempfile);
  close(fd);

  sprintf(tempfile, "%s_%u", tempfile, (unsigned int) (pthread_self()));

  fprintf(stderr, "temp filename = %s\n", tempfile);

  sprintf(cmd, "./info.sh > %s", tempfile); 
  fprintf(stderr, "cmd = %s\n", cmd);

  if(system(cmd) != 0)
    {
      fprintf(stderr, "error getting server info\n");
    }
  else
    {
      serve_file(client, tempfile, true);
      remove(tempfile);
    }
#endif
}

// START FUNC DECL
void acquire_namespace(
		       int ns_map_indx
		       )
// STOP FUNC DECL
{
#ifdef XXX
 init:
  pthread_mutex_lock(&g_tablespace_lock);

  if ( namespace_map[ns_map_indx].token == 0) {
    namespace_map[ns_map_indx].busy=1;
    pthread_mutex_unlock(&namespace_map_lock);
  }
  else {
    pthread_mutex_unlock(&namespace_map_lock);
    fprintf(stderr, "namespace busy, waiting for 1 sec\n");
    sleep(1);
    goto init; 
  }
  fprintf(stderr, "namepace acquired\n");
#endif
}

// START FUNC DECL
void release_namespace(
		       int ns_map_indx
		       )
// STOP FUNC DECL
{
#ifdef XXX
  pthread_mutex_lock(&namespace_map_lock);
  namespace_map[ns_map_indx].busy=0;
  pthread_mutex_unlock(&namespace_map_lock);
  fprintf(stderr, "namepace released \n");
#endif
}

// START FUNC DECL
void execute_plugin(
		    char *path, 
		    char *query_string, 
		    int client, 
		    int ns_map_indx
		    )
// STOP FUNC DECL
{
  char tempfile[L_tmpnam + 16]; // 16 concatenating thread_id 
  char cmd[sizeof(tempfile) + strlen(path) + strlen(query_string) + 8]; // 8 for extra spaces and all
  char query_string_new[strlen(query_string)]; // to store query string split by space

  fprintf(stderr, "executing plugin\n");

  for ( int i = 0; i < L_tmpnam+16; i++ ) { tempfile[i] = '\0'; }
  for ( int i = 0; i < L_tmpnam+16+32; i++ ) { cmd[i] = '\0'; }
  strcpy(tempfile, "_tempf_XXXXXX");
  int fd = mkstemp(tempfile);
  close(fd);
  sprintf(tempfile, "%s_%u", tempfile, (unsigned int) (pthread_self()));
 
  fprintf(stderr, "temp filename = %s\n", tempfile);

  split_query_str(query_string, query_string_new);
  sprintf(cmd, "%s %s > %s", path, query_string_new, tempfile); 
  fprintf(stderr, "cmd = %s\n", cmd);

  // setup namespace and wait if another thread is executing on the same namespace
  acquire_namespace(ns_map_indx);

  if(system(cmd) != 0)
    {
      fprintf(stderr, "error executing Q plugin\n");
      release_namespace(ns_map_indx);
    }
  else
    {
      release_namespace(ns_map_indx);
      char tempfile_json[sizeof(tempfile) + 5]; // 5 for .json extension
      serve_file(client, tempfile, true);
      remove(tempfile);
    }
}

// START FUNC DECL
int execute_q_query(
		    char *query_string,
		    unsigned int token,
		    int client
		    )
// STOP FUNC DECL
{
  int status = 0;
#define RSLT_BUF_SIZE 65536
  char rslt_buf[RSLT_BUF_SIZE];

  // conv_query_to_argc_argv
  for ( int i = 0; i < RSLT_BUF_SIZE; i++ ) { rslt_buf[i] = '\0'; }
  // status = q(argc, argv, rslt_buf, RSLT_BUF_SIZE); cBYE(status); 
  if ( *rslt_buf != '\0' ) { fprintf(stdout, "%s", rslt_buf); }
 BYE:
  return status ;
}


#ifdef XXX
// START FUNC DECL
int generate_namespace(
		       char *buf
		       )
// STOP FUNC DECL
{
  int status = 0;
  time_t cur_time;
  cur_time = time(NULL);
  sprintf(buf, "%ju", (uintmax_t)cur_time);
  int cur_namespace_map_indx = 0;

  pthread_mutex_lock(&namespace_map_lock);

  cur_namespace_map_indx = namespace_map_count;
  namespace_map_count++;

  strcpy(namespace_map[cur_namespace_map_indx].id, buf);
  sprintf(namespace_map[cur_namespace_map_indx].q_root, "/tmp/q_root_%s", buf);
  sprintf(namespace_map[cur_namespace_map_indx].q_data, "/tmp/q_data_%s", buf);
  namespace_map[cur_namespace_map_indx].busy=0;

  pthread_mutex_unlock(&namespace_map_lock);

  // remove q root/data directory
  char cmd[64];
  sprintf(cmd, "rm -rf %s", namespace_map[cur_namespace_map_indx].q_root);
  system(cmd);
  sprintf(cmd, "rm -rf %s", namespace_map[cur_namespace_map_indx].q_data);
  system(cmd);

  // create q root/data directory with rwx to u, rwx to g and rx to o
  mkdir(namespace_map[cur_namespace_map_indx].q_root, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  mkdir(namespace_map[cur_namespace_map_indx].q_data, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
 BYE:
  return status ;
}
#endif

// START FUNC DECL
int send_start_msg(
		    int content_length,
		    int client
		    )
// STOP FUNC DECL
{
  int status = 0;
  char buf[1024];

  if ( content_length <= 0 ) { go_BYE(-1); }
  if ( client         <= 0 ) { go_BYE(-1); }

  sprintf(buf, "HTTP/1.1 200 OK\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "Content-Type: application/json\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "Content-Length: %d\r\n", content_length);
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
BYE:
  return status ;
}

// START FUNC DECL
int send_identifier(
		    unsigned int token,
		    int client
		    )
// STOP FUNC DECL
{
  int status = 0;
  char buf[1024];
  char tbuf[1024];

  sprintf(buf, "HTTP/1.1 200 OK\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "Content-Type: application/json\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(tbuf, "{ \"id\" : \"%u\"}", token);
  sprintf(buf, "Content-Length: %ld\r\n", strlen(tbuf));
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "{ \"id\" : \"%u\"}", token);
  send(client, buf, strlen(buf), 0);

  // fprintf(stderr, "sending identifier=%u done\n", token);
 BYE:
  return status ;
}


// START FUNC DECL
int process_req(
		char *method,
		char *url, 
		char *encoded_query_string, 
		char *encoded_post_params,
		int client
		)
// STOP FUNC DECL
{
  int status = 0;
  char buf[1024];
  int numchars;
  struct stat st;
  unsigned int token = 0; // to be set 
  char qrslt_buf[RSLT_BUF_SIZE]; 
  char lgcl_name[MAX_LEN_LGCL_NAME+1];
  char *query_string = NULL;
  char *post_params = NULL;
  FILE *ofp = NULL;
  char *Y = NULL; size_t nY = 0;

  if ( method == NULL ) { go_BYE(-1); }
  if ( url == NULL ) { go_BYE(-1); } 
  if ( encoded_query_string == NULL ) { go_BYE(-1); } 
  if ( encoded_post_params == NULL ) { go_BYE(-1); }
  if ( *encoded_query_string != '\0' )  {
    status = url_decode(encoded_query_string, &query_string); cBYE(status);
  }
  if ( *encoded_post_params != '\0' )  {
    status = url_decode(encoded_post_params, &post_params); cBYE(status);
  }

  zero_string(qrslt_buf, RSLT_BUF_SIZE);
  if ( strcasecmp(url, "init") == 0) {
    zero_string(lgcl_name, MAX_LEN_LGCL_NAME+1);
    bool is_null = true;
    status = extract_S(query_string, "TABLESPACE=", "&", lgcl_name, 
		       MAX_LEN_LGCL_NAME,  &is_null); 
    if ( is_null == true ) { go_BYE(-1); }
    for ( int i = 0; i < g_num_tablespace; i++ ) { 
      if ( g_tablespace[i].token != 0 ) { continue; }
      if ( strcmp(g_tablespace[i].lgcl_name, lgcl_name) == 0 ) {
        token = (unsigned int)mrand48();
        g_tablespace[i].token = token;
        // remove and re-create q_docroot, q_data_dir
        char cmd[MAX_LEN_DIR_NAME+65];
        sprintf(cmd, "rm -rf %s", g_tablespace[i].q_docroot);  system(cmd);
        sprintf(cmd, "rm -rf %s", g_tablespace[i].dsk_data_dir); system(cmd);
        sprintf(cmd, "rm -rf %s", g_tablespace[i].ram_data_dir); system(cmd);
	status = mkdir(g_tablespace[i].q_docroot, 0777); cBYE(status); 
	status = mkdir(g_tablespace[i].dsk_data_dir, 0777); cBYE(status); 
	if ( strlen(g_tablespace[i].ram_data_dir) != 0 ) { 
	  status = mkdir(g_tablespace[i].ram_data_dir, 0777); cBYE(status); 
	}
	//------------------------------------
	// Bootstrap the directory 
        for ( int i = 0; i < MAX_NUM_ARGS; i++ ) { 
          zero_string(qargv[i], MAX_LEN_ARG+1);
        }
	int qargc = 6; 
	strcpy(qargv[0], "q");
	strcpy(qargv[1], "bootstrap");
	strcpy(qargv[2], g_tablespace[i].q_docroot); 
	strcpy(qargv[3], g_tablespace[i].bs_docroot); 
	strcpy(qargv[4], g_tablespace[i].dsk_data_dir);
	strcpy(qargv[5], g_tablespace[i].ram_data_dir);
	status = q(qargc, qargv, qrslt_buf, RSLT_BUF_SIZE, stdout);
	cBYE(status);
	//------------------------------------
	char token_msg[2*(MAX_LEN_DIR_NAME+1) + 1024];
	zero_string(token_msg, (2*(MAX_LEN_DIR_NAME+1) + 1024));
	sprintf(token_msg, "{  \
	    \"TOKEN\" : \"%u\",  \
	    \"Q_DOCROOT\" : \"%s\",  \
	    \"DSK_DATA_DIR\" : \"%s\",  \
	    \"RAM_DATA_DIR\" : \"%s\"  \
	    }\n", 
	    token, 
	    g_tablespace[i].q_docroot, 
	    g_tablespace[i].dsk_data_dir,
	    g_tablespace[i].ram_data_dir);
	int content_length = strlen(token_msg);
	status = send_start_msg(content_length, client); cBYE(status);
	send(client, token_msg, content_length, 0);
	break;
      }
    }
    if ( token == 0 ) { 
      status = send_msg("Server busy", 500, client); cBYE(status);
    }
  }
  else if (strcasecmp(url, "free") == 0) {
    char msg[1024]; long long ltemp; bool is_null = true;
    status = extract_I8(query_string, "TOKEN=", "&", &ltemp, &is_null);
    cBYE(status);
    if ( is_null == true ) { go_BYE(-1); }
    if ( ltemp <= 0 ) { go_BYE(-1); }
    token = ltemp;
    bool token_found = false;
    for ( int i = 0; i < g_num_tablespace; i++ ) { 
      if ( g_tablespace[i].token == token ) { 
        token_found = true;
        // remove q_docroot, q_data_dir
        char cmd[MAX_LEN_DIR_NAME+65];
	zero_string(cmd, MAX_LEN_DIR_NAME+65);
        sprintf(cmd, "rm -rf %s", g_tablespace[i].q_docroot);  system(cmd);

	zero_string(cmd, MAX_LEN_DIR_NAME+65);
        sprintf(cmd, "rm -rf %s", g_tablespace[i].dsk_data_dir); system(cmd);

	if ( strlen(g_tablespace[i].ram_data_dir) != 0 ) { 
	  zero_string(cmd, MAX_LEN_DIR_NAME+65);
          sprintf(cmd, "rm -rf %s", g_tablespace[i].ram_data_dir); system(cmd);
	}

	g_tablespace[i].token = 0;
	//------------------------
	sprintf(msg, "Destroyed token %u \n", token);
        status = send_msg(msg, 200, client); cBYE(status);
	break;
      }
    }
    if ( token_found == false ) { 
      fprintf(stderr, "Server does not know of token %u \n", token);
      go_BYE(-1); 
    }
  }
  else if (strcasecmp(url, "info") == 0) {
    send_server_info(client);
  }
  else if (strcasecmp(url, "plugins") == 0) {
    /*
      if ( valid_qid(query_string, id_buf, query_string_no_qid, &map_indx)) {
      //execute_plugin(path, query_string, client); 
      execute_plugin(path, query_string_no_qid, client, map_indx); 
      }
      else {
      send_invalid_req(client);
      }
    */
  }
  else if ( strcasecmp(url, "q" ) == 0) {
    char envbuf1[MAX_LEN_DIR_NAME+65];
    char envbuf2[MAX_LEN_DIR_NAME+65];
    char tempfile[64];
    long long ltemp; bool is_null = true;
    zero_string(tempfile, 64);

    if ( strcasecmp(url, "q" ) == 0 ) {

      zero_string(envbuf1, MAX_LEN_DIR_NAME+65);
      zero_string(envbuf2, MAX_LEN_DIR_NAME+65);
      status = extract_I8(query_string, "TOKEN=", "&", &ltemp, &is_null);
      cBYE(status);
      if ( is_null == true ) { go_BYE(-1); }
      if ( ltemp <= 0 ) { go_BYE(-1); }
      token = ltemp;
      int tbspc_idx = -1;
      for ( int i = 0; i < g_num_tablespace; i++ ) { 
	if ( g_tablespace[i].token == token ) { 
	  tbspc_idx = i;
	  break;
	}
      }
      if ( tbspc_idx < 0 ) { 
	status = send_msg("Invalid token", 500, client); 
	go_BYE(-1); 
      }
      // Break up query string and pass it to Q
      sprintf(envbuf1, "Q_DOCROOT=%s",  g_tablespace[tbspc_idx].q_docroot); 
      putenv(envbuf1);

      int qargc = 0;
      status = conv_query_string_to_args(encoded_query_string, &qargc, qargv);
      cBYE(status);
      /* THIS IS A DUMMY FOR TESTING 
	 int qargc = 2; 
	 qargv = malloc(qargc * sizeof(char *));
	 return_if_malloc_failed(qargv);
	 qargv[0] = strdup("q");
	 qargv[1] = strdup("list_tbls");
      */
      /* TODO P1: Open this file on RAMFS for performance boost */
      for ( int i = 0; i < qargc; i++ ) {
	fprintf(stderr, "qargv[%d] = %s \n", i, qargv[i]);
      }

      strcpy(tempfile, "/tmp/_tempf_XXXXXX");
      int fd = mkstemp(tempfile);
      close(fd);
      ofp = fopen(tempfile, "w"); 
      return_if_fopen_failed(ofp, tempfile, "w");
      status = q(qargc, qargv, qrslt_buf, RSLT_BUF_SIZE, ofp); cBYE(status);
      fclose_if_non_null(ofp);
      // Check whether output file exists 
      // -------------------------------------------------
      bool is_output_file = false;
      fd = open(tempfile, O_RDWR);
      if ( fd <= 0 ) { go_BYE(-1); }
      struct stat filestat;
      status = fstat(fd, &filestat); cBYE(status);
      size_t filesz = filestat.st_size;
      if ( filesz > 0 ) {
	is_output_file = true;
      }
      if ( *qrslt_buf == '\0' ) {
	if ( is_output_file == false ) { 
          status = send_msg("", 200, client); cBYE(status);
	}
	else {
	  status = csv_to_json(tempfile, &Y, &nY); cBYE(status);
          status = send_json(Y, nY, client); cBYE(status);
	  free_if_non_null(Y);
	}
      }
      else {
        status = send_scalars(qrslt_buf, client); cBYE(status);
      }
      close(fd);
      unlink(tempfile);
    }
    else {
      go_BYE(-1);
    }
  }
  else {
    bool is_file = false;
    bool discard_headers;
    if ( strcmp(method, "GET") == 0 ) { 
      discard_headers = true;
    }
    else if ( strcmp(method, "POST") == 0 ) { 
      discard_headers = false;
    }
    else {
      go_BYE(-1);
    }

    if ( ( *encoded_query_string == '\0' ) && 
         ( *encoded_post_params == '\0' ) ) {
      is_file = true;
    }
    is_file = true; // TODO P0 Just for debugging
    if ( is_file == true ) {
      // Assumption is that url is a file on the server
      bool file_exists = true;
      if ( stat(url, &st) < 0 ) { file_exists = false; }
      if ( file_exists == false ) { 
	file_exists =  true;
	url = "hello_world.html";
      }
      if ( file_exists == false ) { 
	/* read and disacard headers */
	while ((numchars > 0) && strcmp("\n", buf)) {
	  numchars = get_line(client, buf, sizeof(buf));
	  // fprintf(stderr, "request line to discard = %s\n", buf);
	} 
	not_found(client);
      } 
      else {
	serve_file(client, url, discard_headers);
      }
    }
    else { 
      fprintf(stderr, "need to execute a script\n");
    }
  }
 BYE:
  free_if_non_null(Y);
  free_if_non_null(query_string);
  free_if_non_null(post_params);
  fclose_if_non_null(ofp);
  return status ;
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
#define METHOD_LEN 8
#define BUF_LEN    2048
#define URL_LEN    128
// START FUNC DECL
void *accept_request(
		     void *voidptr
		     )
// STOP FUNC DECL
{
  int status = 0;
  int *iptr = (int *)voidptr;
  int client = *iptr;

  char buf[BUF_LEN];
  char post_params[BUF_LEN];
  char in_url[BUF_LEN];
  char query_string[BUF_LEN];

  char *url = NULL;
  char base_url[URL_LEN];
  ssize_t num_chars;
  char method[8];

  // Initialize arrays to null
  for ( int i = 0; i < 8;       i++ ) { method[i] = '\0'; }

  for ( int i = 0; i < BUF_LEN; i++ ) { buf[i] = '\0'; }
  for ( int i = 0; i < BUF_LEN; i++ ) { in_url[i] = '\0'; }
  for ( int i = 0; i < BUF_LEN; i++ ) { post_params[i] = '\0'; }
  for ( int i = 0; i < BUF_LEN; i++ ) { query_string[i] = '\0'; }

  for ( int i = 0; i < URL_LEN; i++ ) { base_url[i] = '\0'; }

  num_chars = get_line(client, in_url, BUF_LEN-1);
  if ( num_chars == 0 ) { go_BYE(-1); }

  // fprintf(stderr, "request line =[%s]\n", buf); fflush(stderr);
  int bufidx = -1;
  int content_length = 0;
  if ( strncasecmp(in_url, "POST ", 5) == 0 ) {
    strcpy(method, "POST"); 
  }
  else if ( strncasecmp(in_url, "GET ", 4) == 0 ) { 
    strcpy(method, "GET");
  }
  else {
    unimplemented(client); go_BYE(-1); 
  }
  // START: Consume headers
  bool is_content = false;
  for ( int lno = 0; ; lno++ ) { 
    //fprintf(stderr, "Reading Line %d of headers \n", lno);
    zero_string_to_nullc(buf);
    if ( is_content == false ) { 
      num_chars = get_line(client, buf, BUF_LEN-1);
      char *cptr = strstr(buf, "Content-Length: ");
      if ( cptr != NULL ) { 
	char *endptr; char tempbuf[16]; int idx = 0;
	for ( char *cptr = buf + strlen("Content-Length: "); 
	      (( *cptr != '\n') && ( *cptr != '\0')); cptr++ ) {
	  if ( idx >= 8 ) { go_BYE(-1); }
	  tempbuf[idx++] = *cptr;
	}
	tempbuf[idx++] = '\0';
	content_length = strtoll(tempbuf, &endptr, 10); 
	if ( *endptr != '\0' ) { go_BYE(-1); }
	if ( content_length <= 0 ) { go_BYE(-1); } // TODO: P1 Improve
	// fprintf(stderr, "content_length = %d \n", content_length);
      }
      if ( num_chars == 1 ) { 
	is_content = true;  // next line is content
      }
    }
    else {
      if ( strcmp(method, "GET") == 0 ) { 
	break;
	/* Nothing to do */
      }
      else if ( strcmp(method, "POST") == 0 ) { 
        num_chars = recv (client, post_params, content_length, 0);
        if ( num_chars != content_length ) { go_BYE(-1); }
        post_params[content_length] = '&'; // helps with parsing
        break;
      }
      else {
	go_BYE(-1);
      }
    }
    // fprintf(stderr, "received %3d = %s \n", (int)num_chars, buf);
  }
  // STOP: Consume headers
  // START: Extract the URL by jumping over the space and reading till HTTP
  bufidx = strlen(method) + 1; // +1 for space after method
  // skip over any white space before url
  for ( ; ( ( isspace(in_url[bufidx]) ) && ( bufidx < BUF_LEN) ); bufidx++ ) {
  }
  url = in_url + bufidx; 
  char *cptr = strstr(url, " HTTP/1.1");
  if ( cptr == NULL ) { go_BYE(-1); }
  *cptr = '\0';
  // fprintf(stderr, "url = %s\n", url); fflush(stderr);
  // STOP: Extract the URL by jumping over the space and reading till HTTP
  //-----------------------------------------------
  // Now get the base url  = url minus get parameters
  int len = strlen(url);
  if ( ( len == 0 ) || ( ( len == 1 ) && ( url[0] == '/' ) ) ) {
    // TODO: What do we do here? If anything?
  }
  else {
    char *cptr = strstr(url, "?"); 
    if ( cptr == NULL ) { /* there are no GET parameters */
      strcpy(base_url, url+1); // +1 to get rid of slash
    }
    else {
      strncpy(base_url, url+1, (cptr-url-1));
      strcpy(query_string, url + (1 + (cptr - url)));
      strcat(query_string, "&"); /* helps with splitting things up */
    }
  }
  if ( strcasecmp(base_url, "shutdown") == 0 ) { 
    g_is_shutdown = true;
    status = send_msg("SHUTTING DOWN", 200, client); 
  }
  else {
    fprintf(stderr, "method = %s \n", method);
    fprintf(stderr, "base_url = %s \n", base_url);
    fprintf(stderr, "query_string = %s \n", query_string);
    fprintf(stderr, "post_params = %s \n", post_params);
    status = process_req(method, base_url, query_string, post_params, client); 
    if ( status < 0 ) { 
      status = send_msg("UNIDENTIFIED ERROR", 500, client); 
    }
  }
  close(client);

 BYE:
  return ((void *)g_accept_request_ret_val);
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
// START FUNC DECL
void bad_request(
		 int client
		 )
// STOP FUNC DECL
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "<P>Your browser sent a bad request, ");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "such as a POST without a Content-Length.\r\n");
  send(client, buf, sizeof(buf), 0);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
// START FUNC DECL
void cat(
	 int client, 
	 FILE *resource
	 )
// STOP FUNC DECL
{
  char buf[1024];

  fgets(buf, sizeof(buf), resource);
  while (!feof(resource)) {
    send(client, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), resource);
  }
}

// START FUNC DECL
int send_msg(
	     char *msg,
	     int status_code,
	     int client
	     )
// STOP FUNC DECL
{
  int status = 0;
  char buf[1024];
  char tbuf[1024];

  if ( status_code == 200 ) { 
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
  }
  else if ( status_code == 500 ) { 
    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
  }
  else {
    go_BYE(-1);
  }
  send(client, buf, strlen(buf), 0);

  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);

  if ( msg == NULL ) { go_BYE(-1); }

  sprintf(buf, "Content-Type: application/json\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(tbuf, "{ \"msg\" : \"%s\"}", msg);
  sprintf(buf, "Content-Length: %ld\r\n", strlen(tbuf));
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "{ \"msg\" : \"%s\"}", msg);
  send(client, buf, strlen(buf), 0);

 BYE:
  return status ;
}
// START FUNC DECL
int send_json(
	      char *Y,
	      size_t nY,
	      int client
	      )
// STOP FUNC DECL
{
  int status = 0;
  char buf[4096];
  size_t len, bytes_sent;

  zero_string(buf, 4096);
  sprintf(buf, "HTTP/1.0 200 OK\r\n"); len = strlen(buf);
  bytes_sent = send(client, buf, len, 0);
  if ( bytes_sent != len ) { err_go_BYE(); }

  sprintf(buf, SERVER_STRING); len = strlen(buf);
  bytes_sent = send(client, buf, len, 0);
  if ( bytes_sent != len ) { err_go_BYE(); }

  sprintf(buf, "Content-Type: application/json\r\n");len = strlen(buf);
  bytes_sent = send(client, buf, len, 0);
  if ( bytes_sent != len ) { err_go_BYE(); }

  sprintf(buf, "Content-Length: %ld\r\n", nY); len = strlen(buf);
  bytes_sent = send(client, buf, len, 0);
  if ( bytes_sent != len ) { err_go_BYE(); }

  sprintf(buf, "\r\n");len = strlen(buf);
  bytes_sent = send(client, buf, len, 0);
  if ( bytes_sent != len ) { err_go_BYE(); }

  bytes_sent = send(client, Y, nY, 0);
  if ( bytes_sent != nY ) { err_go_BYE(); }

 BYE:
  return status ;
}


// START FUNC DECL
int send_scalars(
		 char *scalars,
		 int client
		 )
// STOP FUNC DECL
{
  int status = 0;
  char buf0[1024];
  char buf[4096];

  if ( scalars == NULL ) { go_BYE(-1); }

  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  send(client, buf, strlen(buf), 0);

  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);

  sprintf(buf, "Content-Type: application/json\r\n");
  send(client, buf, strlen(buf), 0);

  strcpy(buf, "{ " );

  char *saveptr = NULL;
  for ( int i = 0; ; i++ ) {
    char *this_scalar = NULL;
    if ( i == 0 ) {
      this_scalar = strtok_r(scalars, ":", &saveptr); 
    }
    else {
      this_scalar = strtok_r(NULL, ":", &saveptr); 
    }
    if ( ( this_scalar == NULL ) || ( *this_scalar == '\0' ) ) { break; }
    if ( i > 0 ) { strcat(buf, " , " ); }
    sprintf(buf0, "\"val%d\" : \"", i);
    for ( char *cptr = this_scalar;  
	  ( ( *cptr != '\n' ) && ( *cptr != '\0' ) ); cptr++ ) { 
      strncat(buf0, cptr, 1);
    }
    strcat(buf0, "\""); 
    strcat(buf, buf0);
  }
  strcat(buf, " }" );

  sprintf(buf0, "Content-Length: %ld\r\n", strlen(buf));
  send(client, buf0, strlen(buf0), 0);

  sprintf(buf0, "\r\n");
  send(client, buf0, strlen(buf0), 0);

  send(client, buf, strlen(buf), 0);


 BYE:
  return status ;
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
// START FUNC DECL
void error_die(
	       const char *sc
	       )
// STOP FUNC DECL
{
  perror(sc);
  exit(1);
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
// START FUNC DECL
int get_line(
	     int sock, 
	     char *buf, 
	     int size
	     )
// STOP FUNC DECL
{
  int i = 0;
  char c = '\0';
  int n;

  while ((i < size - 1) && (c != '\n')) {
    n = recv(sock, &c, 1, 0);
    /* DEBUG printf("%02X\n", c); */
    if (n > 0)
      {
	if (c == '\r')
	  {
	    n = recv(sock, &c, 1, MSG_PEEK);
	    /* DEBUG printf("%02X\n", c); */
	    if ((n > 0) && (c == '\n'))
	      recv(sock, &c, 1, 0);
	    else
	      c = '\n';
	  }
	buf[i] = c;
	i++;
      }
    else
      c = '\n';
  }
  buf[i] = '\0';
  return i ;
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
// START FUNC DECL
void headers(
	     int client, 
	     const char *filename
	     )
// STOP FUNC DECL
{
  char buf[1024];
  (void)filename;  /* could use filename to determine file type */

  strcpy(buf, "HTTP/1.1 200 OK\r\n");
  send(client, buf, strlen(buf), 0);

  strcpy(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);

  /*
    sprintf(buf, "Location: http://www.yahoo.com/");
    send(client, buf, strlen(buf), 0);
  */

  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);

  strcpy(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
}


// START FUNC DECL
void send_invalid_req(
		      int client
		      )
// STOP FUNC DECL
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 400 Bad Request\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><TITLE>Bad Request</TITLE>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "your request because the Q identifier is invalid\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "or missing.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}
/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
// START FUNC DECL
void not_found(
	       int client
	       )
// STOP FUNC DECL
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "your request because the resource specified\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "is unavailable or nonexistent.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
// START FUNC DECL
void serve_file(
		int client, 
		const char *filename,
		bool discard_headers
		)
// STOP FUNC DECL
{
  FILE *fp = NULL;
  int numchars = 1;
  char buf[1024];

  if ( discard_headers == true ) { 
    buf[0] = 'A'; buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))  { 
      /* read & discard headers */
      numchars = get_line(client, buf, sizeof(buf));
    }
  }

  fp = fopen(filename, "r");
  if ( fp == NULL) {
    not_found(client);
  }
  else {
    headers(client, filename);
    cat(client, fp);
  }
  fclose_if_non_null(fp);
}

/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
// START FUNC DECL
int startup(
	    uint16_t *port
	    )
// STOP FUNC DECL
{
  int httpd = 0;
  struct sockaddr_in name;

  httpd = socket(PF_INET, SOCK_STREAM, 0);
  if (httpd == -1)
    error_die("socket");
  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(*port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
    error_die("bind");

  if (listen(httpd, SERVER_REQ_Q) < 0)
    error_die("listen");
  return httpd ;
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
// START FUNC DECL
void unimplemented(
		   int client
		   )
// STOP FUNC DECL
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</TITLE></HEAD>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}

/**********************************************************************/

#define MAXLINE 512
int read_conf_file(
		   char *infile,
		   TABLE_SPACE_TYPE **ptr_tbspc,
		   int *ptr_num_tbspc
		   )
{
  int status = 0;
  TABLE_SPACE_TYPE *tbspc = NULL;
  int num_tbspc = 0;
  FILE *ifp = NULL;

  *ptr_tbspc = NULL;
  *ptr_num_tbspc = 0;
  //-----------------------------------------------------
  if ( infile == NULL ) { go_BYE(-1); } 
  status = num_lines(infile, &num_tbspc); cBYE(status);
  num_tbspc--; // -1 for header line 
  if ( num_tbspc < 1 ) { go_BYE(-1); } 
  if ( num_tbspc >  SERVER_REQ_Q ) { go_BYE(-1); }
  tbspc = malloc(num_tbspc * sizeof(TABLE_SPACE_TYPE));
  return_if_malloc_failed(tbspc); 
  for ( int i = 0; i < num_tbspc; i++ ) { 
    zero_tbspc(&(tbspc [i]));
  }
  //-----------------------------------------------------
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  int lno = 0; int i = 0;
  char *line = NULL;
  for ( lno = 0; lno < num_tbspc+1; lno++ ) {  // +1 for header line
    char in_line[MAXLINE];
    int num_commas = 0;
    if ( feof(ifp) ) { break; }
    char *cptr = fgets(in_line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    free_if_non_null(line);
    line = strdup(in_line); 
    /* strip quotes */
    int idx = 0;
    for ( char *cptr = in_line; *cptr != '\0'; cptr++ ) { 
      if ( *cptr != '"' ) { 
	line[idx++] = *cptr;
      }
    }
    line[idx] = '\0';
    if ( lno == 0 ) { // header line 
      if ( strcmp(line, "lgcl_name,q_docroot,dsk_data_dir,ram_data_dir,bs_docroot\n") != 0 ) {
	go_BYE(-1);
      }
      continue; 
    } 
    status = count_char(line, ',', &num_commas); cBYE(status);
    int num_flds = 5;
    int len;
    if ( num_commas != (num_flds-1) ) { go_BYE(-1); }
    cptr = line;
    for ( int j = 0; j < num_flds; j++ ) {
      int k = 0;
      for ( ; (( *cptr != ',' ) && ( *cptr != '\n' ) && ( *cptr != '\0' )); ) { 
        switch ( j ) { 
	case 0 : tbspc[i].lgcl_name[k++]  = *cptr++; break;
	case 1 : tbspc[i].q_docroot[k++]  = *cptr++; break;
	case 2 : tbspc[i].dsk_data_dir[k++] = *cptr++; break;
	case 3 : tbspc[i].ram_data_dir[k++] = *cptr++; break;
	case 4 : tbspc[i].bs_docroot[k++] = *cptr++; break;
	default : go_BYE(-1); break;
	}
      }
      cptr++; // jump over delim
    }
    // Eliminate trailing slash if any 
    //------------------------------------------------
    len = strlen(tbspc[i].bs_docroot);
    if ( len == 0 ) { go_BYE(-1); }
    if ( tbspc[i].bs_docroot[len-1] == '/' ) {
      tbspc[i].bs_docroot[len-1] = '\0';
    }
    //------------------------------------------------
    len = strlen(tbspc[i].q_docroot);
    if ( len == 0 ) { go_BYE(-1); }
    if ( tbspc[i].q_docroot[len-1] == '/' ) {
      tbspc[i].q_docroot[len-1] = '\0';
    }
    //------------------------------------------------
    len = strlen(tbspc[i].dsk_data_dir);
    if ( len == 0 ) { go_BYE(-1); }
    if ( tbspc[i].dsk_data_dir[len-1] == '/' ) {
      tbspc[i].dsk_data_dir[len-1] = '\0';
    }
    //------------------------------------------------
    len = strlen(tbspc[i].ram_data_dir);
    if ( len != 0 ) { 
      if ( tbspc[i].ram_data_dir[len-1] == '/' ) {
        tbspc[i].ram_data_dir[len-1] = '\0';
      }
    }
    //------------------------------------------------
    i++;
  }
  free_if_non_null(line);
  char cwd[MAX_LEN_DIR_NAME+1];
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  for ( int i = 0; i < num_tbspc; i++ ) { 
    for ( int j = i+1; j < num_tbspc; j++ ) { 
      if ( strcmp(tbspc[i].q_docroot, tbspc[j].q_docroot) == 0 ) { go_BYE(-1); }
      if ( strcmp(tbspc[i].dsk_data_dir, tbspc[j].dsk_data_dir) == 0 ) { go_BYE(-1); }
      if ( ( strlen(tbspc[i].ram_data_dir) != 0 ) && 
           ( strlen(tbspc[j].ram_data_dir) != 0 ) ) { 
        if ( strcmp(tbspc[i].ram_data_dir, tbspc[j].ram_data_dir) == 0 ) { 
	  go_BYE(-1); 
	}
      }
    }
    bool check_dirs_exist = false; // TODO P1 Set this to true
    if ( check_dirs_exist ) { 
      // check that directories exist
      status = chdir(tbspc[i].dsk_data_dir); cBYE(status);
      if ( strlen(tbspc[i].ram_data_dir) != 0 ) { 
        status = chdir(tbspc[i].ram_data_dir); cBYE(status);
      }
      status = chdir(tbspc[i].q_docroot); cBYE(status);
      status = chdir(tbspc[i].bs_docroot); cBYE(status);
      status = chdir(cwd); cBYE(status);
    }
  }
  *ptr_num_tbspc = num_tbspc;
  *ptr_tbspc     = tbspc;
 BYE:
  fclose_if_non_null(ifp);
  return status ;
}

/**********************************************************************/


int main(
	 int argc,
	 char **argv
	 )
{
  int status = 0;
  int server_sock = -1;
  uint16_t port = SERVER_PORT;
  int client_sock = -1;
  struct sockaddr_in client_name;
  socklen_t client_name_len = sizeof(client_name);
  qargv = NULL; 

  long long seed = 2059879141 * ( ( getpid() *1024 ) + getppid() );
  srand48((long int)seed);
  g_tablespace = NULL;
  g_num_tablespace = 0;
  g_is_shutdown = false;

  if ( argc != 2 ) { go_BYE(-1); }
  // What does this initialization do? TODO
  nullify(g_accept_request_ret_val, ERR_MSG_LEN);
  qargv = malloc(MAX_NUM_ARGS * sizeof(char *));
  return_if_malloc_failed(qargv);
  for ( int i = 0; i < MAX_NUM_ARGS; i++ ) { 
    qargv[i] = malloc((MAX_LEN_ARG+1) * sizeof(char));
    return_if_malloc_failed(qargv[i]);
    zero_string(qargv[i], MAX_LEN_ARG+1);
  }

  g_num_tablespace = 0 ;
  char *conf_file = argv[1];
  if ( *conf_file == '\0' ) { go_BYE(-1); }

  status = read_conf_file(conf_file, &g_tablespace, &g_num_tablespace); 
  cBYE(status);

  server_sock = startup(&port);

  if ( pthread_mutex_init(&g_tablespace_lock, NULL) != 0) {
    fprintf(stderr, "mutex init failed\n");
    error_die("mutex_init");
  }

#undef THREADED
#ifdef THREADED
  printf("httpd running on port %d in multi threaded mode!\n", port);
#else
  printf("httpd running on port %d in single thread mode!\n", port);
#endif

  while (1) {
    client_sock = accept(server_sock,
			 (struct sockaddr *)&client_name,
			 &client_name_len);

    int flag=1; int sz = sizeof(int);
    setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sz);
    if (client_sock == -1) {
      error_die("accept");
    }

#ifdef THREADED
    pthread_t newthread;
    if (pthread_create(&newthread , NULL, accept_request, &client_sock) != 0)
      perror("pthread_create");
#else
    accept_request(&client_sock);
#endif

    if ( g_is_shutdown == true ) { fprintf(stderr, "Shutting down\n"); break; }
    if ( *g_accept_request_ret_val != '\0' ) { 
      // Control should not come here (as of now)
      go_BYE(-1);
    }
    nullify(g_accept_request_ret_val, ERR_MSG_LEN);
  }
  close(server_sock);
 BYE:
  free_if_non_null(g_tablespace);
  return status ;
}
