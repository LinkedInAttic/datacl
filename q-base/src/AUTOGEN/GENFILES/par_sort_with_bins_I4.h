extern int
par_sort_with_bins_I4(
    int  *in,  /* [nR] */
    long long nR,
    int  *binlb,  /* [num_bins] */
    int  *binub,  /* [num_bins] */
    int num_bins,
    char *srt_ordr,
    int nT,
    unsigned short *bin_idx, /* [nR] */
    int  *out /* [nR] */
    )
;

