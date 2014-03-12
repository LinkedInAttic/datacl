#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "open_temp_file.h"
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
#include "find_conns.h"

/*
Arguments are
\be
\item \(T_1\)
\item \(f_{lb}\)
\item \(f_{ub}\)
\item \(T_2\)
\item \(f_2\)
\item \(T_3\)
\item \(f_3\)
\ee

Let \(T_1[i].f_lb = l, T_1[i].f_ub = u\).
\(\forall j: l < j < u, T_2[j].f_2 = k \Rightarrow T_3[k].f_3 \leftarrow 1\)

*/
  int
  core_find_conns(
      long long *flbI8, 
      long long *fubI8, 
      long long nR1, 
      char *nn_flb_X, 
      char *nn_fub_X, 
      int *f2I4, 
      long long nR2, 
      int *f3I4, 
      long long nR3
      )
  {
    int status = 0;
  for ( long long i = 0; i < nR1; i++ ) { 
    long long lbvalI8 = flbI8[i];
    long long ubvalI8 = fubI8[i];
    if ( ( nn_flb_X != NULL ) && ( nn_flb_X[i] == 0 ) ) { continue; }
#ifdef DEBUG
    if ( ( nn_flb_X == NULL ) && ( nn_fub_X != NULL ) ) { go_BYE(-1); }
    if ( ( nn_fub_X == NULL ) && ( nn_flb_X != NULL ) ) { go_BYE(-1); }
    if ( ( nn_flb_X != NULL ) && ( nn_fub_X != NULL ) && 
	( nn_flb_X[i] != nn_fub_X[i] ) ) { go_BYE(-1); }
    if ( ( lbvalI8 < 0 ) || ( lbvalI8 >= nR2 ) ) { go_BYE(-1); }
    if ( ( ubvalI8 < 0 ) || ( ubvalI8 >= nR2 ) ) { go_BYE(-1); }
    if ( lbvalI8 >= ubvalI8 ) { go_BYE(-1); }
#endif
    for ( long long j = lbvalI8; j < ubvalI8; j++ ) {
      int f3valI4 = f2I4[j];
#ifdef DEBUG
      if ( ( f3valI4 < 0 ) || ( f3valI4 >= nR3 ) ) { go_BYE(-1); }
#endif
      f3I4[f3valI4]++;
    }
  }
BYE:
  return(status);
  }

/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
find_conns(
	       char *t1,
	       char *flb,
	       char *fub,
	       char *t2,
	       char *f2,
	       char *t3,
	       char *f3
	       )
