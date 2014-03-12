extern int
str_xform(
    long long nR,
    char *in,
    int *sz_in,
    char *nn_in,
    char *str_op_spec,
    FILE *ofp,
    FILE *nn_ofp,
    FILE *sz_ofp,
    bool *ptr_is_some_null
    )
;
//----------------------------
extern int
clean_buffer(
	     char *buffer, 
	     int *ptr_bufptr,
	     int xform_enum
	     )
;
//----------------------------
