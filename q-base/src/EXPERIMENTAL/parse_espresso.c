#include <ctype.h> 
#include <stdio.h> 
#include "constants.h"
#include "macros.h"
#include "fsize.h"

#define LINETYPE_SIMPLE_FIELD 1
#define LINETYPE_MAP_FIELD    2
#define MAXLINE 65536
#define MAXVAL  1024

char g_buffer[MAXLINE];

  /*-------------------------------------------------------------*/
  int
    consume_char(
	char **ptr_cptr,
	char to_consume
	)
    {
      int status = 0;
      char *cptr = *ptr_cptr;
      if ( *cptr != to_consume ) { go_BYE(-1); }
      cptr++;
      *ptr_cptr = cptr;
BYE:
      return(status);
    }
  /*-------------------------------------------------------------*/
  /*--------------------------------------------------------------*/
  int
    copy_till_char(
	char **ptr_cptr,
	char *buffer,
	char breaker
	)
    {
      int status = 0;
      char *cptr = *ptr_cptr;
  for ( int i = 0; ( ( *cptr != '\0' ) && ( *cptr != breaker) ); cptr++, i++ ) { buffer[i] = *cptr; }
  *ptr_cptr = cptr;
   BYE:
  return(status);
    }
  /*--------------------------------------------------------------*/
int
consume_path_instances_currentstates(
    FILE *ofp,
    int lno, 
    int version,
    char *path
    )
{
  int status = 0;
  char *cptr, *cptr2;
  cptr = strstr(path, "/INSTANCES/");
  if ( cptr == NULL ) { return(status); }
  cptr2 = strstr(path, "/CURRENTSTATES/");
  if ( cptr2 != NULL ) { 
  fprintf(ofp, "%d,%d", lno, version);
  char *xptr = path;
  consume_char(&xptr, '/'); // consume first slash
  // consume till next slash
  copy_till_char(&xptr, g_buffer, '/'); // this is cluster name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // cluster name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is INSTANCES
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is instance name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // instance name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is CURRENTSTATES
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); 
  consume_char(&xptr, '/'); // consume slash
  /* this is sessionid */
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); 
  zero_string_to_nullc(g_buffer);

  copy_till_char(&xptr, g_buffer, '/'); 
  /* this is resource_group_name */
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); 
  zero_string_to_nullc(g_buffer);

  fprintf(ofp, "\n");
  }

BYE:
  return(status);
}

int
consume_path_instances_healthreport(
    FILE *ofp,
    int lno, 
    int version, 
    char *path
    )
{
  int status = 0;
  char *cptr, *cptr2;
  cptr = strstr(path, "/INSTANCES/");
  if ( cptr == NULL ) { return(status); }
  cptr2 = strstr(path, "/HEALTHREPORT/defaultPerfCounters");
  if ( cptr2 != NULL ) { 
  fprintf(ofp, "%d,%d", lno, version);
  char *xptr = path;
  consume_char(&xptr, '/'); // consume first slash
  // consume till next slash
  copy_till_char(&xptr, g_buffer, '/'); // this is cluster name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // cluster name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is INSTANCES
  if ( strcmp(g_buffer, "INSTANCES") != 0 ) { go_BYE(-1); }
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is instance name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // instance name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is HEALTHREPORT
  if ( strcmp(g_buffer, "HEALTHREPORT") != 0 ) { go_BYE(-1); }
  consume_char(&xptr, '/'); // consume slash

  fprintf(ofp, "\n");
  }

BYE:
  return(status);
}


/*--------------------------------------------------------------*/
int
consume_path_instances_messages(
    FILE *ofp,
    int lno, 
    long long ll_version, 
    char *path
    )
{
  int status = 0;
  char *cptr, *cptr2;
  cptr = strstr(path, "/INSTANCES/");
  if ( cptr == NULL ) { return(status); }
  cptr2 = strstr(path, "/MESSAGES/");
  if ( cptr2 != NULL ) { 
  fprintf(ofp, "%d,%d", lno, (int)ll_version);
  char *xptr = path;
  consume_char(&xptr, '/'); // consume first slash
  // consume till next slash
  copy_till_char(&xptr, g_buffer, '/'); // this is cluster name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // cluster name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is INSTANCES
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is instance name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // instance name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is MESSAGES
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); 
  /* this is sessionid undescore resource_group_name */
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); 
  zero_string_to_nullc(g_buffer);

  fprintf(ofp, "\n");
  }

