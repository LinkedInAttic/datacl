// MAC #include <sys/sysinfo.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "aux_meta.h"
#include "is_fld.h"
#include "is_a_in_b.h"
// #include "is_A_in_b.h"
#include "dir_is_a_in_b.h"
#include "rename_tbl.h"
#include "rename_fld.h"
#include "meta_data.h"
#include "list_files.h"
// TODO P1 #include "orphan_files.h"
#include "list_tbls.h"
#include "logger.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "del_tbl.h"
#include "del_fld.h"
#include "xfer.h"
#include "s_to_f.h"
#include "f_to_s.h"
#include "f1f2_to_s.h"
#include "regex_match.h"
#include "f1s1opf2.h"
#include "f1opf2.h"
#include "f1opf2f3.h"
#include "f1f2opf3.h"
#include "copy_tbl.h"
#include "copy_fld.h"
#include "copy_fld_ranges.h"
#include "mv_fld.h"
#include "mk_nn_fld.h"
#include "break_nn_fld.h"
#include "bindmp.h"
#include "binld.h"
#include "pr_fld.h"
#include "tbl_meta.h"
#include "dump.h"
#include "dld.h"
#include "fld_meta.h"
#include "set_meta.h"
#include "unset_meta.h"
#include "get_meta.h"
#include "is_aux_fld.h"
#include "dup_fld.h"
#include "get_nR.h"
#include "drop_aux_fld.h"
#include "fop.h"
#include "sortf1f2.h"
#include "count_ht.h"
#include "count.h"
#include "countf.h"
#include "lkp_sort.h"
#include "rng_sort.h"
#include "t1f1t2f2opt3f3.h"
#include "srt_join.h"
#include "rng_join.h"
#include "import.h"
#include "subsample.h"
#include "mk_bins.h"
#include "count_vals.h"
#include "app_tbl.h"
#include "set_val.h"
#include "get_val.h"
#include "num_in_range.h"
#include "wisifxthenyelsez.h"
#include "flush.h"
#include "stride.h"
#include "crossprod.h"
#include "pack.h"
#include "unpack.h"
// #include "udf_eq_B.h"
#include "udf_deg2.h"
#include "percentiles.h"
#include "patmatch.h"
// KISHORE
#include "top_n.h"
#include "approx_unique.h"
#include "approx_quantile.h"
#include "approx_frequent.h"
// COMPOUND
#include "compound.h"

// GPU stuff
#include "g_describe.h"
#include "g_load.h"
#include "g_print_reg_data.h"
#include "g_store.h"
#include "g_del_fld.h"
#include "g_add_fld.h"
#include "g_aux.h"
#include "g_f1s1opf2.h"
#include "g_f1f2opf3.h"
#include "g_f_to_s.h"
#include "g_f1f2_to_s.h"
#include "g_shift.h"
#include "g_count.h"
#include "g_countf.h"

extern char g_docroot[MAX_LEN_DIR_NAME+1];
extern char g_cwd[MAX_LEN_DIR_NAME+1];
extern FILE *g_ofp;


// START FUNC DECL
int
q_core(
       int argc, 
       char **argv, 
       char *rslt_buf, 
       int sz_rslt_buf
       )
