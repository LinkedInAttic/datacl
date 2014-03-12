#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "set_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "rng_sort.h"

#include "qsort_asc_I4.h"
#include "qsort_dsc_I4.h"

#include "qsort_asc_I8.h"
#include "qsort_dsc_I8.h"


#define ASCENDING  1
#define DESCENDING 2
/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
rng_sort(
	 char *t1,
	 char *f1,
	 char *mode, /* A for ascending, D for descending */
	 char *t2,
	 char *lb,
	 char *ub
	 )
/* STOP FUNC DECL */
{
  int status = 0;
  char *f1_X  = NULL; size_t f1_nX  = 0;
  char *lb_X  = NULL; size_t lb_nX  = 0;
  char *ub_X = NULL; size_t ub_nX = 0;

  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN,  nn_f1_id = INT_MIN;
  int lb_id = INT_MIN,  nn_lb_id = INT_MIN;
  int ub_id = INT_MIN, nn_ub_id = INT_MIN;
  TBL_REC_TYPE t1_rec,  t2_rec;
  FLD_REC_TYPE f1_rec,  nn_f1_rec;
  FLD_REC_TYPE lb_rec,  nn_lb_rec;
  FLD_REC_TYPE ub_rec, nn_ub_rec;
  long long nR1, nR2;  int imode;
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( mode == NULL ) || ( *mode == '\0' ) ) { go_BYE(-1); }
  if ( ( lb == NULL ) || ( *lb == '\0' ) ) { go_BYE(-1); }
  if ( ( ub == NULL ) || ( *ub == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb, ub) == 0 ) { go_BYE(-1); }
  if ( strcmp(mode,"A") == 0 ) { 
    imode = ASCENDING; 
  }
  else if ( strcmp(mode,"D") == 0 ) { 
    imode = DESCENDING; 
  }
  else {
    go_BYE(-1);
  }

  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  if ( t1_id < 0 ) { go_BYE(-1); }
  nR1 = t1_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( f1_rec.is_external == true ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, true); cBYE(status);
  /* Restrictions of current implementation */
  switch ( f1_rec.fldtype ) { 
  case I4 : case I8 : break;
  default : go_BYE(-1); break;
  }
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  /*--------------------------------------------------------*/
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  if ( t2_id < 0 ) { go_BYE(-1); }
  nR2 = t2_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, lb, &lb_id, &lb_rec, &nn_lb_id, &nn_lb_rec); 
  cBYE(status);
  if ( lb_id < 0 ) { go_BYE(-1); }
  status = get_data(lb_rec, &lb_X, &lb_nX, false); cBYE(status);
  if ( lb_rec.fldtype != I8 ) { go_BYE(-1); }
  if ( nn_lb_id >= 0 ) { go_BYE(-1); }
  long long *lbI8  = (long long *)lb_X;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, ub, &ub_id, &ub_rec, &nn_ub_id, &nn_ub_rec); 
  cBYE(status);
  if ( ub_id < 0 ) { go_BYE(-1); }
  status = get_data(ub_rec, &ub_X, &ub_nX, false); cBYE(status);
  /* Restrictions of current implementation */
  if ( ub_rec.fldtype != I8 ) { go_BYE(-1); }
  if ( nn_ub_id >= 0 ) {  go_BYE(-1); }
  long long *ubI8 = (long long *)ub_X;
  /*--------------------------------------------------------*/
  switch ( f1_rec.fldtype ) { 
  case I4 : 
#pragma omp parallel for 
    for ( int i = 0; i < nR2; i++ ) {
      if ( ( lbI8[i] <  0 ) || ( lbI8[i] >= nR1 ) ) { status = -1; continue; }
      if ( ( ubI8[i] <= 0 ) || ( ubI8[i] >  nR1 ) ) { status = -1; continue; }
      if ( lbI8[i] >= ubI8[i] ) { status = -1; continue; }
      int *f1I4 = (int *)f1_X; f1I4 += lbI8[i];
      long long num_to_sort = ubI8[i] - lbI8[i];
      if ( ubI8[i] > 1 ) {
	switch ( imode ) {
	case ASCENDING : 
	  qsort_asc_I4(f1I4, num_to_sort, sizeof(int), NULL);
	  break;
	case DESCENDING : 
	  qsort_dsc_I4(f1I4, num_to_sort, sizeof(int), NULL);
	  break;
	default : 
	  break;
	}
      }
    }
    break;
  case I8 : 
#pragma omp parallel for 
    for ( int i = 0; i < nR2; i++ ) {
      if ( ( lbI8[i] < 0  ) || ( lbI8[i] >= nR1 ) ) { status = -1; continue; }
      if ( ( ubI8[i] <= 0 ) || ( ubI8[i] >  nR1 ) ) { status = -1; continue; }
      if ( lbI8[i] >= ubI8[i] ) { status = -1; continue; }
      long long *f1I8 = (long long *)f1_X; f1I8 += lbI8[i];
      long long num_to_sort = ubI8[i] - lbI8[i];
      if ( ubI8[i] >1 ) { 
	switch ( imode ) {
	case ASCENDING : 
	  qsort_asc_I8(f1I8, num_to_sort, sizeof(long long), NULL);
	  break;
	case DESCENDING : 
	  qsort_dsc_I8(f1I8, num_to_sort, sizeof(long long), NULL);
	  break;
	default : 
	  break;
	}
      }
    }
    break;
  default :
    go_BYE(-1);
    break;
  }
  /*-----------------------------------------------------------*/
 BYE:
  rs_munmap(f1_X,  f1_nX);
  rs_munmap(lb_X,  lb_nX);
  rs_munmap(ub_X, ub_nX);
  return(status);
}
