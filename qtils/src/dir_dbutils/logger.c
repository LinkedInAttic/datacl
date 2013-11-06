/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#include "qtypes.h"
// START FUNC DECL
int lock_log_file(
    )
// STOP FUNC DECL
{
  int status = 0;
  return(status);
}
// START FUNC DECL
int unlock_log_file(
    )
// STOP FUNC DECL
{
  int status = 0;
  return(status);
}

void log_request(
    int argc,
    char **argv,
    unsigned long long *ptr_t_before
    )

{
  int status = 0;

  FILE *lfp = NULL;
  struct timeval Tps;
  struct timezone Tpf;
  unsigned long long t_before_sec = 0, t_before_usec = 0;

  char *logfile = getenv("Q_LOGFILE");
  if ( logfile != NULL ) {
    gettimeofday(&Tps, &Tpf); 
    t_before_sec  = (unsigned long long)Tps.tv_sec;
    t_before_usec = (unsigned long long)Tps.tv_usec;
    *ptr_t_before = t_before_sec * 1000000 + t_before_usec;

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

void
log_response(
    int status,
    unsigned long long t_before
    )
{
  FILE *lfp = NULL;
  struct timeval Tps;
  struct timezone Tpf;
  long long t_after_sec = 0, t_after_usec = 0, t_after = 0, t;
  
  char *logfile = getenv("Q_LOGFILE");
  if ( logfile != NULL ) {
    gettimeofday (&Tps, &Tpf); 
    t_after_sec  = (long long)Tps.tv_sec;
    t_after_usec = (long long)Tps.tv_usec;
    t_after = t_after_sec * 1000000 + t_after_usec;
    t = t_after - t_before;

    lfp = fopen(logfile, "a");
    if ( lfp != NULL ) { 
      fprintf(lfp, ",%d,%lld,%lld\n", status, t, t_after);
    }
    fclose_if_non_null(lfp);
  }
}
