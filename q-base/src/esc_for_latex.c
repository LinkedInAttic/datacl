#include <stdio.h>
// START FUNC DECL
int
esc_for_latex(
    )
// STOP FUNC DECL
{
  int status = 0;
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
  return status ;
}
