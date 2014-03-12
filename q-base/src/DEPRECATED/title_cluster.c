#undef DATA_DEBUG

#include <limits.h> 
#include <float.h> 
#include <stdio.h> 
extern size_t getline(char **lineptr, size_t *n, FILE *stream);
#include <math.h> 
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "hash_string.h"
#include "sort_asc_int.h"
#include "sort_desc_int.h"

typedef struct t_b_type { 
  int tid;
  int bid;
} T_B_TYPE;

typedef struct id_mass_type { 
  int id;
  long long mass;
} ID_MASS_TYPE;

static int sortcompare(
		       const void *ii, 
		       const void *jj
		       )
{ 
  T_B_TYPE val1, val2;
  T_B_TYPE *ptr1, *ptr2;
  ptr1 = (T_B_TYPE *)ii;
  ptr2 = (T_B_TYPE *)jj;
  val1 = *ptr1;
  val2 = *ptr2;

  if ( val1.bid > val2.bid )  {
    return (1);
  }
  else if ( val1.bid < val2.bid ) {
    return (-1);
  }
  else {
    return(0);
  }
}

static int sortcompare_id_mass(
			       const void *ii, 
			       const void *jj
			       )
{ 
  ID_MASS_TYPE val1, val2;
  ID_MASS_TYPE *ptr1, *ptr2;
  ptr1 = (ID_MASS_TYPE *)ii;
  ptr2 = (ID_MASS_TYPE *)jj;
  val1 = *ptr1;
  val2 = *ptr2;

  if ( val1.mass < val2.mass )  {
    return (1);
  }
  else if ( val1.mass > val2.mass ) {
    return (-1);
  }
  else {
    return(0);
  }
}
int g_nT;
int g_desired_nB;
long long g_total_title_mass;
char *dbg_t_selected = NULL;
unsigned long long g_n_from_to_iprob;
char **g_clean_titles = NULL; 
int g_min_wt_cover_perc;
int g_chk1_ctr = 0; /* This is used to check whether sequential search
		       and binary search yield same answers  */
int g_chk2_ctr = 0; /* This is used to check whether cache results are
		       same as direct computation */

int
print_bin(
	  FILE *fp,
	  int bid,
	  T_B_TYPE *t_b,
	  int *t_b_lb,
	  int *t_b_ub,
	  int *title_mass
	  )
{
  int status = 0;
  int j = 0;
  ID_MASS_TYPE *id_mass = NULL;

  // START: Determine order in which to print bins 
  int num_titles_in_bin = t_b_ub[bid] - t_b_lb[bid]; 
  id_mass = (ID_MASS_TYPE *)malloc(num_titles_in_bin * sizeof(ID_MASS_TYPE));
  return_if_malloc_failed(id_mass);
  j = 0;
  for ( int t = t_b_lb[bid]; t < t_b_ub[bid]; t++, j++ ) { 
    int tid = t_b[t].tid;
    id_mass[j].id = tid;
    id_mass[j].mass = title_mass[tid];
  }
  qsort(id_mass, num_titles_in_bin, sizeof(ID_MASS_TYPE),
	sortcompare_id_mass);
  // STOP: Determine order in which to print bins 
  j = 1;
  for ( int i = 0; i < num_titles_in_bin; i++ ) { 
    int tid = id_mass[i].id;
    fprintf(fp, "%d,%d,%d,%s\n", j, tid, title_mass[tid], 
	    g_clean_titles[tid]);
  }
 BYE:
  free_if_non_null(id_mass);
  return(status);
}

int
mk_bin_names(
	     T_B_TYPE *t_b,
	     int nB,
	     int *t_b_lb,
	     int *t_b_ub,
	     int *title_mass, /* [nT] */
	     char **title_name, /* nT] */
	     char ***ptr_bin_name
	     )
{
  int status = 0;
  char **bin_name = NULL;
  bin_name = (char **)malloc(nB * sizeof(char *));
  return_if_malloc_failed(bin_name);
  for ( int bid = 0; bid < nB; bid++ ) { 
    int t_lb = t_b_lb[bid];
    int t_ub = t_b_ub[bid];
    int max_title_mass = INT_MIN;
    int max_tid = -1;
    for ( int t = t_lb; t < t_ub; t++ ) { 
      int tid = t_b[t].tid;
      if ( ( tid < 0 ) || ( tid >= g_nT ) ) { go_BYE(-1); }
      int this_title_mass = title_mass[tid];
      if ( this_title_mass > max_title_mass ) { 
	this_title_mass = max_title_mass;
	max_tid = tid;
      }
    }
    if ( ( max_tid < 0 ) || ( max_tid >= g_nT ) ) { go_BYE(-1); }
    int len = strlen(title_name[max_tid]) + 1;
    bin_name[bid] = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(bin_name[bid]);
    zero_string(bin_name[bid], len);
    strcpy(bin_name[bid], title_name[max_tid]);
  }
 BYE:
  *ptr_bin_name = bin_name;
  return(status);
}

