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
#include "add_tbl.h"
#include "del_tbl.h"
#include "meta_globals.h"
#include "t1f1opt2f2.h"
#include "extract_I8.h"
#include "extract_I4.h"
#include "extract_S.h"
#include "find_closest_I4.h"
#include "assign_I1.h"

/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
t1f1opt2f2(
	   char *t1,
	   char *f1,
	   char *str_op_spec,
	   char *t2,
	   char *f2
	   )
/* STOP FUNC DECL */
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN;
  int nn_f1_id = INT_MIN; // nn_f2_id = INT_MIN;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE f2_rec; // nn_f2_rec;
  long long nR1, nR2; 
  int lbI4, ubI4;
  bool is_null;
#define MAX_LEN_OP 32
  char op[MAX_LEN_OP];
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  char strbuf[128];
  int ddir_id = -1, fileno = -1;
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }

  zero_string(op, MAX_LEN_OP);
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(nn_opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(strbuf, 128);
  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = t1_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); cBYE(status);
  }
  /*--------------------------------------------------------*/
  /*--------------------------------------------------------*/
  status = extract_S(str_op_spec, "op=[", "]", op, MAX_LEN_OP, &is_null);
  if ( is_null ) { go_BYE(-1); }
  if ( strcmp(op, "copy_range") == 0 ) {
    status = del_tbl(t2, -1); cBYE(status);
    status = extract_I4(str_op_spec, "lb=[", "]", &lbI4, &is_null);
    if ( is_null ) { go_BYE(-1); }
    status = extract_I4(str_op_spec, "ub=[", "]", &ubI4, &is_null);
    if ( is_null ) { go_BYE(-1); }
    if ( lbI4 > ubI4 ) { go_BYE(-1); }
    /*--------------------------------------------------------*/
    /* Restrictions of current implementation */
    if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    /*--------------------------------------------------------*/
    long long lbidx = LLONG_MIN, ubidx = LLONG_MIN;
    status = find_closest_I4((int *)f1_X, nR1, lbI4, "lowest",  &lbidx); cBYE(status);
    if ( ( lbidx < 0 ) || ( lbidx >= nR1 ) ) { go_BYE(-1); }
    status = find_closest_I4((int *)f1_X, nR1, ubI4, "highest", &ubidx); cBYE(status);
    if ( ( ubidx < 0 ) || ( ubidx >= nR1 ) ) { go_BYE(-1); }

    int fldsz;
    status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);
    nR2 = ubidx - lbidx;
    if (nR2 <= 0 ) { go_BYE(-1); }
    status = mk_temp_file(nR2 * fldsz, &ddir_id, &fileno); cBYE(status);
    status = q_mmap(ddir_id, fileno, &f2_X,  &f2_nX, true); cBYE(status);
    memcpy(f2_X, f1_X + (lbidx * fldsz), (nR2 * fldsz));
    rs_munmap(f2_X, f2_nX);

    sprintf(strbuf, "%lld", nR2);
    status = add_tbl(t2, strbuf, &t2_id, &t2_rec); cBYE(status);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = f1_rec.fldtype;
    status = add_fld(t2_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
    cBYE(status);
    /* TODO: P2 Set meta data for sorttype */
  }
  else if ( strcmp(op, "idx_to_cfld") == 0 ) {
    status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
    chk_range(t2_id, 0, g_n_tbl);
    nR2 = t2_rec.nR;
    if ( nn_f1_id >= 0 ) { go_BYE(-1); } // NOT IMPLEMENTED

    // allocate space
    status = mk_temp_file(nR2 * sizeof(char), &ddir_id, &fileno); cBYE(status);
    status = q_mmap(ddir_id, fileno, &f2_X,  &f2_nX, true); cBYE(status);
#ifdef IPP
    ippsZeo_8u(f2_X, f2_nX);
#else
    assign_const_I1(f2_X, f2_nX, (char)0);
#endif

    int *f1I4 = (int *)f1_X;
    long long *f1I8 = (long long *)f1_X;
    switch (f1_rec.fldtype ) { 
    case I4 : 
      for ( long long i = 0; i < nR1; i++ ) { 
	int idx = f1I4[i];
	if ( ( idx < 0 ) || ( idx > nR2 ) ) { go_BYE(-1); }
	f2_X[idx] = 1;
      }
	break;
      case I8 : 
      for ( long long i = 0; i < nR1; i++ ) { 
	long long idx = f1I8[i];
	if ( ( idx < 0 ) || ( idx > nR2 ) ) { go_BYE(-1); }
	f2_X[idx] = 1;
      }
	break;
      default : 
	go_BYE(-1);
	break;
    }
    rs_munmap(f2_X, f2_nX);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = I1;
    status = add_fld(t2_id, f2, ddir_id, fileno, &f2_id, &f2_rec); cBYE(status);
  }
  else {
    go_BYE(-1);
  }

  /*-----------------------------------------------------------*/
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  return(status);
}
