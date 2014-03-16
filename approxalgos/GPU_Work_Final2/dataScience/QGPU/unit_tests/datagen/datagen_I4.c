#include "mmap.h"
#include "macros.h"
#include "new_mk_temp_file.h"
#include <stdlib.h>

int main(int argc, char** argv)
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  int *Y = NULL;
  long long int i, j;

  if(argc != 4)
  {
    printf("Usage: <exec> filename num_elements mode\nmode can be:\n1. incr_mod100\n2. allones\n3. evenones\n4. all10s\n5. rand-0-10\n6. mult2\n7. mult3\n");
    exit(-1);
  }
  char cwd[1024]; getcwd(cwd, 1024);
  long long nelem = atoll(argv[2]);

  status = mk_temp_file( nelem * sizeof(int), cwd, argv[1] );
  cBYE(status);
  status = rs_mmap( argv[1], &X, &nX, true );
  cBYE(status);
  if ( nX == 0 ) { go_BYE(-1); }
  if ( X == NULL ) { go_BYE(-1); }

  int prev = 0;
  int mode = 0;

  if( strcmp( argv[3], "incr_mod100" ) == 0 )
  { mode = 1; }
  else if( strcmp( argv[3], "allones" ) == 0 )
  { mode = 2; }
  else if( strcmp( argv[3], "evenones" ) == 0 )
  { mode = 3; }
  else if( strcmp( argv[3], "all10s" ) == 0 )
  { mode = 4; }
  else if( strcmp( argv[3], "rand-0-10" ) == 0 )
  { mode = 5; }
  else if( strcmp( argv[3], "mult2" ) == 0 )
  { mode = 6; }
  else if( strcmp( argv[3], "mult3" ) == 0 )
  { mode = 7; }
  else
  { go_BYE(-1); }

  Y = (int*)X;
  i = 0; j = 0;

  for(i=0; i<nelem; i++)
  {
    if(mode == 1)
    {
      Y[i] = j++;
      if ( j == 100 ) { j = 0; } 
    }
    else if(mode == 2)
    {
      Y[i] = 1;
    }
    else if(mode == 3)
    {
      if(i % 2 == 0) { Y[i] = 1; }
      else { Y[i] = 0; }
    }
    else if(mode == 4)
    {
      Y[i] = 10;
    }
    else if(mode == 5)
    {
      Y[i] = rand() % 10;
    }
    else if(mode == 6)
    {
      Y[i] = prev + 2;
      prev = Y[i];
    }
    else if(mode == 7)
    {
      Y[i] = prev + 3;
      prev = Y[i];
    }
    else
    { go_BYE(-1); }
  }

BYE:
  return(status);
}
