#include <alloca.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

extern void
_quicksort (
	    void *const pbase, 
	    size_t total_elems, 
	    size_t size, 
	    __compar_fn_t cmp
	    );
