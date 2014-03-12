#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "aux_dld.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "del_fld.h"
#include "add_aux_fld.h"
#include "extract_S.h"
#include "vec_f_to_s.h"
#include "dld.h"
#include "read_meta_data.h"
#include "read_csv.h"
#include "set_meta.h"
#include "mk_dict.h"
#include "mk_file.h"
#include "is_tbl.h"
#include "is_a_in_b.h"
#include "s_to_f.h"
#include "is_fld.h"
#include "f1opf2.h"
#include "mk_temp_file.h"
#include "vec_f1s1opf2.h"
#include "f1opf2_cum.h"
#include "f1opf2_shift.h"
#include "is_aux_fld.h"
#include "f_to_s.h"
#include "drop_aux_fld.h"

#define BUFLEN  64

extern char g_cwd[MAX_LEN_DIR_NAME+1];
extern DDIR_REC_TYPE *g_ddirs;

// last review 9/5/2013
// START FUNC DECL
int 
dld(
    char *tbl,
    char *meta_data_file,
    char *data_file,
    char *in_aux_info
    )
// STOP FUNC DECL
{
  int status = 0;
  int n_flds, tbl_id = -1;
  long long nR;
  char strbuf[BUFLEN];
  int filenos[MAX_NUM_FLDS_TO_LOAD];
  int nn_filenos[MAX_NUM_FLDS_TO_LOAD];
  int len_filenos[MAX_NUM_FLDS_TO_LOAD];
  char rslt_buf[BUFLEN];
  char filename[32]; zero_string(filename, 32);
  char data_dir[MAX_LEN_DIR_NAME+1];

  TBL_REC_TYPE tbl_rec;
  FLD_PROPS_TYPE fld_props[MAX_NUM_FLDS_TO_LOAD];

  FLD_TYPE fldtype[MAX_NUM_FLDS_TO_LOAD];
  HT_REC_TYPE *hts[MAX_NUM_FLDS_TO_LOAD];
  int dict_tbl_id[MAX_NUM_FLDS_TO_LOAD];
  int sz_hts[MAX_NUM_FLDS_TO_LOAD];
  int n_hts[MAX_NUM_FLDS_TO_LOAD];
  char flds[MAX_NUM_FLDS_TO_LOAD][MAX_LEN_FLD_NAME+1];
  char dicts[MAX_NUM_FLDS_TO_LOAD][MAX_LEN_TBL_NAME+1];
  bool ignore_hdr = false; // default 
  char fld_delim = '"'; // default 
  char fld_sep = ','; // default 
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;

  zero_string(data_dir, (MAX_LEN_DIR_NAME+1));
  int ddir_id = 0;
  if ( chk_dsk_ddir_id(g_ddirs) == false ) { go_BYE(-1); }
  strcpy(data_dir, g_ddirs[0].name);

  // Initialization 
  zero_string(rslt_buf, BUFLEN);
  for  ( int i = 0; i < MAX_NUM_FLDS_TO_LOAD; i++ ) {
    dict_tbl_id[i] = -1;
    hts[i] = NULL;
    sz_hts[i] = 0;
    n_hts[i] = 0;
    zero_string(flds[i], MAX_LEN_FLD_NAME+1);
    filenos[i] = -1;
    nn_filenos[i] = -1;
    len_filenos[i] = -1;
    zero_string(dicts[i], MAX_LEN_TBL_NAME+1);

    fld_props[i].is_load = true; /* set by read meta data */
    fld_props[i].is_all_def = false; /* set by read meta data */
    fld_props[i].is_dict_old = false; /* set by read meta data */
    fld_props[i].is_lkp = false; /* set by read meta data */
    fld_props[i].is_trunc = false; /* set by read meta data */
    fld_props[i].is_null_if_missing = false; /* set by read meta data */
    fld_props[i].maxlen = 0; /* set by print_cell() called by read_meta_data */
    fld_props[i].is_any_null = false; /* set by read_csv */
  }
  // Read meta data 
  status = read_meta_data(meta_data_file, flds, fldtype, fld_props,
			  dicts, &n_flds);
  cBYE(status);
  //---------------------------------------
  // Set up for strings to be treated as lookups
  for ( int i = 0; i < n_flds; i++ ) { 
    // skip if not to load 
    if ( fld_props[i].is_load == false ) {  continue; }
    // skip if this field is NOT a string to be treated as lookup
    if ( fld_props[i].is_lkp == false ) {  continue; }
    if ( strlen(dicts[i]) == 0 ) { 
      fprintf(stderr, "No dictionary provided for field [%s]\n", flds[i]);
      go_BYE(-1); 
    }
    if ( fld_props[i].is_dict_old ) { 
      status = is_tbl(dicts[i], &(dict_tbl_id[i]), &tbl_rec); cBYE(status);
      if (dict_tbl_id[i] < 0 ) { 
        fprintf(stderr, "Could not find dictionary [%s]\n", dicts[i]);
	go_BYE(-1); 
      }
    }
    else { 
      status = setup_ht_dict(&(hts[i]), &(sz_hts[i]), &(n_hts[i])); cBYE(status);
    }
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    // skip if not to load 
    if ( fld_props[i].is_load == false ) {  continue; }
    if ( fldtype[i] == SC ) { 
      if ( fld_props[i].maxlen < 1 ) { go_BYE(-1); }
    }
    else {
      if ( fldtype[i] != SV ) { 
        if ( fld_props[i].maxlen != 0 ) { go_BYE(-1); }
      }
    }
    if ( ( fldtype[i] == SC ) ||  ( fldtype[i] == SV ) ) {
      if ( fld_props[i].is_trunc == true ) { 
        if ( fld_props[i].maxlen == 0 ) { go_BYE(-1); }
      }
    }
  }
  /*----------------------------------------------------*/
  status = process_aux_info(in_aux_info, &ignore_hdr, &fld_sep, &fld_delim);
  cBYE(status);

  status = read_csv(data_dir, data_file, fld_delim, fld_sep, '\n', ignore_hdr, flds, 
		    n_flds, fldtype, fld_props, filenos, nn_filenos, 
		    len_filenos, hts, sz_hts, n_hts, dict_tbl_id, &nR);
  cBYE(status);
  sprintf(strbuf, "%lld", nR);
  status = del_tbl(tbl, -1); cBYE(status);
  status = add_tbl(tbl, strbuf, &tbl_id, &tbl_rec); cBYE(status);

  status = chdir(data_dir); cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) {
    if( fld_props[i].is_load == false ) { continue; }
    if ( filenos[i] <= 0 ) { go_BYE(-1); }
    if ( fldtype[i] == SV ) { 
      if ( len_filenos[i] <= 0 ) { 
	go_BYE(-1); }
    }
    else {
      if ( len_filenos[i] > 0 ) { go_BYE(-1); }
    }
    if ( fld_props[i].is_all_def == false ) { 
      if ( nn_filenos[i] <= 0 ) { go_BYE(-1); }
      // Check whether nn is really needed 
      if ( fld_props[i].is_any_null == false ) {
        zero_string_to_nullc(filename); mk_file_name(filename, nn_filenos[i]);
        unlink(filename);
	nn_filenos[i] = -1; 
      }
    }
    if ( fldtype[i] == SV ) { 
      if ( len_filenos[i] <= 0 ) { go_BYE(-1); }
    }
  }
  status = chdir(g_cwd); cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) { 
    if( fld_props[i].is_load == false ) { continue; }
    if ( fld_props[i].is_lkp == true ) {
      if ( fld_props[i].is_dict_old == true )  {
        fldtype[i] = I4;
      }
      else {
        fldtype[i] = I8;
      }
    }
    zero_fld_rec(&fld_rec);
    fld_rec.fldtype = fldtype[i];
    if ( fld_rec.fldtype == SC ) { 
      fld_rec.len     = fld_props[i].maxlen;
    }
    else {
      fld_rec.len     = 0;
    }
    status = add_fld(tbl_id, flds[i], ddir_id, filenos[i], 
		     &fld_id, &fld_rec); 
    cBYE(status);
    if ( nn_filenos[i] > 0 ) {
      if ( fld_props[i].is_all_def == true ) { go_BYE(-1); }
      zero_fld_rec(&nn_fld_rec); nn_fld_rec.fldtype = I1; 
      status = add_aux_fld(tbl, tbl_id, flds[i], fld_id, ddir_id,
			   nn_filenos[i], nn, &nn_fld_id, &nn_fld_rec);
      cBYE(status);
    }
    if ( len_filenos[i] > 0 ) { 
      int fldsz = 0; long long filesz = 0;
      FLD_REC_TYPE off_fld_rec, len_fld_rec; 
      int off_fld_id  = -1, len_fld_id  = -1;
      int off_fileno  = -1, tmp_fileno  = -1, xxx_fileno  = -1;
      int off_ddir_id = -1, tmp_ddir_id = -1, xxx_ddir_id = -1; 
      char *len_X = NULL; size_t len_nX = 0;
      char *off_X = NULL; size_t off_nX = 0;
      char *tmp_X = NULL; size_t tmp_nX = 0;
      char *xxx_X = NULL; size_t xxx_nX = 0;

      zero_fld_rec(&len_fld_rec); len_fld_rec.fldtype = I2;
      status = add_aux_fld(tbl, tbl_id, flds[i], fld_id, ddir_id,
			   len_filenos[i], len, &len_fld_id, &len_fld_rec);
      cBYE(status);

      status = q_mmap(ddir_id, len_filenos[i], &len_X, &len_nX, 0); cBYE(status);

      /* tmp is len incremented by 1 */
      status = get_fld_sz(I2, &fldsz); cBYE(status);
      filesz = nR * fldsz; 
      status = mk_temp_file(filesz, &tmp_ddir_id, &tmp_fileno); cBYE(status);
      if ( tmp_ddir_id != ddir_id ) { go_BYE(-1); }
      status = q_mmap(ddir_id, tmp_fileno, &tmp_X, &tmp_nX, 1); cBYE(status);

      /* xxx is tmp shifted by 1 with a 0 brought in */
      status = get_fld_sz(I2, &fldsz); cBYE(status);
      filesz = nR * fldsz; 
      status = mk_temp_file(filesz, &xxx_ddir_id, &xxx_fileno); cBYE(status);
      if ( xxx_ddir_id != ddir_id ) { go_BYE(-1); }
      status = q_mmap(ddir_id, xxx_fileno, &xxx_X, &xxx_nX, 1); cBYE(status);

      status = get_fld_sz(I8, &fldsz); cBYE(status);
      filesz = nR * fldsz; 
      status = mk_temp_file(filesz, &off_ddir_id, &off_fileno); cBYE(status);
      if ( off_ddir_id != ddir_id ) { go_BYE(-1); }
      status = q_mmap(ddir_id, off_fileno, &off_X, &off_nX, 1); cBYE(status);

      status = vec_f1s1opf2(nR, I2, len_X, NULL, "1", "+", tmp_X, NULL, I2);
      cBYE(status);
      status = f1opf2_shift(tmp_X, nR, I2, 1, "0", xxx_X); cBYE(status);
      status = f1opf2_cum(xxx_X, nR, I2, I8, off_X); cBYE(status);

      zero_fld_rec(&off_fld_rec); off_fld_rec.fldtype = I8;
      status = add_aux_fld(tbl, tbl_id, flds[i], fld_id, ddir_id,
			   off_fileno, off, &off_fld_id, &off_fld_rec);
      cBYE(status);

      rs_munmap(tmp_X, tmp_nX);
      rs_munmap(len_X, len_nX);
      rs_munmap(off_X, off_nX);
      rs_munmap(xxx_X, xxx_nX);
      status = q_delete(ddir_id, tmp_fileno);
      status = q_delete(ddir_id, xxx_fileno);
    }
    if ( fld_props[i].is_lkp == true ) {
      if ( fld_props[i].is_dict_old ) { 
	sprintf(strbuf, "%d", dict_tbl_id[i]);
	status = int_set_meta(tbl_id, fld_id, "dict_tbl_id", strbuf, true);
	cBYE(status);
        if ( ( fld_props[i].is_any_null == true ) && 
             ( fld_props[i].is_null_if_missing == false ) ) {
	  go_BYE(-1);
	}
      }
      else {
	char buffer[BUFLEN], buf0[BUFLEN], buf1[BUFLEN];
	int dict_tbl_id; TBL_REC_TYPE tblrec;
	char *dict_tbl = dicts[i];
	char fk_fld[MAX_LEN_FLD_NAME+1];

	if ( ( strlen(dict_tbl) + strlen("fk_") ) >= MAX_LEN_FLD_NAME ) {
	  fprintf(stderr, "dict_tbl = %s \n", dict_tbl);
	  go_BYE(-1);
	}
	strcpy(fk_fld, "fk_"); strcat(fk_fld, dict_tbl);

	int txt_fld_id, txtnum; char txtfile[32]; zero_string(txtfile, 32);
	int keynum; char keyfile[32]; zero_string(keyfile, 32);
	int len_fld_id, lennum; char lenfile[32]; zero_string(lenfile, 32);
	int off_fld_id, offnum; char offfile[32]; zero_string(offfile, 32);
	int fld_id; 
	FLD_REC_TYPE fld_rec, txt_fld_rec, len_fld_rec, off_fld_rec;

	txtnum = get_max_fileno(); mk_file_name(txtfile, txtnum);
	keynum = get_max_fileno(); mk_file_name(keyfile, keynum);
	lennum = get_max_fileno(); mk_file_name(lenfile, lennum);
	offnum = get_max_fileno(); mk_file_name(offfile, offnum);

	// add the lookup table 
	sprintf(strbuf, "%d", n_hts[i]);
	status = add_tbl(dict_tbl, strbuf, &dict_tbl_id, &tblrec); cBYE(status);
	// dump the hashtable into files 
	status = chdir(data_dir); cBYE(status);
	status = dump_ht(hts[i], sz_hts[i], n_hts[i], data_dir,
			 txtfile, keyfile, lenfile, offfile); 
	cBYE(status);
	status = chdir(g_cwd); cBYE(status);
	// add txt field
	zero_fld_rec(&txt_fld_rec); txt_fld_rec.fldtype = SV;
	status = add_fld(dict_tbl_id, "txt", ddir_id, txtnum, 
			 &txt_fld_id, &txt_fld_rec); 
	cBYE(status);
	// add key field (for debugging). Delete this later TODO P4
	zero_fld_rec(&fld_rec); fld_rec.fldtype = I8;
	status = add_fld(dict_tbl_id, "key", ddir_id, keynum, 
			 &fld_id, &fld_rec); 
	cBYE(status);
	// add len field as aux field 
	zero_fld_rec(&len_fld_rec); len_fld_rec.fldtype = I2;
	status = add_aux_fld(NULL, dict_tbl_id, NULL, txt_fld_id, ddir_id, 
	    lennum, len, &len_fld_id, &len_fld_rec); 
	cBYE(status);
	// add off field as aux field 
	zero_fld_rec(&off_fld_rec); off_fld_rec.fldtype = I8;
	status = add_aux_fld(NULL, dict_tbl_id, NULL, txt_fld_id, ddir_id, 
	    offnum, off, &off_fld_id, &off_fld_rec); 

	// set tbltype after adding txt, len, off
	status = set_meta(dict_tbl, NULL, "tbltype", "txt_lkp"); cBYE(status);
	// add the idx field as I8 (needed because of is_a_in_b)
	status = s_to_f(dict_tbl, "idx", 
			"op=[seq]:start=[0]:incr=[1]:fldtype=[I8]"); cBYE(status);
	// create the fk_fld 
	status = is_a_in_b(tbl, flds[i], dict_tbl, "key", "", "idx", fk_fld);
	cBYE(status);
	// 
	status = is_aux_fld(tbl, fk_fld, "nn", &nn_fld_id, buffer);cBYE(status);
	if ( nn_fld_id >= 0 ) { // check whether it is really needed 
	  status = mk_name_aux_fld(fk_fld, nn, buffer); cBYE(status);
	  status = f_to_s(tbl, buffer, "sum", buffer, BUFLEN); cBYE(status);
	  status = read_nth_val(buffer, ":", 0, buf0, BUFLEN); cBYE(status);
	  status = read_nth_val(buffer, ":", 1, buf1, BUFLEN); cBYE(status);
	  if ( strcmp(buf0, buf1) == 0 ) { 
	    status = drop_aux_fld(tbl, fk_fld, "nn", -1); cBYE(status); 
	  }
	}
	// convert fk_fld to I4
	status = f1opf2(tbl, fk_fld, "op=[conv]:newtype=[I4]", fk_fld); 
	cBYE(status);
	// convert idx fld to I4
	status = f1opf2(dict_tbl, "idx", "op=[conv]:newtype=[I4]", "idx"); 
	cBYE(status);
	int fk_fld_id, nn_fk_fld_id;
	FLD_REC_TYPE fk_fld_rec, nn_fk_fld_rec;
	status = is_fld(tbl, -1, fk_fld, &fk_fld_id, &fk_fld_rec, 
	    &nn_fk_fld_id,&nn_fk_fld_rec); cBYE(status);
	status = add_to_fld_info(fk_fld_id, fk_fld_txt_lkp, txt_fld_id);
	cBYE(status);
	// delete key field. Not needed any longer
	status = del_fld(tbl, -1, "key", -1); cBYE(status);
      }
    }
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( fld_props[i].is_load == false ) {  continue; }
    if ( fld_props[i].is_lkp == false ) {  continue; }
    if ( hts[i] != NULL ) { 
      for ( int j = 0; j < sz_hts[i]; j++ ) {
        free_if_non_null(hts[i][j].strkey);
      }
      free_if_non_null(hts[i]);
    }
  }
 BYE:
  return status ;
}