// STOP FUNC DECL
{
  int status = 0;
  int    tbl_id = -1; TBL_REC_TYPE tbl_rec;
  int    fld_id = -1; FLD_REC_TYPE fld_rec;
  int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;
  bool b_is_tbl, b_is_fld;
  long long nR;
  FILE *l_ofp = NULL;
  COMP_EXPR_TYPE *comp_expr = NULL; int n_comp_expr = 0;
  unsigned long long t_before; 

  log_request(argc, argv, &t_before);
  bool is_request_logged = true;
  // START: ACCESSING META DATA 
  if ( strcmp(argv[1], "list_files") == 0 ) {  
    if ( argc != 3 ) { go_BYE(-1); }
    status = list_files(argv[2]); cBYE(status);
  }
  else if ( strcmp(argv[1], "dump") == 0 ) {  
    if ( ( argc != 5 ) && ( argc != 6 ) ) { go_BYE(-1); }
    if ( argc == 5 ) { 
      status = dump(argv[2], argv[3], argv[4], "" ); cBYE(status);
    }
    else {
      status = dump(argv[2], argv[3], argv[4], argv[5] ); cBYE(status);
    }
  }
  else if ( strcmp(argv[1], "list_tbls") == 0 ) {  
    if ( argc != 2 ) { go_BYE(-1); }
    status = list_tbls(); cBYE(status);
  }
  /* TODO P1 
     else if ( strcmp(argv[1], "orphan_files") == 0 ) {  
     if ( argc != 3 ) { go_BYE(-1); }
     status = orphan_files(argv[2]); cBYE(status);
     }
  */
  else if ( strcmp(argv[1], "set_ddir") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = set_ddir(argv[2], argv[3]); cBYE(status);
  }
  else if ( strcmp(argv[1], "get_ddir") == 0 ) {  
    if ( argc != 3 ) { go_BYE(-1); }
    char ddir[MAX_LEN_DIR_NAME+1]; 
    zero_string(ddir, (MAX_LEN_DIR_NAME+1));
    status = get_ddir(argv[2], ddir, MAX_LEN_DIR_NAME); cBYE(status);
    snprintf(rslt_buf, sz_rslt_buf, "%s", ddir); 
  }
  else if ( strcmp(argv[1], "is_ddir") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    int ddir_id;
    status = is_ddir(argv[2], &ddir_id); cBYE(status);
    if ( ddir_id < 0 ) { 
      snprintf(rslt_buf, sz_rslt_buf, "-1:0"); 
    }
    else {
      snprintf(rslt_buf, sz_rslt_buf, "1:%d", ddir_id);
    }
  }
  else if ( strcmp(argv[1], "is_tbl") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    status = is_tbl(argv[2], &tbl_id, &tbl_rec); cBYE(status);
    if ( tbl_id < 0 ) { b_is_tbl = false; } else { b_is_tbl = true; }
    snprintf(rslt_buf, sz_rslt_buf, "%d:%d\n", b_is_tbl, tbl_id);
  }
  else if ( strcmp(argv[1], "is_fld") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = is_fld( argv[2], -1, argv[3], 
		     &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
    cBYE(status);
    if ( fld_id < 0 ) { 
      b_is_fld = false; 
    } 
    else { 
      b_is_fld = true; 
    }
    snprintf(rslt_buf, sz_rslt_buf, "%d:%d\n", b_is_fld, fld_id);
  }
  else if ( strcmp(argv[1], "is_nn_fld") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = is_aux_fld(argv[2], argv[3], "nn", &fld_id, rslt_buf); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "is_aux_fld") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = is_aux_fld(argv[2], argv[3], argv[4], &fld_id, rslt_buf); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "describe") == 0 ) {  
    switch ( argc ) { 
    case 3 : status = tbl_meta(argv[2], false, NULL, NULL);  cBYE(status); break;
    case 4 : status = fld_meta(argv[2], argv[3], "", &fld_id, true);  cBYE(status); break;
    case 5 : status = fld_meta(argv[2], argv[3], argv[4], &fld_id, true);  cBYE(status); break;
    default : go_BYE(-1); break;
    }
  }
  else if ( strcmp(argv[1], "file_to_fld") == 0 ) {  
    if ( argc != 3 ) { go_BYE(-1); }
    status = file_to_fld(argv[2], &fld_id, rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "get_nR") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    status = get_nR(argv[2], &nR); cBYE(status);
    snprintf(rslt_buf, sz_rslt_buf, "%lld\n", nR);
  }
  // STOP: ACCESSING META DATA 
  // START: MODIFYING META DATA 
  else if ( strcmp(argv[1], "add_tbl") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = add_tbl(argv[2], argv[3], &tbl_id, &tbl_rec); cBYE(status);
  }
  else if ( strcmp(argv[1], "add_fld") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = ext_add_fld(argv[2], argv[3], argv[4]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "dup_fld") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = dup_fld(argv[2], argv[3], argv[4]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "delete") == 0 ) {  
    switch ( argc ) { 
    case 3 : status = iter_del_tbl(argv[2]); cBYE(status); break;
    case 4 : status = iter_del_fld(argv[2], argv[3]); cBYE(status); break;
    default : go_BYE(-1); break;
    }
  }
  else if ( strcmp(argv[1], "rename") == 0 ) {  
    switch ( argc ) { 
    case 4 : status = rename_tbl(argv[2], argv[3]); cBYE(status); break;
    case 5 : status = rename_fld(argv[2], argv[3], argv[4]); cBYE(status); break;
    default : go_BYE(-1); break;
    }
  }
  else if ( strcmp(argv[1], "drop_nn_fld") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = drop_aux_fld( argv[2], argv[3], "nn", -1); cBYE(status);
  }
  else if ( strcmp(argv[1], "drop_aux_fld") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = drop_aux_fld( argv[2], argv[3], argv[4], -1); cBYE(status);
  }
  else if ( strcmp(argv[1], "mk_nn_fld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = mk_nn_fld(argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "break_nn_fld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = break_nn_fld(argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "dld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = dld(argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "set_meta") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = set_meta(argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "unset_meta") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = unset_meta(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "get_meta") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = get_meta(argv[2], argv[3], argv[4], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "mk_idx") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    char buffer[1024];
    sprintf(buffer, "op=[seq]:start=[0]:incr=[1]:fldtype=[%s]", argv[4]);
    status = s_to_f(argv[2], argv[3], buffer); cBYE(status);
  }
  else if ( strcmp(argv[1], "s_to_f") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = s_to_f(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "count_vals") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = count_vals(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "num_in_range") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = num_in_range(argv[2], argv[3], argv[4], argv[5], argv[6], 
			  argv[7], rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1opf2") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = f1opf2(argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "wisifxthenyelsez") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = wisifxthenyelsez(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "set_val") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = set_val(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "get_val") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = get_val(argv[2], argv[3], argv[4], rslt_buf, sz_rslt_buf); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "xfer") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = xfer(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "stride") == 0 ) {  
    if ( argc != 9 ) { go_BYE(-1); }
    status = ext_stride(argv[2], argv[3], argv[4], argv[5], argv[6], 
			argv[7], argv[8]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "subsample") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = subsample(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "pack") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = pack(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "unpack") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = unpack(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1s1opf2") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = f1s1opf2(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1f2opf3") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = f1f2opf3(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1opf2f3") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = f1opf2f3(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "top_n") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = ext_top_n(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "approx_unique") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    int is_good;
    long long num_uq_I8;
    double accuracy_F8;
    status = ext_approx_unique(argv[2], argv[3], argv[4], &num_uq_I8,
			       &accuracy_F8, &is_good);
    cBYE(status);
    snprintf(rslt_buf, sz_rslt_buf, "%lld:%lf:%d", num_uq_I8, accuracy_F8, is_good);
  }
  else if ( strcmp(argv[1], "approx_quantile") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    int is_good;
    long long num_uq_I8;
    double accuracy_F8;
    status = ext_approx_quantile(argv[2], argv[3], argv[4], argv[5], argv[6],
				 argv[7], &is_good);
    cBYE(status);
    snprintf(rslt_buf, sz_rslt_buf, "%d", is_good);
  }
  else if ( strcmp(argv[1], "approx_frequent") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    int is_good;
    long long num_uq_I8;
    double accuracy_F8;
    status = ext_approx_frequent(argv[2], argv[3], argv[4], argv[5], argv[6],
				 argv[7], &is_good);
    cBYE(status);
    snprintf(rslt_buf, sz_rslt_buf, "%d", is_good);
  }
  else if ( strcmp(argv[1], "mk_bins") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = mk_bins(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "copy_tbl") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = copy_tbl(argv[2], argv[3]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "copy_fld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = copy_fld(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "copy_fld_ranges") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = copy_fld_ranges(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "patmatch") == 0 ) {  
    if ( argc != 10 ) { go_BYE(-1); }
    status = patmatch(argv[2], argv[3], argv[4], argv[5], argv[6],
		      argv[7], argv[8], argv[9]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "regex_match") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = regex_match(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "count_ht") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = count_ht(argv[2], argv[3], argv[4], argv[5]);  cBYE(status);
  }
  else if ( strcmp(argv[1], "count") == 0 ) {  
    if ( ( argc != 7 ) && ( argc != 8 ) ) { go_BYE(-1); }
    if ( argc == 7 ) { 
      status = count(argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
    }
    else { 
      status = count(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    }
    cBYE(status);
  }
  else if ( strcmp(argv[1], "countf") == 0 ) {  
    if ( ( argc != 8 ) && ( argc != 9 ) ) { go_BYE(-1); }
    if ( argc == 8 ) {
      status = countf(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], "safe_mode=[true]");
    }
    else {
      status = countf(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]); 
    }
    cBYE(status);
  }
  else if ( strcmp(argv[1], "fop") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = fop(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "sortf1f2") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = sortf1f2(argv[2], argv[3], argv[4], argv[5]); cBYE(status);
  }
  else if ( strcmp(argv[1], "mv_fld") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = mv_fld(argv[2], argv[3], argv[4]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f_to_s") == 0 ) {  
    if ( argc == 5 ) { 
      status = f_to_s(argv[2], argv[3], argv[4], rslt_buf, sz_rslt_buf);
      cBYE(status);
    }
    else if ( argc == 6 ) { 
      if ( is_legal_env_var(argv[5]) == false ) { go_BYE(-1); }
      status = f_to_s(argv[2], argv[3], argv[4], rslt_buf, sz_rslt_buf);
      cBYE(status);
      sprintf(rslt_buf,"%s=%s", argv[5], rslt_buf);
      putenv(rslt_buf);
      zero_string(rslt_buf, sz_rslt_buf);
    }
  }
  else if ( strcmp(argv[1], "f1f2_to_s") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = f1f2_to_s(argv[2], argv[3], argv[4], argv[5], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "rng_join") == 0 ) { 
    if ( argc != 9 ) { go_BYE(-1); }
    status = rng_join(argv[2], argv[3], argv[4], argv[5], argv[6], 
		      argv[7], argv[8]);   
    cBYE(status);
  }
  else if ( strcmp(argv[1], "bindmp") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = bindmp(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "binld") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = binld(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "rng_sort") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = rng_sort(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "lkp_sort") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = lkp_sort(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "pr_fld") == 0 ) {  
    switch ( argc ) { 
    case 4 : 
      status = pr_fld(argv[2], argv[3], "", g_ofp);
      cBYE(status);
      break; 
    case 5 : 
      status = pr_fld(argv[2], argv[3], argv[4], g_ofp);
      cBYE(status);
      break; 
    case 6 : 
      if ( argv[5][0] == '\0' ) {
	status = pr_fld(argv[2], argv[3], argv[4], g_ofp);
	cBYE(status);
      }
      else {
	l_ofp = fopen(argv[5], "w");
	return_if_fopen_failed(l_ofp,  argv[5], "w");
	status = pr_fld(argv[2], argv[3], argv[4], l_ofp);
	fclose_if_non_null(l_ofp);
	cBYE(status);
      }
      break; 
    default : 
      go_BYE(-1);
      break;
    }
  }
  else if ( strcmp(argv[1], "crossprod") == 0 ) {  
    if ( argc != 11 ) { go_BYE(-1); }
    status = crossprod(argv[2], argv[3], argv[4], argv[5], argv[6], 
		       argv[7], argv[8], argv[9], argv[10]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "app_tbl") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = app_tbl(argv[2], argv[3]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "t1f1t2f2opt3f3") == 0 ) {  
    if ( argc != 10 ) { go_BYE(-1); }
    status = t1f1t2f2opt3f3(argv[2], argv[3], argv[4], argv[5], 
			    argv[6], argv[7], argv[8], argv[9]);
    cBYE(status);
  }
  /* TODO P0 WHY DID THIS DISAPPEAR 
     else if ( strcmp(argv[1], "is_A_in_b") == 0 ) {  
     if ( argc != 7 ) { go_BYE(-1); }
     status = is_A_in_b(argv[2], argv[3], argv[4], argv[5], argv[6]);
     cBYE(status);
     }
  */
  else if ( strcmp(argv[1], "percentiles") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = percentiles(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "is_a_in_b") == 0 ) {  
    if ( argc != 9 ) { go_BYE(-1); }
    status = is_a_in_b(argv[2], argv[3], argv[4], argv[5], argv[6], 
		       argv[7], argv[8]); 
    cBYE(status);
  }
   else if ( strcmp(argv[1], "udf_deg2") == 0 ) {  
   if ( argc != 11 ) { go_BYE(-1); }
   status = udf_deg2(argv[2], argv[3], argv[4], argv[5], 
                     argv[6], argv[7], argv[8], argv[9], argv[10]);
   cBYE(status);
   }
  //---------------------------------------------------------------------
  /* DO WE NEED UDF FOR EQ B ? TODO P2
     else if ( strcmp(argv[1], "udf_eq_B") == 0 ) {  
     if ( argc != 6 ) { go_BYE(-1); }
     status = udf_eq_B(argv[2], argv[3], argv[4], argv[5]);
     cBYE(status);
     }
  */
  else if ( strcmp(argv[1], "dir_is_a_in_b") == 0 ) {  
    if ( argc != 12 ) { go_BYE(-1); }
    status = dir_is_a_in_b(argv[2], argv[3], argv[4], argv[5], argv[6], 
			   argv[7], argv[8], argv[9], argv[10], argv[11]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "srt_join") == 0 ) {  
    if ( argc != 9 ) { go_BYE(-1); }
    status = srt_join(argv[2], argv[3], argv[4], argv[5], argv[6], 
		      argv[7], argv[8]);   
    cBYE(status);
  }
  /* TODO P1
     else if ( strcmp(argv[1], "import") == 0 ) {  
     if ( argc != 6 ) { go_BYE(-1); }
     status = import(argv[2], argv[3], argv[4], argv[5]);  cBYE(status);
     }
  */
  else if ( strcmp(argv[1], "g_describe") == 0 ) {  
    if ( argc != 3 ) { go_BYE(-1); }
    status = g_describe(argv[2], rslt_buf, sz_rslt_buf); cBYE(status);
  }
  else if ( strcmp(argv[1], "g_head") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = g_print_reg_data(argv[2], argv[3], "head", rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_tail") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = g_print_reg_data(argv[2], argv[3], "tail", rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_load") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = g_load(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "g_store") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = g_store(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "g_del_fld") == 0 ) {  
    if ( argc != 3 ) { go_BYE(-1); }
    status = g_del_fld(argv[2]); cBYE(status);
  }
  else if ( strcmp(argv[1], "g_add_fld") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = g_add_fld(argv[2], argv[3], argv[4]); cBYE(status);
  }
  else if ( strcmp(argv[1], "g_info") == 0 ) {  
    if ( argc != 2 ) { go_BYE(-1); }
    status = g_info(); cBYE(status);
  }
  else if ( strcmp(argv[1], "g_f1s1opf2") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = g_f1s1opf2(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_f1f2opf3") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = g_f1f2opf3(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_count") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = g_count(argv[2], argv[3], argv[4]); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_countf") == 0 ) {  
    /* TODO P4: Remember to deal with safe mode */
    if ( argc != 6 ) { go_BYE(-1); }
    status = g_countf(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_f_to_s") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = g_f_to_s(argv[2], argv[3], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_f1f2_to_s") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = g_f1f2_to_s(argv[2], argv[3], argv[4], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "g_shift") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = g_shift(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  /* TODO P2. Need to think through why we invented flush in the first place
     else if ( strcmp(argv[1], "flush") == 0 ) {  
     if ( argc != 5 ) { go_BYE(-1); }
     status = flush(argv[2], argv[3], argv[4]);
     cBYE(status);
     }
  */
  else if ( strcmp(argv[1], "no_op") == 0 ) {  
    // Nothing to do except log parameters
  }
  else if ( strcmp(argv[1], "stop_compound") == 0 ) {
    if ( argc != 4 ) { go_BYE(-1); }
    // If no file is provided that stores the compound expression, we
    // assume that it is "compound_expressions.txt" in q_docroot
    char full_name[2*(MAX_LEN_DIR_NAME+1)];
    if ( argv[3][0] == '\0' ) {
      if ( argv[2][0] != '\0' ) { go_BYE(-1); }
      strcpy(full_name, g_docroot);
      strcat(full_name, "/compound_expressions.txt");
      status = stop_compound_expr(); cBYE(status);
    }
    else {
      if ( argv[2][0] == '\0' ) { 
        sprintf(full_name, "%s/%s", g_cwd,   argv[3]);
      }
      else {
        sprintf(full_name, "%s/%s", argv[2], argv[3]);
      }
    }
    int tbl_id = -1; TBL_REC_TYPE tbl_rec;
    /* START: Process compound expression */
    /* First, let us check whether this is a valid compound expression */
    status = chk_comp_expr(full_name, &comp_expr, 
			   &n_comp_expr, &tbl_id, &tbl_rec); 
    if ( status < 0 ) { 
      release_comp_expr(&comp_expr, &n_comp_expr);  go_BYE(-1);
    }
    /* Now, let's execute it */
    status = exec_comp_expr(tbl_id, tbl_rec, comp_expr, n_comp_expr); 
    if ( status < 0 ) { WHEREAMI; }
    /* Release resources acquired */
    release_comp_expr(&comp_expr, &n_comp_expr); 
    /* Delete the compound expressions file */
    unlink(full_name);
    /* STOP: Process compound expression */
  }
  else {
    go_BYE(-1);
  }
  if ( getenv("Q_RUN_TIME_CHECKS") != NULL ) {
    if ( strcmp(getenv("Q_RUN_TIME_CHECKS"), "TRUE") == 0 ) {
      status = dump(NULL, NULL, NULL, ""); cBYE(status);
    }
  }
 BYE:
  if ( is_request_logged == true ) {
    log_response(status, t_before);
  }
  free_if_non_null(comp_expr);
  fclose_if_non_null(l_ofp);
  return(status);
}
