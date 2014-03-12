extern int
set_fld_id_cnt(
		   int list_id, 
		   int pos, 
		   char *mode, 
		   int fld_id, 
		   char *fld,
		   long long cnt
		   )
;
//----------------------------
extern int
get_fld_cnt(
	    int list_id, 
	    int position, 
	    char *mode,
	    long long *ptr_cnt
	    )
;
//----------------------------
extern int
get_fld_id(
	   int list_id, 
	   int position, 
	   char *mode,
	   int *ptr_fld_id
	   )
;
//----------------------------
extern int
get_fld_name(
	     int list_id, 
	     int position, 
	     char *mode,
	     char **ptr_fld
	     )
;
//----------------------------
extern int
get_sum_fld_cnt(
		    int list_id,
		    int num_in_list,
		    char *mode,
		    long long *ptr_sum_cnt
		    )
;
//----------------------------
extern int
is_fld_in_list(
		   int list_id,
		   int fld_id,
		   bool *ptr_b_is_fld_in_list
		   )
;
//----------------------------
extern int
is_list(
	    char *list, 
	    bool *ptr_b_is_list
	    )
;
//----------------------------
extern int
get_list_id(
	    char *list,
	    int *ptr_list_id,
	    int *ptr_tbl_id,
	    int *ptr_xlist_id
	    )
;
//----------------------------
extern int
get_fld_in_list(
		int list_id,
		char *mode,
		int position,
		int *ptr_fld_id
		)
;
//----------------------------
extern int
get_num_in_list(
		int list_id,
		int *ptr_n
		)
;
//----------------------------
extern int
is_excl_fld_for_list(
		     char *xfld, 
		     int xlist_id, 
		     bool *ptr_brslt
		     )
;
//----------------------------
extern int
del_xcl_fld_from_xlist(
		       char *xfld, 
		       int xlist_id
		       )
;
//----------------------------
extern int
num_exclusions(
	       int xlist_id, 
	       int *ptr_num_in_xlist
	       )
;
//----------------------------
extern int
set_xlist_info(
		   int xlist_id,
		   char *cum_fld, 
		   int cum_fld_id
		   )
;
//----------------------------
extern int
get_xlist_info(
		   int xlist_id,
		   char **ptr_fld, 
		   int *ptr_fld_id
		   )
;
//----------------------------
extern int
del_xlist(
    int list_id,
	  int xlist_id
	  )
;
//----------------------------
extern int
del_aux_fld_if_exists(
		      int tbl_id,
		      int list_id, 
		      int pos, 
		      char *mode
		      )
;
//----------------------------
extern int
and_not_excl(
    char *list
    )
;
//----------------------------
extern int
add_to_xlist(
		  char *list,
		  int list_id,
		  char *cum_fld, 
		  int cum_fld_id
		  )
;
//----------------------------
extern int
update_xlist(
		  char *cum_fld, 
		  char *list,
		  int cum_fld_id
		  )
;
//----------------------------
extern int
del_all_lmt_flds(
    char *list
    )
;
//----------------------------
extern int
is_fld_in_xlist(
    char *xfld, 
    int xlist_id, 
    bool *ptr_b_exists
    )
;
//----------------------------