BYE:
  return(status);
}

/*--------------------------------------------------------------*/
int
consume_path_instances_status_updates(
    FILE *ofp,
    int lno, 
    long long ll_version, 
    char *path
    )
{
  int status = 0;
  char *cptr, *cptr2;
  cptr = strstr(path, "/INSTANCES/");
  if ( cptr == NULL ) { return(status); }
  cptr2 = strstr(path, "/STATUSUPDATES/");
  if ( cptr2 != NULL ) { 
  fprintf(ofp, "%d,%d", lno, (int)ll_version);
  char *xptr = path;
  consume_char(&xptr, '/'); // consume first slash
  // consume till next slash
  copy_till_char(&xptr, g_buffer, '/'); // this is cluster name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // cluster name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is INSTANCES
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is instance name 
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); // instance name 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); // this is STATUSUPDATES
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); 
  /* this is sessionid undescore resource_group_name */
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer); 
  zero_string_to_nullc(g_buffer);
  consume_char(&xptr, '/'); // consume slash

  copy_till_char(&xptr, g_buffer, '/'); /* this is resource_key */
  if ( *g_buffer == '\0' ) { go_BYE(-1); }
  fprintf(ofp, ",%s", g_buffer);  /* this is resource_key */
  zero_string_to_nullc(g_buffer);

  fprintf(ofp, "\n");
  }

BYE:
  return(status);
}

/*--------------------------------------------------------------*/
int
consume_path_idealstates(
    FILE *ofp,
    int lno, 
    long long ll_version, 
    char *path
    )
{
  int status = 0;
  char *cptr;
  cptr = strstr(path, "/IDEALSTATES/");
  if ( cptr == NULL ) { return(status); }
    fprintf(ofp, "%d,%d", lno, (int)ll_version);
    char *xptr = path;
    // consume first slash
    xptr++;
    // consume till next slash
    for ( int i = 0; *xptr != '/'; xptr++, i++ ) { 
      g_buffer[i] = *xptr;
    }
    if ( *g_buffer == '\0' ) { go_BYE(-1); }
    fprintf(ofp, ",%s", g_buffer); // cluster name 
    zero_string_to_nullc(g_buffer);
    xptr++; // consume slash
    // throw away till next slash
    for ( ; *xptr != '/'; xptr++ ) { }
    xptr++; // consume slash
    // consume till next slash
    for ( int i = 0; ( ( *xptr != '/' ) && ( *xptr != '\0' ) ) ; xptr++, i++ ) { 
      g_buffer[i] = *xptr;
    }
    if ( *g_buffer == '\0' ) { go_BYE(-1); }
    fprintf(ofp, ",%s\n", g_buffer); // resource group name 
    zero_string_to_nullc(g_buffer);
BYE:
  return(status);
}

