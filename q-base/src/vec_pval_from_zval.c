#include <math.h>
// START FUNC DECL
double normcdf(
	       double tvalue
	       ) 
// STOP FUNC DECL
{
  // Cumulative distribution function for the standard normal distribution
  // JS double xabs = Math.abs(tvalue);
  double xabs = fabs(tvalue);
  double cumnorm;
  if (xabs > 37) {
    cumnorm = 0;
  } 
  else {
    // JS double exponential = Math.exp(-Math.pow(xabs,2)/2);
          double exponential = exp( -1.0 * (xabs * xabs ) / 2.0 ) ;

    if (xabs < 7.07106781186547) {
      double build = 0.0352624965998911 * xabs + 0.700383064443688;
      build = build * xabs + 6.37396220353165;
      build = build * xabs + 33.9128660783830;
      build = build * xabs + 112.079291497871;
      build = build * xabs + 221.213596169931;
      build = build * xabs + 220.206867912376;
      cumnorm = exponential * build;
      build = 0.0883883476483184 * xabs + 1.75566716318264;
      build = build * xabs + 16.0641775792070;
      build = build * xabs + 86.7807322029461;
      build = build * xabs + 296.564248779674;
      build = build * xabs + 637.333633378831;
      build = build * xabs + 793.826512519948;
      build = build * xabs + 440.413735824752;
      cumnorm = cumnorm / build;          
    } 
    else {
      double build = xabs + 0.65;
      build = xabs + 4/build;
      build = xabs + 3/build;
      build = xabs + 2/build;
      build = xabs + 1/build;
      cumnorm = exponential/build/2.506628274631;         
    }

    if (tvalue > 0) {
      cumnorm = 1 - cumnorm;        
    }
  }
  return cumnorm;
}

// START FUNC DECL
double normPvalue(
		  double tvalue
		  ) 
// STOP FUNC DECL
{
  double pval = 2*(1 - normcdf(tvalue));
  return pval;
}


// START FUNC DECL
void
vec_pval_from_zval( 
			    double *in, 
			    int nR, 
			    double *out
			    )
// STOP FUNC DECL
{
  for ( int i = 0; i < nR; i++ ) { 
    double inval = *in;
    double outval = normPvalue(inval);
    *out = outval;
    in++;
    out++;
  }
}