/* STOP FUNC DECL */
{
  int status = 0;
  char *flb_X = NULL; size_t flb_nX = 0;
  char *fub_X = NULL; size_t fub_nX = 0;
  char *nn_flb_X = NULL; size_t nn_flb_nX = 0;
  char *nn_fub_X = NULL; size_t nn_fub_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *f3_X = NULL; size_t f3_nX = 0;

  int t1_id = INT_MIN, t2_id = INT_MIN, t3_id = INT_MIN;
  int flb_id = INT_MIN, fub_id = INT_MIN, f2_id = INT_MIN, f3_id = INT_MIN;
  int nn_flb_id = INT_MIN, nn_fub_id = INT_MIN, nn_f2_id = INT_MIN, nn_f3_id = INT_MIN;

  TBL_REC_TYPE t1_rec, t2_rec, t3_rec;
  FLD_REC_TYPE flb_rec, nn_flb_rec;
  FLD_REC_TYPE fub_rec, nn_fub_rec;
  FLD_REC_TYPE f2_rec, nn_f2_rec;
  FLD_REC_TYPE f3_rec, nn_f3_rec;
  long long nR3;
  long long nR1 = INT_MIN, nR2 = INT_MIN; 
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  int ddir_id = INT_MAX;
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( flb == NULL ) || ( *flb == '\0' ) ) { go_BYE(-1); }
  if ( ( fub == NULL ) || ( *fub == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( t3 == NULL ) || ( *t3 == '\0' ) ) { go_BYE(-1); }
  if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }

  if ( strcmp(flb, fub) == 0 ) { go_BYE(-1); }
  if ( strcmp(t1, t3) == 0 ) { go_BYE(-1); }
  if ( strcmp(t2, t3) == 0 ) { go_BYE(-1); }

  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(nn_opfile, (MAX_LEN_FILE_NAME+1));
  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR;
  /*--------------------------------------------------------*/
  status=is_fld(NULL, t1_id, flb, &flb_id, &flb_rec, &nn_flb_id, &nn_flb_rec); 
  cBYE(status);
  chk_range(flb_id, 0, g_n_fld);
  status = get_data(flb_rec, &flb_X, &flb_nX, false); cBYE(status);
  if ( nn_flb_id >= 0 ) { 
    status = get_data(nn_flb_rec, &nn_flb_X, &nn_flb_nX, false); cBYE(status);
  }
  if ( flb_rec.fldtype != I8 ) { go_BYE(-1); }
  long long *flbI8 = (long long *)flb_X;
  /*--------------------------------------------------------*/
  status=is_fld(NULL, t1_id, fub, &fub_id, &fub_rec, &nn_fub_id, &nn_fub_rec); 
  cBYE(status);
  chk_range(fub_id, 0, g_n_fld);
  status = get_data(fub_rec, &fub_X, &fub_nX, false); cBYE(status);
  if ( nn_fub_id >= 0 ) { 
    status = get_data(nn_fub_rec, &nn_fub_X, &nn_fub_nX, false); cBYE(status);
  }
  if ( fub_rec.fldtype != I8 ) { go_BYE(-1); }
  long long *fubI8 = (long long *)fub_X;
  /*--------------------------------------------------------*/
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbls[t2_id].nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec); 
  cBYE(status);
  chk_range(f2_id, 0, g_n_fld);
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  if ( nn_f2_id >= 0 ) { go_BYE(-1); }
  if ( f2_rec.fldtype != I4 ) { go_BYE(-1); }
  int *f2I4 = (int *)f2_X;
  /*--------------------------------------------------------*/
  status = is_tbl(t3, &t3_id, &t3_rec); cBYE(status);
  chk_range(t3_id, 0, g_n_tbl);
  nR3 = g_tbls[t3_id].nR;
  /*--------------------------------------------------------*/
  /* Make space for output if necessary */
  int *f3I4 = NULL;
  status = is_fld(NULL, t3_id, f3, &f3_id, &f3_rec, &nn_f3_id, &nn_f3_rec); 
  cBYE(status);
  if ( f3_id >= 0 ) {
    status = get_data(f3_rec, &f3_X, &f3_nX, true); cBYE(status);
    if ( nn_f3_id >= 0 ) { go_BYE(-1); }
    f3I4 = (int *)f3_X;
  }
  else {
    status = mk_temp_file(opfile, nR3 * sizeof(int), &ddir_id);
    cBYE(status);
    status = q_mmap(ddir_id, opfile, &f3_X, &f3_nX, true); cBYE(status);
    f3I4 = (int *)f3_X;
#ifdef IPP
    ippsSet_32s(0, f3I4, nR3); // TODO: P3: cilkfor?
#else
    assign_const_I4(f3I4, nR3, 0); 
#endif
    status = add_fld(t3_id, f3, ddir_id, opfile, I4, -1, &f3_id, &f3_rec); cBYE(status);
  }
  core_find_conns(flbI8, fubI8, nR1, nn_flb_X, nn_fub_X, f2I4, nR2, f3I4, nR3);

  rs_munmap(f3_X, f3_nX);
  /*-----------------------------------------------------------*/
 BYE:
  rs_munmap(flb_X, flb_nX);
  rs_munmap(fub_X, fub_nX);
  rs_munmap(nn_flb_X, nn_flb_nX);
  rs_munmap(nn_fub_X, nn_fub_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(f3_X, f3_nX);
  return(status);
}
