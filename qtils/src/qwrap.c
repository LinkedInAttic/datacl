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
#include "mmap.h"
#include "q.h"

#define RSLT_BUF_SIZE 4096
// START FUNC DECL
int
main(
     int argc,
     char **argv
     )
// STOP FUNC DECL
{
  int status = 0;
  char rslt_buf[RSLT_BUF_SIZE];

  zero_string(rslt_buf, RSLT_BUF_SIZE);
  status = q(argc, argv, rslt_buf, RSLT_BUF_SIZE);
  cBYE(status);
  if ( *rslt_buf != '\0' ) {
    fprintf(stdout, "%s", rslt_buf);
  }
 BYE:
  return(status);
}

