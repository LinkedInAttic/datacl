#include <alloca.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

extern void
qsort_with_idx (
    int *srt_ordr,
	    void *const pbase, 
	    size_t total_elems, 
	    size_t size, 
	    __compar_fn_t cmp
	    );
