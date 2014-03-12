#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "sort2_asc_int.h"


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
// STOP FUNC DECL
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
    if ( lb < 0 ) { lb = 0; }
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
  char *t1file = NULL, *t2file = NULL, *opfile = NULL;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  F1TYPE *T1 = NULL;
  F2TYPE *T2 = NULL;
  int mid, pid, tid, xpid;
  FILE *ofp = NULL;
  int nR1, nR2, lb, ub, tlb, tub;
  F4TYPE *tids = NULL; int sz_tids = 8192; int n_tids = 0;

  tids = (F4TYPE *)malloc(sz_tids * sizeof(F4TYPE));
  return_if_malloc_failed(tids);
  if ( argc != 4 ) {
    fprintf(stderr, "Usage is %s <tbl1> <tbl2> <optbl> \n", argv[0]); 
    go_BYE(-1); 
  }
  t1file = argv[1];
  t2file = argv[2];
  opfile = argv[3];
  if ( strcmp(t1file, t2file) == 0 ) { go_BYE(-1); }
  if ( strcmp(t2file, opfile) == 0 ) { go_BYE(-1); }

  ofp = fopen(opfile, "wb");
  return_if_fopen_failed(ofp,  opfile, "wb");
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

  for ( int i = 0; i < nR1; ) {
    int j;
    pid  = T1[i].pid;
    for ( j = i + 1; j < nR1; j++ ) { 
      xpid   = T1[j].pid;
      if ( xpid != pid ) { break; }
    }
    /* This means that rows with lb = i and ub = j all belong to the
     * same project */
        for ( int k = i; k < j; k++ ) { 
          mid  = T1[k].mid;
          status = bin_search((F3TYPE *)T2, nR2, mid, &lb, &ub);
	  cBYE(status);
          /* All the titles in rows lb to ub belong to member mid */
          for ( int l = lb; l < ub; l++ ) { 
            tid = T2[l].tid;
            status = bin_search((F3TYPE *)tids, n_tids, tid, &tlb, &tub);
	    cBYE(status);
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
	if ( ( pid % 1000 ) == 0 ) {
	  fprintf(stderr, "Completed project %d \n", pid);
	}
	for ( int ii = 0; ii < n_tids; ii++ ) { 
	  int tid_ii, cnt_ii;
	  long long cnt;
	  tid_ii = tids[ii].tid;
	  cnt_ii = tids[ii].cnt;
	  for ( int jj = ii+1; jj < n_tids; jj++ ) { 
	    int tid_jj, cnt_jj;
	    long long ll_cnt;
	    tid_jj = tids[jj].tid;
	    cnt_jj = tids[jj].cnt;
	    // We write a strange format. Essentially, we write 2 long
	    // longs. First is created by concatenating tid_ii and
	    // tid_jj. Second is the count promoted to a long long
	    fwrite(&tid_ii, sizeof(int), 1, ofp);
	    fwrite(&tid_jj, sizeof(int), 1, ofp);
	    cnt = cnt_ii * cnt_jj;
	    ll_cnt = (long long)cnt;
	    fwrite(&ll_cnt, sizeof(long long), 1, ofp);
	  }
	}
	i = j;
	for ( int ii = 0; ii < n_tids; ii++ ) {
	  tids[0].tid = -1;
	  tids[0].cnt = 0;
	}
	n_tids = 0;

  }
BYE:
  free_if_non_null(tids);
  fclose_if_non_null(ofp);
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  return(status);
}
// START FUNC DECL