int
print_bins(
	   T_B_TYPE *t_b,
	   int nB,
	   int *t_b_lb,
	   int *t_b_ub,
	   long long *bin_mass,
	   int *title_mass,
	   char **bin_name /* [nB] */
	   )
{
  int status = 0;
  long long  min_bin_mass_to_pr;
  int num_bins_to_pr = 0;
  FILE *sfp  = NULL; // summary 
  FILE *bfp  = NULL; // for each bin
  char fname[1024];
  ID_MASS_TYPE *id_mass = NULL;

  id_mass = (ID_MASS_TYPE *)malloc(nB * sizeof(ID_MASS_TYPE));
  return_if_malloc_failed(id_mass);
  zero_string(fname, 1024);
  sfp = fopen("_bins.csv", "w");
  return_if_fopen_failed(sfp,  "_bins.csv", "w");
  min_bin_mass_to_pr = (long long)(
				   (double)g_total_title_mass / g_desired_nB * 0.1 );
  // START: Determine order in which to print bins 
  for ( int bid = 0; bid < nB; bid++ ) { 
    if ( bin_mass[bid] >= min_bin_mass_to_pr ) { 
      id_mass[num_bins_to_pr].id = bid;
      id_mass[num_bins_to_pr].mass = bin_mass[bid];
      num_bins_to_pr++;
    }
  }
  qsort(id_mass, num_bins_to_pr, sizeof(ID_MASS_TYPE),
	sortcompare_id_mass);
  // STOP: Determine order in which to print bins 
  for ( int i = 0; i < num_bins_to_pr; i++ ) { 
    int bid = id_mass[i].id;
    // fprintf(stderr, "%d: %d : %lld \n", i, bid, id_mass[i].mass);
    if ( ( bid < 0 ) || ( bid >= nB ) ) { go_BYE(-1); }
    int num_titles_in_bin = t_b_ub[bid] -  t_b_lb[bid];
    if ( num_titles_in_bin < 1 ) { go_BYE(-1); }
    fprintf(sfp, "%d,%lld,%s,bin_%d\n",
	    (i+1), bin_mass[bid], bin_name[bid], (i+1) );
    sprintf(fname, "_bin_%d.csv", (i+1));
    bfp = fopen(fname, "w");
    return_if_fopen_failed(bfp, fname, "w");
    status = print_bin(bfp, bid, t_b, t_b_lb, t_b_ub, title_mass);
    cBYE(status);
    fclose_if_non_null(bfp);
    zero_string(fname, 1024);
    // fprintf(stderr, "early exit \n"); go_BYE(-1);
  }
 BYE:
  free_if_non_null(id_mass);
  fclose_if_non_null(bfp);
  fclose_if_non_null(sfp);
  return(status);
}


