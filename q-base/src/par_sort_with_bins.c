#include "qtypes.h"
#include "par_sort_with_bins_I4.h"
#include "par_sort_with_bins_I8.h"
extern int g_num_cores;

// START FUNC DECL
int
par_sort_with_bins(
    char *in, 
    long long nR,
    char *lb, 
    char *ub, 
    int num_bins,
    FLD_TYPE fldtype, 
    char *srt_ordr,
    unsigned short *bin_idx,
    char *out
    )
// STOP FUNC DECL
{
  int status = 0;
  if ( ( srt_ordr == NULL ) || ( *srt_ordr == '\0' ) ) { go_BYE(-1); }
  if ( ( strcasecmp(srt_ordr, "asc") != 0 ) && 
       ( strcasecmp(srt_ordr, "dsc") != 0 ) ) {
    go_BYE(-1);
  }
  // TODO P4 What should nT be?
  int nT = g_num_cores * 4; if ( nT < 2 ) { go_BYE(-1); }
  switch ( fldtype ) { 
    case I4 : 
      status = par_sort_with_bins_I4((int *)in, nR, (int *)lb, 
	  (int *)ub, num_bins, srt_ordr, nT, bin_idx, (int *)out);
      break;
    case I8 : 
      status = par_sort_with_bins_I8((long long *)in, nR, (long long *)lb, 
	  (long long *)ub, num_bins, srt_ordr, nT, bin_idx, (long long *)out);
      break;
    default : 
      go_BYE(-1);
      break;
  }
BYE:
  return(status);
}
