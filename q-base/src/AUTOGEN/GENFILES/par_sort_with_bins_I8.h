extern int
par_sort_with_bins_I8(
    long long  *in,  /* [nR] */
    long long nR,
    long long  *binlb,  /* [num_bins] */
    long long  *binub,  /* [num_bins] */
    int num_bins,
    char *srt_ordr,
    int nT,
    unsigned short *bin_idx, /* [nR] */
    long long  *out /* [nR] */
    )
;