int
merge_bins(
	   int *title_mass,
	   int nT,
	   int inb1,
	   int inb2,
	   T_B_TYPE *t_b,
	   int **ptr_t_b_lb,
	   int **ptr_t_b_ub,
	   long long **ptr_bin_mass,
	   int *ptr_nB
	   )
{
  int status = 0;
  int b1, b2, nB;
  int *t_b_lb = NULL;
  int *t_b_ub = NULL;
  long long *bin_mass = NULL;
  int t2_lb, t2_ub;

  nB = *ptr_nB;
  // ERROR CHECKING AND making sure b1 < b2 
  if ( inb1 < inb2 ) { b1 = inb1; b2 = inb2; } else { b1 = inb2; b2 = inb1; }
  if ( b1 == b2 ) { go_BYE(-1); }
  if ( ( b1 < 0 ) || ( b1 >= *ptr_nB ) ) { go_BYE(-1); }
  if ( ( b2 < 0 ) || ( b2 >= *ptr_nB ) ) { go_BYE(-1); }
  //----------------------------------------------------
  // fprintf(stderr, "Merging Bin %5d and Bin %5d \n", b1, b2);
  print_bin(stderr, b1, t_b, *ptr_t_b_lb, *ptr_t_b_ub, title_mass);
  print_bin(stderr, b2, t_b, *ptr_t_b_lb, *ptr_t_b_ub, title_mass);

  //----------------------------------------------------

  // Give b2's titles to b1 
  t2_lb = (*ptr_t_b_lb)[b2];
  t2_ub = (*ptr_t_b_ub)[b2];
  for ( int t = t2_lb; t < t2_ub; t++ ) { 
    t_b[t].bid = b1;
  }
  // Reduce the bin numbers for all bins > b2
  for ( int t = 0; t < nT; t++ ) { 
    if ( t_b[t].bid > b2 ) {
      t_b[t].bid = t_b[t].bid - 1;
    }
  }
  // Re-sort t_b on bid
  qsort(t_b, nT, sizeof(T_B_TYPE), sortcompare);
  // Re-alloc t_b_lb and t_b_ub
  //---------------------------------------------------
  t_b_lb = (int *)malloc( (nB-1) * sizeof(int));
  return_if_malloc_failed(t_b_lb);
  t_b_ub = (int *)malloc( (nB-1) * sizeof(int));
  return_if_malloc_failed(t_b_ub);
  bin_mass   = (long long *)malloc( (nB-1) * sizeof(long long));
  return_if_malloc_failed(bin_mass);
  //---------------------------------------------------
  // Re-create t_b_lb and t_b_ub
  for ( int bid = 0; bid < nB-1; bid++ ) { 
    t_b_lb[bid]   = INT_MAX;
    t_b_ub[bid]   = INT_MIN;
    bin_mass[bid] = 0;
  }
  for ( int i = 0; i < nT; i++ ) {
    int bid = t_b[i].bid;
    int tid = t_b[i].tid;
    int tmass = title_mass[tid];
    if ( ( bid < 0 ) || ( bid >= nB ) ) { 
      go_BYE(-1); 
    }
    if ( i < t_b_lb[bid] ) {
      t_b_lb[bid] = i;
    }
    if ( i > t_b_ub[bid] ) {
      t_b_ub[bid] = i;
    }
    bin_mass[bid] += tmass;
  }
  for ( int bid = 0; bid < nB-1; bid++ ) { 
    t_b_ub[bid] = t_b_ub[bid] + 1; // since ub is exclusive
    if ( t_b_lb[bid] == INT_MAX ) { 
      go_BYE(-1);
    }
    if ( t_b_ub[bid] == INT_MIN ) { 
      go_BYE(-1);
    }
  }
  //---------------------------------------------------

  free_if_non_null(*ptr_t_b_lb);
  free_if_non_null(*ptr_t_b_ub);
  free_if_non_null(*ptr_bin_mass);
  *ptr_t_b_lb = t_b_lb;
  *ptr_t_b_ub = t_b_ub;
  *ptr_bin_mass   = bin_mass;
  *ptr_nB = *ptr_nB - 1;
 BYE:
  return(status);
}

int
check_bins(
	   T_B_TYPE *t_b, /* [nT] */
	   int nT,
	   int *t_b_lb,  /* [nB] */
	   int *t_b_ub,  /* [nB] */
	   long long *bin_mass, /* [nB] */
	   int *title_mass, /* [nT] */
	   int nB
	   )
{
  int status = 0;
  long long total_bin_mass = 0;
  for ( int j = 0; j < g_nT; j++ ) { dbg_t_selected[j] = 0; }
  if ( nB < g_desired_nB ) { go_BYE(-1); }
  if ( ( nB < 0 ) || (nB > g_nT ) ) { go_BYE(-1); }
  int num_t_selected = 0;

  for ( int bid = 0; bid < nB; bid++ ) { 
    if ( bin_mass[bid] < 0 ) { go_BYE(-1); }
    int tlb = t_b_lb[bid];
    int tub = t_b_ub[bid];
    if ( bin_mass[bid] < 0 ) { 
      go_BYE(-1);
    }
    total_bin_mass += bin_mass[bid];
    //-------------------------------------------
    long long chk_bin_mass = 0;
    for ( int j = tlb; j < tub; j++ ) { 
      int tid = t_b[j].tid;
      chk_bin_mass += title_mass[tid];
      dbg_t_selected[tid] = 1;
      num_t_selected++;
      if ( t_b[j].bid != bid ) { go_BYE(-1); }
    }
    if ( chk_bin_mass != bin_mass[bid] ) {
      go_BYE(-1);
    }
  }
  for ( int j = 0; j < g_nT; j++ ) { 
    if ( dbg_t_selected[j] == 0 ) { 
      go_BYE(-1);
    }
  }
  if ( num_t_selected != g_nT ) { 
    go_BYE(-1);
  }
  if ( total_bin_mass != g_total_title_mass ) { 
    go_BYE(-1); 
  }
 BYE:
  return(status);
}

