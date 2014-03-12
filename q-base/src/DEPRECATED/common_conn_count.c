#include "qtypes.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "ext_f_to_s.h"
#include "common_conn_count.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "mmap.h"
#include "mk_temp_file.h"
#include "bin_search_I4.h"
#include "add_fld.h"
    	
// START FUNC DECL
int
find_common_conns(
		  int *X1,
		  int n1,
		  int *X2,
		  int n2,
		  int *ptr_n
		  )
// STOP FUNC DECL
{
  int status =  0;
  int n = 0, ctr1 = 0, ctr2 = 0;
  for ( ; (( ctr1 < n1 ) && ( ctr2 < n2 ) ) ; ) {
    int val1 = *X1;
    int val2 = *X2;
    if ( val1 == val2 ) {
      n++;
      X1++;
      X2++;
      ctr1++;
      ctr2++;
    }
    else if ( val1 < val2 ) {
      X1++;
      ctr1++;
    }
    else if ( val2 < val1 ) {
      X2++;
      ctr2++;
    }
    else {
      go_BYE(-1);
    }
  }
  *ptr_n = n;
 BYE:
  return(status);
}

// START FUNC DECL
int
int_common_conn_count(
		      int *TM_mid,
		      long long *TC_lb,
		      long long *TC_ub,
		      long long nTM,
		      int *TC_from,
		      int *TC_to,
		      int *TC_common_conns,
		      long long nTC
		      )
// STOP FUNC DECL
{
  int status = 0;

  if ( nTC < 1048576 ) { go_BYE(-1); } // need to bullet proof
  int nT = 1024;
  long long block_size = nTC / nT;
  int *max_TC_to = TC_to + nTC;

  cilkfor ( int tid = 0; tid < nT; tid++ ) {
    if ( status < 0 ) { continue; }
    long long lb = tid * block_size;
    long long ub = lb + block_size;
    if ( tid == (nT -1) ) { ub = nTC; }
    long long t_n_skip = 0;
    int skip1 = 0, skip2 = 0, skip3 = 0, skip4 = 0;
    for ( long long i = lb; i < ub; i++ ) {
      if ( status < 0 ) { continue; }
      int from; long long fromlb, fromub;
      int to;   long long tolb,   toub;
      int n = 0; long long frompos = -1, topos = -1;
      from = TC_from[i];
      to   = TC_to[i];
      bool skip = false;
	/* find lb/ub in TC for from */
	status = bin_search_I4(TM_mid, nTM, from, &frompos, ""); 
	if ( frompos < 0 ) { 
	  skip = true;
	  skip1++;
	}
	else {
	  fromlb = TC_lb[frompos];
	  fromub = TC_ub[frompos];
	}
	if ( skip == false ) {
	  /* find lb/ub in TC for to */
	  status = bin_search_I4(TM_mid, nTM, to, &topos, ""); 
	  if ( topos < 0 ) { 
	    skip = true;
	    skip3++;
	  }
	  else {
	    tolb = TC_lb[topos];
	    toub = TC_ub[topos];
	    if ( ( tolb <= 0 ) && ( toub <= 0 ) ) {
	      skip = true;
	    skip4++;
	    }
	  }
	}
	if ( skip == false ) {
	  if ( toub   <= tolb ) { status = -1; continue; }
	  if ( fromub <= fromlb ) { status = -1; continue; }
	  if ( ( fromlb < 0 ) || ( tolb < 0 ) )  { status = -1; continue; }
	  if ( ( fromub >= nTC ) || ( tolb >= nTC ) )  { status = -1; continue; }
	  if ( ( TC_to + fromlb ) >= max_TC_to ) { status = -1; continue; }
	  if ( ( TC_to + tolb   ) >= max_TC_to ) { status = -1; continue; }
	  status = find_common_conns(TC_to+fromlb, fromub-fromlb, 
				     TC_to+tolb, toub-tolb, &n);
	  if ( status < 0 ) { continue; }
	}
	else {
	  n = 0;
	}
      TC_common_conns[i] = n;
      if ( skip == true ) { t_n_skip++; }
    }
  }
  cBYE(status);

 BYE:
  return(status);
}

// START FUNC DECL
int
common_conn_count(
		  char *TM,
		  char *TM_mid,
		  char *TM_TC_lb,
		  char *TM_TC_ub,
		  char *TC,
		  char *TC_from,
		  char *TC_to,
		  char *TC_cnt /* count of number of common connections */
		  )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE TM_rec, TC_rec;
  long long nTM, nTC;
  int TM_id, TC_id;
