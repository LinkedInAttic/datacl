#include <stdio.h>
//#include "qtypes.h"
#include "macros.h"
#include "mmap.h"
//#include "auxil.h"
//#include "mk_file.h"
//#include "open_file_in_dir.h"
#include "new_mk_temp_file.h"
//#include "meta_globals.h"

#define MAX_LEN_DIR_NAME 250

#include <sys/statvfs.h>
// START FUNC DECL
int
get_disk_space (
    const char * dev_path,
    unsigned long long *ptr_nbytes,
    const char *mode
    )
// STOP FUNC DECL
{
  int status = 0;
  struct statvfs sfs;
  *ptr_nbytes = -1;
  if ( dev_path == NULL ) { go_BYE(-1); }
  if ( mode == NULL ) { go_BYE(-1); }

  status = statvfs ( dev_path, &sfs); cBYE(status);
  if ( strcmp(mode, "capacity") == 0 ) {
    *ptr_nbytes = (unsigned long long)sfs.f_bsize * sfs.f_blocks;
  }
  else if ( strcmp(mode, "free_space") == 0 ) {
    *ptr_nbytes = (unsigned long long)sfs.f_bsize * sfs.f_bfree;
  }
  else { go_BYE(-1); }
BYE:
  return(status);
}

// START FUNC DECL
int
avail_space(
            const char *dir,
            size_t *ptr_avail_space
            )
// STOP FUNC DECL
{
  int status = 0;
  struct statvfs sbuf;

  status = statvfs(dir , &sbuf); cBYE(status);
  *ptr_avail_space = sbuf.f_bsize * sbuf.f_bavail;
 BYE:
  return(status);
}

// START FUNC DECL
int
open_file_in_dir(
                 const char *cwd,
                 const char *dir,
                 const char *fname,
                 size_t filesize
                 )
// STOP FUNC DECL
{
  int status = 0;
  FILE *fp = NULL;

  if ( cwd == NULL ) { go_BYE(-1); }
  if ( dir == NULL ) { go_BYE(-1); }
  if ( fname == NULL ) { go_BYE(-1); }

  if ( filesize > 0 ) {
    /* Check that there is space to write */
    size_t space_available;
    status = avail_space(dir, &space_available); cBYE(status);
    if ( space_available <= filesize  ) {
      fprintf(stderr, "Space (%lld bytes) in directory [%s] not available \n",
              (long long)filesize, dir);
      go_BYE(-1);
    }
  }
  /* Check that directory is accessible */
  status = chdir(dir);
  if ( status != 0 ) {
    fprintf(stderr, "Directory [%s] not accessible \n", dir);
    go_BYE(-1);
  }
  // Create temp file 
  fp = fopen(fname, "wb");
  return_if_fopen_failed(fp,  fname, "wb");
  fclose_if_non_null(fp);
  // Get back to where you once belonged
  status = chdir(cwd); cBYE(status);
 BYE:
  return(status);
}


int
mk_file(
    const char *cwd,
    const char *dir,
    const char *filename,
    size_t filesize
    )
// STOP FUNC DECL
{
  int status= 0;
  int result, fd;

  if ( cwd == NULL ) { go_BYE(-1); }
  if ( dir == NULL ) { go_BYE(-1); }
  if ( filename == NULL ) { go_BYE(-1); }
  if ( filesize <= 0 ) { go_BYE(-1); }

  /* Check that directory is accessible */
  status = chdir(dir);
  if ( status != 0 ) {
    fprintf(stderr, "Directory [%s] not accessible \n", dir);
    go_BYE(-1);
  }
  /* Open a file for writing.  - Creating the file if it doesn't
   *  exist.  - Truncating it to 0 size if it already exists. (not
   *  really needed)
   *
   * Note: "O_WRONLY" mode is not sufficient when mmaping.
   */
  fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
  if (fd == -1) {
    fprintf(stderr, "Error opening %s file for writing\n", filename);
    go_BYE(-1);
  }
  /* Stretch the file size to the size of the (mmapped) array of ints */
  result = lseek(fd, filesize - 1, SEEK_SET);
  if (result == -1) {
    close(fd);
    fprintf(stderr, "Error calling lseek() to 'stretch' file [%s]\n", filename);
    go_BYE(-1);
  }

  /* Something needs to be written at the end of the file to have the
   * file actually have the new size.  Just writing an empty string at
   * the current file position will do.
   *
   * Note: - The current position in the file is at the end of the
   * stretched file due to the call to lseek().  - An empty string is
   * actually a single '\0' character, so a zero-byte will be written
   * at the last byte of the file.
   */
  result = write(fd, "", 1);
  close(fd);
  if (result != 1) {
    fprintf(stderr, "Error writing last byte of file \n");
    go_BYE(-1);
  }
  status = chdir(cwd); cBYE(status);
BYE:
  return(status);
}


// START FUNC DECL
int
mk_temp_file(
    size_t filesz,
    const char *dir,
    const char *filename
    )
// STOP FUNC DECL
{
  int status = 0;
  char cwd[MAX_LEN_DIR_NAME+1];
	unsigned long long free_space = 0;

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
//  *ptr_fileno = g_max_fileno;
BYE:
  return(status);
}