int 
calc_error_in_bin(
		  T_B_TYPE *t_b, 
		  int b1, 
		  int b2, 
		  int *t_b_lb,
		  int *t_b_ub,
		  unsigned long long *from_to_iprob, 
		  unsigned long long n_from_to_iprob,
		  int *title_lb, 
		  int *title_ub, 
		  int *title_mass,
		  int b1_mass,
		  int b2_mass,
		  double min_error, 
		  double *ptr_error
		  )
{
  int status = 0;
  double error = 0;
  int b1_lb = t_b_lb[b1];
  int b1_ub = t_b_ub[b1];
  int b2_lb = t_b_lb[b2];
  int b2_ub = t_b_ub[b2];
  // double chk_error = 0; 
  /* See if results available in cache */
  /* TODO */
  /* Now, check cache versus direct computation */
  if ( ( g_chk2_ctr & 0x00000FFF ) == 0 ) { /* Make check only some times */



    for ( int t1 = b1_lb; t1 < b1_ub; t1++ ) {
      int probe;
      int tid1 = t_b[t1].tid;
      int t1_mass = title_mass[tid1];
      int t1_lb  = title_lb[tid1];
      int t1_ub  = title_ub[tid1];
      double t1_mass_by_b1_mass = (double)t1_mass /  (double)b1_mass;
      if ( ( b1_ub - b1_lb ) == 1 ) {
	if ( t1_mass != b1_mass ) { 
	  go_BYE(-1);
	}
      }
      for ( int t2 = b2_lb; t2 < b2_ub; t2++ ) { 
	int tid2 = t_b[t2].tid;
	int t2_mass = title_mass[tid2];
	double t2_mass_by_b2_mass = (double)t2_mass /  (double)b2_mass;
	double t1t2_error = FLT_MAX, chk_t1t2_error = FLT_MAX;
	int iprob = INT_MAX, chk_iprob = INT_MAX;
	if ( ( b2_ub - b2_lb ) == 1 ) { /* if there is one title in bin */
	  if ( t2_mass != b2_mass ) { 
	    go_BYE(-1);
	  }
	}
	else if ( ( b2_ub - b2_lb ) > 1 ) { /* if there is > 1 title in bin */
	  if ( b2_mass < t2_mass ) { 
	    go_BYE(-1);
	  }
	}
	else {
	  go_BYE(-1); 
	}
	// Now we need to find the relatedness of titles t1 and t2
	// BINARY SEARCH 
	int save_t1_lb = t1_lb;
	int save_t1_ub = t1_ub;
	for ( ;  t1_ub > t1_lb;  ) { 
	  bool is_last_iter = false;
	  probe = (t1_lb + t1_ub) / 2 ;
	  if ( ( probe == t1_lb ) || ( probe == t1_ub ) )  { 
	    is_last_iter = true;
	  }
	  unsigned long long ltemp = from_to_iprob[probe];
	  int from_tid = ltemp >> 48;
	  if ( from_tid != tid1 ) { go_BYE(-1); }
	  int to_tid = ( ltemp >> 32 ) & 0x0000FFFF;
	  if ( to_tid == from_tid ) { go_BYE(-1); }
	  if ( to_tid > g_nT ) { go_BYE(-1); }
	  if ( to_tid == tid2 ) {
	    iprob = ltemp & 0x00000000FFFFFFFF;
	    if ( iprob > 1024 ) { 
	      go_BYE(-1); 
	    }
	    t1t2_error = 1.0 - ( iprob / 1024.0 ) ;
	    break;
	  }
	  else if ( to_tid < tid2 ) {
	    t1_lb = probe;
	  }
	  else if ( to_tid > tid2 ) { 
	    t1_ub = probe;
	  }
	  else { go_BYE(-1); }
	  if ( is_last_iter ) { break; }
	}
	if ( ( g_chk1_ctr & 0x00000FFF ) == 0 )  {
	  t1_lb = save_t1_lb;
	  t1_ub = save_t1_ub;
	  for ( int k = t1_lb; k < t1_ub; k++ ) {
	    unsigned long long ltemp = from_to_iprob[k];
	    int from_tid = ltemp >> 48;
	    if ( from_tid != tid1 ) { go_BYE(-1); }
	    int to_tid = ( ltemp >> 32 ) & 0x0000FFFF;
	    if ( to_tid == from_tid ) { go_BYE(-1); }
	    if ( to_tid > g_nT ) { go_BYE(-1); }
	    if ( to_tid == tid2 ) {
	      chk_iprob = ltemp & 0x00000000FFFFFFFF;
	      if ( iprob > 1024 ) { 
		go_BYE(-1); 
	      }
	      chk_t1t2_error = 1.0 - ( iprob / 1024.0 ) ;
	      break;
	    }
	  }
	  if ( chk_t1t2_error != t1t2_error ) { 
	    go_BYE(-1); 
	  }
	  if ( chk_iprob != iprob ) { 
	    go_BYE(-1); 
	  }
	}
	g_chk1_ctr++;

	//---------------------------------------
	if ( t1t2_error <= 0 ) {
	  go_BYE(-1);
	}
	if ( iprob == INT_MAX ) {
	  t1t2_error = 1.0;
	}
	//------------------------------------------
	if ( t1t2_error == FLT_MAX ) { go_BYE(-1); }
	error += t1t2_error * t1_mass_by_b1_mass * t2_mass_by_b2_mass;
	if ( error > min_error ) {
	  *ptr_error = FLT_MAX; 
	  break;
	}
      }
      if ( *ptr_error >= FLT_MAX ) { break; }
    }
    /* TODO 
       if ( error != chk_error ) { 
       fprintf(stderr, "Cache and direct computation not in sync\n"); go_BYE(-1); 
       }
    */
  }
  else {
    fprintf(stderr, "Not checking cache versus computation \n");
  }
  // TODO g_chk2_ctr++;
  if ( error <= 0 ) {
    go_BYE(-1);
  }

  *ptr_error = error;
 BYE:
  return(status);
}

