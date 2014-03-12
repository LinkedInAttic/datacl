#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "aux_meta.h"
#include "meta_globals.h"

// last review 9/22/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
binld(
       char *tbl,
       char *str_flds,
       char *str_flds_spec,
       char *infile,
       char *indir
       )
// STOP FUNC DECL
{
  int status = 0;
  char **flds = NULL;  char **spec = NULL;
  int rec_size = -1, n_flds = INT_MAX, chk_n_flds = INT_MIN;
  TBL_REC_TYPE tbl_rec; FLD_REC_TYPE fld_rec;
  int tbl_id = INT_MIN, fld_id = INT_MIN;
  long long nR;
  FILE *ifp = NULL;
  char  *X  = NULL; size_t  nX  = 0;
  char str_nR[32]; 
  char cwd[MAX_LEN_DIR_NAME+1];

  char *Xs[MAX_NUM_FLDS_TO_LOAD]; size_t nXs[MAX_NUM_FLDS_TO_LOAD];
  FLD_TYPE fldtype[MAX_NUM_FLDS_TO_LOAD];
  int fldsz[MAX_NUM_FLDS_TO_LOAD];
  int ddir_id[MAX_NUM_FLDS_TO_LOAD];
  char opfiles[MAX_NUM_FLDS_TO_LOAD][MAX_LEN_FILE_NAME+1];
  int filenos[MAX_NUM_FLDS_TO_LOAD];
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds_spec == NULL ) || ( *str_flds_spec == '\0' ) ) { go_BYE(-1); }
  if ( ( infile == NULL ) || ( *infile == '\0' ) ) { go_BYE(-1); }
  zero_string(str_nR, 32);
  for ( int i = 0; i < MAX_NUM_FLDS_TO_LOAD; i++ ) { 
    ddir_id[i] = INT_MAX;
    Xs[i] = NULL;
    nXs[i] = 0;
    zero_string(opfiles[i], MAX_NUM_FLDS_TO_LOAD);
    fldtype[i] = -1;
    fldsz[i] = INT_MAX;
  }
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  status = explode(str_flds, ':', &flds, &n_flds);
  cBYE(status);
  if ( n_flds < 1                    ) { go_BYE(-1); }
  if ( n_flds > MAX_NUM_FLDS_TO_LOAD ) { go_BYE(-1); }
  //--------------------------------------------------------
  for ( int i = 0; i < n_flds; i++ ) { 
    status = chk_fld_name(flds[i]); cBYE(status);
    for ( int j = i+1; j < n_flds; j++ ) { 
      if ( strcmp(flds[i], flds[j]) == 0 ) { 
	fprintf(stderr, "Duplicate field names not allowed \n");
	go_BYE(-1);
      }
    }
  }
  //--------------------------------------------------------
  if ( ( str_flds_spec == NULL ) || ( *str_flds_spec == '\0' ) ) { go_BYE(-1); }
  status = explode(str_flds_spec, ':', &spec, &chk_n_flds);
  cBYE(status);
  if ( chk_n_flds != n_flds ) { go_BYE(-1); }
  //--------------------------------------------------------
  rec_size = 0;
  for ( int j = 0; j < n_flds; j++ ) { 
    status = unstr_fldtype(spec[j], &(fldtype[j])); cBYE(status);
    status = get_fld_sz(fldtype[j], &(fldsz[j])); cBYE(status);
    rec_size += fldsz[j];
    switch ( fldtype[j] ) { 
      case I1 : case I2 : case I4 : case I8 : case F4 : case F8 : break;
      default : go_BYE(-1); break; 
    }
  }
  /* mmap input file and get number of rows */
  if ( ( indir != NULL ) && ( *indir != '\0' ) ) { 
    status = chdir(indir); cBYE(status);
  }
  status = rs_mmap(infile, &X, &nX, 0); cBYE(status);
  nR = nX / rec_size;
  if ( nR * rec_size != nX ) { go_BYE(-1); }
  status = chdir(cwd); cBYE(status);
  sprintf(str_nR, "%lld", nR);
  /* Create output files for each field */
  for ( int j = 0; j < n_flds; j++ ) { 
    size_t filesz = fldsz[j] * nR; 
    status = mk_temp_file(filesz, &(ddir_id[j]), &(filenos[j])); cBYE(status);
  }
  for ( int j = 0; j < n_flds; j++ ) { 
    status = q_mmap(ddir_id[j], filenos[j], &(Xs[j]), &(nXs[j]), 1); cBYE(status);
  }
  //--------------------------------------------------------
  for ( long long i = 0; i < nR; i++ ) { 
    for ( int j = 0; j < n_flds; j++ ) { 
      memcpy(Xs[j], X, fldsz[j]);
      Xs[j] += fldsz[j];
      X     += fldsz[j];
    }
  }
  for ( int j = 0; j < n_flds; j++ ) { 
    rs_munmap(Xs[j], nXs[j]);
  }
  // Delete table if it exists 
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id >= 0 ) { 
    status = del_tbl(NULL, tbl_id); cBYE(status);
  }
  // Create an empty table
  status = add_tbl(tbl, str_nR, &tbl_id, &tbl_rec); cBYE(status);
  // Add fields to table 
  for ( int j = 0; j < n_flds; j++ ) { 
    zero_fld_rec(&fld_rec); fld_rec.fldtype = fldtype[j];
    // TODO P0 Need to convert to fileno from opfile 
    status = add_fld(tbl_id, flds[j], ddir_id[j], filenos[j], &fld_id, &fld_rec);
    cBYE(status);
  }
BYE:
  fclose_if_non_null(ifp);
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( spec != NULL ) { free_if_non_null(spec[i]); }
    if ( flds != NULL ) { free_if_non_null(flds[i]); }
  }
  free_if_non_null(spec);
  free_if_non_null(flds);
  return status ;
}