#define BUFSZ 32
  char buffer[BUFSZ];
  FLD_REC_TYPE TM_mid_rec;   int TM_mid_id;
  FLD_REC_TYPE TM_TC_lb_rec; int TM_TC_lb_id;
  FLD_REC_TYPE TM_TC_ub_rec; int TM_TC_ub_id;
  FLD_REC_TYPE nn_fld_rec;   int nn_fld_id;
  FLD_REC_TYPE TC_from_rec;  int TC_from_id;
  FLD_REC_TYPE TC_to_rec;    int TC_to_id;
  FLD_REC_TYPE TC_cnt_rec;   int TC_cnt_id;
  char *TM_mid_X = NULL;   size_t TM_mid_nX = 0;
  char *TM_TC_lb_X = NULL; size_t TM_TC_lb_nX = 0;
  char *TM_TC_ub_X = NULL; size_t TM_TC_ub_nX = 0;
  char *TC_from_X = NULL;  size_t TC_from_nX = 0;
  char *TC_to_X = NULL;    size_t TC_to_nX = 0;
  char cnt_opfile[MAX_LEN_FILE_NAME+1];
  int cnt_ddir_id; 
  char *cnt_X = NULL; size_t cnt_nX = 0;

  if ( ( TM == NULL ) || ( *TM == '\0' ) ) { go_BYE(-1); }
  if ( ( TM_mid == NULL ) || ( *TM_mid == '\0' ) ) { go_BYE(-1); }
  if ( ( TM_TC_lb == NULL ) || ( *TM_TC_lb == '\0' ) ) { go_BYE(-1); }
  if ( ( TM_TC_ub == NULL ) || ( *TM_TC_ub == '\0' ) ) { go_BYE(-1); }
  if ( ( TC == NULL ) || ( *TC == '\0' ) ) { go_BYE(-1); }
  if ( ( TC_from == NULL ) || ( *TC_from == '\0' ) ) { go_BYE(-1); }
  if ( ( TC_to == NULL ) || ( *TC_to == '\0' ) ) { go_BYE(-1); }
  if ( ( TC_cnt == NULL ) || ( *TC_cnt == '\0' ) ) { go_BYE(-1); }
  zero_string(cnt_opfile, MAX_LEN_FILE_NAME+1);

  status = is_tbl(TM, &TM_id, &TM_rec); cBYE(status);
  if ( TM_id < 0 ) { go_BYE(-1); }
  nTM = TM_rec.nR;
  //---------------------------------------------------------------
  status = is_fld(NULL, TM_id, TM_mid, &TM_mid_id, &TM_mid_rec, 
		  &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( TM_mid_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( TM_mid_rec.fldtype != I4 ) { go_BYE(-1); }
  status = ext_f_to_s(TM, TM_mid, "is_sorted", buffer, BUFSZ); cBYE(status);
  if ( strcmp(buffer, "ascending") != 0 ) { go_BYE(-1); }
  status = get_data(TM_mid_rec, &TM_mid_X, &TM_mid_nX, false); 
  //---------------------------------------------------------------
  status = is_fld(NULL, TM_id, TM_TC_lb, &TM_TC_lb_id, &TM_TC_lb_rec, 
		  &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( TM_TC_lb_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( TM_TC_lb_rec.fldtype != I8 ) { go_BYE(-1); }
  status = get_data(TM_TC_lb_rec, &TM_TC_lb_X, &TM_TC_lb_nX, false); 
  //---------------------------------------------------------------
  status = is_fld(NULL, TM_id, TM_TC_ub, &TM_TC_ub_id, &TM_TC_ub_rec, 
		  &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( TM_TC_ub_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( TM_TC_ub_rec.fldtype != I8 ) { go_BYE(-1); }
  status = get_data(TM_TC_ub_rec, &TM_TC_ub_X, &TM_TC_ub_nX, false); 
  //---------------------------------------------------------------
  status = is_tbl(TC, &TC_id, &TC_rec); cBYE(status);
  if ( TC_id < 0 ) { go_BYE(-1); }
  nTC = TC_rec.nR;
  //---------------------------------------------------------------
  status = is_fld(NULL, TC_id, TC_from, &TC_from_id, &TC_from_rec, 
		  &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( TC_from_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( TC_from_rec.fldtype != I4 ) { go_BYE(-1); }
  status = get_data(TC_from_rec, &TC_from_X, &TC_from_nX, false); 
  //---------------------------------------------------------------
  status = is_fld(NULL, TC_id, TC_to, &TC_to_id, &TC_to_rec, 
		  &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( TC_to_id < 0 ) { go_BYE(-1); }
  if ( nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( TC_to_rec.fldtype != I4 ) { go_BYE(-1); }
  status = get_data(TC_to_rec, &TC_to_X, &TC_to_nX, false); 
  //---------------------------------------------------------------
  // allocate space for output
  int fldsz; size_t filesz;
  status = get_fld_sz(I4, &fldsz); cBYE(status);
  filesz = fldsz * nTC;

  status = mk_temp_file(cnt_opfile, filesz, &cnt_ddir_id); cBYE(status);
  status = q_mmap(cnt_ddir_id, cnt_opfile, &cnt_X, &cnt_nX, true); cBYE(status);
  //---------------------------------------------------------------
  status = int_common_conn_count((int *)TM_mid_X, (long long *)TM_TC_lb_X, 
				 (long long *) TM_TC_ub_X, nTM, 
				 (int *)TC_from_X, (int *) TC_to_X, 
				 (int *)cnt_X, nTC); 
  cBYE(status);
  /* add fields */
  status = add_fld(TC_id, TC_cnt, cnt_ddir_id, cnt_opfile, I4, 
      -1, &TC_cnt_id, &TC_cnt_rec);
  cBYE(status);
 BYE:
  rs_munmap(cnt_X, cnt_nX);
  return(status);
}
