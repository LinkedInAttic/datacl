#include <stdio.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "mk_file.h"
#include "open_file_in_dir.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

// START FUNC DECL
int
mk_temp_file(
    size_t filesz,
    char *dir,
    char *filename
    )
// STOP FUNC DECL
{
  int status = 0;
  extern char cwd[MAX_LEN_DIR_NAME+1];

  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); } 
  if ( filesz < 0 ) { go_BYE(-1); }
  if ( ( dir == NULL ) || ( *dir == '\0' ) )   { go_BYE(-1); }
  if ( ( filename == NULL ) || ( *filename == '\0' ) )  { go_BYE(-1); }
  status = get_disk_space(dir, &free_space, "free_space"); cBYE(status);
  if ( free_space < filesz ) { go_BYE(-1); }

  // Make empty file with that name 
  status = open_file_in_dir(cwd, dir, filename, filesz); cBYE(status);
  // Stretch it to appropriate size
  status = mk_file(cwd, dir, filename, filesz); cBYE(status);
BYE:
  return(status);
}
