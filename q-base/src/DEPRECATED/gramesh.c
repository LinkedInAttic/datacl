#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "mk_file.h"

#define NEXT 1
#define PREV 2
int
gramesh(
    char *str_pgk_grp_id, /* ID of the page key group of interest */
    char *next_or_prev, /* are we interested in next page or prev page */
    char *str_nR, /* size of main table */
    char *str_num_sessions, /* the number of sessions */
    char *str_num_pgk_grp, /* number of unique page key groups */
    char *q_data_dir,
    char *lb_file, /* index into main table: lower bound of session */
    char *ub_file, /* index into main table: upper bound of session */
    char *pgk_grp_id_file,
    char *opfile_imm
    )
{
  int status = 0;
  char *endptr = NULL;
  char cwd[MAX_LEN_DIR_NAME+1];
  char *imm_X = NULL; size_t imm_nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0;
  char *ub_X = NULL; size_t ub_nX = 0;
  char *pgk_grp_id_X = NULL; size_t pgk_grp_id_nX = 0;

  zero_string(cwd, (MAX_LEN_DIR_NAME+1));
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  //----------------------------------------------------------
  int inext_or_prev;
  if ( strcasecmp(next_or_prev, "NEXT") == 0 ) {
    inext_or_prev = NEXT;
  }
  else if ( strcasecmp(next_or_prev, "PREV") == 0 ) {
    inext_or_prev = PREV;
  }
  else {
    go_BYE(-1);
  }
  //----------------------------------------------------------
  int desired_pgk_grp_id = strtoll(str_pgk_grp_id, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( desired_pgk_grp_id <= 0 ) { go_BYE(-1); }
  //--------------------------------------------
  long long nR = strtoll(str_nR, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( nR <= 0 ) { go_BYE(-1); }
  //--------------------------------------------
  long long num_pgk_grp = strtoll(str_num_pgk_grp, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( num_pgk_grp <= 0 ) { go_BYE(-1); }
  //--------------------------------------------
  int num_sessions = strtoll(str_num_sessions, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( num_sessions <= 0 ) { go_BYE(-1); }
  //--------------------------------------------
  // allocate space 
  unlink(opfile_imm);
  status = mk_file(cwd, q_data_dir, opfile_imm, (num_pgk_grp * sizeof(int)));
  cBYE(status);

  // Initialize immediate count to 0
  chdir(q_data_dir);
  status = rs_mmap(opfile_imm, &imm_X, &imm_nX, 1); cBYE(status);
  int *immI4 = (int *)imm_X;
  for ( int i = 0; i < num_pgk_grp; i++ ) { 
    immI4[i] = 0;
  }
  // get access to data

  status = rs_mmap(lb_file, &lb_X, &lb_nX, 0); cBYE(status);
  status = rs_mmap(ub_file, &ub_X, &ub_nX, 0); cBYE(status);
  if ( ( lb_nX / sizeof(int) ) != num_sessions ) { go_BYE(-1); }
  if ( lb_nX != ub_nX ) { go_BYE(-1); }

  status = rs_mmap(pgk_grp_id_file, &pgk_grp_id_X, &pgk_grp_id_nX, 0); cBYE(status);
  if ( ( pgk_grp_id_nX / sizeof(int) ) != nR ) { go_BYE(-1); }
  chdir(cwd);


  for ( long long i = 0; i < num_sessions; i++ ) { 
    int *f_lbI4 = (int *)lb_X;
    int *f_ubI4 = (int *)ub_X;
    int *f_pgk_grp_id = (int *)pgk_grp_id_X;
    int lbI4 = f_lbI4[i];
    int ubI4 = f_ubI4[i];
#ifdef DEBUG
    if ( lbI4 >= ubI4 ) { go_BYE(-1); }
    if ( lbI4 < 0 ) { go_BYE(-1); }
    if ( ubI4 < 0 ) { go_BYE(-1); }
    if ( lbI4 >= nR ) { go_BYE(-1); }
    if ( ubI4 > nR ) { go_BYE(-1); }
#endif
    // process this session
    int num_incr_this_sess = 0; // just for debugging
    switch ( inext_or_prev ) { 
      case NEXT : 
    for ( long long j = lbI4; j < ubI4; j++ ) {
      int pgk_grp_id = f_pgk_grp_id[j];
      if ( pgk_grp_id != desired_pgk_grp_id ) { continue; }
      int nextj = j + 1 ;
      if ( nextj != ubI4 ) { /* If not the last */
        int next_pgk_grp_id = f_pgk_grp_id[nextj];
#ifdef DEBUG
	if ( ( next_pgk_grp_id < 0 ) || ( next_pgk_grp_id > num_pgk_grp ) ) {
	  go_BYE(-1);
	}
#endif
	immI4[next_pgk_grp_id]++;
        num_incr_this_sess++; // just for debugging
      }
    }
    break;
      case PREV : 
    for ( long long j = ubI4-1; j >= lbI4; j-- ) {
      int pgk_grp_id = f_pgk_grp_id[j];
      if ( pgk_grp_id != desired_pgk_grp_id ) { continue; }
      int prevj = j - 1 ;
      if ( prevj >= lbI4 ) { /* If not the first */
        int prev_pgk_grp_id = f_pgk_grp_id[prevj];
#ifdef DEBUG
	if ( ( prev_pgk_grp_id < 0 ) || ( prev_pgk_grp_id > num_pgk_grp ) ) {
	  go_BYE(-1);
	}
#endif
	immI4[prev_pgk_grp_id]++;
        num_incr_this_sess++; // just for debugging
      }
    }
    break; 
      default : 
    go_BYE(-1);
    break;
    }
    /* This is possible because only occurrence of desired pgk_grp may
     * be the last click 
    if ( num_incr_this_sess == 0 ) {
      go_BYE(-1);
    }
    */
  }
#ifdef DEBUG
  long long chk_nR = 0;
  for ( int i = 0; i < num_pgk_grp; i++ ) { 
    chk_nR += immI4[i];
  }
  if ( chk_nR > nR ) { go_BYE(-1); }
#endif
BYE:
  chdir(cwd);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  rs_munmap(imm_X, imm_nX);
  rs_munmap(pgk_grp_id_X, pgk_grp_id_nX);
  return(status);
}
