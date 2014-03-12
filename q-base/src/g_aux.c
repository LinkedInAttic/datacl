#include <curl/curl.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "dbauxil.h"
#include "auxil.h"
#include "aux_meta.h"
#include "g_meta_data.h"
#include "url.h"

extern 	DDIR_REC_TYPE *g_ddirs;

/* the function to invoke as the data recieved */
size_t static 
write_callback_func(
		    void *buffer,
		    size_t size,
		    size_t nmemb,
		    void *userp
		    )
{
  char **response_ptr =  (char**)userp;

  /* assuming the response is a string */
  *response_ptr = strndup(buffer, (size_t)(size *nmemb));
  return((size_t)(size * nmemb));

}

// START FUNC DECL
int
g_num_free_regs(
		GPU_REG_TYPE *gpu_reg,
		int n_gpu_reg
		)
// STOP FUNC DECL
{
  int num_free_regs = 0;

  for ( int i = 0; i < n_gpu_reg; i++ ) { 
    if ( gpu_reg[i].is_busy == false ) { 
      num_free_regs++;
    }
  }
  return(num_free_regs);
}
//
// START FUNC DECL
int
g_is_fld(
	 GPU_REG_TYPE *gpu_reg,
	 int n_gpu_reg,
	 char *d_fld,
	 int *ptr_d_fld_id,
	 GPU_REG_TYPE *ptr_gpu_reg

	 )
// STOP FUNC DECL
{
  int status = 0;

  *ptr_d_fld_id = -1;
  if ( ( d_fld == NULL ) || ( *d_fld == '\0' ) ) { go_BYE(-1); }
  for ( int i = 0; i < n_gpu_reg; i++ ) { 
    if ( strcmp(d_fld, gpu_reg[i].d_fld) == 0 ) {
      *ptr_d_fld_id = i;
      *ptr_gpu_reg = gpu_reg[i];
      break;
    }
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
g_meta_to_str(
	      char *buffer, 
	      char *d_fld, 
	      TBL_REC_TYPE tbl_rec, 
	      FLD_REC_TYPE h_fld_rec
	      )
// STOP FUNC DECL
{
  int status = 0;
  char tempbuf[MAX_LEN_DIR_NAME+MAX_LEN_FLD_NAME+MAX_LEN_TBL_NAME];
  char str_fldtype[8]; zero_string(str_fldtype, 8);
  char *X = NULL; size_t nX = 0;

  if   ( buffer == NULL ) { go_BYE(-1); }
  if ( ( d_fld  == NULL ) || ( *d_fld == '\0' ) ) { go_BYE(-1); }

  char filename[32]; zero_string(filename, 32);
  mk_file_name(filename, h_fld_rec.fileno);
  char *str_dir = g_ddirs[h_fld_rec.ddir_id].name;
  if ( *str_dir == '\0' ) { go_BYE(-1); }
  status = mk_str_fldtype(h_fld_rec.fldtype, str_fldtype); cBYE(status);
  status = get_data(h_fld_rec, &X, &nX, 0); cBYE(status);
  sprintf(tempbuf, "d_fld=%s ", d_fld); strcat(buffer, tempbuf);
  sprintf(tempbuf, "h_fld=%s ", h_fld_rec.name); strcat(buffer, tempbuf);
  sprintf(tempbuf, "nR=%lld ", tbl_rec.nR); strcat(buffer, tempbuf);
  sprintf(tempbuf, "tbl=%s ", tbl_rec.name); strcat(buffer, tempbuf);
  sprintf(tempbuf, "filesz=%llu ", (unsigned long long)nX); strcat(buffer, tempbuf);
  sprintf(tempbuf, "fldtype=%s ", str_fldtype); strcat(buffer, tempbuf);
  sprintf(tempbuf, "filename=%s/%s ",  str_dir, filename); strcat(buffer, tempbuf);
 BYE:
  rs_munmap(X, nX);
  return status ;
}

// START FUNC DECL
int is_fldtype_goodfor_gpu(
			   FLD_TYPE fldtype
			   )
// STOP FUNC DECL
{
  switch ( fldtype ) { 
  case I1 : case I2 : case I4 : case I8 : case F4 : case F8 :
    return(true);
    break;
  default : 
    return(false);
    break;
  }
}

 
// START FUNC DECL
int g_call_gpu(
	       char *url, 
	       char **ptr_response
	       )
// STOP FUNC DECL
{
  int status = 0;

  CURLcode rslt; long http_code; CURL *curl_handle = NULL; 
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, ptr_response);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback_func);
  rslt = curl_easy_perform(curl_handle);
  if ( rslt != CURLE_OK ) { go_BYE(-1); }
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
  if ( http_code != 200 ) { go_BYE(-1); }
  /*
  if ( *ptr_response != NULL ) {
    fprintf(stderr, "Response from GPU is %s \n", *ptr_response);
  }
  else {
    fprintf(stderr, "No Response from GPU \n");
  }
  */
 BYE:
  curl_easy_cleanup(curl_handle);
  return status ;
}

