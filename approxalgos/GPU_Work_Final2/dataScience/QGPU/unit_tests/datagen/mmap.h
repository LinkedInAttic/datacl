#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C"
#endif
int
rs_mmap(
		      const char *file_name,
		      char **ptr_mmaped_file,
		      size_t *ptr_file_size,
		      bool is_write
		      )
;
/*
extern void
zero_string(
    char *X,
    const int nX
    )
;
extern void
zero_string_to_nullc(
    char *X
    )
;
*/