int
consume_path_liveinstances(
    FILE *ofp,
    int lno, 
    char *path
    )
{
  int status = 0;
  char *cptr;
  cptr = strstr(path, "/LIVEINSTANCES/");
  if ( cptr == NULL ) { return(status); }
    fprintf(ofp, "%d", lno);
    char *xptr = path;
    // consume first slash
    xptr++;
    // consume till next slash
    for ( int i = 0; *xptr != '/'; xptr++, i++ ) { 
      g_buffer[i] = *xptr;
    }
    if ( *g_buffer == '\0' ) { go_BYE(-1); }
    fprintf(ofp, ",%s", g_buffer); // cluster name 
    zero_string_to_nullc(g_buffer);
    xptr++; // consume slash
    // throw away till next slash
    for ( ; *xptr != '/'; xptr++ ) { }
    xptr++; // consume slash
    // consume till next slash
    for ( int i = 0; ( ( *xptr != '/' ) && ( *xptr != '\0' ) ) ; xptr++, i++ ) { 
      g_buffer[i] = *xptr;
    }
    if ( *g_buffer == '\0' ) { go_BYE(-1); }
    fprintf(ofp, ",%s\n", g_buffer); // instance name
    zero_string_to_nullc(g_buffer);
BYE:
  return(status);
}

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/
int
consume_simple_fields(
		      char *in_buffer,
		      int lno,
		      int linetype,
		      char *field_name, /* used for maps, NULL for
					   simple fields  */
		      FILE *ofp
		      )
{
  int status = 0;
  if ( strlen(in_buffer) != 0 ) {
    char *yptr = in_buffer;
    for ( ; *yptr != '\0' ; ) {
      fprintf(ofp, "%d,%d,%s,", lno, linetype, field_name);
      for ( int i = 0; ( ( *yptr != '\0' ) && ( *yptr != ':' ) ) ; ) {
	g_buffer[i++] = *yptr++;
      }
      fprintf(ofp, "%s,", g_buffer);
      zero_string_to_nullc(g_buffer);
      if ( *yptr != ':' ) { 
	fprintf(stderr, "lno = %d. buffer = %s \n", lno, g_buffer);
	go_BYE(-1); 
      }
      yptr++; /* to consume the colon */
      for ( int i = 0; ( ( *yptr != '\0' ) && ( *yptr != ',' ) ) ; ) {
	g_buffer[i++] = *yptr++;
      }
      fprintf(ofp, "%s\n", g_buffer);
      zero_string_to_nullc(g_buffer);
      if ( *yptr != '\0' ) { 
	if ( *yptr != ',' ) { go_BYE(-1); }
      }
      yptr++; /* to consume the comma */
    }
  }
 BYE:
  return(status);
}
int
get_val(
	char *line,
	char *needle,
	char delim,
	char *valtype,
	long long *ptr_llval,
	char *out
	)
{
  int status = 0;
  char *cptr, *xptr, *endptr;

  cptr = strstr(line, needle); 
  if ( cptr == NULL ) { 
    return(status);
  }
  xptr = cptr + strlen(needle);
  if ( delim == ' ') { 
    for ( int i = 0; !isspace(*xptr) ; xptr++, i++ ) { 
      g_buffer[i] = *xptr;
    }
  }
  else {
    for ( int i = 0; *xptr != delim ; xptr++, i++ ) { 
      g_buffer[i] = *xptr;
    }
  }
  if ( strcmp(valtype, "long long") == 0 ) { 
    if ( ( g_buffer[0] == '0' ) &&  ( g_buffer[1] == 'x' ) ) {
      *ptr_llval = strtoll(g_buffer, &endptr, 16);
    }
    else {
      *ptr_llval = strtoll(g_buffer, &endptr, 10);
    }
    if ( *endptr != '\0' ) { 
      fprintf(stderr, "g_buffer = %s \n", g_buffer);
      go_BYE(-1); 
    }
  }
  else if ( strcmp(valtype, "char string") == 0 ) {
    strcpy(out, g_buffer);
  }
  else { go_BYE(-1);
  }
  zero_string_to_nullc(g_buffer);
 BYE:
  return(status);
}
/*---------------------------------------------------------*/
int
get_nested_val(
	char *line,
	char *needle,
	char open_delim,
	char close_delim,
	char *out
	)
{
  int status = 0;
  char *cptr, *xptr;
  int i = 0, nest_count = 0;

  if ( line == NULL ) { go_BYE(-1); }
  cptr = strstr(line, needle); if ( cptr == NULL ) { go_BYE(-1); }
  xptr = cptr + strlen(needle);
  if ( *xptr != open_delim ) { go_BYE(-1); }
  xptr++; nest_count++;
  for ( ; ; i++, xptr++ ) {
    if ( *xptr == close_delim ) { 
      if ( nest_count == 1 ) { 
	break; 
      }
      nest_count--;
    }
    if ( *xptr == open_delim ) { 
      nest_count++;
    }
    out[i] = *xptr;
  }
BYE:
  return(status);
}
/*---------------------------------------------------------*/

