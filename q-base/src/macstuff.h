#ifndef __COMPAR_FN_T
# define __COMPAR_FN_T
typedef int (*__compar_fn_t) (__const void *, __const void *);

# ifdef __USE_GNU
typedef __compar_fn_t comparison_fn_t;
# endif
#endif

/*
#ifndef __MAC_FLT_VALUES
#define __MAC_FLT_VALUES
#define DBL_MAX		1.79769313486231470e+308
#define DBL_MIN		4.94065645841246544e-324
#define FLT_MIN 1.175494351E-38F
#define FLT_MAX 3.402823466E+38F
#endif
*/