int
get_min_max_bin_mass(
		     long long *bin_mass, 
		     int nB, 
		     int desired_nB, 
		     long long *ptr_min_bin_mass,
		     long long *ptr_max_bin_mass,
		     bool *ptr_is_suff_mass_cover
		     )
{
  int status = 0;
  int xnB = (int)(desired_nB * log(desired_nB));
  int ynB = (int)(desired_nB / log(desired_nB));
  long long min_bin_mass = LLONG_MAX;
  long long max_bin_mass = LLONG_MIN;
  long long mass_cover = 0;
  long long *alt_bin_mass = NULL;

  alt_bin_mass = (long long *)malloc(nB * sizeof(long long));
  return_if_malloc_failed(alt_bin_mass);
  for ( int i = 0; i < nB; i++ ) { 
    alt_bin_mass[i] = bin_mass[i];
  }
  qsort(alt_bin_mass, nB, sizeof(long long), sort_desc_int);
  for ( int i = 0; i < desired_nB; i++ ) { 
    mass_cover += alt_bin_mass[i];
  }
  if ( ( ( mass_cover * 100.0 ) / g_total_title_mass ) >=
       g_min_wt_cover_perc ) {
    fprintf(stderr, "Sufficient mass has been covered \n");
    fprintf(stderr, "%lld out of %lld \n", mass_cover, g_total_title_mass);
    *ptr_is_suff_mass_cover = true;
    goto BYE;
  }

  //-------------------------------------------------------------

  if ( nB <= xnB ) {
    max_bin_mass = LLONG_MAX; // no longer excludes anybody
    min_bin_mass = LLONG_MIN; // no longer excludes anybody
  }
  else {
    min_bin_mass = alt_bin_mass[xnB];
    max_bin_mass = alt_bin_mass[ynB];
  }
  //-------------------------------------------------------------
  if ( min_bin_mass > max_bin_mass ) { go_BYE(-1); }
  //-------------------------------------------------------------
  *ptr_min_bin_mass = min_bin_mass;
  *ptr_max_bin_mass = max_bin_mass;
  /* fprintf(stderr, "xnB = %5d, min mass = %lld, max mass = %lld \n", xnB, 
     min_bin_mass, max_bin_mass);
  */
 BYE:
  free_if_non_null(alt_bin_mass);
  return(status);
}
int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  int nT; /* number of titles */
  int nB; /* number of bins */

  char *from_to_iprob_file = NULL; 
  unsigned long long *from_to_iprob = NULL;  /* [n_from_to_iprob] */
  int n_from_to_iprob;
  char *tids_file = NULL; int *tids = NULL;
  char *title_mass_file = NULL; int *title_mass = NULL;
  long long total_title_mass = 0; 
  long long min_bin_mass = LLONG_MAX;
  long long max_bin_mass = LLONG_MIN;
  char *title_lb_file = NULL; 
  int *title_lb = NULL; /* [n_from_to_iprob] */
  char *title_ub_file = NULL; 
  int *title_ub = NULL; /* [n_from_to_iprob] */

  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  char *X3 = NULL; size_t nX3 = 0;
  char *X4 = NULL; size_t nX4 = 0;
  char *X5 = NULL; size_t nX5 = 0;

  char *clean_title_file = NULL; FILE *tfp = NULL;
  char *line = NULL; size_t line_len;

  int *t_b_lb = NULL; /* [nB] ptr to t_b */
  int *t_b_ub = NULL; /* [nB] ptr to t_b */
  long long *bin_mass = NULL; /* [nB] */
  T_B_TYPE *t_b = NULL; /* [nT] */
  int desired_nB; /* desired number of bins */
  bool is_suff_mass_cover = false; 
  /* is_suff_mass_cover tells us whether the top desired_nB bins cover 
     X % of the total mass where X is input */
