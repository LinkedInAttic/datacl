extern int
init_fb_idx(
    const char *tbl,
    const char *in_flds
    )
;
//----------------------------
extern int
undo_fb_idx(
    char *tbl
    )
;
//----------------------------
extern int
x_mk_fb_idx(
    char *tbl,
    char *fld,
    char *options
    )
;
//----------------------------
extern int
is_fb_idx(
    char *tbl,
    char *fld,
    int *ptr_is_idx
    )
;
//----------------------------
extern int
x_del_fb_idx(
    char *tbl,
    char *fld
    )
;
//----------------------------
extern int
x_query_fb_idx(
    char *tbl,
    char *select_fld,
    char *where_clause,
    char *output_format,
    char *rslt_buf,
    int sz_rslt_buf,
    int *ptr_n_hits
    )
;
//----------------------------
