#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "sort2_asc_int.h"
#include "open_temp_file.h"

bool g_write_to_temp_dir = false;

typedef struct f1type {
  int pid;
  int mid;
} F1TYPE;

typedef struct f2type {
  int mid;
  int tid;
} F2TYPE;

typedef struct f3type {
  int key;
  int val;
} F3TYPE;

typedef struct f4type {
  int tid;
  int cnt;
} F4TYPE;

static int
bin_search(
	   F3TYPE *X,
	   int nX,
	   int key,
	   int *ptr_lb,
	   int *ptr_ub
	   )
{
  int status = 0;
  int pos, lb, ub, midpoint;

  if ( X == NULL ) go_BYE(-1);
  *ptr_lb = *ptr_ub = -1;
  if ( nX == 1 ) { 
    if ( X[0].key == key ) { 
      *ptr_lb = 0; 
      *ptr_ub = 1;
    }
    else {
      *ptr_lb = -1;
      *ptr_ub = -1;
    }
    return(status); 
  }

  lb = 0; ub = nX - 1;
  pos = -1;
  for ( ; lb <= ub ; ) { 
    midpoint = ( lb + ub )  / 2 ;
    if ( X[midpoint].key == key ) { 
      pos = midpoint;
      break;
    }
    else if ( key < X[midpoint].key ) { /* reduce ub */
      ub = min(ub - 1, midpoint);
    }
    else /* key > X[midpoint] */ { /* increase lb */
      lb = max(lb + 1, midpoint);
    }
  }
  // Now stretch on both sides to find range of values with this key
  if ( pos >= 0 ) {
    for ( lb = pos ; lb >= 0 ; lb-- ) {
      if ( X[lb].key != key ) { 
	lb++;
	break;
      }
    }
    if ( lb < 0 ) { lb++; }
    for ( ub = pos ; ub < nX ; ub++ ) {
      if ( X[ub].key != key ) { 
	break;
      }
    }
    if ( ub > nX ) { ub = nX; }
  }
  else {
    lb = ub = -1;
  }
  if ( ( ( lb < 0 ) && ( ub >= 0 ) ) || ( ( lb >= 0 ) && ( ub < 0 ) ) ) {
    go_BYE(-1);
  }
  *ptr_lb = lb;
  *ptr_ub = ub;
 BYE:
  return(status);
}

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *t1file = NULL, *t2file = NULL, *opfile = NULL, *opfile2 = NULL;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  F1TYPE *T1 = NULL;
  F2TYPE *T2 = NULL;
  int pid, tid, xpid;
  FILE *ofp = NULL, *ofp2 = NULL;
  int nR1, nR2, lb, ub, tlb, tub;
  F4TYPE *tids = NULL; int sz_tids = 8192; int n_tids = 0;
  double prob; int sum_cnt; // for prob calcs
  int num_useful_projects = 0;

  tids = (F4TYPE *)malloc(sz_tids * sizeof(F4TYPE));
  return_if_malloc_failed(tids);
  if ( argc != 5 ) {
    fprintf(stderr, "Usage is %s <tbl1> <tbl2> <optbl> <optbl2> \n", argv[0]); 
    go_BYE(-1); 
  }
  t1file  = argv[1];
  t2file  = argv[2];
  opfile  = argv[3];
  opfile2 = argv[4];
  if ( strcmp(t1file, t2file) == 0 ) { go_BYE(-1); }
  if ( strcmp(t2file, opfile) == 0 ) { go_BYE(-1); }

  ofp = fopen(opfile, "wb");
  return_if_fopen_failed(ofp,  opfile, "wb");
  ofp2 = fopen(opfile2, "wb");
  return_if_fopen_failed(ofp2,  opfile2, "wb");
  // mmap first file 
  status = rs_mmap(t1file, &X1, &nX1, 0);
  cBYE(status);
  nR1 = nX1 / sizeof(F1TYPE);
  if ( nR1 * sizeof(F1TYPE) != nX1 ) { go_BYE(-1); }
  if ( nR1 < 1 ) { go_BYE(-1); }
  T1 = (F1TYPE *)X1;

  // mmap first file 
  status = rs_mmap(t2file, &X2, &nX2, 0);
  cBYE(status);
  nR2 = nX2 / sizeof(F2TYPE);
  if ( nR2 * sizeof(F2TYPE) != nX2 ) { go_BYE(-1); }
  if ( nR2 < 1 ) { go_BYE(-1); }
  T2 = (F2TYPE *)X2;

  int prj_ctr = 0;
  for ( int i = 0; i < nR1; ) {
    int j; int effective_n_tids; 
    int members_in_project; float titles_per_member;
    pid  = T1[i].pid;
    for ( j = i + 1; j < nR1; j++ ) { 
      xpid   = T1[j].pid;
      if ( xpid != pid ) { break; }
    }
    prj_ctr++;
    members_in_project = (j - i);
    /* This means that rows with lb = i and ub = j all belong to the
     * same project */
    for ( int k = i; k < j; k++ ) {
      int mid;
      mid  = T1[k].mid;
      status = bin_search((F3TYPE *)T2, nR2, mid, &lb, &ub);
      cBYE(status);
      // fprintf(stderr, "%d: For %d, lb = %d, ub = %d\n", k, mid, lb, ub);
      /* All the titles in rows lb to ub belong to member mid */
      for ( int l = lb; l < ub; l++ ) { 
	tid = T2[l].tid;
	status = bin_search((F3TYPE *)tids, n_tids, tid, &tlb, &tub);
	/*
	cBYE(status);
	fprintf(stderr, "%d: For %d: Adding %d to ", k, mid, tid);
	for ( int itemp = 0; itemp < n_tids; itemp++ ) { 
	  fprintf(stderr, "(%d, %d) ", tids[itemp].tid, tids[itemp].cnt);
	}
	fprintf(stderr, "\n");
	*/
	if ( tlb < 0 ) { /* not found */
	  for ( int dbg = 0; dbg < n_tids; dbg++ ) { 
	    if ( tids[dbg].tid == tid ) { 
	      go_BYE(-1); // ERROR 
	    }
	  }
	  if ( n_tids >= sz_tids ) { 
	    go_BYE(-1); // TO BE IMPLEMENTED
	  }
	  tids[n_tids].tid = tid;
	  tids[n_tids].cnt = 1;
	  n_tids++;
	  qsort(tids, n_tids, sizeof(F4TYPE), sort2_asc_int);
	}
	else {
	  if ( (tub - tlb) > 1 ) { 
	    go_BYE(-1); // title is not unique
	  }
	  tids[tlb].cnt = tids[tlb].cnt + 1;
	}
      }
    }
    //----------------------------------------------------------
    sum_cnt = 0;
    for ( int ii = 0; ii < n_tids; ii++ ) { 
      sum_cnt += tids[ii].cnt;
    }
    if ( ( sum_cnt == 0 ) || ( n_tids == 1 ) ) {
      /*
      fprintf(stderr, "Skipping Project %d ", prj_ctr);
      if ( n_tids == 1 ) { 
        fprintf(stderr, "because all titles same\n");
      }
      else {
        fprintf(stderr, "because sum == 0 \n");
        //TODO Should control ever come here?
      }
      */
      // SET UP FOR NEXT PROJECT
      for ( int ii = 0; ii < n_tids; ii++ ) {
        tids[ii].tid = -1;
        tids[ii].cnt = 0;
      }
      n_tids = 0;
      i = j; 
      continue;
    }
    //----------------------------------------------------------
    /* INTUITION: Now we have tids[n_tids] which is an array of (tid,
    cnt). The intuition is to delete titles which occur infrequently as
    an aberration. So, suppose we have [(t1, 100), (t2, 200), (t3, 1)],
    then we would eliminate t3 and have [(t1, 100), (t2, 200)]. Now, let
    us be a bit more precise. Delete a title if it accounts for less
    than 10% of the total titles.
    */
#define TITLE_FRAC1 0.10
    titles_per_member = sum_cnt / (float)(j - i);
    for ( int ii = 0; ii < n_tids; ii++ ) {
      float frac1;
      frac1 = (float)tids[ii].cnt  / (float)members_in_project;
      if ( frac1 < TITLE_FRAC1 ) {
	tids[ii].tid = INT_MAX;
	tids[ii].cnt = 0;
      }
    }
    //----------------------------------------------------------
    sum_cnt = 0; effective_n_tids = 0;
    for ( int ii = 0; ii < n_tids; ii++ ) { 
      if ( tids[ii].cnt > 0 ) { 
	effective_n_tids++;
        sum_cnt += tids[ii].cnt;
      }
    }
    /*
    if ( effective_n_tids != n_tids ) { 
      fprintf(stderr, "In Project %d, reducing from %d to %d \n",
	  prj_ctr, n_tids, effective_n_tids);
      for ( int ii = 0; ii < n_tids; ii++ ) {
	if ( tids[ii].cnt > 0 ) { 
          fprintf(stderr, "(tid, cnt) = (%d, %d) \n", 
	      tids[ii].tid, tids[ii].cnt);
	}
      }
    }
    */
    if ( ( sum_cnt == 0 ) || ( effective_n_tids == 1 ) ) {
      /*
      fprintf(stderr, "Skipping... Project %d ", prj_ctr);
      if ( effective_n_tids == 1 ) { 
        fprintf(stderr, "-- only one dominant title \n");
      }
      else {
        fprintf(stderr, "-- too much incoherence \n");
      }
      */
      // SET UP FOR NEXT PROJECT
      for ( int ii = 0; ii < n_tids; ii++ ) {
        tids[ii].tid = -1;
        tids[ii].cnt = 0;
      }
      n_tids = 0;
      i = j; 
      continue;
    }

    num_useful_projects++;
    //----------------------------------------------------------
    for ( int ii = 0; ii < n_tids; ii++ ) { 
      int tid_ii, cnt_ii; double denom;
      tid_ii = tids[ii].tid;
      cnt_ii = tids[ii].cnt;
      if ( ( tid_ii == INT_MAX ) || (cnt_ii == 0 ) ) {
	continue;
      }
      fwrite(&tid_ii, sizeof(int), 1, ofp2);
      fwrite(&cnt_ii, sizeof(int), 1, ofp2);
      // fprintf(stderr, "%d,%d,%d\n", prj_ctr, tid_ii, cnt_ii);
      denom = (double)sum_cnt * sum_cnt;
      for ( int jj = 0; jj < n_tids; jj++ ) { 
	int tid_jj, cnt_jj;
	tid_jj = tids[jj].tid;
	cnt_jj = tids[jj].cnt;
        if ( ( tid_jj == INT_MAX ) || (cnt_jj == 0 ) ) {
  	  continue;
	}
	prob = cnt_ii * 2.0 * (double)cnt_ii * (double)cnt_jj /  denom;
	// Note the order of writing We are writing
	// 1. Title j
	// 2. Title i 
	// 3. prob(title_j|title_i)
	// Actually, its not yet a prob. Summing and scaling TBD later
	fwrite(&tid_jj, sizeof(int), 1, ofp);
	fwrite(&tid_ii, sizeof(int), 1, ofp);
	fwrite(&prob, sizeof(double), 1, ofp);
      }
    }
    // SET UP FOR NEXT PROJECT
    for ( int ii = 0; ii < n_tids; ii++ ) {
      tids[ii].tid = -1;
      tids[ii].cnt = 0;
    }
    n_tids = 0;
    i = j;
    if ( ( prj_ctr % 1000 ) == 0 ) { 
      fprintf(stderr, "Processed %d good projects out of %d\n", 
      num_useful_projects, prj_ctr);
    }
  }
  fprintf(stderr, "Found %d good projects out of %d\n", 
      num_useful_projects, prj_ctr);
  fclose_if_non_null(ofp);
  fclose_if_non_null(ofp2);
  // sortbindmp(opfile2, "II", "AA");
  // sortbindmp(opfile, "LD", "AA");
 BYE:
  free_if_non_null(tids);
  fclose_if_non_null(ofp);
  fclose_if_non_null(ofp2);
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  return(status);
}
