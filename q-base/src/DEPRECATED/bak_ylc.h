extern int
callback_pr(
	    void *vptr,
	    int argc,
	    char **argv,
	    char **azColName
	    )
;
//----------------------------
extern int 
callback_sval(
	      void *vptr,
	      int argc, 
	      char **argv, 
	      char **azColName
	      )
;
//----------------------------
extern int
callback_ival(
	      void *vptr,
	      int argc,
	      char **argv,
	      char **azColName
	      )
;
//----------------------------
extern int
callback_lval(
	      void *vptr,
	      int argc,
	      char **argv,
	      char **azColName
	      )
;
//----------------------------
extern int
callback_num_rslts(
		   void *vptr,
		   int argc,
		   char **argv,
		   char **azColName
		   )
;
//----------------------------
extern int
db_get_sval(
	    sqlite3 *db,
	    char *qstr,
	    char **ptr_sval
	    )
;
//----------------------------
extern int
db_get_ival(
	    sqlite3 *db,
	    char *qstr,
	    int *ptr_ival
	    )
;
//----------------------------
extern int
db_get_lval(
	    sqlite3 *db,
	    char *qstr,
	    long long *ptr_lval
	    )
;
//----------------------------
extern int
db_get_num_rslts(
		 sqlite3 *db,
		 char *qstr,
		 int *ptr_num_rslts
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
