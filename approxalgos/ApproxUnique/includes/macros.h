#ifndef __MACROS_H
#define __MACROS_H
#define WHEREAMI { fprintf(stderr, "Line %3d of File %s \n", __LINE__, __FILE__);  }
/*-------------------------------------------------------*/
#define go_BYE(x) { WHEREAMI; status = x ; goto BYE; }
/*-------------------------------------------------------*/

#define cBYE(x) { if ( (x) < 0 ) { go_BYE((x)) } }
#define fclose_if_non_null(x) { if ( (x) != NULL ) { fclose((x)); (x) = NULL; } } 
#define free_if_non_null(x) { if ( (x) != NULL ) { free((x)); (x) = NULL; } }
#define return_if_fopen_failed(fp, file_name, access_mode) { if ( fp == NULL ) { fprintf(stderr, "Unable to open file %s for %s \n", file_name, access_mode); go_BYE(-1); } }
#define return_if_malloc_failed(x) { if ( x == NULL ) { fprintf(stderr, "Unable to allocate memory\n"); go_BYE(-1); } }

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))
#define sqr(X)  ((X) * (X))

#define MACRO_GETDOCROOT {  \
  if ( ( strlen(argv[1]) == 0 ) || ( strcmp(argv[1], "_" ) == 0 ) ) { \
    docroot = getenv("EU_DOC_ROOT");  \
  } \
  else { \
    docroot = argv[1]; \
  } \
}
#define rs_munmap(X, nX) { \
  if ( ( X == NULL ) && ( nX != 0 ) ) {  WHEREAMI; return(-1); } \
  if ( ( X != NULL ) && ( nX == 0 ) )  { WHEREAMI; return(-1); } \
  if ( X != NULL ) { munmap(X, nX); X = NULL; nX = 0; } \
}

#define unlink_if_non_null(x) { if ( x != NULL ) { unlink( x ); } }

#define mcr_alloc_null_str(x, y) { \
  x = (char *)malloc(y * sizeof(char)); \
  return_if_malloc_failed(x); \
  zero_string(x, y); \
}

#define asm_time(x) { \
  __asm__{  \
    RDTSC  \
      mov DWORD PTR x, eax \
      mov DWORD PTR x+4, eax \
  } \
}

#define chk_range(xval, lb_incl, ub_excl) { if ( ( (xval) < (lb_incl) ) || ( (xval) >= (ub_excl ) ) ) { go_BYE(-1); } }

#define mk_file_name(filename, fileno) {  \
  if ( filename == NULL ) { go_BYE(-1); } \
  if ( fileno < 0  ) { go_BYE(-1); } \
  sprintf(filename, "_%d", fileno); \
}

#ifdef CILK
#define cilkfor _Cilk_for
#else
#define cilkfor for
#endif
#endif
