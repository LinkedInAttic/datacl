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
mk_name_new_fld(
		char *str1, 
		char *str2, 
		char *str3, 
		char *prefix,
		char new_fld[MAX_LEN_FLD_NAME+1]
		)
;
//----------------------------
extern int
count_ones(
	   char *tbl,
	   char *fld,
	   long long *ptr_fld_cnt
	   )
;
//----------------------------
extern int
fetch_rows(
	   sqlite3 *db,
	   char *tbl,
	   char *fld,
	   char *where,
	   char *order_by,
	   void **ptr_X,
	   int *ptr_n
	   )
;
//----------------------------
