#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"

#define reset_pattern_search() {		\
    mat_idx = -1;				\
    pat_idx = 0;				\
    target = fp[pat_idx];			\
    min_num_wanted = lb[pat_idx];		\
    max_num_wanted = ub[pat_idx];		\
    pat_len = 0; \
    num_seen = 0;				\
    is_in_pat = false; \
    if ( restart > 0 ) { i = restart; } \
    restart = -1; \
  }

#define advance_pattern_search() {		\
    pat_idx++;					\
    target = fp[pat_idx];			\
    min_num_wanted = lb[pat_idx];		\
    max_num_wanted = ub[pat_idx];		\
    num_seen = 0;				\
  }

// START FUNC DECL
int
inner_patmatch_I2(
		  short *f1, /* Destination */
		  char *fb, /* Break field */
		  char *fout, 
		  long long nR1,
		  short *fp, /* TYPE */ /* Source */
		  int *lb,
		  int *ub,
		  long long nR2
		  )
// STOP FUNC DECL
{
  int status = 0;

  if ( nR2 <= 1 ) { go_BYE(-1); }

  long long mat_idx = -1;
  int pat_idx = 0; 
  short target = fp[pat_idx];  
  int min_num_wanted = lb[pat_idx];
  int max_num_wanted = ub[pat_idx];
  int num_seen = 0;
  int pat_len = 0;
  short first_of_pattern; 
  long long restart = -1;
  int min_pattern_length = 0;
  bool isx = false; /* whether first element of pattern occurs later */
  bool is_in_pat = false; /* whether a potential pattern match is under way */
  
  first_of_pattern = fp[0];
  // Quick checks on pattern. Subsequent elements should be different
  for ( int i = 1; i < nR2; i++ ) { 
    if ( fp[i] == fp[i-1] ) { go_BYE(-1); }
    if ( fp[i] == fp[0] ) { isx = true; }
  }
  for ( int i = 0; i < nR2; i++ ) { 
    min_pattern_length += lb[i];
  }

  for ( long long i = 0; i < nR1; ) { 
    int item = f1[i]; /* item to be matched against */
    /* If you have started a pattern match and you come across a break,
     * then reset */
    if ( ( isx == true ) && ( is_in_pat == true ) && 
      ( item == first_of_pattern ) ) {
      if ( restart < 0 ) { 
	/* This check makes sure that we remember earliest location for
	 * restart */
        restart = i;
      }
    }
    if ( fb != NULL ) {
      if ( ( num_seen > 0 ) && ( fb[i] == 1 ) ) {
        if ( ( pat_idx == (nR2-1) ) && ( num_seen >= min_num_wanted ) ) { 
	  fout[mat_idx] = 1;
	}
	reset_pattern_search();
	continue;
      }
    }
    /*-------------------------------------------------------------------*/
    if ( ( pat_idx == 0 ) || ( pat_idx == (nR2-1) ) ) {
      if ( item  == target ) {
	if ( ( num_seen == 0 ) && ( pat_idx == 0 ) ) {
	  mat_idx  = i; /* here is where pattern match starts */
	  is_in_pat = true;
	}
	num_seen++;
	pat_len++;
	i++; 
      }
      else {
	if ( num_seen >= min_num_wanted ) {
	  if ( pat_idx == (nR2-1) ) { /* matched to end of pattern */
	    fout[mat_idx] = 1;
	    reset_pattern_search();
	  }
	  else { /* more to be matched */
	    advance_pattern_search();
	  }
	}
	else {
	  i++;
	  reset_pattern_search(); 
	}
      }
    }
    else {
      if ( item  == target ) {
	if ( num_seen <= max_num_wanted ) {
	  num_seen++;
	  pat_len++;
	  i++;
	}
	else {
	  reset_pattern_search(); 
	}
      }
      else {
	if ( num_seen == max_num_wanted ) { /* successful */
          advance_pattern_search();
	}
	else {
	  reset_pattern_search();
	}
      }
    }
  }
  /* Check to make sure that pattern has not just ended */
  if ( ( pat_idx == (nR2-1) ) && ( num_seen >= min_num_wanted ) ) {
    fout[mat_idx] = 1;
  }
BYE:
  return(status);
}
