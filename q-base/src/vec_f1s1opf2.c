#ifdef IPP
#include "ipp.h"
#include "ippi.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "vec_f1s1opf2.h"

#include "add_scalar_I1.h"
#include "sub_scalar_I1.h"
#include "mul_scalar_I1.h"
#include "div_scalar_I1.h"
#include "rem_scalar_I1.h"

#include "add_scalar_I4.h"
#include "add_scalar_I2.h"
#include "sub_scalar_I2.h"
#include "mul_scalar_I2.h"
#include "div_scalar_I2.h"
#include "rem_scalar_I2.h"

#include "add_scalar_I4.h"
#include "sub_scalar_I4.h"
#include "mul_scalar_I4.h"
#include "div_scalar_I4.h"
#include "rem_scalar_I4.h"

#include "add_scalar_I8.h"
#include "sub_scalar_I8.h"
#include "mul_scalar_I8.h"
#include "div_scalar_I8.h"
#include "rem_scalar_I8.h"

#include "add_scalar_F4.h"
#include "sub_scalar_F4.h"
#include "mul_scalar_F4.h"
#include "div_scalar_F4.h"

#include "add_scalar_F8.h"
#include "sub_scalar_F8.h"
#include "mul_scalar_F8.h"
#include "div_scalar_F8.h"

#include "and_scalar_I1.h"
#include "or_scalar_I1.h"
#include "bwise_not_or_scalar_I8.h"
#include "bwise_not_and_scalar_I8.h"
#include "bwise_and_scalar_I4.h"
#include "bwise_and_scalar_I8.h"
#include "bwise_and_scalar_I1.h"
#include "bwise_or_scalar_I4.h"
#include "bwise_or_scalar_I8.h"
#include "bwise_or_scalar_I1.h"
#include "bwise_xor_scalar_I4.h"
#include "bwise_xor_scalar_I8.h"
#include "bwise_xor_scalar_I1.h"

#include "shift_left_scalar_I4.h"
#include "shift_left_scalar_I8.h"
#include "shift_right_scalar_UI4.h"
#include "shift_right_scalar_UI8.h"

#include "cmp_le_scalar_I4.h"
#include "cmp_ge_scalar_I4.h" 
#include "cmp_eq_scalar_I4.h"
#include "cmp_ne_scalar_I4.h"
#include "cmp_gt_scalar_I4.h"
#include "cmp_lt_scalar_I4.h"

#include "cmp_le_scalar_I8.h"
#include "cmp_ge_scalar_I8.h"
#include "cmp_eq_scalar_I8.h"
#include "cmp_ne_scalar_I8.h"
#include "cmp_lt_scalar_I8.h"
#include "cmp_gt_scalar_I8.h"

#include "cmp_le_scalar_F4.h" 
#include "cmp_ge_scalar_F4.h"
#include "cmp_eq_scalar_F4.h"
#include "cmp_ne_scalar_F4.h"
#include "cmp_lt_scalar_F4.h"
#include "cmp_gt_scalar_F4.h"

#include "cmp_le_scalar_F8.h" 
#include "cmp_ge_scalar_F8.h"
#include "cmp_eq_scalar_F8.h"
#include "cmp_ne_scalar_F8.h"
#include "cmp_lt_scalar_F8.h"
#include "cmp_gt_scalar_F8.h"

#include "cmp_le_scalar_I1.h"
#include "cmp_ge_scalar_I1.h"
#include "cmp_eq_scalar_I1.h"
#include "cmp_ne_scalar_I1.h"
#include "cmp_lt_scalar_I1.h"
#include "cmp_gt_scalar_I1.h"

#include "cmp_le_scalar_I2.h"
#include "cmp_ge_scalar_I2.h"
#include "cmp_eq_scalar_I2.h"
#include "cmp_ne_scalar_I2.h"
#include "cmp_lt_scalar_I2.h"
#include "cmp_gt_scalar_I2.h"

#include "dbg_cmp_neq_mult_scalar_I1.h"
#include "dbg_cmp_neq_mult_scalar_I2.h"
#include "dbg_cmp_neq_mult_scalar_I4.h"
#include "dbg_cmp_neq_mult_scalar_I8.h"

#include "cmp_eq_mult_scalar_I1.h"
#include "cmp_eq_mult_scalar_I2.h"
#include "cmp_eq_mult_scalar_I4.h"
#include "cmp_eq_mult_scalar_I8.h"

