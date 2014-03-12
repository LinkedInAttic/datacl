#include <stdio.h>
int
main()
{
  unsigned int uitemp;
  for ( ; ; ) { 
    uitemp = fgetc(stdin);
    if ( uitemp == EOF ) { break; } 
    if ( ( uitemp == '_' ) || ( uitemp == '#' ) || ( uitemp == '$' ) || 
         ( uitemp == '%' ) || ( uitemp == '&' ) || ( uitemp == '~' ) || 
         ( uitemp == '^' ) || ( uitemp == '{' ) || ( uitemp == '}' ) ) {
      fputc('\\', stdout);
    }
    fputc(uitemp, stdout);
  }
}

