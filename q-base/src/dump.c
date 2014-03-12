#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "extract_S.h"
#include "spooky_hash.h"
#include "qsort_asc_I8.h"

extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
dump(
     char *tblfile,
     char *fldfile,
     char *fld_info_file,
     char *options
     )
// STOP FUNC DECL
{
  int status = 0;
  FILE *fp = NULL;
  TBL_REC_TYPE *x = NULL; FLD_REC_TYPE *y = NULL;
  unsigned long long thashes[MAX_NUM_TBLS];
  unsigned long long fhashes[MAX_NUM_FLDS];
  unsigned long long seedUI8;
  char tempstr[MAX_LEN_TBL_NAME+1+MAX_LEN_FLD_NAME+1];

  seedUI8 = get_time_usec();
  if ( ( tblfile == NULL ) || ( *tblfile == '\0' ) ) { 
    fp = fopen("/dev/null", "a"); 
  }
  else {
    fp = fopen(tblfile, "w"); return_if_fopen_failed(fp, tblfile, "w");
  }
  fprintf(fp, "tbl_id,tbltype,name,nR\n");
  int num_tbls = 0;
  for ( int i = 0; i < g_n_tbl; i++ ) { 
    char str_tbltype[16];
    x = &(g_tbls[i]);
    if ( ( x->name == NULL ) || ( x->name[0] == '\0' ) ) {
      continue;
    }
    thashes[num_tbls++] = spooky_hash64(x->name, strlen(x->name), seedUI8);
    status = chk_tbl_meta(i); cBYE(status);
    status = mk_str_tbltype(g_tbls[i].tbltype, str_tbltype);
    fprintf(fp, "%d,%s,%s,%lld\n", i, str_tbltype, x->name, x->nR);
  }
  fclose_if_non_null(fp);
  qsort_asc_I8(thashes, num_tbls, sizeof(long long), NULL);
  for ( int i = 1; i < num_tbls; i++ ) {
    if ( thashes[i] == thashes[i-1] ) { go_BYE(-1); }
  }
 //--------------------------------------------------------------
  bool mc_readable = false;
  if ( ( options != NULL ) && ( *options != '\0' ) ) { 
#define BUFLEN 32
    char buf[BUFLEN]; bool is_null; zero_string(buf, BUFLEN);
    status = chk_aux_info(options); cBYE(status);
    status = extract_S(options, "mc_readable=[", "]", buf, BUFLEN, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( strcasecmp(buf, "true") == 0 ) { 
      mc_readable = true;
    }
  }
  /*-------------------------------------------------*/
  if ( ( fld_info_file == NULL ) || ( *fld_info_file == '\0' ) ) { 
    fp = fopen("/dev/null", "a"); 
  }
  else {
    fp = fopen(fld_info_file, "w"); 
    return_if_fopen_failed(fp, fld_info_file, "w");
  }
  fprintf(fp, "id,mode,fld_id,aux_id\n");

  for ( int i = 0; i < g_n_fld_info; i++  ) { 
    char strbuf[32]; int aux_id;
    if ( g_fld_info[i].mode == 0 ) { 
      if ( is_zero_fld_info_rec(g_fld_info[i]) == false ) { go_BYE(-1); }
      continue; 
    }
    status = chk_fld_info(g_fld_info[i]); cBYE(status);
    status = mk_str_fld_info_mode(g_fld_info[i].mode, strbuf); cBYE(status);
    switch ( g_fld_info[i].mode ) { 
      case fk_fld_txt_lkp : 
	aux_id = g_fld_info[i].xxx.dict_fld_id; 
	chk_range(aux_id, 0, g_n_fld); 
	break;
      case fk_fld_len : 
	aux_id = g_fld_info[i].xxx.len_fld_id; 
	chk_range(aux_id, 0, g_n_fld); 
	break;
      case fk_fld_off : 
	aux_id = g_fld_info[i].xxx.off_fld_id; 
	chk_range(aux_id, 0, g_n_fld); 
	break;
      default : 
	go_BYE(-1);
	break;
    }
    fprintf(fp, "%d,%s,%d,%d\n", i, strbuf, g_fld_info[i].fld_id, aux_id);
  }
  fclose_if_non_null(fp); 
  /* Check that all non-null values come before null values */
  int num_non_null_fld_info = 0;
  for ( int i = 0; i < g_n_fld_info; i++  ) { 
    if ( is_zero_fld_info_rec(g_fld_info[i]) == true ) { 
      break;
    }
    num_non_null_fld_info++;
  }
  //--------------------------------------------------------------
  for ( int i = num_non_null_fld_info; i < g_n_fld_info; i++ ) { 
    if ( is_zero_fld_info_rec(g_fld_info[i]) == false ) { go_BYE(-1); }
  }
  //--------------------------------------------------------------
  // Check that values are unique and sorted ascending
  long long *X = (long long *)g_fld_info;
  for ( int i = 1; i < num_non_null_fld_info; i++ ) { 
    if ( X[i] <= X[i-1] ) { printf("i = %d, \n", i); go_BYE(-1); }
  }
  //--------------------------------------------------------------
  /*-------------------------------------------------*/
  if ( ( fld_info_file == NULL ) || ( *fld_info_file == '\0' ) ) { 
    fp = fopen("/dev/null", "a"); 
  }
  else {
    fp = fopen(fldfile, "w"); return_if_fopen_failed(fp, fldfile, "w");
  }
  fprintf(fp, "id,tbl_id,ddir_id,parent_id,nn_fld_id,len,");
  fprintf(fp, "dict_fld_id,len_fld_id,off_fld_id,");
  fprintf(fp, "filesz,is_external,srttype,fldtype,auxtype,");
  fprintf(fp, "sumI8,minI8,maxI8,");
  fprintf(fp, "sumF8,minF8,maxF8,");
  fprintf(fp, "is_sum_nn,is_min_nn,is_max_nn,");
  fprintf(fp, "name,fileno");
  fprintf(fp, "\n");
  int num_flds = 0;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    char str_fldtype[32]; zero_string(str_fldtype, 32);
    char str_auxtype[32]; zero_string(str_auxtype, 32);
    char str_srttype[32]; zero_string(str_srttype, 32);
    y = &(g_flds[i]);
    if ( y->fileno <= 0 ) { 
      if ( is_zero_fld_rec(g_flds[i]) == false ) { go_BYE(-1); }
    }
    else {
      int tbl_id = y->tbl_id; 
      chk_range(tbl_id, 0, g_n_tbl);
      long long nR = g_tbls[tbl_id].nR;
      status = chk_fld_meta(i, nR); cBYE(status);
      
      sprintf(tempstr, "%s:%s", g_tbls[tbl_id].name, g_flds[i].name);
      fhashes[num_flds++] = spooky_hash64(tempstr, strlen(tempstr), seedUI8);

      zero_string(str_fldtype, 32);
      status = mk_str_fldtype(y->fldtype, str_fldtype); cBYE(status);
      status = mk_str_auxtype(y->auxtype, str_auxtype); cBYE(status);
      status = mk_str_srttype(y->srttype, str_srttype); cBYE(status);

      fprintf(fp, "%d,%d,%d,%d,%d,%d,", 
	      i, y->tbl_id, y->ddir_id, y->parent_id, y->nn_fld_id, y->len);
      int dict_fld_id, len_fld_id, off_fld_id; 
      status = get_fld_info(fk_fld_txt_lkp, i, &dict_fld_id); cBYE(status);
      status = get_fld_info(fk_fld_len,     i, &len_fld_id); cBYE(status);
      status = get_fld_info(fk_fld_off,     i, &off_fld_id); cBYE(status);
      fprintf(fp, "%d,%d,%d,", dict_fld_id, len_fld_id, off_fld_id);

      fprintf(fp, "%llu,", (unsigned long long)y->filesz); 

      fprintf(fp, "%d,%s,%s,%s,", 
	      y->is_external, str_srttype, str_fldtype, str_auxtype);

      if ( mc_readable == true ) { 
          fprintf(fp, "%lld,%lld,%lld,", 
	      y->sumval.sumI8, y->minval.minI8, y->maxval.maxI8);
          fprintf(fp, "%lf,%lf,%lf,", 
	      y->sumval.sumF8, y->minval.minF8, y->maxval.maxF8);
      }
      else {
      switch ( y->fldtype ) {
	case B : case I1 : case I2 : case I4 : case I8 : 
          fprintf(fp, "%lld,%lld,%lld,", 
	      y->sumval.sumI8, y->minval.minI8, y->maxval.maxI8);
	  break;
	case F4 : case F8 : 
          fprintf(fp, "%lf,%lf,%lf,", 
	      y->sumval.sumF8, y->minval.minF8, y->maxval.maxF8);
	  break;
	default : 
          fprintf(fp, ",,,");
	  break;
      }
      }
      if ( ( y->is_sum_nn != 0 ) && ( y->is_sum_nn != 1 ) ) { go_BYE(-1); }
      if ( ( y->is_min_nn != 0 ) && ( y->is_min_nn != 1 ) ) { go_BYE(-1); }
      if ( ( y->is_max_nn != 0 ) && ( y->is_max_nn != 1 ) ) { go_BYE(-1); }
      fprintf(fp, "%d,%d,%d,", y->is_sum_nn, y->is_min_nn, y->is_max_nn);

      if ( y->fileno <= 0 ) { go_BYE(-1); }
      if ( y->parent_id < 0 ) { 
        status = chk_fld_name(y->name); cBYE(status); 
      }
      fprintf(fp, "%s,%d", y->name, y->fileno);

      /* TODO P2: Print file size as well */
      fprintf(fp, "\n");  /* record delimiter */
    }
  }
  fclose_if_non_null(fp);
  qsort_asc_I8(fhashes, num_flds, sizeof(long long), NULL);
  for ( int i = 1; i < num_flds; i++ ) {
    if ( fhashes[i] == fhashes[i-1] ) { go_BYE(-1); }
  }
 BYE:
  fclose_if_non_null(fp);
  return(status);
}
