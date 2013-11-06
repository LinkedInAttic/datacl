/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#include <sys/sysinfo.h>
#include "qtypes.h"
#include "mmap.h"
#include "is_tbl.h"
#include "aux_meta.h"
#include "is_fld.h"
#include "is_a_in_b.h"
#include "is_A_in_b.h"
#include "dir_is_a_in_b.h"
#include "rename_tbl.h"
#include "rename_fld.h"
#include "meta_data.h"
#include "list_files.h"
#include "list_tbls.h"
#include "logger.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "del_tbl.h"
#include "del_fld.h"
#include "xfer.h"
#include "ext_s_to_f.h"
#include "ext_f_to_s.h"
#include "f1f2_to_s.h"
#include "regex_match.h"
#include "f1s1opf2.h"
#include "f1T1opf2.h"
#include "f1opf2.h"
#include "f1opf2f3.h"
#include "f1f2opf3.h"
#include "copy_fld.h"
#include "mv_fld.h"
#include "mk_nn_fld.h"
#include "copy_range.h"
#include "range_to_cfld.h"
#include "cfld_to_range.h"
#include "bindmp.h"
#include "binld.h"
#include "pr_fld.h"
#include "tbl_meta.h"
#include "dump.h"
#include "dld.h"
#include "fld_meta.h"
#include "set_meta.h"
#include "get_meta.h"
#include "is_nn_fld.h"
#include "dup_fld.h"
#include "get_nR.h"
#include "set_nR.h"
#include "drop_nn_fld.h"
#include "fop.h"
#include "sortf1f2.h"
#include "count_ht.h"
#include "count.h"
#include "countf.h"
#include "countT.h"
#include "lkpsort.h"
#include "rng_sort.h"
#include "get_t2f2_val.h"
#include "t1f1t2f2opt3f3.h"
#include "srt_join.h"
#include "rng_join.h"
#include "subsample.h"
#include "mk_bins.h"
#include "count_vals.h"
#include "app_tbl.h"
#include "set_val.h"
#include "num_in_range.h"
#include "wisifxthenyelsez.h"
#include "stride.h"
#include "crossprod.h"
#include "pack.h"
#include "unpack.h"
#include "udf_eq_B.h"
//
// #include "pr_dict.h"
// #include "fastbit.h"

#include "orig_meta_globals.h"

int g_num_cores; // number of cores on this machine
long long g_block_size;
char *g_docroot;
char *g_data_dir;
int g_alt_ddir_id;
bool g_write_to_temp_dir = false;
char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
q(
     int argc,
     char **argv,
     char *rslt_buf,
     int sz_rslt_buf
     )
