#include <sqlite3.h>
#include "constants.h"
#include "macros.h"
#include "md5global.h"
#include "mddriver.h"
#include "qtypes.h"
#include "auxil.h"
#include "ylc_auxil.h"
#include "ylc_dbauxil.h"
#include "ylc_globals.h"
#include "mmap.h"
#include "tbl_meta.h"
#include "dup_fld.h"
#include "fop.h"
#include "fld_meta.h"
#include "ext_f_to_s.h"
#include "del_fld.h"

int ylc_limit(
	      char *list,
	      char  *str_limits
	      )
{
  int status = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id = -1, xlist_id = -1;
  bool b_is_list;  int list_id = -1, num_in_list = -1;
  char **Y = NULL; int nY = 0;
  long long limit[MAX_NUM_IN_LIST];
  char *fld = NULL, *xcl_fld = NULL;
  char lmt_fld[MAX_LEN_FLD_NAME+1];
  /*-----------------------------------------------------------*/
  zero_string(lmt_fld, (MAX_LEN_FLD_NAME+1));

  status = is_list(list, &b_is_list);
  if ( b_is_list == false ) { 
    fprintf(stderr, "List [%s] does not exist \n", list); go_BYE(-1);
  }

  status = get_list_id(list, &list_id, &tbl_id, &xlist_id); cBYE(status); 
  if ( list_id < 0 ) { go_BYE(-1); }

  status = get_num_in_list(list_id, &num_in_list); cBYE(status); 
  if ( num_in_list <= 0 ) { go_BYE(-1); }
  /*-----------------------------------------------------------*/
  status = get_tbl_meta(tbl_id, &tbl_rec); cBYE(status);
  char *tbl = tbl_rec.name;
  /*-----------------------------------------------------------*/
  status = break_str(str_limits, ":", &Y, &nY); cBYE(status);
  if ( nY != num_in_list ) { go_BYE(-1); }
  for ( int i = 0; i < num_in_list; i++ ) { 
    char *endptr;
    limit[i] = strtoll(Y[i], &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    free_if_non_null(Y[i]);
    if ( ( limit[i] <= 0 ) && ( limit[i] != -1 ) )  { go_BYE(-1); }
  }
  free_if_non_null(Y);
  /*-----------------------------------------------------------*/
  for ( int i = 0; i < num_in_list; i++ ) {
    int pos = i + 1; 
    if ( limit[i] <= 0 ) { /* No limit */
      status = del_aux_fld_if_exists(tbl_id, list_id, pos, "lmt_");
      cBYE(status);
    }
    else {
      char str_limit_spec[64]; zero_string(str_limit_spec, 64);
      int lmt_fld_id = -1; long long lmt_fld_cnt = -1;

      status = get_fld_cnt(list_id, pos, "lmt_", &lmt_fld_cnt); cBYE(status);
      if ( lmt_fld_cnt <= limit[i] ) {
	/* Nothing to do */
	continue;
      }
      // Delete current xcl field 
      status = del_aux_fld_if_exists(tbl_id, list_id, pos, "lmt_"); 
#ifdef TODO
      if ( xcl_cnt < limit[i] ) { /* xcl field can work as lmt field */
	// TODO: P2 We can do some optimization here
      }
#endif
      status = get_fld_name(list_id, pos, "xcl_", &xcl_fld); cBYE(status);
      status = get_fld_name(list_id, pos, "",     &fld    ); cBYE(status);
      status = mk_name_new_fld( list, tbl, fld, "l_", lmt_fld); cBYE(status);
      status = dup_fld(tbl, xcl_fld, lmt_fld); cBYE(status);
      sprintf(str_limit_spec, "op=[zero_after_n]:limit=[%lld]", limit[i]);
      status = fop(tbl, lmt_fld, str_limit_spec); cBYE(status);
      status = fld_meta(tbl, lmt_fld, "", &lmt_fld_id, false); cBYE(status);
      status = count_ones(tbl, lmt_fld, &lmt_fld_cnt); cBYE(status);
      status = set_fld_id_cnt(list_id, pos, "lmt_", lmt_fld_id, lmt_fld, lmt_fld_cnt);
      cBYE(status);
      free_if_non_null(xcl_fld);
      free_if_non_null(fld);
    }
  }
 BYE:
  free_if_non_null(xcl_fld);
  free_if_non_null(fld);
  return(status);
}
