extern int 
f1f2opf3_arith(
	       long long nR,
	       FLD_META_TYPE f1_meta,
	       FLD_META_TYPE f2_meta,
	       char *str_meta_data,
	       char *f1_X,
	       char *nn_f1_X,
	       bool is_f2_scalar,
	       char *f2_X,
	       char *nn_f2_X,
	       char *op,
	       char **ptr_opfile,
	       char **ptr_nn_opfile,
	       bool *ptr_is_any_null
	       )
;
//----------------------------
extern int
internal_f1f2opf3_arith_int( 
			    int *i1ptr, 
			    char *nn_i1ptr,
			    bool is_f2_scalar,
			    int *i2ptr, 
			    char *nn_i2ptr,
			    long long nR, 
			    int iop,
			    FILE *ofp,
			    FILE *nn_ofp,
			    bool *ptr_is_any_null
			    )
;
//----------------------------
extern int
internal_f1f2opf3_arith_long_long( 
			    long long *ll1ptr, 
			    char *nn_ll1ptr,
			    bool is_f2_scalar,
			    long long *ll2ptr, 
			    char *nn_ll2ptr,
			    long long nR, 
			    int iop,
			    FILE *ofp,
			    FILE *nn_ofp,
			    bool *ptr_is_any_null
			    )
;
//----------------------------
extern int
internal_f1f2opf3_cond_int( 
			   int *i1ptr, 
			   char *nn_i1ptr,
			   bool is_f2_scalar,
			   int *i2ptr, 
			   char *nn_i2ptr,
			   long long nR, 
			   int iop,
			   FILE *ofp,
			   FILE *nn_ofp,
			   bool *ptr_is_any_null
			   )
;
//----------------------------
