#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
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
#define GPU_PORT 8080
#define SERVER_REQ_Q 16 // number of concurrent requests 

typedef struct table_space_type {
  unsigned int token; // token > 0 => busy 
  char lgcl_name[MAX_LEN_DIR_NAME+1];
  char bs_docroot[MAX_LEN_DIR_NAME+1];
  char q_docroot[MAX_LEN_DIR_NAME+1];
  char q_data_dir[MAX_LEN_DIR_NAME+1];
} TABLE_SPACE_TYPE;

char **qargv;
extern int mkstemp(char *template);

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
  int qargc = 0;
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
    zero_string(qargv[i], MAX_LEN_ARG);
  }
  status = url_decode(encoded_qstr, &qstr); cBYE(status);

  /* Now we have the q command of the form "f1s1opf2 T1 f1 100 + f2 " */
  // skip over leading spaces 
  bool all_done = false;
  char *cptr = qstr;
  for ( ; ; ) { 
    // skip over spaces, if any 
    for ( ; *cptr != '\0' ; cptr++) {
      if ( !isspace(*cptr) ) { break; }
    }
    if ( *cptr == '\0' ) { break; }
    // Now start copying text into qargv[qargc][..] */
    int argidx = 0;
    for ( ; ; cptr++) {
      if ( ( isspace(*cptr) ) || ( *cptr == '\0' ) ) {
        qargc++; 
	if ( *cptr == '\0' ) { all_done = true; }
        cptr++; /* skip over space that caused break */ 
        break; 
      }
      if ( argidx >= MAX_LEN_ARG ) { go_BYE(-1); }
      qargv[qargc][argidx++] = *cptr;
    }
    if ( all_done == true ) { break; } 
  }
  // Delete enclosing single quotes if any 
  for ( int i = 0; i < qargc; i++ ) { 
    if ( qargv[i][0] == '\'' ) {
      int len = strlen(qargv[i]);
      if ( qargv[i][len-1] != '\'' ) { go_BYE(-1); }
      for ( int j = 0; j < len-2; j++ ) {
        qargv[i][j] = qargv[i][j+1];
      }
      qargv[i][len-1] = '\0';
      qargv[i][len-2] = '\0';
    }
  }
  //----------------------------------------------------
  *ptr_qargc = qargc;
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
  nullify(ptr_tbspc->q_docroot,  MAX_LEN_DIR_NAME+1);
  nullify(ptr_tbspc->q_data_dir, MAX_LEN_DIR_NAME+1);
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
      serve_file(client, tempfile);
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
      serve_file(client, tempfile);
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
int process_get(
		char *url, 
		char *encoded_query_string, 
		int client
		)
