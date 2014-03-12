extern int 
glue_files(
    char **infiles,
    int num_infiles,
    char *opfile
    )
;
extern int
xfer_val_to_opfile(
    FILE *ifp,
    FILE *ofp,
    bool  *ptr_is_eof
    )
;

