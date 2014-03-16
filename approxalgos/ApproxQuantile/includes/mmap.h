#include <stdbool.h>
extern int
rs_mmap(
		      const char *file_name,
		      char **ptr_mmaped_file,
		      size_t *ptr_file_size,
		      bool is_write
		      )
;
//----------------------------
extern void
zero_string(
    char *X,
    const int nX
    )
;
//----------------------------
extern void
zero_string_to_nullc(
    char *X
    )
;
//----------------------------