// STOP FUNC DECL
{
  int status = 0;
  char buf[1024];
  int numchars;
  struct stat st;
  char id_buf[16];
  int map_indx = -1;
  unsigned int token = 0; // to be set 
  char qrslt_buf[1024]; int sz_qrslt_buf = 1024;
#define MAX_LEN_LGCL_NAME 31
  char lgcl_name[MAX_LEN_LGCL_NAME+1];
  char *query_string = NULL;

  status = url_decode(encoded_query_string, &query_string); cBYE(status);

  if ( strcasecmp(url, "gq") == 0) {
    char envbuf1[MAX_LEN_DIR_NAME+65];
    char envbuf2[MAX_LEN_DIR_NAME+65];
    char msg[1024]; long long ltemp; bool is_null = true;

    sprintf(msg, "GQ executed query_string %s \n", encoded_query_string);
    status = send_msg(msg, 200, client); cBYE(status);
 BYE:
  free_if_non_null(query_string);
  return status ;
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
#define METHOD_LEN 8
#define BUF_LEN    1024
#define URL_LEN    256
#define PATH_LEN   512
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
  char base_url[URL_LEN];
  char *url = NULL;
  char query_string[URL_LEN];

  // Initialize arrays to null
  for ( int i = 0; i <    BUF_LEN; i++ ) { buf[i] = '\0'; }
  for ( int i = 0; i <    URL_LEN; i++ ) { base_url[i] = '\0'; }
  for ( int i = 0; i <    URL_LEN; i++ ) { query_string[i] = '\0'; }

  int num_chars = get_line(client, buf, BUF_LEN-1);
  if ( num_chars == 0 ) { go_BYE(-1); }

  // fprintf(stderr, "request line =[%s]\n", buf); fflush(stderr);
  int bufidx = 0;
  // Currently we handle only GET method. Hence 1st ffour characters
  // should be "GET "
  if ( strncasecmp(buf, "GET ", 4) != 0 ) { 
    unimplemented(client); go_BYE(-1); 
  }
  bufidx = 4;
  // skip over any white space before url
  for ( ; ( ( isspace(buf[bufidx]) ) && ( bufidx < BUF_LEN) ); bufidx++ ) {
  }
  url = buf + bufidx; 
  if ( strlen(url) == 0 ) {
    unimplemented(client); go_BYE(-1); 
  }
  // Eliminate the training " HTTP/1.1"
  char *cptr = strstr(url, " HTTP/1.1");
  if ( cptr == NULL ) { go_BYE(-1); }
  *cptr = '\0';
  // fprintf(stderr, "url = %s\n", url); fflush(stderr);
  // Now get the base url 
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
  status = process_get(base_url, query_string, client); 
  if ( status < 0 ) { 
    status = send_msg("UNIDENTIFIED ERROR", 500, client); 
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
  while (!feof(resource))
    {
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

  if ( ( msg == NULL ) || ( *msg == '\0' ) ) { go_BYE(-1); }

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
		const char *filename
		)
// STOP FUNC DECL
{
  FILE *resource = NULL;
  int numchars = 1;
  char buf[1024];

  buf[0] = 'A'; buf[1] = '\0';
  while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
    numchars = get_line(client, buf, sizeof(buf));

  resource = fopen(filename, "r");
  if (resource == NULL) {
    not_found(client);
  }
  else {
    headers(client, filename);
    cat(client, resource);
  }
  fclose(resource);
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
  if ( num_tbspc <= 1 ) { go_BYE(-1); } // 1 for header line 
  num_tbspc--; // -1 for header line 
  tbspc = malloc(num_tbspc * sizeof(TABLE_SPACE_TYPE));
  return_if_malloc_failed(tbspc); 
  for ( int i = 0; i < SERVER_REQ_Q; i++ ) { 
    zero_tbspc(&(tbspc [i]));
  }
  //-----------------------------------------------------
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  int lno = 0; int i = 0;
  for ( lno = 0; lno < num_tbspc+1; lno++ ) {  // +1 for header line
    char line[MAXLINE];
    int num_commas = 0;
    if ( feof(ifp) ) { break; }
    char *cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    if ( lno == 0 ) { // header line 
      if ( strcmp(line, "lgcl_name,q_docroot,q_data_dir,bs_docroot\n") != 0 ) {
	go_BYE(-1);
      }
      continue; 
    } 
    status = count_char(line, ',', &num_commas); cBYE(status);
    int num_flds = 4;
    if ( num_commas != (num_flds-1) ) { go_BYE(-1); }
    cptr = line;
    for ( int j = 0; j < num_flds; j++ ) {
      int k = 0;
      for ( ; (( *cptr != ',' ) && ( *cptr != '\n' ) && ( *cptr != '\0' )); ) { 
        switch ( j ) { 
	case 0 : tbspc[i].lgcl_name[k++]  = *cptr++; break;
	case 1 : tbspc[i].q_docroot[k++]  = *cptr++; break;
	case 2 : tbspc[i].q_data_dir[k++] = *cptr++; break;
	case 3 : tbspc[i].bs_docroot[k++] = *cptr++; break;
	default : go_BYE(-1); break;
	}
      }
      cptr++; // jump over delim
    }
    if ( strlen(tbspc[i].lgcl_name)  == 0 ) { go_BYE(-1); }
    if ( strlen(tbspc[i].bs_docroot) == 0 ) { go_BYE(-1); }
    if ( strlen(tbspc[i].q_docroot)  == 0 ) { go_BYE(-1); }
    if ( strlen(tbspc[i].q_data_dir) == 0 ) { go_BYE(-1); }
    i++;
  }
  char cwd[MAX_LEN_DIR_NAME+1];
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  for ( int i = 0; i < num_tbspc; i++ ) { 
    for ( int j = i+1; j < num_tbspc; j++ ) { 
      if ( strcmp(tbspc[i].q_docroot, tbspc[j].q_docroot) == 0 ) { go_BYE(-1); }
      if ( strcmp(tbspc[i].q_data_dir, tbspc[j].q_data_dir) == 0 ) { go_BYE(-1); }
    }
    bool check_dirs_exist = false; // TODO P1 Set this to true
    if ( check_dirs_exist ) { 
      // check that directories exist
      status = chdir(tbspc[i].q_data_dir); cBYE(status);
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

GPU_REG_TYPE *g_gpu_reg;
int           g_n_gpu_reg;


int main(
	 int argc,
	 char **argv
	 )
{
  int status = 0;
  int server_sock = -1;
  uint16_t port = GPU_PORT;
  int client_sock = -1;
  struct sockaddr_in client_name;
  socklen_t client_name_len = sizeof(client_name);
  qargv = NULL; 
  //---- gpu stuff
  struct timeval start, end;
  string rslt_buf, cmd;
  vector<string> argv; // Create vector to hold our words

  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  g_gpu_reg = NULL;
  g_n_gpu_reg = 0;

  status = chk_env_vars(g_docroot, g_data_dir, g_gpu_server, &g_gpu_port);
  cBYE(status);
  if ( getcwd(g_cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  status = g_init_meta_data(g_docroot); cBYE(status);

  //---- gpu stuff

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

    if ( *g_accept_request_ret_val != '\0' ) { 
      // TODO: send error message to client 
    }
    nullify(g_accept_request_ret_val, ERR_MSG_LEN);
  }

  close(server_sock);
 BYE:
  return status ;
}