#define MAX_LEN_CLEAN_TITLE 48
  /* ok to truncate title len to 32 since this is just for debugging */
  long long mass_cutoff;
  char **bin_name = NULL;


  if ( argc != 7 ) { go_BYE(-1); }
  from_to_iprob_file  = argv[1];
  tids_file           = argv[2];
  title_mass_file     = argv[3];
  title_lb_file       = argv[4]; /* pointer to from_to_iprob_file */
  title_ub_file       = argv[5]; /* pointer to from_to_iprob_file */
  desired_nB          = 256; /* TODO: This should be input */
  clean_title_file    = argv[6];
  g_min_wt_cover_perc = 80; /* TODO: This should be input */

  g_desired_nB = desired_nB;
  status = rs_mmap(from_to_iprob_file, &X1, &nX1, 0); cBYE(status);
  status = rs_mmap(tids_file, &X2, &nX2, 0); cBYE(status);
  /* Note that we need to open the following 3 as writable because we
   * need to strike out titles for which no related info is provided */
  status = rs_mmap(title_mass_file,  &X3, &nX3, 1); cBYE(status);
  status = rs_mmap(title_lb_file,  &X4, &nX4, 1); cBYE(status);
  status = rs_mmap(title_ub_file, &X5, &nX5, 1); cBYE(status);

  // X1 is  ((t1, t2), prob) ((short, short), int))
  from_to_iprob = (unsigned long long *)X1;
  n_from_to_iprob = nX1 / sizeof(long long);
  if ( ( n_from_to_iprob * sizeof(long long) ) != nX1 ) { go_BYE(-1); }
  g_n_from_to_iprob = n_from_to_iprob;

  // X2 is (t1) 
  tids       = (int *)X2;
  nT = nX2 / sizeof(int);
  if ( ( nT * sizeof(int) ) != nX2 ) { go_BYE(-1); }
  // Note that 32767 has to deal with our usage of shorts for compression
  if ( ( nT < 0 ) || ( nT > 32767 ) ) { go_BYE(-1); }
#ifdef DATA_DEBUG
  // Check: \(tid \in \{0, nT-1\}\) 
  for ( int i = 0; i < nT; i++ ) { 
    if ( ( tids[i] < 0 ) || ( tids[i] >= nT ) ) { go_BYE(-1); }
  }
#endif
  g_nT = nT; // fpr debugging only
  // Read clean title file -- just for debugging
  g_clean_titles = (char **)malloc(nT * sizeof(char *));
  return_if_malloc_failed(g_clean_titles);
  for ( int i = 0; i < nT; i++ ) { 
    g_clean_titles[i] = (char *)malloc(MAX_LEN_CLEAN_TITLE * sizeof(char));
    return_if_malloc_failed(g_clean_titles[i]);
    zero_string(g_clean_titles[i], MAX_LEN_CLEAN_TITLE);
  }
  tfp = fopen(clean_title_file, "r");
  return_if_fopen_failed(tfp, clean_title_file, "r");
  for ( int i = 0; !feof(tfp) ; i++ ) { 
    char *cptr = NULL;
    getline(&line, &line_len, tfp);
    if ( line_len == 0 ) { break; }
    if ( i == nT ) { break; }
    strncpy(g_clean_titles[i], line, MAX_LEN_CLEAN_TITLE);
    free_if_non_null(line);
    for ( cptr = g_clean_titles[i]; *cptr != '\0'; cptr++ ) { 
      // We want to truncate after comma. Hence below
      if ( *cptr == ',' ) { *cptr = '\0'; }
      if ( *cptr == '\n' ) { *cptr = '\0'; }
    }
  }
  // Check: tid is unique