extern char g_docroot[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int
g_info(
    )
// STOP FUNC DECL
{
  int status = 0;
  char str_fldtype[8];

  // GPU meta data stuff
  char         *gpu_reg_X = NULL;
  size_t        gpu_reg_nX = 0;
  GPU_REG_TYPE *gpu_reg = NULL; 
  int           n_gpu_reg = 0; 

  status = g_mmap_meta_data(g_docroot, &gpu_reg_X, &gpu_reg_nX,
			    &gpu_reg, &n_gpu_reg);

  fprintf(stdout,"idx,filesz,nR,fldtype,filename,tbl,h_fld,d_fld\n");
  for ( int i = 0; i < n_gpu_reg; i++ ) { 
    if ( gpu_reg[i].is_busy == false ) { continue; }
    zero_string(str_fldtype, 8);
    status = mk_str_fldtype(gpu_reg[i].fldtype, str_fldtype); cBYE(status);
    fprintf(stdout,"%d,%llu,%lld,%s,%s,%s,%s,%s\n",
	    i, (unsigned long long)gpu_reg[i].filesz, 
	    gpu_reg[i].nR, 
	    str_fldtype, 
	    gpu_reg[i].filename, 
	    gpu_reg[i].tbl, 
	    gpu_reg[i].h_fld,
	    gpu_reg[i].d_fld
	    );
  }
 BYE:
  return status ;
}
extern char g_gpu_server[MAX_LEN_SERVER_NAME+1];
extern int  g_gpu_port;
// START FUNC DECL
int
g_make_url(
	   char *command,
	   char *url
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *enc_command = NULL;
  status = url_encode(command, &enc_command); cBYE(status);
  sprintf(url, "http://%s:%d/gq?COMMAND=gq %s", 
	  g_gpu_server, g_gpu_port, enc_command);
 BYE:
  free_if_non_null(enc_command);
  return status ;
}
// START FUNC DECL
int
g_chk_f_to_s(
	       GPU_REG_TYPE reg1,
	       const char *str_op
	       )
// STOP FUNC DECL
{
  int status = 0;
BYE:
  return status ;
}
//
// START FUNC DECL
int
g_chk_count(
	       GPU_REG_TYPE reg_1,
	       GPU_REG_TYPE reg_c,
	       GPU_REG_TYPE reg_2
	       )
// STOP FUNC DECL
{
  int status = 0;
BYE:
  return status ;
}

// START FUNC DECL
int
g_chk_countf(
	       GPU_REG_TYPE reg_1,
	       GPU_REG_TYPE reg_c,
	       GPU_REG_TYPE reg_2,
	       GPU_REG_TYPE reg_o
	       )
// STOP FUNC DECL
{
  int status = 0;
BYE:
  return status ;
}


// START FUNC DECL
int
g_chk_shift(
	       GPU_REG_TYPE reg1,
	       GPU_REG_TYPE reg2,
	       long long shift_amt,
	       long long newval
	       )
// STOP FUNC DECL
{
  int status = 0;
BYE:
  return status ;
}

// START FUNC DECL
int
g_chk_f1s1opf2(
	       GPU_REG_TYPE reg1,
	       GPU_REG_TYPE reg2,
	       const char *str_op
	       )
// STOP FUNC DECL
{
  int status = 0;
  if ( reg1.nR != reg2.nR ) { go_BYE(-1); }
  if ( ( str_op == NULL ) || ( *str_op == '\0' ) ) { go_BYE(-1); }

  if ( ( strcmp(str_op, "==") == 0 ) || 
       ( strcmp(str_op, "!=") == 0 ) || 
       ( strcmp(str_op, ">") == 0 ) || 
       ( strcmp(str_op, "<") == 0 ) || 
       ( strcmp(str_op, ">=") == 0 ) || 
       ( strcmp(str_op, "<=") == 0 ) ) {
    if ( reg2.fldtype != I1 ) { go_BYE(-1); }
  }
  if ( ( strcmp(str_op, "+") == 0 ) || 
       ( strcmp(str_op, "-") == 0 ) || 
       ( strcmp(str_op, "*") == 0 ) || 
       ( strcmp(str_op, "/") == 0 ) ) { 
    if ( reg2.fldtype != reg1.fldtype ) { go_BYE(-1); }
  }

  switch ( reg1.fldtype ) { 
  case I1 : 
    if ( ( strcmp(str_op, "==") == 0 ) || 
	 ( strcmp(str_op, "!=") == 0 ) ) { 
      /* all is well */
    }
    else {
      go_BYE(-1);
    }
    break;
  case I2 : 
    if ( ( strcmp(str_op, "==") == 0 ) || 
	 ( strcmp(str_op, "!=") == 0 ) ) { 
      /* all is well */
    }
    else {
      go_BYE(-1);
    }
    break;
  case I4 : 
    if ( ( strcmp(str_op, "==") == 0 ) || 
	 ( strcmp(str_op, "!=") == 0 ) ||
	 ( strcmp(str_op, "+") == 0 ) ) { 
      /* all is well */
    }
    else {
      go_BYE(-1);
    }
    break;
  case I8 : 
    if ( ( strcmp(str_op, "==") == 0 ) || 
	 ( strcmp(str_op, "!=") == 0 ) ) { 
      /* all is well */
    }
    else {
      go_BYE(-1);
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
g_chk_f1f2opf3(
	       GPU_REG_TYPE reg1,
	       GPU_REG_TYPE reg2,
	       const char *str_op,
	       GPU_REG_TYPE reg3
	       )
// STOP FUNC DECL
{
  int status = 0;

  if ( ( strcmp(str_op, "==") == 0 ) || 
       ( strcmp(str_op, "!=") == 0 ) || 
       ( strcmp(str_op, ">") == 0 ) || 
       ( strcmp(str_op, "<") == 0 ) || 
       ( strcmp(str_op, ">=") == 0 ) || 
       ( strcmp(str_op, "<=") == 0 ) ) {
    if ( reg3.fldtype != I1 ){ go_BYE(-1); }
  }

  if ( ( str_op == NULL ) || ( *str_op == '\0' ) ) { go_BYE(-1); }
  switch ( reg1.fldtype ) { 
  case I1 : 
    if ( ( strcmp(str_op, "==") == 0 ) || 
	 ( strcmp(str_op, "!=") == 0 ) ) { 
      /* all is well */
    }
    else {
      go_BYE(-1);
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  return status ;
}

// START FUNC DECL
int
g_chk_join(
	   GPU_REG_TYPE reg_ls,
	   GPU_REG_TYPE reg_ld,
	   GPU_REG_TYPE reg_vs,
	   GPU_REG_TYPE reg_vd,
	   GPU_REG_TYPE reg_vd_nn
	   )
// STOP FUNC DECL
{
  int status = 0;
  long long nR = reg_ls.nR;
  if ( nR != reg_ld.nR ) { go_BYE(-1); }
  if ( nR != reg_vs.nR ) { go_BYE(-1); }
  if ( nR != reg_vd.nR ) { go_BYE(-1); }
  if ( nR != reg_vd_nn.nR ) { go_BYE(-1); }

  if ( reg_ls.fldtype != reg_ld.fldtype ) { go_BYE(-1); }
  if ( reg_vs.fldtype != reg_vd.fldtype ) { go_BYE(-1); }
  if ( reg_ls.fldtype != I4 ) { go_BYE(-1); }
  if ( reg_vd_nn.fldtype != I1 ) { go_BYE(-1); }
  switch ( reg_vs.fldtype ) { 
  case I1 : case I2 : case I4 : case I8 : break;
  default : go_BYE(-1); break;
  }

 BYE:
  return status ;
}

// START FUNC DECL
int
g_chk_f1f2_to_s(
		GPU_REG_TYPE reg1,
		GPU_REG_TYPE reg2,
		const char *str_op
		)
// STOP FUNC DECL
{
  int status = 0;

  if ( ( strcmp(str_op, "sum") == 0 ) || 
       ( strcmp(str_op, "min") == 0 ) || 
       ( strcmp(str_op, "max") == 0 ) ) {
    /* all is well */
  }
  else {
    go_BYE(-1);
  }
  if ( reg1.fldtype != I1 ) { go_BYE(-1); }
  switch ( reg2.fldtype ) { 
  case I1 : case I2 : case I4 : break;
  default : go_BYE(-1); break;
  }
 BYE:
  return status ;
}
// START FUNC DECL
void
zero_gpu_reg(
	     GPU_REG_TYPE *ptr_gpu_reg
	     )
// STOP FUNC DECL
{
  ptr_gpu_reg->reg     = NULL;
  ptr_gpu_reg->nR      = 0;
  ptr_gpu_reg->filesz  = 0;
  ptr_gpu_reg->is_busy = false;
  zero_string(ptr_gpu_reg->filename, MAX_LEN_DIR_NAME+63);
  zero_string(ptr_gpu_reg->tbl, MAX_LEN_TBL_NAME+1);
  zero_string(ptr_gpu_reg->h_fld, MAX_LEN_FLD_NAME+1);
  zero_string(ptr_gpu_reg->d_fld, MAX_LEN_FLD_NAME+1);
}