#include "cmp_lt_or_gt_I4.h"
#include "cmp_le_or_ge_I4.h"
#include "cmp_gt_and_lt_I4.h"
#include "cmp_ge_and_le_I4.h"

#include "cmp_lt_or_gt_I1.h"
#include "cmp_le_or_ge_I1.h"
#include "cmp_gt_and_lt_I1.h"
#include "cmp_ge_and_le_I1.h"

#include "cmp_lt_or_gt_I2.h"
#include "cmp_le_or_ge_I2.h"
#include "cmp_gt_and_lt_I2.h"
#include "cmp_ge_and_le_I2.h"

#include "f1opf2_cum.h"

#include "assign_I1.h"
#include "sort_asc_I8.h"
#include "sort_asc_F8.h"

// last review 9/7/2013
/*---------------------------------------------------------------*/
/* START FUNC DECL  */
int 
vec_f1s1opf2(
	     long long nR,
	     FLD_TYPE f1_fldtype,
	     char *f1_X,
	     char *nn_f1_X,
	     char *str_scalar,
	     char *op,
	     char *f2_X,
	     char *nn_f2_X,
	     FLD_TYPE f2_fldtype
	     )
/* STOP FUNC DECL  */
{
  int status = 0;
  int nT; long long block_size;
  long long svals_I8[MAX_NUM_SCALARS]; 
  int       svals_I4[MAX_NUM_SCALARS]; 
  short     svals_I2[MAX_NUM_SCALARS]; 
  char      svals_I1[MAX_NUM_SCALARS]; 
  float     svals_F4[MAX_NUM_SCALARS]; 
  double    svals_F8[MAX_NUM_SCALARS]; 
  long long sval_I8 = LLONG_MAX;
  int       sval_I4 = INT_MAX;
  short     sval_I2 = SHRT_MAX;
  char      sval_I1 = SCHAR_MAX;
  float     sval_F4 = FLT_MAX;
  double    sval_F8 = DBL_MAX;
  int num_scalars = -1;

  char      *hashes_I1 = NULL;
  short     *hashes_I2 = NULL;
  int       *hashes_I4 = NULL;
  long long *hashes_I8 = NULL;
  unsigned int n = 0; // size of hash table 
  unsigned int a = 0, b = 0; // coefficients for hash function

  status =  break_into_scalars(str_scalar, f1_fldtype, &num_scalars,
	&sval_I8, &sval_I4, &sval_I2, &sval_I1, &sval_F4, &sval_F8,
	svals_I8, svals_I4, svals_I2, svals_I1, svals_F4, svals_F8,
	&hashes_I1, &hashes_I2, &hashes_I4, &hashes_I8, &n, &a, &b);
  cBYE(status);
  /*-------------------------------------------- */
  status = partition(nR, 8192, -1, &block_size, &nT); cBYE(status);
  // fprintf(stderr, "Number of partitions = %d   \n", nT);
  // fprintf(stderr, "block_size           = %lld \n", block_size);
  // fprintf(stderr, "%d\n", nT);
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP

    long long lb = (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = ub - lb; 
    // fprintf(stderr, "%d,%lld,%lld\n", tid,lb, ub);

    char      *f1I1 = NULL, *opI1 = NULL; 
    short     *f1I2 = NULL, *opI2 = NULL;
    int       *f1I4 = NULL, *opI4 = NULL;
    long long *f1I8 = NULL, *opI8 = NULL;
    float     *f1F4 = NULL, *opF4 = NULL;
    double    *f1F8 = NULL, *opF8 = NULL;
    unsigned char       *f1UI1 = NULL, *opUI1 = NULL;
    unsigned int        *f1UI4 = NULL, *opUI4 = NULL;
    unsigned long long  *f1UI8 = NULL, *opUI8 = NULL;
    // Handle  nn field if needed 
    if ( nn_f2_X != NULL ) { 
#ifdef IPP
      ippsCopy_8u(nn_f1_X+lb, nn_f2_X+lb, nX); 
#else
      assign_I1(nn_f2_X+lb, nn_f1_X+lb, nX); 
#endif
    }
    char *tmp_ptr = NULL; long long nXB = 0;
    switch ( f1_fldtype ) {
      case I2 : 
      f1I2 = (short *)f1_X; f1I2 += lb;
      opI1 = (char  *)f2_X; opI1 += lb;
      opI2 = (short *)f2_X; opI2 += lb;
      if ( strcmp(op, "==") == 0 ) {
	if ( num_scalars == 1 ) { 
	  cmp_eq_scalar_I2(f1I2, sval_I2, nX, opI1);
	}
	else {
	  cmp_eq_mult_scalar_I2(f1I2, nX, svals_I2, num_scalars, 
	      hashes_I2, n, a, b, opI1);
	}
      }
      else if ( strcmp(op, "!=") == 0 ) {
	if ( num_scalars == 1 ) { 
	  cmp_ne_scalar_I2(f1I2, sval_I2, nX, opI1);
	}
	else {
	  dbg_cmp_neq_mult_scalar_I2(f1I2, nX, svals_I2, num_scalars, opI1);
	}
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_scalar_I2(f1I2, sval_I2, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_scalar_I2(f1I2, sval_I2, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_scalar_I2(f1I2, sval_I2, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_scalar_I2(f1I2, sval_I2, nX, opI1);
      }
      else if ( strcmp(op, "<||>") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_lt_or_gt_I2(f1I2, nX, svals_I2[0], svals_I2[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "<=||>=") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_le_or_ge_I2(f1I2, nX, svals_I2[0], svals_I2[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">&&<") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_gt_and_lt_I2(f1I2, nX, svals_I2[0], svals_I2[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">=&&<=") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_ge_and_le_I2(f1I2, nX, svals_I2[0], svals_I2[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "+") == 0 ) {
	add_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else { if ( status < 0 ) { WHEREAMI; } status = -1; continue; }
      break;
      case  I4  : 
      f1I4  = (int          *)f1_X; f1I4  += lb;
      f1UI4 = (unsigned int *)f1_X; f1UI4 += lb;
      opI4  = (int          *)f2_X; opI4  += lb;
      opI1  = (char         *)f2_X; opI1  += lb;
      opUI4 = (unsigned int *)f2_X; opUI4 += lb;
      if ( strcmp(op, "+") == 0 ) {
#ifdef IPP
	ippsAddC_32s_Sfs(f1I4, sval_I4, opI4, (int)nX, 0); 
#else
	add_scalar_I4(f1I4, sval_I4, nX, opI4);
#endif
      }
      else if ( strcmp(op, "-") == 0 ) {
#ifdef IPP
	ippsSubC_32s_Sfs(f1I4, sval_I4, opI4, (int)nX, 0); 
#else
	sub_scalar_I4(f1I4, sval_I4, nX, opI4);
#endif
      }
      else if ( strcmp(op, "*") == 0 ) {
#ifdef IPP
	ippsMulC_32s_Sfs(f1I4, sval_I4, opI4, (int)nX, 0); 
#else
	mul_scalar_I4(f1I4, sval_I4, nX, opI4);
#endif
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "<<") == 0 ) {
#ifdef IPP
	ippsLShiftC_32s(f1I4, sval_I4, opI4, nX);
#else
	shift_left_scalar_I4(f1I4, sval_I4, nX, opI4);
#endif
      }
      else if ( strcmp(op, ">>") == 0 ) {
	shift_right_scalar_UI4(f1UI4, sval_I4, nX, opUI4);
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "<||>") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_lt_or_gt_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "<=||>=") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_le_or_ge_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">&&<") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_gt_and_lt_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">=&&<=") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_ge_and_le_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "==") == 0 ) {
	if ( num_scalars == 1 ) { 
	  cmp_eq_scalar_I4(f1I4, sval_I4, nX, opI1);
	}
	else {
	  cmp_eq_mult_scalar_I4(f1I4, nX, svals_I4, num_scalars, 
	      hashes_I4, n, a, b, opI1);
	}
      }
      else if ( strcmp(op, "!=") == 0 ) {
	if ( num_scalars == 1 ) { 
	  cmp_ne_scalar_I4(f1I4, sval_I4, nX, opI1);
	}
	else {
	  dbg_cmp_neq_mult_scalar_I4(f1I4, nX, svals_I4, num_scalars, opI1);
	}
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "&") == 0 ) {
	bwise_and_scalar_I4(f1UI4, (unsigned int)sval_I4, nX, opUI4);
      }
      else if ( strcmp(op, "|") == 0 ) {
	bwise_or_scalar_I4(f1UI4, (unsigned int)sval_I4, nX, opUI4);
      }
      else if ( strcmp(op, "^") == 0 ) {
	bwise_xor_scalar_I4(f1UI4, (unsigned int)sval_I4, nX, opUI4);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
      break;
      case I8 : 
      f1I8  = (long long          *)f1_X; f1I8  += lb;
      f1UI8 = (unsigned long long *)f1_X; f1UI8 += lb;

      opUI8 = (unsigned long long *)f2_X; opUI8 += lb;
      opI8  = (long long          *)f2_X; opI8  += lb;
      opI1  = (char               *)f2_X; opI1  += lb;
      if ( strcmp(op, "+") == 0 ) {
	add_scalar_I8(f1I8, sval_I8, nX, opI8); 
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_I8(f1I8, sval_I8, nX, opI8); 
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_I8(f1I8, sval_I8, nX, opI8); 
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_I8(f1I8, sval_I8, nX, opI8); 
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem_scalar_I8(f1I8, sval_I8, nX, opI8); 
      }
      else if ( strcmp(op, "<<") == 0 ) {
	shift_left_scalar_I8(f1I8, sval_I8, nX, opI8); 
      }
      else if ( strcmp(op, ">>") == 0 ) {
	shift_right_scalar_UI8(f1UI8, sval_I8, nX, opUI8); 
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_scalar_I8(f1I8, sval_I8, nX, opI1); 
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_scalar_I8(f1I8, sval_I8, nX, opI1); 
      }
      else if ( strcmp(op, "==") == 0 ) {
	cmp_eq_scalar_I8(f1I8, sval_I8, nX, opI1); 
      }
      else if ( strcmp(op, "!=") == 0 ) {
	cmp_ne_scalar_I8(f1I8, sval_I8, nX, opI1); 
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_scalar_I8(f1I8, sval_I8, nX, opI1); 
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_scalar_I8(f1I8, sval_I8, nX, opI1); 
      }
      else if ( strcmp(op, "&") == 0 ) {
	bwise_and_scalar_I8(f1UI8, sval_I8, nX, opUI8); 
      }
      else if ( strcmp(op, "|") == 0 ) {
	bwise_or_scalar_I8(f1UI8, sval_I8, nX, opUI8); 
      }
      else if ( strcmp(op, "^") == 0 ) {
	bwise_xor_scalar_I8(f1UI8, sval_I8, nX, opUI8); 
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
      break;
      case F4 : 
      f1F4 = (float *)f1_X; f1F4 += lb;
      opF4 = (float *)f2_X; opF4 += lb;
      opI1 = (char  *)f2_X; opI1 += lb;
      if ( strcmp(op, "+") == 0 ) {
#ifdef IPP
	ippsAddC_32f(f1F4,  sval_F4, opF4, nX);
#else
	add_scalar_F4(f1F4, sval_F4, nX, opF4);
#endif
      }
      else if ( strcmp(op, "-") == 0 ) {
#ifdef IPP
	ippsSubC_32f(f1F4,  sval_F4, opF4, nX);
#else
	sub_scalar_F4(f1F4, sval_F4, nX, opF4);
#endif
      }
      else if ( strcmp(op, "*") == 0 ) {
#ifdef IPP
	ippsMulC_32f(f1F4,  sval_F4, opF4, nX);
#else
	mul_scalar_F4(f1F4, sval_F4, nX, opF4);
#endif
      }
      else if ( strcmp(op, "/") == 0 ) {
#ifdef IPP
	ippsDivC_32f(f1F4,  sval_F4, opF4, nX);
#else
	div_scalar_F4(f1F4, sval_F4, nX, opF4);
#endif
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "==") == 0 ) {
	cmp_eq_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "!=") == 0 ) {
	cmp_ne_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
      break;
      case F8 : 
      f1F8 = (double *)f1_X; f1F8 += lb;
      opF8 = (double *)f2_X; opF8 += lb;
      opI1 = (char   *)f2_X; opI1 += lb;
      if ( strcmp(op, "+") == 0 ) {
#ifdef IPP
	ippsAddC_64f(f1F8,  sval_F8, opF8, nX);
#else
	add_scalar_F8(f1F8, sval_F8, nX, opF8);
#endif
      }
      else if ( strcmp(op, "-") == 0 ) {
#ifdef IPP
	ippsSubC_64f(f1F8,  sval_F8, opF8, nX);
#else
	sub_scalar_F8(f1F8, sval_F8, nX, opF8);
#endif
      }
      else if ( strcmp(op, "*") == 0 ) {
#ifdef IPP
	ippsMulC_64f(f1F8,  sval_F8, opF8, nX);
#else
	mul_scalar_F8(f1F8, sval_F8, nX, opF8);
#endif
      }
      else if ( strcmp(op, "/") == 0 ) {
#ifdef IPP
	ippsDivC_64f(f1F8,  sval_F8, opF8, nX);
#else
	div_scalar_F8(f1F8, sval_F8, nX, opF8);
#endif
      }

      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "==") == 0 ) {
	cmp_eq_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "!=") == 0 ) {
	cmp_ne_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
      break;
      case B : 
      // 8 bits in a byte 
      if ( ( ( lb / 8 ) * 8 ) != lb ) { status = -1; continue; }
      tmp_ptr = f1_X; tmp_ptr += (lb/8); f1UI8 = (unsigned long long *)tmp_ptr;
      tmp_ptr = f2_X; tmp_ptr += (lb/8); opUI8 = (unsigned long long *)tmp_ptr;
      long long tempI8;
      nXB = nX / 64;  // 64 bits in an unsigned long long
      status = stoI8(str_scalar, &tempI8); 
      if ( strcmp(op, "==") == 0 ) {
	if ( status < 0 ) { break; }
	switch ( tempI8 ) { 
	  case 1 : 
	    bwise_and_scalar_I8(f1UI8, 0xFFFFFFFFFFFFFFFF, nXB, opUI8);
	    break;
	  case 0 : 
	    bwise_not_or_scalar_I8(f1UI8, 0, nXB, opUI8);
	    break;
	  default : 
	    status = -1;
	    break;
	}
      }
      else if ( strcmp(op, "!=") == 0 ) {
	if ( status < 0 ) { break; }
	switch ( tempI8 ) { 
	  case 1 : 
	    bwise_not_and_scalar_I8(f1UI8, 0, nXB, opUI8);
	    break;
	  case 0 : 
	    bwise_or_scalar_I8(f1UI8, 0, nXB, opUI8);
	    break;
	  default : 
	    status = -1;
	    break;
	}
      }
      else {
	status = -1; continue;
      }
      break;

      case I1 : 
      f1I1 = (char *)f1_X; f1I1 += lb;
      opI1 = (char *)f2_X; opI1 += lb;

      f1UI1 = (unsigned char *)f1_X; f1UI1 += lb;
      opUI1 = (unsigned char *)f2_X; opUI1 += lb;
      if ( strcmp(op, "+") == 0 ) {
	add_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "&") == 0 ) {
	bwise_and_scalar_I1(f1UI1, (unsigned char)sval_I1, nX, opUI1);
      }
      else if ( strcmp(op, "|") == 0 ) {
	bwise_or_scalar_I1(f1UI1, (unsigned char)sval_I1, nX, opUI1);
      }
      else if ( strcmp(op, "^") == 0 ) {
	bwise_xor_scalar_I1(f1UI1, (unsigned char)sval_I1, nX, opUI1);
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "<||>") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_lt_or_gt_I1(f1I1, nX, svals_I1[0], svals_I1[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "<=||>=") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_le_or_ge_I1(f1I1, nX, svals_I1[0], svals_I1[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">&&<") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_gt_and_lt_I1(f1I1, nX, svals_I1[0], svals_I1[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">=&&<=") == 0 ) {
	if ( num_scalars == 2 ) { 
	  cmp_ge_and_le_I1(f1I1, nX, svals_I1[0], svals_I1[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "==") == 0 ) {
	if ( num_scalars == 1 ) { 
	  cmp_eq_scalar_I1(f1I1, sval_I1, nX, opI1);
	}
	else {
	  cmp_eq_mult_scalar_I1(f1I1, nX, svals_I1, num_scalars, 
	      hashes_I1, n, a, b, opI1);
	}
      }
      else if ( strcmp(op, "!=") == 0 ) {
	if ( num_scalars == 1 ) { 
	  cmp_ne_scalar_I1(f1I1, sval_I1, nX, opI1);
	}
	else {
	  dbg_cmp_neq_mult_scalar_I1(f1I1, nX, svals_I1, num_scalars, opI1);
	}
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else { 
	status = -1; continue; 
      }
      break;
      default : 
      status = -1; continue;
    }
  }
  cBYE(status);
  /*--------------------------------------------*/
 BYE:
  free_if_non_null(hashes_I1);
  free_if_non_null(hashes_I2);
  free_if_non_null(hashes_I4);
  free_if_non_null(hashes_I8);
  return(status);
}
