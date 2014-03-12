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
#include "lkp_sort.h"

// last review 9/22/2013
/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
lkp_sort(
	char *t1,
	char *f1,
	char *t2,
	char *cnt, /* t2[i] is the number of times t1.f1 has value i */
	/* Hence, t1.f1 < num_rows(t2) */
	char *idx_f1,
	char *srt_f1
	)
/* STOP FUNC DECL */
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *srt_f1_X = NULL; size_t srt_f1_nX = 0;
  char *idx_f1_X = NULL; size_t idx_f1_nX = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;

  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int cnt_id = INT_MIN, nn_cnt_id = INT_MIN;
  int srt_f1_id = INT_MIN;
  int idx_f1_id = INT_MIN;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE cnt_rec, nn_cnt_rec;
  FLD_REC_TYPE srt_f1_rec, idx_f1_rec;
  FLD_TYPE srt_fldtype, idx_fldtype; 
  long long nR1, nR2; 
  long long *offsets = NULL; bool mk_idx;
  long long *idx_f1I8 = NULL;
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt == NULL ) || ( *cnt == '\0' ) ) { go_BYE(-1); }
  if ( ( srt_f1 == NULL ) || ( *srt_f1 == '\0' ) ) { go_BYE(-1); }
  if   ( idx_f1 == NULL ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  if ( strcmp(f1, srt_f1) == 0 ) { go_BYE(-1); }
  if ( strcmp(f1, idx_f1) == 0 ) { go_BYE(-1); }
  if ( strcmp(srt_f1, idx_f1) == 0 ) { go_BYE(-1); }
  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  if ( t1_id < 0 ) { go_BYE(-1); }
  nR1 = t1_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  /* Restrictions of current implementation */
  if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  int *f1I4 = (int *)f1_X;
  /*--------------------------------------------------------*/
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  if ( t2_id < 0 ) { go_BYE(-1); }
  nR2 = t2_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, cnt, &cnt_id, &cnt_rec, &nn_cnt_id, &nn_cnt_rec); 
  cBYE(status);
  if ( cnt_id < 0 ) { go_BYE(-1); }
  status = get_data(cnt_rec, &cnt_X, &cnt_nX, false); cBYE(status);
  if ( cnt_rec.fldtype != I8 ) { go_BYE(-1); }
  if ( nn_cnt_id >= 0 ) { go_BYE(-1); }
  long long *cntI8 = (long long *)cnt_X;
  /*--------------------------------------------------------*/
  /* Make output storage */
  int fldsz = 0; size_t filesz = 0;
  int srt_ddir_id = -1, srt_fileno = -1;
  int idx_ddir_id = -1, idx_fileno = -1;
  srt_fldtype = f1_rec.fldtype;
  status = get_fld_sz(srt_fldtype, &fldsz); cBYE(status);
  filesz = nR1 * fldsz; 
  status = mk_temp_file(filesz,  &srt_ddir_id, &srt_fileno); cBYE(status);
  status = q_mmap(srt_ddir_id, srt_fileno, &srt_f1_X, &srt_f1_nX, true); 
  cBYE(status);
  int *srt_f1I4 = (int *)srt_f1_X;

  if ( *idx_f1 != '\0' ) { 
    mk_idx = true;
    idx_fldtype = I8;
    status = get_fld_sz(idx_fldtype, &fldsz); cBYE(status);
    filesz = nR1 * fldsz; 
    status = mk_temp_file(filesz, &idx_ddir_id, &idx_fileno); cBYE(status);
    status = q_mmap(idx_ddir_id, idx_fileno, &idx_f1_X, &idx_f1_nX, true); 
    cBYE(status);
    idx_f1I8 = (long long *)idx_f1_X;
  }
  else {
    mk_idx = false;
  }
  /*--------------------------------------------------------*/

  offsets = malloc(nR2 * sizeof(long long));
  return_if_malloc_failed(offsets);
  offsets[0] = 0;
  for ( int i = 1; i < nR2; i++ ) { offsets[i] = offsets[i-1] + cntI8[i-1] ; } 
  //--------------------------------------------------------------
  for ( long long i = 0; i < nR1; i++ ) { 
    int I4val = f1I4[i];
    if ( ( I4val < 0 ) || ( I4val >= nR2 ) ) { go_BYE(-1); }
    int offset = offsets[I4val];
    if ( ( offset < 0 ) || ( offset >= nR1 ) ) { go_BYE(-1); }
    srt_f1I4[offset] = I4val;
    if ( mk_idx == true ) {
      idx_f1I8[offset] = i;
    }
    offsets[I4val] = offset + 1;
  }
  /*-----------------------------------------------------------*/
  rs_munmap(srt_f1_X, srt_f1_nX);
  rs_munmap(idx_f1_X, idx_f1_nX);
  zero_fld_rec(&srt_f1_rec); srt_f1_rec.fldtype = srt_fldtype;
  status = add_fld(t1_id, srt_f1, srt_ddir_id, srt_fileno, &srt_f1_id, &srt_f1_rec);
  cBYE(status);
  if ( mk_idx == true ) { 
    zero_fld_rec(&idx_f1_rec); idx_f1_rec.fldtype = idx_fldtype;
    status = add_fld(t1_id, idx_f1, idx_ddir_id, idx_fileno, &idx_f1_id, &idx_f1_rec);
    cBYE(status);
  }
  /*-----------------------------------------------------------*/
 BYE:
  free_if_non_null(offsets);
  rs_munmap(f1_X, f1_nX);
  rs_munmap(srt_f1_X, srt_f1_nX);
  rs_munmap(idx_f1_X, idx_f1_nX);
  rs_munmap(cnt_X, cnt_nX);
  return(status);
}
