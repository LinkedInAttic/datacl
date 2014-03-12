#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char line[64];
  char *infile = NULL, *outfile = NULL;
  FILE *ifp = NULL, *ffp = NULL, *tfp = NULL;
  int from, to;
  char fromfile[1024];
  char tofile[1024];

  zero_string(line, 64);
  if ( argc != 3 ) { go_BYE(-1); }
  infile = argv[1];
  strcpy(fromfile, argv[2]); strcat(fromfile, "_from.bin"); 
  strcpy(tofile,   argv[2]); strcat(tofile,   "_to.bin");     
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");

  ffp = fopen(fromfile, "wb");
  return_if_fopen_failed(ffp, fromfile, "wb");
  tfp = fopen(tofile, "wb");
  return_if_fopen_failed(tfp, tofile, "wb");
  
  for ( long long i = 0; !feof(ifp); i++ ) {
    char *cptr = fgets(line, 64, ifp);
    if ( cptr == NULL ) { break; }
    if ( *cptr == '\0' ) { break; }
    sscanf(line,"%d,%d\n", &from, &to);
    fwrite(&from, sizeof(int), 1, ffp);
    fwrite(&to,   sizeof(int), 1, tfp);
    // reverse the egde since we exported it only one way
    fwrite(&to,   sizeof(int), 1, ffp);
    fwrite(&from, sizeof(int), 1, tfp);
    zero_string_to_nullc(line);
    if ( ( i % 1000000 ) == 0 ) { 
      fprintf(stderr, "Wrote %lld M lines \n", ( i / 1000000 ) );
    }
  }

BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(ffp);
  fclose_if_non_null(tfp);
  return(status);
}
