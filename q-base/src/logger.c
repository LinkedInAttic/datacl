#include "qtypes.h"
#include "auxil.h"
// START FUNC DECL
int lock_log_file(
    )
// STOP FUNC DECL
{
  int status = 0;
  // TODO: P3
  return status ;
}
// START FUNC DECL
int unlock_log_file(
    )
// STOP FUNC DECL
{
  int status = 0;
  // TODO: P3
  return status ;
}

// START FUNC DECL
void log_request(
    int argc,
    char **argv,
    unsigned long long *ptr_t_before
    )
// STOP FUNC DECL
{
  int status = 0;

  FILE *lfp = NULL;

  char *logfile = getenv("Q_LOGFILE");
  if ( logfile != NULL ) {
    *ptr_t_before = get_time_usec();

    status = lock_log_file();
    lfp = fopen(logfile, "a");
    if ( lfp != NULL ) { 
      fprintf(lfp, "%llu,%d,", *ptr_t_before, argc); 
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

// START FUNC DECL
void
log_response(
    int status,
    unsigned long long t_before
    )
// STOP FUNC DECL
{
  FILE *lfp = NULL;
  long long t, t_after;
  
  char *logfile = getenv("Q_LOGFILE");
  if ( logfile != NULL ) {
    t_after = get_time_usec();
    t = t_after - t_before;

    lfp = fopen(logfile, "a");
    if ( lfp != NULL ) { 
      fprintf(lfp, ",%d,%lld,%lld\n", status, t, t_after);
    }
    fclose_if_non_null(lfp);
  }
}
