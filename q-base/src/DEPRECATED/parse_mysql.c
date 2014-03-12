#include <ctype.h> 
#include <stdio.h> 
#include "constants.h"
#include "macros.h"
#include "fsize.h"

#define MAXLINE 65536

char g_buffer[MAXLINE];

int
get_val_1(
	char *line,
	char *needle,
	long long *ptr_llval
	)
{
  int status = 0;
  char *cptr, *xptr, *endptr;

  cptr = strstr(line, needle);
  if ( cptr == NULL ) { go_BYE(-1); }
  xptr = cptr + strlen(needle);
  for ( int i = 0; ( !isspace(*xptr) && ( *xptr != '\0') ); xptr++, i++ ) { 
    if ( isdigit(*xptr) ) { 
      g_buffer[i] = *xptr;
    }
  }
  if ( !isspace(*xptr) )  { go_BYE(-1); }
  *ptr_llval = strtoll(g_buffer, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  zero_string_to_nullc(g_buffer);
BYE:
  return(status);
}

#define MAX_NUM_FLDS 32
int
main(
     int argc,
     char **argv 
     )
{
  int status = 0;
  char *infile = NULL, *opfile = NULL, *str_tbl = NULL, *str_flds = NULL;
  char line[MAXLINE];
  char data[MAXLINE];
  char end_marker[MAXLINE];

  char *cptr,  *endptr;
  FILE *ifp = NULL; /* inout file */
  FILE *ofp = NULL; /* common fields */
  int lno;
  bool record_started = false;
  long long start_lno, server_id, end_log_pos, group_id, table_id;
  int desired_tbl_id;
  int desired_fld_ids[MAX_NUM_FLDS]; 
  long long fld_vals[MAX_NUM_FLDS]; 
  int num_flds = 0, fld_marker = 0;
  char **rec_markers = NULL;


  zero_string(line,     MAXLINE);
  zero_string(data,     MAXLINE);
  zero_string(g_buffer,   MAXLINE);
  zero_string(end_marker,   MAXLINE);

  if ( argc != 5 ) { go_BYE(-1); }
  infile   = argv[1];
  str_tbl  = argv[2];
  str_flds = argv[3];
  opfile   = argv[4];

  desired_tbl_id = strtoll(str_tbl, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }

  cptr = strtok(str_flds, ":");
  if ( cptr == NULL ) { go_BYE(-1); }
  desired_fld_ids[num_flds++] = strtoll(cptr, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  for ( ; ; ) { 
    cptr = strtok(NULL, ":");
    if ( cptr == NULL ) { break; }
    desired_fld_ids[num_flds++] = strtoll(cptr, &endptr, 10);
  }
  /* trivial sort */
  for ( int i = 0; i < num_flds; i++ ) { 
    for ( int j = i+1; j < num_flds; j++ ) { 
      int iswap;
      if ( desired_fld_ids[i] > desired_fld_ids[j] ) {
	iswap = desired_fld_ids[i];
	desired_fld_ids[i] = desired_fld_ids[j];
	desired_fld_ids[j] = iswap;
      }
    }
  }
  /* basic checks */
  for ( int i = 0; i < num_flds; i++ ) { 
    if ( desired_fld_ids[i] <= 0 ) { go_BYE(-1); }
  }
  for ( int i = 1; i < num_flds; i++ ) { 
    if ( desired_fld_ids[i] == desired_fld_ids[i-1] ) { go_BYE(-1); }
  }
  /*--------------------------------------------------------*/
  /* Make rec_markers */
  rec_markers = malloc(num_flds * sizeof(char **));
  return_if_malloc_failed(rec_markers);
  for ( int i = 0; i < num_flds; i++ ) { 
    rec_markers[i] = malloc(MAXLINE * sizeof(char *));
    return_if_malloc_failed(rec_markers[i]);
    zero_string(rec_markers[i], MAXLINE);
    sprintf(rec_markers[i], "###   @%d=", desired_fld_ids[i]);
  }
  /*--------------------------------------------------------*/

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  ofp = fopen(opfile, "w");
  return_if_fopen_failed(ofp, opfile, "w");


  for ( lno = 1; ; lno++ ) { 
    if ( feof(ifp) ) { break; }
    zero_string_to_nullc(line);
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    if ( !record_started ) { /* Look for start of record  */
      cptr = strstr(line, "Update_rows: table id ");
      if ( cptr == NULL ) { continue; }
      fprintf(stderr, "Record started -- %5d \n", lno);
      start_lno = lno;
      /* Extract field from line that looks like 
	 #120130 15:29:23 server id 5973  end_log_pos 1128455  group_id
	 17179869186      Update_rows: table id 3149 flags: STMT_END_F
      */
      status = get_val_1(line, " server id ", &server_id);
      cBYE(status);
      status = get_val_1(line, " end_log_pos ", &end_log_pos);
      cBYE(status);
      status = get_val_1(line, " group_id ", &group_id);
      cBYE(status);
      status = get_val_1(line, " table id ", &table_id);
      cBYE(status);
      sprintf(end_marker, "# at %lld", end_log_pos);
      fprintf(stderr, "%s\n", end_marker);
      record_started = true;
      fld_marker = 0;
    }
    else {
      if ( fld_marker < num_flds ) {
        cptr = strstr(line, rec_markers[fld_marker]);
        if ( cptr != NULL ) { 
          status = get_val_1(line, rec_markers[fld_marker],
	      &(fld_vals[fld_marker]));
          cBYE(status);
	  fld_marker++;

        }
      }
      /*------------------*/
      cptr = strstr(line, end_marker);
      if ( cptr == NULL ) { continue; }
      if ( ( desired_tbl_id < 0 ) || ( desired_tbl_id == table_id ) ) {
        fprintf(ofp,"%lld,%lld,%lld,%lld,%lld", start_lno, server_id, 
	    end_log_pos, group_id, table_id);
	for ( int i = 0; i < num_flds; i++ ) {
          fprintf(ofp,",%lld", fld_vals[i]);
	}
        fprintf(ofp,"\n");
      }
      fprintf(stderr, "Record ended -- %5d \n", lno);
      record_started = false;
      for ( int i = 0; i < num_flds; i++ ) { 
	fld_vals[i] = -1;
      }
    }
    zero_string_to_nullc(line);
  }
  if ( record_started ) { go_BYE(-1); }
 BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  return(status);
}