#ifdef DATA_DEBUG
  for ( int i = 0; i < nT; i++ ) { 
    for ( int j = i+1; j < nT; j++ ) { 
      if ( tids[i] == tids[j] ) { go_BYE(-1); }
    }
  }
#endif
  mcr_alloc_null_str(dbg_t_selected, nT);
  // X3 is mass for title 
  title_mass    = (int *)X3;
  if ( nX3 != nX2 ) { go_BYE(-1); }
  for ( int i = 0; i < nT; i++ ) { 
    if ( title_mass[i] < 0 ) { go_BYE(-1); }
    total_title_mass += title_mass[i];
  }
  g_total_title_mass = total_title_mass;
  mass_cutoff = ( 1.5 * g_total_title_mass ) / (float)desired_nB;
  fprintf(stderr, "g_total_mass = %lld, mass_cutoff = %lld \n",
	  g_total_title_mass, mass_cutoff);

  // the rows for title i in from_to_iprob are from title_lb[i]
  // (inclusive) to title_ub[i] (exclusive)
  title_lb      = (int *)X4;
  if ( nX4 != nX2 ) { go_BYE(-1); }

  title_ub      = (int *)X5;
  if ( nX5 != nX2 ) { go_BYE(-1); }

#ifdef DATA_DEBUG
  for ( int i = 0; i < nT; i++ ) {
    bool is_dump = false;
    if ( title_lb[i] >= title_ub[i] ) { is_dump = true; }
    if (( title_lb[i] < 0 ) || ( title_lb[i] >= n_from_to_iprob )) {
      is_dump = true;
    }
    if (( title_ub[i] < 0 ) || ( title_ub[i] >= n_from_to_iprob )) {
      is_dump = true;
    }
    /* is_dump = true for titles for which no relatedness information is
     * provided */
    if ( is_dump ) { 
      title_lb[i] = -1;
      title_ub[i] = -1;
      title_mass[i] = 0;
    }
  }
  // Check no overlap in intervals
  for ( int i = 0; i < nT; i++ ) { 
    for ( int j = i+1; j < nT; j++ ) { 
      int lbi = title_lb[i];
      int ubi = title_ub[i];
      int lbj = title_lb[j];
      int ubj = title_ub[j];
      if ( ( lbi < 0 ) || ( ubi < 0 ) || ( lbj < 0 ) || ( ubj < 0 ) ) {
	continue;
      }
      if ( ( lbi < lbj ) && ( ubi > lbj ) ) { go_BYE(-1); }
      if ( ( lbj < lbi ) && ( ubj > lbi ) ) { go_BYE(-1); }
      if ( ( lbi > lbj ) && ( ubi < lbj ) ) { go_BYE(-1); }
      if ( ( lbj > lbi ) && ( ubj < lbi ) ) { go_BYE(-1); }
    }
  }
