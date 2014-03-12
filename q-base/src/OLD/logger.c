#include "qtypes.h"
// START FUNC DECL
int lock_log_file(
    )
// STOP FUNC DECL
{
  int status = 0;
BYE:
  return(status);
}
// START FUNC DECL
int unlock_log_file(
    )
// STOP FUNC DECL
{
  int status = 0;
BYE:
  return(status);
}

void log_request(
    int argc,
    char **argv,
    long long *ptr_log_file_id
    )

{
  int status = 0;

  FILE *lfp = NULL;
  struct timeval Tps;
  struct timezone Tpf;
  long long t_before_sec = 0, t_before_usec = 0, t_before = 0;

  char *logfile = getenv("Q_LOGFILE");
  if ( logfile != NULL ) {
    gettimeofday(&Tps, &Tpf); 
    t_before_sec  = (long long)Tps.tv_sec;
    t_before_usec = (long long)Tps.tv_usec;
    t_before = t_before_sec * 1000000 + t_before_usec;
    *ptr_log_file_id = (t_before << 16) + getpid();

    status = lock_log_file();
    lfp = fopen(logfile, "a");
    if ( lfp != NULL ) { 
      fprintf(lfp, "%lld,%d,%lld,", *ptr_log_file_id, argc, t_before_sec);
      for ( int j = 0; j < argc; j++ ) { 
	if ( j > 0 ) { 
	  fprintf(lfp, ",");
	}
	fprintf(lfp, "\"");
	for ( char *cptr = argv[j]; *cptr != '\0'; cptr++ ) { 
	  if ( ( *cptr == '"' ) || ( *cptr == '\\' ) )  {
	    fprintf(lfp, "\\");
	  }
	  fprintf(lfp, "%c", *cptr);
	}
	fprintf(lfp, "\"");
      }
      for ( int i = argc; i < MAX_NUM_ARGS; i++ ) {
	fprintf(lfp, ",\"\"");
      }
    }
    fclose_if_non_null(lfp);
    status = unlock_log_file();
  }
}

void
log_response(
    int status,
    long long log_file_id
    )
{
  FILE *lfp = NULL;
  struct timeval Tps;
  struct timezone Tpf;
  long long t_after_sec = 0, t_after_usec = 0, t_after = 0;
  
  char *logfile = getenv("Q_LOGFILE");
  if ( logfile != NULL ) {
    gettimeofday (&Tps, &Tpf); 
    t_after_sec  = (long long)Tps.tv_sec;
    t_after_usec = (long long)Tps.tv_usec;
    t_after = t_after_sec * 1000000 + t_after_usec;
    status = lock_log_file();
    lfp = fopen(logfile, "a");
    if ( lfp != NULL ) { 
      fprintf(lfp, "%lld,%d,%lld\n", log_file_id, status, t_after);
    }
    fclose_if_non_null(lfp);
    status = unlock_log_file();
  }
}
