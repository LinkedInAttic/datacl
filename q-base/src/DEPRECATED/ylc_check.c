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
#include "f1f2opf3.h"
#include "is_fld.h"
#include "ext_f_to_s.h"
#include "is_fld.h"
#include "tbl_meta.h"
#include "fld_meta.h"

// START FUNC DECL
int ylc_check(
    )
// STOP FUNC DECL
{
  int status =  0;
  char *mode = NULL;
  int num_lists = 0; char **lists = NULL;
  FLD_REC_TYPE fld_rec;    int fld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;
#define NUM_MODES 4
  long long mode_cnt[NUM_MODES];
  int mode_fld_id[NUM_MODES];

  mode = malloc(16); 
  /* Make sure that all flds referenced are Q fields */
  status = fetch_rows(g_db, "list", "name", "", "order by id", 
		      (void **)&lists, &num_lists);
  cBYE(status);
  if ( num_lists == 0 ) { fprintf(stderr, "Nothing to check\n"); goto BYE; } 
  for ( int i = 0; i < num_lists; i++ ) {
    int tbl_id, xlist_id, list_id, num_in_list;
    status = get_list_id(lists[i], &list_id, &tbl_id, &xlist_id); cBYE(status);
    char *list_name = lists[i];
    status = get_num_in_list(list_id, &num_in_list); cBYE(status);
    if ( num_in_list == 0 ) { continue; } 
    for ( int position = 1; position <= num_in_list; position++ ) {
      for ( int mode_idx = 0; mode_idx < NUM_MODES; mode_idx++ ) {
	switch ( mode_idx ) { 
	case 0 : strcpy(mode, ""); break; 
	case 1 : strcpy(mode, "new_"); break; 
	case 2 : strcpy(mode, "xcl_"); break; 
	case 3 : strcpy(mode, "lmt_"); break; 
	default : go_BYE(-1); break;
	}
	char *fld = NULL; long long cnt; int fld_id;
	status = get_fld_name(list_id, position, mode, &fld); cBYE(status);
	if ( fld == NULL ) { go_BYE(-1); }
        status = get_fld_cnt(list_id, position, mode, &cnt); cBYE(status);
        mode_cnt[mode_idx] = cnt;
        status = get_fld_id(list_id, position, mode, &fld_id); cBYE(status);
        mode_fld_id[mode_idx] = fld_id;
	status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, 
			&nn_fld_rec);
	cBYE(status);
	if ( fld_id < 0 ) { 
	  fprintf(stderr, "MISSING! List %s, Pos = %d, Mode = %s, Fld [%s]\n", 
		  lists[i], position, mode, fld); 
	  continue;
	}
	if ( fld_rec.fldtype != B ) { go_BYE(-1); }
	free_if_non_null(fld); 
      }
      for ( int i = 1; i < NUM_MODES; i++ ) {
	if ( ( mode_cnt[i] > 0 ) && ( mode_cnt[i-1] > 0 ) ) {
	  if ( mode_cnt[i] == mode_cnt[i-1] ) { 
	    if ( mode_fld_id[i] != mode_fld_id[i-1] ) {
	    fprintf(stderr, "Error for List %s Position %d Mode %d \n",
		list_name, position, i); 
	    fprintf(stderr, "cnt[%d] = %lld, cnt[%d] = %lld \n",
		i, mode_cnt[i], i-1, mode_cnt[i-1]);
	    fprintf(stderr, "fld_id[%d] = %d, fld_id[%d] = %d \n",
		i, mode_fld_id[i], i-1, mode_fld_id[i-1]);
	    }
	  }
	  else if ( mode_cnt[i] > mode_cnt[i-1] ) { 
	    fprintf(stderr, "Error for List %s Position %d Mode %d \n",
		list_name, position, i); 
	    fprintf(stderr, "cnt[%d] = %lld, cnt[%d] = %lld \n",
		i, mode_cnt[i], i-1, mode_cnt[i-1]);
	    fprintf(stderr, "fld_id[%d] = %d, fld_id[%d] = %d \n",
		i, mode_fld_id[i], i-1, mode_fld_id[i-1]);
	    go_BYE(-1); 
	  }
	}
      }
    }
  }
  fprintf(stderr, "All checks passed\n");
 BYE:
  if ( num_lists > 0 ) { 
    for ( int i = 0; i < num_lists; i++ ) { 
      if ( lists != NULL ) { free_if_non_null(lists[i]); }
    }
  }
  free_if_non_null(lists);
  free_if_non_null(mode);
  return(status);
}