// STOP FUNC DECL
{
  int status = 0;
  bool b_is_tbl, b_is_fld;
  int tbl_id, fld_id, nn_fld_id;
  unsigned long long t_before; 
  long long nR;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  char *endptr, *cptr;
  FILE *ofp = NULL;
  char *alt_data_dir;

  g_num_cores = get_nprocs();
  if ( g_num_cores < 1 ) { go_BYE(-1); }
  zero_string(g_cwd, (MAX_LEN_DIR_NAME+1));
  if ( getcwd(g_cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); } 
  //----------------------------------------------
  g_docroot = getenv("Q_DOCROOT");
  if ( g_docroot == NULL ) { go_BYE(-1); }
  g_data_dir = getenv("Q_DATA_DIR");
  if ( g_data_dir == NULL ) { go_BYE(-1); }
  //----------------------------------------------
  cptr = getenv("Q_BLOCK_SIZE");
  if ( ( cptr == NULL ) || (  *cptr == '\0' ) ) { 
    g_block_size = 1024; // vector length 
  }
  else {
    g_block_size = (int)strtoll(cptr, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( g_block_size < 1 ) { go_BYE(-1); }
  }
  //----------------------------------------------
  log_request(argc, argv, &t_before);
  //----------------------------------------------
  if ( ( argc == 2 ) && ( strcmp(argv[1], "init") == 0 ) ) {  
    status = init_meta_data(g_docroot);
    cBYE(status);
    goto BYE;
  }
  status = mmap_meta_data(g_docroot, 
			  &g_tbl_X, &g_tbl_nX, &g_tbls, &g_n_tbl, 
			  &g_ht_tbl_X, &g_ht_tbl_nX, &g_ht_tbl, &g_n_ht_tbl, 
			  &g_fld_X, &g_fld_nX, &g_flds, &g_n_fld, 
			  &g_ht_fld_X, &g_ht_fld_nX, &g_ht_fld, &g_n_ht_fld, 
			  &g_ddir_X, &g_ddir_nX, &g_ddirs, &g_n_ddir);
  cBYE(status);
  g_alt_ddir_id = -1;
  alt_data_dir = getenv("Q_ALT_DATA_DIR");
  if ( alt_data_dir != NULL ) { 
    status = get_ddir_id(alt_data_dir, g_ddirs, g_n_ddir, true, &g_alt_ddir_id);
    cBYE(status);
  }
  //----------------------------------------------

  if ( ( argc < 2 ) || ( argc > MAX_NUM_ARGS ) ) { go_BYE(-1); }
  if ( ( g_docroot == NULL ) ||  ( *g_docroot == '\0' ) ) { 
    fprintf(stderr,  "Doc Root not specified\n"); go_BYE(-1);
  }
  if ( strcmp(argv[1], "list_files") == 0 ) {  
    if ( argc != 3 ) { go_BYE(-1); }
    status = list_files(argv[2]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "dump") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = dump(argv[2], argv[3]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "list_tbls") == 0 ) {  
    if ( argc != 2 ) { go_BYE(-1); }
    status = list_tbls();
    cBYE(status);
  }
  else if ( strcmp(argv[1], "orphan_files") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = orphan_files(argv[2], argv[3]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "add_tbl") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = add_tbl(argv[2], argv[3], &tbl_id, &tbl_rec);
    cBYE(status);
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
  else if ( strcmp(argv[1], "set_nR") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = set_nR(argv[2], argv[3]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "rename") == 0 ) {  
    switch ( argc ) { 
      case 4 : status = rename_tbl(argv[2], argv[3]); cBYE(status); break;
      case 5 : status = rename_fld(argv[2], argv[3], argv[4]); cBYE(status); break;
      default : go_BYE(-1); break;
    }
  }
  else if ( strcmp(argv[1], "is_tbl") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    status = is_tbl(argv[2], &tbl_id, &tbl_rec); cBYE(status);
    if ( tbl_id < 0 ) { b_is_tbl = false; } else { b_is_tbl = true; }
    snprintf(rslt_buf, sz_rslt_buf, "%d,%d\n", b_is_tbl, tbl_id);
  }
#ifdef LEVELDB
  else if ( strcmp(argv[1], "n_dict") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    status = n_dict(argv[2], &nD); cBYE(status);
    snprintf(rslt_buf, sz_rslt_buf, "%lld", nD);
  }
  else if ( strcmp(argv[1], "del_dict") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    status = del_dict(argv[2]); cBYE(status);
  }
  else if ( strcmp(argv[1], "list_dicts") == 0 ) { 
    if ( argc != 2 ) { go_BYE(-1); }
    status = xlist_dicts(); cBYE(status);
  }
  else if ( strcmp(argv[1], "is_dict") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    status = is_dict(argv[2], &dict_id); cBYE(status);
    if ( dict_id < 0 ) { b_is_dict = false; } else { b_is_dict = true; }
    snprintf(rslt_buf, sz_rslt_buf, "%d,%d\n", b_is_dict, dict_id);
  }
#endif
  else if ( strcmp(argv[1], "get_nR") == 0 ) { 
    if ( argc != 3 ) { go_BYE(-1); }
    status = get_nR(argv[2], &nR); cBYE(status);
    snprintf(rslt_buf, sz_rslt_buf, "%lld\n", nR);
  }
  else if ( strcmp(argv[1], "drop_nn_fld") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = drop_nn_fld( argv[2], argv[3]); cBYE(status);
  }
  else if ( strcmp(argv[1], "mk_nn_fld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = mk_nn_fld(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
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
    snprintf(rslt_buf, sz_rslt_buf, "%d,%d\n", b_is_fld, fld_id);
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
    status = file_to_fld(argv[2], &fld_id);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "dld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = dld(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "set_meta") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = set_meta(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "get_meta") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = get_meta(argv[2], argv[3], argv[4], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "is_nn_fld") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = is_nn_fld(argv[2], argv[3], &fld_id, rslt_buf); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "mk_idx") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    char buffer[1024];
    sprintf(buffer, "op=[seq]:start=[0]:incr=[1]:fldtype=[%s]", argv[4]);
    status = ext_s_to_f(argv[2], argv[3], buffer);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "s_to_f") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = ext_s_to_f(argv[2], argv[3], argv[4]);
    cBYE(status);
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
  else if ( strcmp(argv[1], "crossprod") == 0 ) {  
    if ( argc != 10 ) { go_BYE(-1); }
    status = crossprod(argv[2], argv[3], argv[4], argv[5], argv[6], 
	argv[7], argv[8], argv[9]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "pack") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = pack(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "unpack") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = unpack(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "udf_eq_B") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = udf_eq_B(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "subsample") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = subsample(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "app_tbl") == 0 ) {  
    if ( argc != 4 ) { go_BYE(-1); }
    status = app_tbl(argv[2], argv[3]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "mk_bins") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = mk_bins(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "get_t2f2_val") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = get_t2f2_val(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "srt_join") == 0 ) {  // TO BE CILK-ified
    if ( argc != 9 ) { go_BYE(-1); }
    status = srt_join(argv[2], argv[3], argv[4], argv[5], argv[6], 
	argv[7], argv[8]);   
    cBYE(status);
  }
  else if ( strcmp(argv[1], "rng_join") == 0 ) {  // TO BE CILK-ified
    if ( argc != 9 ) { go_BYE(-1); }
    status = rng_join(argv[2], argv[3], argv[4], argv[5], argv[6], 
	argv[7], argv[8]);   
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1s1opf2") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = f1s1opf2(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "regex_match") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = regex_match(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1T1opf2") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = f1T1opf2(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1opf2") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = f1opf2(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1opf2f3") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = f1opf2f3(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1f2opf3") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = f1f2opf3(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "lkpsort") == 0 ) {  
    if ( argc != 9 ) { go_BYE(-1); }
    status = lkpsort(argv[2], argv[3], argv[4], argv[5], argv[6],
	argv[7], argv[8]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "rng_sort") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = rng_sort(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "copy_fld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = copy_fld(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "mv_fld") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = mv_fld(argv[2], argv[3], argv[4]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "t1f1t2f2opt3f3") == 0 ) {  
    if ( argc != 10 ) { go_BYE(-1); }
    status = t1f1t2f2opt3f3(argv[2], argv[3], argv[4], argv[5], 
	argv[6], argv[7], argv[8], argv[9], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "range_to_cfld") == 0 ) {  
    if ( argc != 9 ) { go_BYE(-1); }
    status = range_to_cfld(argv[2], argv[3], argv[4], argv[5], 
	argv[6], argv[7], argv[8]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "cfld_to_range") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = cfld_to_range(argv[2], argv[3], argv[4], argv[5], argv[6]); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "copy_range") == 0 ) {  
    if ( argc != 12 ) { go_BYE(-1); }
    status = copy_range(argv[2], argv[3], argv[4], argv[5], 
	argv[6], argv[7], argv[8], argv[9], argv[10], argv[11]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f_to_s") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = ext_f_to_s(argv[2], argv[3], argv[4], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "f1f2_to_s") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = f1f2_to_s(argv[2], argv[3], argv[4], argv[5], rslt_buf, sz_rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "bindmp") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = bindmp(argv[2], argv[3], argv[4], argv[5]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "binld") == 0 ) {  
    if ( argc != 6 ) { go_BYE(-1); }
    status = binld(argv[2], argv[3], argv[4], argv[5]);
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
  else if ( strcmp(argv[1], "is_a_in_b") == 0 ) {  
    if ( argc != 9 ) { go_BYE(-1); }
    status = is_a_in_b(argv[2], argv[3], argv[4], argv[5], argv[6], 
	argv[7], argv[8]); 
    cBYE(status);
  }
  else if ( strcmp(argv[1], "is_A_in_b") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = is_A_in_b(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "dir_is_a_in_b") == 0 ) {  
    if ( argc != 12 ) { go_BYE(-1); }
    status = dir_is_a_in_b(argv[2], argv[3], argv[4], argv[5], argv[6], 
	argv[7], argv[8], argv[9], argv[10], argv[11]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "count_ht") == 0 ) {  
    if ( argc != 5 ) { go_BYE(-1); }
    status = count_ht(argv[2], argv[3], argv[4]);  cBYE(status);
  }
  else if ( strcmp(argv[1], "count") == 0 ) {  
    if ( argc != 7 ) { go_BYE(-1); }
    status = count(argv[2], argv[3], argv[4], argv[5], argv[6]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "countf") == 0 ) {  
    if ( argc != 8 ) { go_BYE(-1); }
    status = countf(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "countT") == 0 ) {  
    if ( argc != 9 ) { go_BYE(-1); }
    status = countT(argv[2], argv[3], argv[4], argv[5], argv[6], 
	argv[7], argv[8]);
    cBYE(status);
  }
  else if ( strcmp(argv[1], "no_op") == 0 ) {  
    // Nothing to do except log parameters
  }
  else if ( strcmp(argv[1], "pr_fld") == 0 ) {  
    switch ( argc ) { 
      case 4 : 
	status = pr_fld(argv[2], argv[3], "", stdout);
	cBYE(status);
	break; 
      case 5 : 
	status = pr_fld(argv[2], argv[3], argv[4], stdout);
	cBYE(status);
	break; 
      case 6 : 
	if ( argv[5][0] == '\0' ) {
	  status = pr_fld(argv[2], argv[3], argv[4], stdout);
	  cBYE(status);
	}
	else {
	  ofp = fopen(argv[5], "w");
	  return_if_fopen_failed(ofp,  argv[5], "w");
  	  status = pr_fld(argv[2], argv[3], argv[4], ofp);
	  fclose_if_non_null(ofp);
	  cBYE(status);
	}
	break; 
      default : 
	go_BYE(-1);
	break;
    }
  }
  else {
    go_BYE(-1);
  }
 BYE:
  unmap_meta_data(g_tbl_X, g_tbl_nX, g_ht_tbl_X, g_ht_tbl_nX, 
		  g_fld_X, g_fld_nX, g_ht_fld_X, g_ht_fld_nX,
		  g_ddir_X, g_ddir_nX);
  fclose_if_non_null(ofp);
  log_response(status, t_before);
  return(status);
}