#define NUM_ZNODE_TYPES 6
#define IDEALSTATES 0
#define INSTANCES_STATUS_UPDATES  1
#define INSTANCES_MESSAGES        2
#define INSTANCES_CURRENTSTATES   3
#define LIVEINSTANCES             4
#define INSTANCES_HEALTHREPORT    5
int
main(
     int argc,
     char **argv 
     )
{
  int status = 0;
  char *infile, *op_prefix; 
  char *opfile1 = NULL,  *opfile3 = NULL, *opfile4 = NULL;
  char **opfiles2 = NULL;
  char line[MAXLINE];
  char data[MAXLINE];
  char buffer[MAXLINE];
  char simple_fields[MAXLINE];
  char map_fields[MAXLINE];

  char str_type[MAXVAL];
  char path[MAXVAL];
  char data_id[MAXVAL];
  char simple_field_name[MAXVAL];

  char *cptr;
  FILE *ifp = NULL; /* inout file */
  FILE *ofp1 = NULL; /* common fields */
  FILE **ofps2 = NULL; /* version and path  */
  FILE *ofp3 = NULL; /* simpleFields */
  FILE *ofp4 = NULL; /* mapFields */
  int lno;
  long long ll_time;
  long long ll_session;
  long long ll_cxid;
  long long ll_zxid;
  long long ll_timeout;
  long long ll_version;
  int itype;


  zero_string(g_buffer, MAXLINE);
  zero_string(line,     MAXLINE);
  zero_string(data,     MAXLINE);
  zero_string(buffer,   MAXLINE);
  zero_string(simple_fields, MAXLINE);
  zero_string(map_fields, MAXLINE);

  zero_string(str_type, MAXVAL);
  zero_string(path,     MAXVAL);
  zero_string(data_id,  MAXVAL);
  zero_string(simple_field_name,  MAXVAL);

  if ( argc != 3 ) { go_BYE(-1); }
  infile    = argv[1];
  op_prefix = argv[2];

  mcr_alloc_null_str(opfile1, (strlen(op_prefix) + 64));
  strcpy(opfile1, op_prefix); strcat(opfile1, "1.csv");
  ofp1 = fopen(opfile1, "w"); return_if_fopen_failed(ofp1, opfile1, "w");
  fprintf(ofp1, "lno,ll_time,ll_session,ll_cxid,ll_zxid,itype\n");

  ofps2 = (FILE **)malloc(NUM_ZNODE_TYPES * sizeof(FILE *));
  return_if_malloc_failed(ofps2);
  opfiles2 = (char **)malloc(NUM_ZNODE_TYPES * sizeof(char *));
  return_if_malloc_failed(opfiles2);
  for ( int i = 0; i < NUM_ZNODE_TYPES; i++ ) { 
    mcr_alloc_null_str(opfiles2[i], (strlen(op_prefix) + 64));
    strcpy(opfiles2[i], op_prefix); 
    strcat(opfiles2[i], "2_");
    // append znode type into file name
    switch ( i ) {
      case IDEALSTATES : 
	strcpy(buffer, "IDEALSTATES"); break;
      case INSTANCES_STATUS_UPDATES : 
	strcpy(buffer, "STATUS_UPDATES"); break;
      case INSTANCES_MESSAGES : 
	strcpy(buffer, "MESSAGES"); break;
      case INSTANCES_CURRENTSTATES : 
	strcpy(buffer, "CURRENTSTATES"); break;
      case LIVEINSTANCES : 
	strcpy(buffer, "LIVEINSTANCES"); break;
      case INSTANCES_HEALTHREPORT : 
	strcpy(buffer, "HEALTHREPORT"); break;
      default : go_BYE(1); break;
    }
    strcat(opfiles2[i], buffer);
    //-----------------------------------------------
    strcat(opfiles2[i], ".csv");
    ofps2[i] = fopen(opfiles2[i], "w"); 
    return_if_fopen_failed(ofps2[i], opfiles2[i], "w");
    switch ( i ) {
      case IDEALSTATES : 
      fprintf(ofps2[i], "lno,ll_version,cluster_name,resource_group_name\n");
	break;
      case INSTANCES_STATUS_UPDATES : 
      fprintf(ofps2[i],
	  "lno,ll_version,cluster_name,instance_name,sessionid_resource_group_name,resource_key\n");
	break;
      case INSTANCES_MESSAGES : 
      fprintf(ofps2[i],
	  "lno,version,cluster_name,instance_name,session_str\n");
	break;
      case INSTANCES_CURRENTSTATES : 
      fprintf(ofps2[i],
	  "lno,version,cluster_name,instance_name,str_session,resource_group_name\n");
	break;
      case LIVEINSTANCES : 
      fprintf(ofps2[i], "lno,cluster_name,instance_name\n");
	break;
      case INSTANCES_HEALTHREPORT : 
      fprintf(ofps2[i],
	  "lno,version,cluster_name,instance_name\n");
	break;
      default : 
	go_BYE(1);
	break;
    }
  }
  

  mcr_alloc_null_str(opfile3, (strlen(op_prefix) + 16));
  strcpy(opfile3, op_prefix); strcat(opfile3, "3.csv");
  ofp3 = fopen(opfile3, "w"); return_if_fopen_failed(ofp3, opfile3, "w");
  fprintf(ofp3, "lno,linetype,field_name,name,value\n");

  mcr_alloc_null_str(opfile4, (strlen(op_prefix) + 16));
  strcpy(opfile4, op_prefix); strcat(opfile4, "4.csv");
  ofp4 = fopen(opfile4, "w"); return_if_fopen_failed(ofp4, opfile4, "w");

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");


  for ( lno = 1; ; lno++ ) { 
    if ( feof(ifp) ) { break; }
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    /* START: Extract common fields  */
    /*---------------------------------------------------------*/
    status = get_val(line, "time:", ' ', "long long", &ll_time, NULL); 
    cBYE(status);
    status = get_val(line, "session:", ' ', "long long", &ll_session, NULL); 
    cBYE(status);
    status = get_val(line, "cxid:", ' ', "long long", &ll_cxid, NULL); 
    cBYE(status);
    status = get_val(line, "zxid:", ' ', "long long", &ll_zxid, NULL); 
    cBYE(status);
    status = get_val(line, "type:", ' ', "char string", NULL, str_type); cBYE(status);
    cBYE(status);
    if ( strcmp(str_type, "createSession") == 0 ) { 
      itype = 1;
    }
    else if ( strcmp(str_type, "create") == 0 ) { 
      itype = 2;
    }
    else if ( strcmp(str_type, "closeSession") == 0 ) { 
      itype = 3;
    }
    else if ( strcmp(str_type, "setData") == 0 ) { 
      itype = 4;
    }
    else if ( strcmp(str_type, "delete") == 0 ) { 
      itype = 5;
    }
    else if ( strcmp(str_type, "error") == 0 ) { 
      itype = 6;
    }
    else {
      fprintf(stderr, "Line %d, type = %s \n", lno, str_type);
      go_BYE(-1);
    }
    switch ( itype ) { 
    case 1 : 
      status = get_val(line, "timeOut:", ' ', "long long", &ll_timeout, NULL); 
      cBYE(status);
      break;
    case 3 : 
      break;
    case 2 : 
    case 4 : 
      if ( itype == 4 ) { 
        status = get_val(line, "version:", ' ', "long long", &ll_version, NULL); 
        cBYE(status);
      }
      else { 
	ll_version = 0;
      }
        status = get_val(line, "data:", ' ', "char string", NULL, data); cBYE(status);
        cBYE(status);
      status = get_val(line, "path:", ' ', "char string", NULL, path); cBYE(status);
      cBYE(status);
      status = consume_path_liveinstances(ofps2[LIVEINSTANCES], lno, path);
      cBYE(status);
      status = consume_path_idealstates(ofps2[IDEALSTATES], lno, ll_version, path);
      cBYE(status);
      if ( itype == 4 ) { 
      status = consume_path_instances_status_updates(
	  ofps2[INSTANCES_STATUS_UPDATES], lno, ll_version, path);
      cBYE(status);
      status = consume_path_instances_messages(
	  ofps2[INSTANCES_MESSAGES], lno, ll_version, path);
      cBYE(status);
      status = consume_path_instances_currentstates(
	  ofps2[INSTANCES_CURRENTSTATES], lno, (int)ll_version, path);
      cBYE(status);
      status = consume_path_instances_healthreport(
	  ofps2[INSTANCES_HEALTHREPORT], lno, (int)ll_version, path);
      cBYE(status);
      }
      /* Old consumption of path 
      fprintf(ofp2, "%d,%lld,\"%s\"\n", lno, ll_version, path);
      */
      if ( strlen(data) != 0 ) {
	status = get_val(data, "\"simpleFields\":{", '}', "char string",
			 NULL, simple_fields); 
	cBYE(status);
	status = consume_simple_fields(simple_fields, lno,
	    LINETYPE_SIMPLE_FIELD, "NULL", ofp3);
	cBYE(status);
	zero_string_to_nullc(map_fields);
        status = get_nested_val(data, "\"mapFields\":", '{', '}', map_fields);
	cBYE(status);
        char *mptr = map_fields;
	for ( ; *mptr != '\0'; ) { 
	    /* Get the name of the map */
	  if ( *mptr == ',' ) { mptr++; }
	  if ( *mptr != '"' ) { go_BYE(-1); } 
	  mptr++;
	    for ( int i = 0; *mptr != '"'; ) { 
	      simple_field_name[i++]  = *mptr++;
	    }
	    mptr++; /* consume the dquote */
	    if ( *mptr != ':' ) { go_BYE(-1); }
	    mptr++; /* Jump over the colon */
	    if ( *mptr != '{' ) { 
	      go_BYE(-1); }
	    mptr++; /* Jump over the open brace */
	    zero_string_to_nullc(simple_fields);
	    for ( int i = 0; ( ( *mptr != '}' ) && ( *mptr != '\0' ) ); 
		  mptr++, i++ ) { 
	      simple_fields[i] = *mptr;
	    }
	    status = consume_simple_fields(simple_fields, lno,
		LINETYPE_MAP_FIELD, simple_field_name, ofp3);
	    cBYE(status);
	    zero_string_to_nullc(simple_fields);
	    zero_string_to_nullc(simple_field_name);
	    if ( *mptr == '}' ) {
	      mptr++; /* consume close brace */
	    }
	  }
	}
      break;
    case 5 : 
      break;
    case 6 : 
      break;
    default : 
      go_BYE(-1);
      break;
    }
    fprintf(ofp1, "%d,%lld,%lld,%lld,%lld,%d\n", lno, ll_time, 
	    ll_session, ll_cxid, ll_zxid, itype);
    zero_string_to_nullc(str_type);
    zero_string_to_nullc(path);
    zero_string_to_nullc(data);
    zero_string_to_nullc(data_id);
    zero_string_to_nullc(simple_fields);
    zero_string_to_nullc(map_fields);
    /* STOP: Extract common fields  */
    if ( ( lno % 100 ) == 0 ) { 
      fprintf(stderr, "Processed LINE %d \n", lno);
    }
  }
  lno--;
  fprintf(stderr, "SUCCESS: Processed %d lines \n", lno);

 BYE:
  for ( int i = 0; i < NUM_ZNODE_TYPES; i++ ) { 
    if ( opfiles2 != NULL ) { 
      free_if_non_null(opfiles2[i]);
      fclose_if_non_null(ofps2[i]);
    }
  }
  free_if_non_null(opfiles2);
  free_if_non_null(ofps2);

  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp3);
  fclose_if_non_null(ofp4);
  free_if_non_null(opfile1);
  free_if_non_null(opfile3);
  free_if_non_null(opfile4);
  return(status);
}