#endif
  t_b = (T_B_TYPE *)malloc(nT * sizeof(T_B_TYPE));
  return_if_malloc_failed(t_b);
  for ( int i = 0; i < nT; i++ ) { 
    t_b[i].tid = i;
    t_b[i].bid = i;
  }
  nB = nT;
  /* The data structure t_b_lb/t_b_ub allows us to pinpoint the
   * titles in a particular bin by pointing in to the t_b structure */
  /* As usual, lb is inclusive, ub is exclusive */
  t_b_lb = (int *)malloc(nB * sizeof(int));
  return_if_malloc_failed(t_b_lb);
  t_b_ub = (int *)malloc(nB * sizeof(int));
  return_if_malloc_failed(t_b_ub);
  bin_mass = (long long *)malloc(nB * sizeof(long long));
  return_if_malloc_failed(bin_mass);
  for ( int i = 0; i< nB; i++ ) { 
    t_b_lb[i] = i;
    t_b_ub[i] = i+1;
    bin_mass[i] = title_mass[i];
  }
  status = check_bins(t_b, nT, t_b_lb, t_b_ub, bin_mass, title_mass, nB);
  cBYE(status);

  for ( int iter = 0; nB > desired_nB; iter++ ) { 
    int num_checks_this_iter = 0;
    double prev_min_error = FLT_MAX;
    // fprintf(stderr, "Iter %4d, nB = %5d \n", iter, nB);
    status = get_min_max_bin_mass(bin_mass, nB, desired_nB, &min_bin_mass,
				  &max_bin_mass, &is_suff_mass_cover);
    cBYE(status);
    if ( is_suff_mass_cover ) { 
      break;
    }
    /* In each iteration, we reduce the number of bins by 1 */
    int bin1_to_merge = -1;
    int bin2_to_merge = -1;
    double min_error = FLT_MAX;
    for ( int b1 = 0; b1 < nB; b1++ ) {
      int b1_lb = t_b_lb[b1];
      int b1_ub = t_b_ub[b1];
      long long b1_mass = bin_mass[b1];
      if ( b1_lb == -1 ) { continue; }
      if ( ( b1_lb < 0 ) || ( b1_lb >= nT ) )  { go_BYE(-1); }
      if ( ( b1_ub <= 0 ) || ( b1_ub > nT ) )  { go_BYE(-1); }
      if ( b1_mass < 0 ) { go_BYE(-1); }
      // Do not merge a bin if it is big enough 
      if ( b1_mass > mass_cutoff ) { continue; } 
      /* Here we put in a hack to focus our attention on the biggest
       * bins */
      if ( b1_mass < min_bin_mass ) { continue; }
      if ( b1_mass > max_bin_mass ) { continue; }

      for ( int b2 = b1+1; b2 < nB; b2++ ) { 
        long long b2_mass = bin_mass[b2];
        int b2_lb = t_b_lb[b2];
        int b2_ub = t_b_ub[b2];
        if ( b2_lb == -1 ) { continue; }
        if ( ( b2_lb < 0 ) || ( b2_lb >= nT ) )  { go_BYE(-1); }
        if ( ( b2_ub <= 0 ) || ( b2_ub > nT ) )  { 
	  go_BYE(-1); 
	}
        if (  b2_mass == -1 ) { continue; }
        if ( b2_mass < 0 ) { go_BYE(-1); }
        if ( b2_mass < min_bin_mass ) { continue; }
        if ( b2_mass > max_bin_mass ) { continue; }
        // Do not merge a bin if it is big enough 
        if ( b2_mass > mass_cutoff ) { continue; } 
	double error;
        num_checks_this_iter++;
	status = calc_error_in_bin(
				   t_b, b1, b2, t_b_lb, t_b_ub,
				   from_to_iprob, n_from_to_iprob,
				   title_lb, title_ub, 
				   title_mass, b1_mass, b2_mass, 
				   min_error, &error);
	cBYE(status);
	if ( error < min_error ) { 
	  min_error = error;
	  bin1_to_merge = b1;
	  bin2_to_merge = b2;
	}
      }
    }
    if ( bin1_to_merge == bin2_to_merge ) { go_BYE(-1); }
    if ( (bin1_to_merge < 0) || (bin1_to_merge > nB) ) { go_BYE(-1); }
    if ( (bin2_to_merge < 0) || (bin2_to_merge > nB) ) { go_BYE(-1); }
    fprintf(stderr, "Iter %5d: Merging %d and %d (error = %lf). Checks = %d\n", 
	    iter, bin1_to_merge, bin2_to_merge, min_error,
	    num_checks_this_iter);
    status = merge_bins(
			title_mass, nT, 
			bin1_to_merge, bin2_to_merge, 
			t_b, &t_b_lb, &t_b_ub, &bin_mass, &nB);
    cBYE(status);
    status = check_bins(t_b, nT, t_b_lb, t_b_ub, bin_mass, title_mass, nB);
    cBYE(status);
    if ( prev_min_error < min_error ) { 
      fprintf(stderr, "Strange! Previous error was %lf \n", prev_min_error);
    }
    prev_min_error = min_error; 
    /* We print the current solution once in a way so that we can
       inspect the results and make sure we are headed in the right
       direction  */
    if ( ( iter & 0x000000FF ) == 0 ) {
      status = mk_bin_names(t_b, nB, t_b_lb, t_b_ub, title_mass,
			    g_clean_titles, &bin_name);
      cBYE(status);
      status = print_bins(t_b, nB, t_b_lb, t_b_ub, bin_mass, title_mass,
			  bin_name);
      cBYE(status);
    }
    //------------------------------------
  }
  status = mk_bin_names(t_b, nB, t_b_lb, t_b_ub, title_mass,
			g_clean_titles, &bin_name);
  cBYE(status);
  status = print_bins(t_b, nB, t_b_lb, t_b_ub, bin_mass, title_mass,
		      bin_name);
  cBYE(status);

 BYE:
  for ( int i = 0; i < nB; i++ ) { 
    free_if_non_null(bin_name[i]);
  }
  free_if_non_null(bin_name);

  free_if_non_null(t_b);
  free_if_non_null(t_b_lb);
  free_if_non_null(t_b_ub);
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  rs_munmap(X3, nX3);
  rs_munmap(X4, nX4);
  rs_munmap(X5, nX5);
  return(status);
}

